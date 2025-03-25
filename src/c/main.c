#include <pebble.h>
#include "main.h"
#include "utils.h"
#include "gbitmap_color_palette_manipulator.h"
#include "health.h"

//layout
Window *s_window;
Layer *s_main_layer, *window_layer;
GRect bounds;
//colors
GColor outer_color, middle_color, inner_color, main_color;

//fonts
GFont font_medium, font_small, font_small_bold, font_large, font_inter, font_inter_b;

static GBitmap *bluetooth_sprite = NULL, *meteoicons_all, *meteoicon_current;

//buffers
char s_battery[] = "100", s_date[] = "18 DEC  ", s_time[]="88:44", format[]="%l:%h";
char s_weather_temp[6], s_health_info[10], s_health_info_text[7];
int h_steps, h_steps_goal, h_sleep, h_sleep_goal, h_distance_m, h_sleep_deep;

//flags
int flag_main_clock, flag_second_hand, flag_bluetooth_alert, flag_time_separator, flag_color_selection, flag_show_visual_health_goal;
int flag_locationService, flag_weatherInterval, flag_temperatureFormat, flag_woeid, flag_secondary_info_type, flag_theme_gradient;
bool is_bluetooth_buzz_enabled, flag_js_is_ready = false, flag_messaging_is_busy = false;

//main gradient background bitmap
GBitmap *gradient_bitmap;

//submitmaps for icons/numbers backgrounds
GBitmap *grad_dig_bt_bg, *grad_dig_batt_bg;
GBitmap *grad_an_bt_bg, *grad_an_batt_bg, *grad_an_3, *grad_an_6, *grad_an_9, *grad_an_12 ;

GRect Circ, Circ2;

// raised from health.c "health_handler" event handler

void update_health_info() {
  if (health_is_available()) {

    h_steps = health_get_metric_sum(HealthMetricStepCount);
    h_steps_goal = health_get_metric_goal(HealthMetricStepCount);
    h_distance_m = health_get_metric_sum(HealthMetricWalkedDistanceMeters);
    h_sleep = health_get_metric_sum(HealthMetricSleepSeconds);
    h_sleep_goal = health_get_metric_goal(HealthMetricSleepSeconds);
    h_sleep_deep = health_get_metric_goal(HealthMetricSleepRestfulSeconds);
    
    switch (flag_secondary_info_type) {
      case SECONDARY_INFO_DISTANCE_STEPS:
         #ifdef PBL_ROUND
         snprintf(s_health_info, sizeof(s_health_info), "%d", h_steps );
         snprintf(s_health_info_text, sizeof(s_health_info_text), "steps");
         #else
         snprintf(s_health_info, sizeof(s_health_info), "st %d",  h_steps);
         #endif
         break;
      case SECONDARY_INFO_DISTANCE_METERS:
         #ifdef PBL_ROUND      
         snprintf(s_health_info, sizeof(s_health_info), "%d",  h_distance_m);
         snprintf(s_health_info_text, sizeof(s_health_info_text), "meters");
         #else
         snprintf(s_health_info, sizeof(s_health_info), "m %d",  h_distance_m);
         #endif
         break;
      case SECONDARY_INFO_DISTANCE_YARDS:
         #ifdef PBL_ROUND      
         snprintf(s_health_info, sizeof(s_health_info), "%d", h_distance_m * 1367/1250 /* 1.09361 */);
         snprintf(s_health_info_text, sizeof(s_health_info_text), "yards");
         #else 
         snprintf(s_health_info, sizeof(s_health_info), "yd %d", h_distance_m * 1367/1250 /* 1.09361 */);
         #endif
         break;      
      case SECONDARY_INFO_SLEEP:
         #ifdef PBL_ROUND
         snprintf(s_health_info, sizeof(s_health_info), "%d",  h_sleep / 3600);
         snprintf(s_health_info_text, sizeof(s_health_info_text), "hours");
         #else
         snprintf(s_health_info, sizeof(s_health_info), "hr %d",  h_sleep / 3600);
         #endif
         break;      
     case SECONDARY_INFO_DEEP_SLEEP:
         #ifdef PBL_ROUND      
         snprintf(s_health_info, sizeof(s_health_info), "%d",  h_sleep_deep / 3600);
         snprintf(s_health_info_text, sizeof(s_health_info_text), "hours");
         #else
         snprintf(s_health_info, sizeof(s_health_info), "hr %d",  h_sleep_deep / 3600);
         #endif
         break;       
    }
      
  } else {
      snprintf(s_health_info, sizeof(s_health_info), "N/A");
  }
}

//retrying weather update
static void update_weather();
void weather_updare_retry(void *context) {
   update_weather();
}
  
  
//calling for weather update
static void update_weather() {
  // Only grab the weather if we can talk to phone AND weather is enabled AND currently message is not being processed and JS on phone is ready
  if (flag_locationService != LOCATION_DISABLED && bluetooth_connection_service_peek() && !flag_messaging_is_busy && flag_js_is_ready && flag_secondary_info_type == SECONDARY_INFO_WEATHER){
   //APP_LOG(APP_LOG_LEVEL_INFO, "**** I am inside 'update_weather()' about to request weather from the phone ***");
    
    //need to have some data - sending dummy
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);  
    Tuplet dictionary[] = {
      TupletInteger(0, 0),
    };
    dict_write_tuplet(iter, &dictionary[0]);
    
     flag_messaging_is_busy = true;
     int msg_result = app_message_outbox_send(); // need to assign result for successfull call
    //APP_LOG(APP_LOG_LEVEL_INFO, "**** I am inside 'update_weather()' message sent and result code = %d***", msg_result);
     
      if (msg_result == APP_MSG_BUSY) { // on initial load could be busy due to Keizel communication
        flag_messaging_is_busy = false;
        app_timer_register(5000, weather_updare_retry, NULL); //re-trying weather update in 5 seconds
      }
  } 
}


// showing temp
static void set_temperature(int w_current) {
   //APP_LOG(APP_LOG_LEVEL_INFO, "**** I am inside 'show_temperature()'; TEMP in Pebble: %d", w_current);
    snprintf(s_weather_temp, sizeof(s_weather_temp), "%i\u00B0", w_current);
    layer_mark_dirty(s_main_layer);
}

//showing weather icon
static void set_weather_icon(int w_icon) {

  if (w_icon < 0 || w_icon > 47) {w_icon = 48;}  // in case icon not available - show "N/A" icon
  
 //APP_LOG(APP_LOG_LEVEL_INFO, "**** I am inside 'set_weather_icon'; Icon IS: %d", w_icon);
  
  if (meteoicon_current)  gbitmap_destroy(meteoicon_current);
  Circ = GRect(0, ICON_HEIGHT*w_icon, ICON_WIDTH, ICON_HEIGHT);
  meteoicon_current = gbitmap_create_as_sub_bitmap(meteoicons_all, Circ);
  replace_gbitmap_color(main_color,GColorBlack , meteoicon_current, NULL); // restoring original color, so main routine could assign new one
   
  layer_mark_dirty(s_main_layer);
  
}

#ifdef PBL_COLOR // only on color watches
// creates main background from gradient array and sub-bitmaps for icons/numbers backgrounds
static void create_gradient_background(int bg_id) {
  
   //APP_LOG(APP_LOG_LEVEL_INFO, "Inside create_gradient_background before main, Heap = %d", (int)heap_bytes_free());
  
  // main bg
  gradient_bitmap = gbitmap_create_with_resource(bg_id);
  
  //APP_LOG(APP_LOG_LEVEL_INFO, "Inside create_gradient_background after main, Heap = %d", (int)heap_bytes_free());
  
  //icon/text bg
  
  #ifdef PBL_RECT
    grad_dig_batt_bg = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2 - 16, bounds.origin.y + 18, 32, 13));
    grad_dig_bt_bg = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2 - 14, bounds.size.h-33, 28, 14));
  
    grad_an_3 = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w - 34, bounds.size.h/2 - 10, 15, 20));
    grad_an_6 = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2-12, bounds.size.h - 36, 24, 18));
    grad_an_9 = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.origin.x + 20, bounds.size.h/2 - 10, 15, 20)); 
    grad_an_12 = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2-12, bounds.origin.y + 23, 24, 13));
                                             
    grad_an_batt_bg = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2 - 11, bounds.origin.y + 2, 22, 15));
    grad_an_bt_bg = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2 - 11,bounds.size.h-17, 22, 17));
  
  #else
      grad_dig_batt_bg = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2 - 16, bounds.origin.y + 23, 32, 15));
      grad_dig_bt_bg = gbitmap_create_as_sub_bitmap(gradient_bitmap,  GRect(bounds.size.w/2 - 14, bounds.size.h-38, 28, 14));
  
      grad_an_3 = gbitmap_create_as_sub_bitmap(gradient_bitmap,  GRect(bounds.size.w - 39, bounds.size.h/2 - 10, 15, 20));
      grad_an_6 = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2-12, bounds.size.h - 42, 24, 18));
      grad_an_9 = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.origin.x + 25, bounds.size.h/2 - 10, 15, 20)); 
      grad_an_12 = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2-12, bounds.origin.y + 23, 24, 14));
  
      grad_an_batt_bg = gbitmap_create_as_sub_bitmap(gradient_bitmap, GRect(bounds.size.w/2 - 10, bounds.origin.y + 4, 20, 15));
  
  #endif
  
   //APP_LOG(APP_LOG_LEVEL_INFO, "Inside create_gradient_background Aftee Sub, Sub = %d", (int)heap_bytes_free());
}
#endif

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  flag_messaging_is_busy = false;
 //APP_LOG(APP_LOG_LEVEL_ERROR, "____Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  flag_messaging_is_busy = false;
 //APP_LOG(APP_LOG_LEVEL_ERROR, "____Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  flag_messaging_is_busy = false;
 //APP_LOG(APP_LOG_LEVEL_INFO, "_____Outbox send success!");
} 

// handle configuration change
void change_time_tick_inteval();
static void battery_handler(BatteryChargeState state);
void bluetooth_handler(bool connected);
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
  bool needs_health = 0;
  bool needs_rerender = 0;
  bool needs_bluetooth = 0;
  bool needs_weather = 0;
  
  
  Tuple *t = dict_read_first(iterator);

  while (t)  {
    
    switch(t->key)    {
        
      // *********  weather data keys
      case KEY_WEATHER_TEMP:
        if (persist_read_int(KEY_WEATHER_TEMP) != t->value->int32) {
          persist_write_int(KEY_WEATHER_TEMP, t->value->int32);
          set_temperature(t->value->int32);
        }   
        break;
      
      case KEY_WEATHER_CODE:
        if (persist_read_int(KEY_WEATHER_CODE) != t->value->int32) {
          persist_write_int(KEY_WEATHER_CODE, t->value->int32);
          set_weather_icon(t->value->int32);
        }  
        break;
           
      case KEY_JSREADY:
          // JS ready lets get the weather
          if (t->value->int16) {
           //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' message 'JS is ready' received !");
            flag_js_is_ready = true;
            needs_weather = 1;
          }
          break;
      
      // *********  config data keys
      case KEY_MAIN_CLOCK:
             if (flag_main_clock != t->value->uint8) {
               persist_write_int(KEY_MAIN_CLOCK, t->value->uint8);
               flag_main_clock = t->value->uint8;
               needs_bluetooth = 1; //- so the icon can be destroyed and recreated to be colored properly
               needs_rerender = 1;
              //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' KEY_MAIN_CLOCK = %d", flag_main_clock);
             }  
           break;
      case KEY_SECOND_HAND:
             if (flag_second_hand != t->value->uint8) {
               persist_write_int(KEY_SECOND_HAND, t->value->uint8);
               flag_second_hand = t->value->uint8;
               change_time_tick_inteval();
             }  
           break;
      
       case KEY_COLOR_SELECTION:
            if (flag_color_selection != t->value->uint8){
              persist_write_int(KEY_COLOR_SELECTION, t->value->uint8);
              flag_color_selection = t->value->uint8;
              needs_rerender = 1;
              needs_bluetooth = 1; //- so the icon can be destroyed and recreated to be colored properly
             //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' KEY_COLOR_SELECTION = %d", flag_color_selection);
            }  
          break;
      case KEY_OUTER_COLOR:
           //if (!gcolor_equal(outer_color, GColorFromHEX(t->value->int32))){
             persist_write_int(KEY_OUTER_COLOR, t->value->int32);
             outer_color = GColorFromHEX(t->value->int32);
            //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' OUTER_COLOR = %d", outer_color.argb );
             needs_rerender = 1;
           //}  
           break;      
      case KEY_MIDDLE_COLOR:
           //if (!gcolor_equal(middle_color, GColorFromHEX(t->value->int32))) {
             persist_write_int(KEY_MIDDLE_COLOR, t->value->int32);
             middle_color  = GColorFromHEX(t->value->int32);
             needs_rerender = 1;
          // }  
           break;      
      case KEY_INNER_COLOR:
           //if (!gcolor_equal(inner_color, GColorFromHEX(t->value->int32))) {
             persist_write_int(KEY_INNER_COLOR, t->value->int32);
             inner_color  = GColorFromHEX(t->value->int32);
             needs_rerender = 1;
          // }  
           break;      
      case KEY_MAIN_COLOR:
          // if (!gcolor_equal(main_color, GColorFromHEX(t->value->int32))) { 
             
             if (persist_exists(KEY_WEATHER_CODE)) {set_weather_icon(persist_read_int(KEY_WEATHER_CODE));}; // restore original icon color so later we could recolor it
             
             persist_write_int(KEY_MAIN_COLOR, t->value->int32);
             main_color  = GColorFromHEX(t->value->int32);
             //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' MAIN_COLOR = %d", main_color.argb );
             needs_rerender = 1;
         //  }  
          
           break;      
      
      case KEY_TIME_SEPARATOR:
           if (flag_time_separator != t->value->uint8) {
             persist_write_int(KEY_TIME_SEPARATOR, t->value->uint8);
             flag_time_separator = t->value->uint8;
             needs_rerender = 1;
           }  
         break;
      
      case KEY_BLUETOOTH_ALERT:
             if (flag_bluetooth_alert != t->value->uint8) {
               persist_write_int(KEY_BLUETOOTH_ALERT, t->value->uint8);
               flag_bluetooth_alert = t->value->uint8;
               needs_bluetooth = 1;
             }
         break;
      
      case KEY_WEATHER_INTERVAL:
           // converting passed char "#", "2" or "P" to number 15, 30 or 60
           if (t->value->uint8 - 20 != flag_weatherInterval && t->value->uint8 !=1) { // precaution, dunno why we get 1 here as well
             persist_write_int(KEY_WEATHER_INTERVAL, t->value->uint8 - 20);
             flag_weatherInterval = t->value->uint8 - 20;
             needs_weather = 1;
            //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' Weather interval set to interval to %d min", flag_weatherInterval);
           }
           break;
        
       case KEY_TEMPERATURE_FORMAT: //if temp format changed from F to C or back - need re-request weather
         if (flag_temperatureFormat != t->value->uint8) {
           persist_write_int(KEY_TEMPERATURE_FORMAT, t->value->uint8);
           flag_temperatureFormat = t->value->uint8;
          //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' switching temp format");
           needs_weather = 1;
         }
         break;
      
       case KEY_LOCATION_SERVICE: 
         if (flag_locationService != t->value->uint8) {
           persist_write_int(KEY_LOCATION_SERVICE, t->value->uint8);
           flag_locationService = t->value->uint8;
          //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' switching KEY_LOCATION_SERVICEt");
           needs_weather = 1;
         }
         break;
      
        case KEY_WOEID: // we never use it in C code just need to know the fact it changed
         if (flag_woeid != t->value->int32) {
           persist_write_int(KEY_WOEID, t->value->int32);
           flag_woeid = t->value->int32;
          //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' switching KEY_WOEID");
           needs_weather = 1;
         }
         break;
      
        case KEY_SECONDARY_INFO_TYPE:
           if (flag_secondary_info_type != t->value->uint8) {
             
             persist_write_int(KEY_SECONDARY_INFO_TYPE, t->value->uint8);
             flag_secondary_info_type = t->value->uint8;
             needs_rerender = 1;
             
             if (flag_secondary_info_type == SECONDARY_INFO_WEATHER){
               needs_weather = 1;
             } else {
               needs_health = 1;
             }
             
                
             
           }  
           break;
      
       case KEY_SHOW_VISUAL_HEALTH_GOAL: // 
         if (flag_show_visual_health_goal != t->value->uint8) {
           persist_write_int(KEY_SHOW_VISUAL_HEALTH_GOAL, t->value->uint8);
           flag_show_visual_health_goal = t->value->uint8;
          //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'inbox_received_callback()' switching KEY_WOEID");
           needs_rerender = 1;
         }
         break;
      
      #ifdef PBL_COLOR // gradients only on color watches
      case KEY_COLOR_THEME_GRADIENT: // reading gradient theme
      
         t->value->uint8 -= 48; // converting ascii char to int code;
      
         if (flag_theme_gradient != t->value->uint8) {
           persist_write_int(KEY_COLOR_THEME_GRADIENT, t->value->uint8);
           flag_theme_gradient = t->value->uint8;
      
           //cleanup
          if (grad_dig_batt_bg) gbitmap_destroy(grad_dig_batt_bg);
          if (grad_dig_bt_bg) gbitmap_destroy(grad_dig_bt_bg);
          if (grad_an_3) gbitmap_destroy(grad_an_3);
          if (grad_an_6) gbitmap_destroy(grad_an_6);
          if (grad_an_9) gbitmap_destroy(grad_an_9);
          if (grad_an_12) gbitmap_destroy(grad_an_12);
          if (grad_an_batt_bg) gbitmap_destroy(grad_an_batt_bg);
          if (grad_an_bt_bg) gbitmap_destroy(grad_an_bt_bg);
          
          if (gradient_bitmap) gbitmap_destroy(gradient_bitmap);
                   
           if (flag_theme_gradient != 74) { // if it's a gradient theme - retreive bitmap 0, 1, 2... from GRADIENT array
             create_gradient_background(GRADIENTS[flag_theme_gradient]);
           }
           
           needs_rerender = 1;
         }
      
         break;
      #endif
      
    }    
    
    t = dict_read_next(iterator);
  
  }
  
  if (needs_health == 1) {
      update_health_info(); // showing health info for the first time
  }
  
  if (flag_color_selection == COLOR_SELECTION_AUTOMATIC) {
    battery_handler(battery_state_service_peek()); // if we switched to automatic colors: restore battery colors
  }  else {
    needs_bluetooth = 1;
  }
  
  if (needs_bluetooth == 1) {
      is_bluetooth_buzz_enabled = false;
      bluetooth_handler(bluetooth_connection_service_peek());
      is_bluetooth_buzz_enabled = true;  
  } 
  
  if (needs_weather == 1) {
    update_weather();
  }  
  
  if (needs_rerender == 1) {
    layer_mark_dirty(s_main_layer);
  }  
  
}



void bluetooth_handler(bool connected) {
  
  if (connected && is_bluetooth_buzz_enabled){ // on bluetooth reconnect - update weather (only if we're not calling this on purpose, otherwise battery will drain in infinite loop)
   //APP_LOG(APP_LOG_LEVEL_INFO, "***** I am inside of 'bluetooth_handler()' about to call 'update_weather();");
    update_weather();
  } 
  
  // destroying BT icon
  if (bluetooth_sprite) {
     gbitmap_destroy(bluetooth_sprite);
     bluetooth_sprite = NULL;
   }
  
  // if Bluetooth alert is totally disabled - exit from here
  if (flag_bluetooth_alert == BLUETOOTH_ALERT_DISABLED) {
    layer_mark_dirty(s_main_layer);
    return;  
  }
  
  // if BT connected - creating and recoloring icon
  if (connected) {
      bluetooth_sprite = gbitmap_create_with_resource(RESOURCE_ID_IMG_BLUETOOTH);
  }
  
  layer_mark_dirty(s_main_layer);
  
  // if this is initial load OR bluetooth alert is silent return without buzz
  if (flag_bluetooth_alert == BLUETOOTH_ALERT_SILENT || is_bluetooth_buzz_enabled == false) return;
    
  switch (flag_bluetooth_alert){
    case BLUETOOTH_ALERT_WEAK:
      vibes_enqueue_custom_pattern(VIBE_PATTERN_WEAK);
      break;
    case BLUETOOTH_ALERT_NORMAL:
      vibes_enqueue_custom_pattern(VIBE_PATTERN_NORMAL);
      break;
    case BLUETOOTH_ALERT_STRONG:
    vibes_enqueue_custom_pattern(VIBE_PATTERN_STRONG);
      break;
    case BLUETOOTH_ALERT_DOUBLE:
      vibes_enqueue_custom_pattern(VIBE_PATTERN_DOUBLE);
      break;    
  }
  
  
}  


static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
 
  if (!(tick_time->tm_min % flag_weatherInterval) && (tick_time->tm_sec == 0)) { // on configured weather interval change - update the weather
       //APP_LOG(APP_LOG_LEVEL_INFO, "**** I am inside 'tick_handler()' about to call 'update_weather();' at minute %d min on %d interval", tick_time->tm_min, flag_weatherInterval);
        update_weather();
  } 
  
  layer_mark_dirty(s_main_layer);
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "TICK-TOCK: Free heap = %d", (int)heap_bytes_free());
  
}




// changes tick inteval to second or minute, depending on flags
void change_time_tick_inteval() {
  
  tick_timer_service_unsubscribe(); // unsubscribing from old interval
  
  if (flag_main_clock == MAIN_CLOCK_ANALOG && flag_second_hand == SECOND_HAND_ENABLED) { // if analog is on and second hand enabled
    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);
  }
  
  layer_mark_dirty(s_main_layer);
  
}


static void battery_handler(BatteryChargeState state) {
  
  // changing colors if they're automatic
  if (flag_color_selection == COLOR_SELECTION_AUTOMATIC) {
    change_battery_color(state.charge_percent);
  }  else {
    layer_mark_dirty(s_main_layer);
  }

  
}


void main_layer_update(Layer* layer, GContext *ctx){
  
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  GRect u_bounds = layer_get_unobstructed_bounds(layer);
  GPoint u_center = grect_center_point(&u_bounds);
 
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
// TEST
//   t->tm_hour = 9;
//   t->tm_min = 10;
//   t->tm_sec =  20;
    
  
  GPoint tick_point;
  int tick;
  int battery_percent = battery_state_service_peek().charge_percent;
  int discharged_tick = (100 - battery_percent) * 60 / 100;
   
  #ifdef PBL_RECT 
  int tick_step = ((bounds.size.w - 9*2) * 2 + (bounds.size.h - 9*2) * 2 ) / 60;
  GRect tick_rect; tick_rect.size.w = 4; tick_rect.size.h = 4;
  
  if (flag_theme_gradient == 74) { // "z" was passed - no gradients
  
      // outer rectangle
      graphics_context_set_fill_color(ctx, outer_color);
      graphics_fill_rect(ctx, bounds, 0, GCornerNone);
      
      // separator between outer & mid
      graphics_context_set_fill_color(ctx, main_color);
      Circ = GRect(bounds.origin.x + 17, bounds.origin.y + 17, bounds.size.w - 34, bounds.size.h - 34);
      graphics_fill_rect(ctx, Circ, 5, GCornersAll);
      
      // mid rectangle
      graphics_context_set_fill_color(ctx, middle_color);
      Circ = GRect(bounds.origin.x + 18, bounds.origin.y + 18, bounds.size.w - 36, bounds.size.h - 36);
      graphics_fill_rect(ctx, Circ, 5, GCornersAll);
      
      // separator between mid & inner
      graphics_context_set_fill_color(ctx, outer_color);
      Circ = GRect(bounds.origin.x + 36, bounds.origin.y + 36, bounds.size.w - 72, bounds.size.h - 72);
      graphics_fill_rect(ctx, Circ, 5, GCornersAll);
      
      // inner rectangle
      graphics_context_set_fill_color(ctx, inner_color);
      Circ = GRect(bounds.origin.x + 37, bounds.origin.y + 37, bounds.size.w - 74, bounds.size.h - 74);
      graphics_fill_rect(ctx, Circ, 5, GCornersAll);
  
  } else {
      graphics_draw_bitmap_in_rect(ctx, gradient_bitmap, bounds);
  }
  
  //drawing ticks
  for (tick = 0; tick<60; tick++) {
    
    if (tick <= discharged_tick) {
        graphics_context_set_fill_color(ctx, middle_color);  //ticks showing discharged battery
    } else {
        graphics_context_set_fill_color(ctx, inner_color);   //ticks showing still charged battery   
    }
    
    if (tick <= 7) {
      tick_point.y = 9; tick_point.x = bounds.size.w/2 + tick*tick_step;
      if (tick == 7) {tick_point.y +=4; tick_point.x-=2;} //rounding corner
    } else if (tick <= 23) {
      tick_point.x = bounds.size.w - 9;
      tick_point.y = 13 + (tick-7)*tick_step;
      if (tick == 23) {tick_point.y -=2; tick_point.x-=2;} //rounding corner
    } else if (tick <= 37) {
      tick_point.y = bounds.size.h - 9;
      tick_point.x = -10 + bounds.size.w - (tick-23)*tick_step;
      if (tick == 37) {tick_point.y -=2; tick_point.x +=2;} //rounding corner
    } else if (tick <= 53) {
      tick_point.y =  -11 + bounds.size.h - (tick-37)*tick_step;
      tick_point.x = 8;
      if (tick == 53) {tick_point.x +=2;} //rounding corner
    } else {
      tick_point.y = 9; 
      tick_point.x = 8 + (tick-53)*tick_step;
    }
    
   
    if ((tick % 5 == 0) && (tick > discharged_tick)) {
      graphics_fill_circle(ctx, tick_point, 3);  // larhe tick at 5 iterval
    } else {
      graphics_fill_circle(ctx, tick_point, 1);  // small tick at 1 interval
    }

   
  }
  
  //*** drawing inner half-rectangles 
  graphics_context_set_fill_color(ctx, outer_color);
  
  int steps_percentage;
  
  if (flag_show_visual_health_goal == 1) { // if step goals are enabled - use them as percentage
    steps_percentage =  h_steps * 105 / h_steps_goal;
  } else { //otherwise split rectangle in half
    steps_percentage = 51;
  }
 
  for (tick = 0; tick<104; tick++) {
    
     
    if (tick <= steps_percentage) {
       graphics_context_set_fill_color(ctx, inner_color );  //current steps
    } else {
       graphics_context_set_fill_color(ctx, outer_color);   //remaining goal steps  
    }
     
     if (tick <= 10) {
         tick_rect.origin.x = bounds.size.w/2 + (tick)*4;
         tick_rect.origin.y = 25;
      } else if (tick <= 39) {
        tick_rect.origin.x = bounds.size.w - 29;
        tick_rect.origin.y = 21 + (tick-10)*4;
      }  else if (tick <= 62) {
        tick_rect.origin.x = bounds.size.w - 25 - (tick-39)*4;
        tick_rect.origin.y = 139 ;
    } else if (tick <= 91) {
       tick_rect.origin.x = 25;
       tick_rect.origin.y = bounds.size.h - 25 - (tick-62)*4;
     } else {
         tick_rect.origin.x = 21 + (tick-91)*4;
         tick_rect.origin.y = 25;
     }
    
      graphics_fill_rect(ctx, tick_rect, 0, GCornerNone);
    
  }

  
  #else
  uint32_t angle;
  
  graphics_context_set_stroke_width(ctx, 4);
  
   if (flag_theme_gradient == 74) { // "z" was passed - no gradients
  
        //drawing sepatators (shades) between circles
        graphics_context_set_stroke_color(ctx, main_color);
        Circ = GRect(bounds.origin.x + 22, bounds.origin.y + 22, bounds.size.w - 44, bounds.size.h - 44);
        graphics_draw_arc(ctx, Circ ,GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
        
        graphics_context_set_stroke_color(ctx, outer_color);
        Circ =  GRect(bounds.origin.x + 43, bounds.origin.y + 43, bounds.size.w - 86, bounds.size.h - 86);
        graphics_draw_arc(ctx, Circ,GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
      
      
        //drawing outer circle
        graphics_context_set_fill_color(ctx, outer_color);
        graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 21, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
        
        //drawing middle circle
        graphics_context_set_fill_color(ctx, middle_color);
        Circ = GRect(bounds.origin.x + 22, bounds.origin.y + 22, bounds.size.w - 44, bounds.size.h - 44);
        graphics_fill_radial(ctx, Circ, GOvalScaleModeFitCircle, 21, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
        
        //drawing inner circle
        graphics_context_set_fill_color(ctx, inner_color);
        Circ = GRect(bounds.origin.x + 44, bounds.origin.y + 44, bounds.size.w - 88, bounds.size.h - 88);
        graphics_fill_radial(ctx, Circ, GOvalScaleModeFitCircle, 46, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
     
  } else  {
        graphics_draw_bitmap_in_rect(ctx, gradient_bitmap, bounds);
  }

  
  //****drawing arcs in middle circle
  int step_percentage;
  if (flag_show_visual_health_goal == 1) {
      step_percentage = h_steps * (TRIG_MAX_ANGLE) / h_steps_goal; // if option to show goal enabled - calculate percentage
  } else {
      step_percentage = TRIG_MAX_ANGLE/2; //otherwise divide circle in 2
  }
  
  // first part of circle
   Circ = GRect(bounds.origin.x + 33, bounds.origin.y + 33, bounds.size.w - 66, bounds.size.h - 66);
   graphics_context_set_stroke_color(ctx, inner_color);
   graphics_draw_arc(ctx, Circ, GOvalScaleModeFitCircle, 0, step_percentage);
    
   // second part of circle
   Circ = GRect(bounds.origin.x + 33, bounds.origin.y + 33, bounds.size.w - 66, bounds.size.h - 66);
   graphics_context_set_stroke_color(ctx, outer_color);
   graphics_draw_arc(ctx, Circ, GOvalScaleModeFitCircle, step_percentage,TRIG_MAX_ANGLE);
 
  
  //drawing ticks
  for (tick = 1; tick<=60; tick++) {
    
    if (tick <= discharged_tick) {
        graphics_context_set_fill_color(ctx, middle_color);  //ticks showing discharged battery
    } else {
        graphics_context_set_fill_color(ctx, inner_color);   //ticks showing still charged battery   
    }
    
     angle = TRIG_MAX_ANGLE * tick / 60;
     Circ = GRect(bounds.origin.x + 10, bounds.origin.y + 10, bounds.size.w - 20, bounds.size.h - 20);
     tick_point = gpoint_from_polar(Circ, GOvalScaleModeFitCircle, angle);
  
    if ((tick % 5 == 0) && (tick > discharged_tick)) {
      graphics_fill_circle(ctx, tick_point, 3);  // larhe tick at 5 iterval
    } else {
      graphics_fill_circle(ctx, tick_point, 1);  // small tick at 1 interval
    }
    
   
  }
  #endif
  
  graphics_context_set_text_color(ctx, main_color);
  
  //displaying date
  strftime(s_date, sizeof(s_date), "%d %b", t);
  uppercase(s_date);
  
   Circ = GRect(bounds.size.w/2 - 51, bounds.size.h/2 + PBL_IF_ROUND_ELSE(19, 26) - (bounds.size.h - u_bounds.size.h)/4, 100, 20);
   graphics_draw_text(ctx, s_date, font_medium, Circ, GTextOverflowModeFill, GTextAlignmentCenter, NULL); 

   if (flag_secondary_info_type == SECONDARY_INFO_WEATHER){ //displaying weather
       Circ = GRect(bounds.size.w/2 + 5 ,bounds.origin.y + PBL_IF_ROUND_ELSE(52, 42), 100, 20);
       graphics_draw_text(ctx, s_weather_temp, font_small_bold, Circ, GTextOverflowModeFill, GTextAlignmentLeft, NULL); 
       if (meteoicon_current) {
         replace_gbitmap_color(GColorBlack, main_color, meteoicon_current, NULL);
         graphics_context_set_compositing_mode(ctx, GCompOpSet);
         Circ = GRect(bounds.size.w/2 - ICON_WIDTH/2 - 11, bounds.origin.y + PBL_IF_ROUND_ELSE(51, 41), ICON_WIDTH, ICON_HEIGHT);
         graphics_draw_bitmap_in_rect(ctx, meteoicon_current, Circ);
       };
   } else {// displaying health
      #ifdef PBL_ROUND
      Circ = GRect(bounds.origin.x ,bounds.origin.y + 47, bounds.size.w, 20);
      graphics_draw_text(ctx, s_health_info_text, font_small_bold, Circ, GTextOverflowModeFill, GTextAlignmentCenter, NULL);      
      Circ = GRect(bounds.origin.x ,bounds.origin.y + 59, bounds.size.w, 20);
      graphics_draw_text(ctx, s_health_info, font_small_bold, Circ, GTextOverflowModeFill, GTextAlignmentCenter, NULL);      
      #else
      Circ = GRect(bounds.origin.x ,bounds.origin.y + 39, bounds.size.w, 20);
      graphics_draw_text(ctx, s_health_info, font_small_bold, Circ, GTextOverflowModeFill, GTextAlignmentCenter, NULL);      
      #endif
   } 
  
  
  if (flag_main_clock == MAIN_CLOCK_ANALOG) {
    
    {//************************ drawing hands ****************************
    int16_t max_hand_length = bounds.size.w / 2 - PBL_IF_ROUND_ELSE(50, 39);
  
    // ******************* hour hand
    int32_t angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  
    // if second hand is disabled - hour hand takes battery health color  
    
    GPoint hand_endpoint = {
      .x = (int16_t)(sin_lookup(angle) * (int32_t)(max_hand_length-12) / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(angle) * (int32_t)(max_hand_length-12) / TRIG_MAX_RATIO) + center.y
    };
    
    //color under hand
    graphics_context_set_stroke_color(ctx, inner_color);
    graphics_context_set_stroke_width(ctx, 4);
    graphics_draw_line(ctx, center, hand_endpoint);
    
    graphics_context_set_stroke_color(ctx, main_color);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, center, hand_endpoint);
     
    // ******************** minute hand
    angle = TRIG_MAX_ANGLE * t->tm_min / 60;
    
    hand_endpoint.x = (int16_t)(sin_lookup(angle) * (int32_t)max_hand_length / TRIG_MAX_RATIO) + center.x;
    hand_endpoint.y = (int16_t)(-cos_lookup(angle) * (int32_t)max_hand_length / TRIG_MAX_RATIO) + center.y;
    
    //color under hand
    graphics_context_set_stroke_color(ctx, inner_color);
    graphics_context_set_stroke_width(ctx, 4);
    graphics_draw_line(ctx, center, hand_endpoint);
    
    graphics_context_set_stroke_color(ctx, main_color);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, center, hand_endpoint);
    
    graphics_context_set_stroke_width(ctx, 1); //resetting for second hand and inner circle
   
    // ***************** second hand
    if (flag_second_hand == SECOND_HAND_ENABLED) {  
      angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
      
      hand_endpoint.x = (int16_t)(sin_lookup(angle) * (int32_t)max_hand_length / TRIG_MAX_RATIO) + center.x;
      hand_endpoint.y = (int16_t)(-cos_lookup(angle) * (int32_t)max_hand_length / TRIG_MAX_RATIO) + center.y;
      
      graphics_context_set_stroke_color(ctx, main_color);  
      graphics_draw_line(ctx, center, hand_endpoint);
    } 
    
    // first circle in the middle
    graphics_context_set_fill_color(ctx, main_color);
    graphics_fill_circle(ctx, center, 5);
    
    // second circle in the middle
    graphics_context_set_stroke_color(ctx, middle_color);  

    graphics_draw_circle(ctx, center, 3);
    }//************************ drawing hands **************************** 
    
    
    
    //drawing middle circle numners
    graphics_context_set_text_color(ctx, main_color);
    graphics_context_set_fill_color(ctx, middle_color);

    if (flag_theme_gradient == 74) { // no gradient theme, plain bg
        Circ = GRect(bounds.size.w/2-12, bounds.origin.y + PBL_IF_ROUND_ELSE(22, 16), 24, 14);
        Circ2 = GRect(bounds.size.w/2-12, bounds.origin.y + 23, 24, PBL_IF_ROUND_ELSE(14, 13));
        graphics_draw_text_with_background(ctx, "12", font_inter, Circ, Circ2 );
      
        Circ = GRect(bounds.size.w/2-PBL_IF_ROUND_ELSE(12,11), bounds.size.h - PBL_IF_ROUND_ELSE(42,37), 24, 18);
        Circ2 = PBL_IF_ROUND_ELSE(GRectZero, GRect(bounds.size.w/2-12, bounds.size.h - 36, 24, 18));
        graphics_draw_text_with_background(ctx, "6", font_inter, Circ, Circ2);
      
        Circ = GRect(bounds.origin.x + PBL_IF_ROUND_ELSE(25, 20), bounds.size.h/2 - 10, 15, 20);
        graphics_draw_text_with_background(ctx, "9", font_inter, Circ, GRectZero );
      
        Circ =  GRect(bounds.size.w - PBL_IF_ROUND_ELSE(39, 34), bounds.size.h/2 - 10, 15, 20);
        graphics_draw_text_with_background(ctx, "3", font_inter, Circ, GRectZero );
    } else { // gradient bg
        Circ =  PBL_IF_ROUND_ELSE(GRect(bounds.size.w/2-12, bounds.origin.y + 23, 24, 14), GRect(bounds.size.w/2-12, bounds.origin.y + 23, 24, 13));
        graphics_draw_bitmap_in_rect(ctx, grad_an_12,Circ);
        Circ = GRect(bounds.size.w/2-12, bounds.origin.y + PBL_IF_ROUND_ELSE(22, 16), 24, 14);
        graphics_draw_text(ctx, "12", font_inter, Circ, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

        Circ = PBL_IF_ROUND_ELSE(GRect(bounds.size.w/2-12, bounds.size.h - 42, 24, 18), GRect(bounds.size.w/2-12, bounds.size.h - 36, 24, 18));
        graphics_draw_bitmap_in_rect(ctx, grad_an_6, Circ);
        Circ = GRect(bounds.size.w/2-PBL_IF_ROUND_ELSE(12,11), bounds.size.h - PBL_IF_ROUND_ELSE(42,37), 24, 18);
        graphics_draw_text(ctx, "6", font_inter, Circ, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
        
        Circ = PBL_IF_ROUND_ELSE(GRect(bounds.origin.x + 25, bounds.size.h/2 - 10, 15, 20), GRect(bounds.origin.x + 20, bounds.size.h/2 - 10, 15, 20));
        graphics_draw_bitmap_in_rect(ctx, grad_an_9, Circ); 
        Circ = GRect(bounds.origin.x + PBL_IF_ROUND_ELSE(25, 20), bounds.size.h/2 - 10, 15, 20);
        graphics_draw_text(ctx, "9", font_inter, Circ, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
        
        Circ = PBL_IF_ROUND_ELSE(GRect(bounds.size.w - 39, bounds.size.h/2 - 10, 15, 20), GRect(bounds.size.w - 34, bounds.size.h/2 - 10, 15, 20));
        graphics_draw_bitmap_in_rect(ctx,  grad_an_3, Circ);
        Circ = GRect(bounds.size.w - PBL_IF_ROUND_ELSE(39, 34), bounds.size.h/2 - 10, 15, 20);
        graphics_draw_text(ctx, "3", font_inter, Circ, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    }  
    
    //drawing battery text
    graphics_context_set_text_color(ctx, inner_color);
    graphics_context_set_fill_color(ctx, outer_color);
    
    snprintf(s_battery, sizeof(s_battery), "%d", battery_percent);  
    #ifdef PBL_ROUND
    if (flag_theme_gradient == 74) { // no gradient bg
      Circ = GRect(bounds.size.w/2 - 10, bounds.origin.y + 4, 20, 15);
      graphics_draw_text_with_background(ctx, s_battery, font_small, Circ, GRectZero );
    } else {
      Circ =  GRect(bounds.size.w/2 - 10, bounds.origin.y + 4, 20, 15);
      graphics_draw_bitmap_in_rect(ctx,  grad_an_batt_bg, Circ); 
      Circ = GRect(bounds.size.w/2 - 10, bounds.origin.y + 4, 20, 15);
      graphics_draw_text(ctx, s_battery, font_small, Circ, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    }
    #else
    if (flag_theme_gradient == 74) { // no gradient bg
       Circ = GRect(bounds.size.w/2 - 10, bounds.origin.y + 2, 20, 15);
       Circ2 = GRect(bounds.size.w/2 - 11, bounds.origin.y + 2, 22, 15);
       graphics_draw_text_with_background(ctx, s_battery, font_small, Circ, Circ2);       
    } else { // gradient bg
       Circ = GRect(bounds.size.w/2 - 11, bounds.origin.y + 2, 22, 15);
       graphics_draw_bitmap_in_rect(ctx,  grad_an_batt_bg, Circ);     
       Circ = GRect(bounds.size.w/2 - 10, bounds.origin.y + 2, 20, 15);
       graphics_draw_text(ctx, s_battery, font_small, Circ, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    }
    
    
    #endif
    
    // drawing bluetooth 
    if (bluetooth_sprite) {
      replace_gbitmap_color(GColorWhite, inner_color, bluetooth_sprite, NULL);
      #ifdef PBL_ROUND
      if (flag_theme_gradient == 74) { // no gradient bg
        Circ = GRect(bounds.size.w/2 - 3,bounds.size.h-18, 6, 13);
        Circ2 = GRect(bounds.size.w/2 - 10,bounds.size.h-18, 20, 17);
        graphics_draw_bitmap_with_background(ctx, bluetooth_sprite,  Circ, Circ2);
      } else {
        Circ = GRect(bounds.size.w/2 - 10,bounds.size.h-18, 20, 17);
        graphics_draw_bitmap_in_rect(ctx, grad_an_batt_bg,  Circ);     
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        Circ = GRect(bounds.size.w/2 - 3,bounds.size.h-18, 6, 13);
        graphics_draw_bitmap_in_rect(ctx, bluetooth_sprite,  Circ);
      }  
      #else
      if (flag_theme_gradient == 74) { // no gradient bg
        Circ = GRect(bounds.size.w/2 - 4,bounds.size.h-15, 6, 13);
        Circ2 = GRect(bounds.size.w/2 - 11,bounds.size.h-17, 22, 17);
        graphics_draw_bitmap_with_background(ctx, bluetooth_sprite,  Circ, Circ2);
      } else { // gradient bg
        Circ = GRect(bounds.size.w/2 - 11,bounds.size.h-17, 22, 17);
        graphics_draw_bitmap_in_rect(ctx, grad_an_batt_bg, Circ);     
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        Circ = GRect(bounds.size.w/2 - 4,bounds.size.h-15, 6, 13);
        graphics_draw_bitmap_in_rect(ctx, bluetooth_sprite,  Circ);
      }  
      #endif
    }  
   
    
  }   
  
  
  else  { // if (flag_main_clock == MAIN_CLOCK_DIGITAL) 
    
    // drawing digital time
   
    // building format 12h/24h
     if (clock_is_24h_style()) {
        strcpy(format, "%H:%M"); // e.g "14:46"
     } else {
        strcpy(format, "%l:%M"); // e.g " 2:46" -- with leading space
     }
     
     if (flag_time_separator == TIME_SEPARATOR_DOT) format[2] = '.';
     strftime(s_time, sizeof(s_time), format, t);
     
     // if time begin with space - eliminate it for even centering
     Circ = GRect(bounds.origin.x-2,bounds.size.h*83/200 - (bounds.size.h - u_bounds.size.h)/8 , bounds.size.w + 4, 70);
     if (s_time[0] == ' ') {
       graphics_draw_text(ctx, &s_time[1], font_large, Circ, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL); 
     } else {
       graphics_draw_text(ctx, s_time, font_large, Circ, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);    
     }
     
    //drawing battery text
    graphics_context_set_text_color(ctx, main_color);
    graphics_context_set_fill_color(ctx, middle_color);
    
   
    snprintf(s_battery, sizeof(s_battery), "%d", battery_percent);  
    #ifdef PBL_ROUND
    if (flag_theme_gradient == 74 ) { // no-gradient theme, solid bg
        Circ = GRect(bounds.size.w/2 - (battery_percent == 100? 16 : 14), bounds.origin.y + 23, (battery_percent == 100? 32 : 28), 15);
        graphics_draw_text_with_background(ctx, s_battery, font_inter_b, Circ, GRectZero);
    } else { // gradient bg
        Circ = GRect(bounds.size.w/2 - 16, bounds.origin.y + 23, 32, 15);
        graphics_draw_bitmap_in_rect(ctx, grad_dig_batt_bg, Circ);
        graphics_draw_text(ctx, s_battery, font_inter_b, Circ, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
      }   
    #else
    
      if (flag_theme_gradient == 74 ) { // no-gradient theme, solid bg
        Circ = GRect(bounds.size.w/2 - (battery_percent == 100? 16 : 14), bounds.origin.y + 17, (battery_percent == 100? 32 : 28), 15);
        Circ2 = GRect(bounds.size.w/2 - (battery_percent == 100? 16 : 14), bounds.origin.y + 18, (battery_percent == 100? 32 : 28), 13);
        graphics_draw_text_with_background(ctx, s_battery, font_inter_b, Circ, Circ2);
      } else { // gradient bg
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing gradieng bg bcuz flag_theme_gradient = %d", flag_theme_gradient);
        Circ =  GRect(bounds.size.w/2 - 16, bounds.origin.y + 18, 32, 13);
        graphics_draw_bitmap_in_rect(ctx, grad_dig_batt_bg, Circ);
        Circ = GRect(bounds.size.w/2 - 16, bounds.origin.y + 17, 32, 15);
        graphics_draw_text(ctx, s_battery, font_inter_b, Circ, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
      }  
    #endif
    

 
    // drawing bluetooth 
    if (bluetooth_sprite) {
       replace_gbitmap_color(GColorWhite, main_color, bluetooth_sprite, NULL);
       #ifdef PBL_ROUND
       if (flag_theme_gradient == 74 ) { // no-gradient theme, solid bg
           Circ =  GRect(bounds.size.w/2 - 3,bounds.size.h-39, 6, 13);
           Circ2 = GRect(bounds.size.w/2 - 14, bounds.size.h-38, 28, 14); 
           graphics_draw_bitmap_with_background(ctx, bluetooth_sprite, Circ, Circ2);
       } else {
           Circ = GRect(bounds.size.w/2 - 14, bounds.size.h-38, 28, 14);
           graphics_draw_bitmap_in_rect(ctx, grad_dig_bt_bg, Circ);
           graphics_context_set_compositing_mode(ctx, GCompOpSet);
           Circ =  GRect(bounds.size.w/2 - 3,bounds.size.h-39, 6, 13);
           graphics_draw_bitmap_in_rect(ctx, bluetooth_sprite, Circ);
       } 
       #else
        
          if (flag_theme_gradient == 74 ) { // no-gradient theme, solid bg
             Circ =  GRect(bounds.size.w/2 - 3,bounds.size.h-33, 6, 13);
             Circ2 = GRect(bounds.size.w/2 - 14, bounds.size.h-33, 28, 14);
             graphics_draw_bitmap_with_background(ctx, bluetooth_sprite, Circ, Circ2);
          } else  {//gradient bg  
             Circ = GRect(bounds.size.w/2 - 14, bounds.size.h-33, 28, 14);
             graphics_draw_bitmap_in_rect(ctx, grad_dig_bt_bg, Circ);
             graphics_context_set_compositing_mode(ctx, GCompOpSet);
             Circ = GRect(bounds.size.w/2 - 3,bounds.size.h-33, 6, 13);
             graphics_draw_bitmap_in_rect(ctx, bluetooth_sprite,  Circ);
          }  
      
       #endif 
    }  
    
   
    
  }
   
    
}


static void window_load(Window *window) {
  
  #ifdef PBL_COLOR //loading gradients only on color warches
  flag_theme_gradient = persist_exists(KEY_COLOR_THEME_GRADIENT)? persist_read_int(KEY_COLOR_THEME_GRADIENT) : 74; //"z" 122 - 48; no gradient
  if (flag_theme_gradient != 74) { // if it's a gradient theme - retreive bitmap 0, 1, 2... from GRADIENT array
      create_gradient_background(GRADIENTS[flag_theme_gradient]);
  }
  #else
  flag_theme_gradient = 74;
  #endif
  
  s_main_layer = layer_create(bounds);
  layer_set_update_proc(s_main_layer, main_layer_update);
  layer_add_child(window_layer, s_main_layer);
  
}

static void window_unload(Window *window) {
  
  layer_destroy(s_main_layer);
  
}

void handle_init(void) {
  
  //going international
  setlocale(LC_ALL, "");
  
  meteoicons_all = gbitmap_create_with_resource(RESOURCE_ID_METEOICONS);
  
  font_large = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_LIBERATION_SANS_BOLD_33, RESOURCE_ID_LIBERATION_SANS_BOLD_25)));
  font_medium = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LIBERATION_SANS_BOLD_14));
  font_inter = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LIBERATION_SANS_BOLD_16));
  font_inter_b = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LIBERATION_SANS_BOLD_15));
  font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LIBERATION_SANS_REGULAR_11));
  font_small_bold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LIBERATION_SANS_BOLD_13));
  
  
  //loading presets
  flag_main_clock = persist_exists(KEY_MAIN_CLOCK)? persist_read_int(KEY_MAIN_CLOCK) : MAIN_CLOCK_DIGITAL;
  flag_second_hand = persist_exists(KEY_SECOND_HAND)? persist_read_int(KEY_SECOND_HAND) : SECOND_HAND_DISABLED;
  flag_bluetooth_alert = persist_exists(KEY_BLUETOOTH_ALERT)? persist_read_int(KEY_BLUETOOTH_ALERT) : BLUETOOTH_ALERT_WEAK;
  flag_time_separator = persist_exists(KEY_TIME_SEPARATOR)? persist_read_int(KEY_TIME_SEPARATOR) : TIME_SEPARATOR_COLON;
  flag_locationService = persist_exists(KEY_LOCATION_SERVICE)? persist_read_int(KEY_LOCATION_SERVICE) : LOCATION_AUTOMATIC;
  flag_weatherInterval = persist_exists(KEY_WEATHER_INTERVAL)? persist_read_int(KEY_WEATHER_INTERVAL) : 60; // default weather update is 1 hour
  flag_temperatureFormat = persist_exists(KEY_TEMPERATURE_FORMAT)? persist_read_int(KEY_TEMPERATURE_FORMAT) : TEMPERATURE_FORMAT_FARENHEIT;
  flag_woeid = persist_exists(KEY_WOEID)? persist_read_int(KEY_WOEID) : 0;
  flag_secondary_info_type = persist_exists(KEY_SECONDARY_INFO_TYPE)? persist_read_int(KEY_SECONDARY_INFO_TYPE) : SECONDARY_INFO_WEATHER;
  
  flag_color_selection = persist_exists(KEY_COLOR_SELECTION)? persist_read_int(KEY_COLOR_SELECTION) : PBL_IF_COLOR_ELSE(COLOR_SELECTION_AUTOMATIC, COLOR_SELECTION_CUSTOM);
  
  outer_color = persist_exists(KEY_OUTER_COLOR)? GColorFromHEX(persist_read_int(KEY_OUTER_COLOR)) : GColorBlack;
  middle_color = persist_exists(KEY_MIDDLE_COLOR)? GColorFromHEX(persist_read_int(KEY_MIDDLE_COLOR)) : GColorLightGray;
  inner_color = persist_exists(KEY_INNER_COLOR)? GColorFromHEX(persist_read_int(KEY_INNER_COLOR)) : GColorWhite;
  main_color = persist_exists(KEY_MAIN_COLOR)? GColorFromHEX(persist_read_int(KEY_MAIN_COLOR)) : GColorBlack;
  
  flag_show_visual_health_goal = persist_exists(KEY_SHOW_VISUAL_HEALTH_GOAL)? persist_read_int(KEY_SHOW_VISUAL_HEALTH_GOAL) : 0;
  
  
  // if secondary info is set to health - init health services
  health_init(); // initializig health service
  if (flag_secondary_info_type != SECONDARY_INFO_WEATHER){
     update_health_info(); // showing health info for the first time
  }  
  
  s_window = window_create();
  window_layer = window_get_root_layer(s_window);
  bounds = layer_get_bounds(window_layer);
  
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  

  window_stack_push(s_window, true);

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
  
    // Open AppMessage
    app_message_open(500, 500); 
  
  
  battery_state_service_subscribe(battery_handler);
  battery_handler(battery_state_service_peek());
  
  is_bluetooth_buzz_enabled = false;
  bluetooth_connection_service_subscribe(bluetooth_handler);
  bluetooth_handler(bluetooth_connection_service_peek());
  is_bluetooth_buzz_enabled = true;  
  
  // showing previouslu saved weather without waiting for update
  if (flag_locationService != LOCATION_DISABLED){
    // reading stored value
    if (persist_exists(KEY_WEATHER_CODE)) set_weather_icon(persist_read_int(KEY_WEATHER_CODE));
    if (persist_exists(KEY_WEATHER_TEMP)) set_temperature(persist_read_int(KEY_WEATHER_TEMP));
  }
  
  change_time_tick_inteval();
  

}

void handle_deinit(void) {
  
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
  health_deinit();
  
  fonts_unload_custom_font(font_large);
  fonts_unload_custom_font(font_medium);
  fonts_unload_custom_font(font_inter);
  fonts_unload_custom_font(font_inter_b);
  fonts_unload_custom_font(font_small);
  fonts_unload_custom_font(font_small_bold);
  
  gbitmap_destroy(meteoicons_all);
  if (bluetooth_sprite)  gbitmap_destroy(bluetooth_sprite);
   
  window_destroy(s_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
