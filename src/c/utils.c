#include <pebble.h>
#include "utils.h"
#include "ctype.h"

// draws text with currently set text color & fill color
void graphics_draw_text_with_background(GContext *ctx, char *text, GFont font, GRect text_box, GRect bg_box ) {

  if (bg_box.origin.x == 0 && bg_box.origin.y == 0 && bg_box.size.w == 0 && bg_box.size.h == 0){ //GRectZero - use text bpx
    graphics_fill_rect(ctx, text_box, 0, GCornerNone);
  } else {
    graphics_fill_rect(ctx, bg_box, 0, GCornerNone);
  }
  
  graphics_draw_text(ctx, text, font, text_box, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  
}


// draws bitmap with currently set fill color as background
void graphics_draw_bitmap_with_background(GContext *ctx, GBitmap *bitmap, GRect bitmap_box, GRect bg_box) {
  
  if (bg_box.origin.x == 0 && bg_box.origin.y == 0 && bg_box.size.w == 0 && bg_box.size.h == 0){ //GRectZero - use bitmap bpx
    graphics_fill_rect(ctx, bitmap_box, 0, GCornerNone);
  } else {
    graphics_fill_rect(ctx, bg_box, 0, GCornerNone);
  }

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, bitmap, bitmap_box);
}

// converts string to uppercase
void uppercase(char *sPtr) {  
    while(*sPtr != '\0') {
         *sPtr = toupper ( ( unsigned char ) *sPtr );
         ++sPtr;
    }
}


//sets battery color indicator according to percentage
extern GColor outer_color, middle_color, inner_color, main_color;

void change_battery_color(int8_t battery_percentage) {
#ifdef PBL_COLOR
  
    main_color = GColorBlack;

    switch (battery_percentage) {
       case 100: 
       case 90: 
       case 80: 
       case 70: 
       case 60: 
       case 50: outer_color = GColorFromHEX(0x005500); middle_color = GColorFromHEX(0x00aa55); inner_color = GColorFromHEX(0x55ff55); break;
       case 40: 
       case 30: 
       case 20: outer_color = GColorFromHEX(0x555500); middle_color = GColorFromHEX(0xaaaa00); inner_color = GColorFromHEX(0xffff00); break;
       case 10: 
       case 0:  outer_color = GColorFromHEX(0x550000); middle_color = GColorFromHEX(0xaa0000); inner_color = GColorFromHEX(0xff5555); break;
   }

#endif
}


