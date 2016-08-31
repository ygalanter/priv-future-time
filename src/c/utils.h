#include <pebble.h>
#pragma once

// draws text with currently set text color & fill color
void graphics_draw_text_with_background(GContext *ctx, char *text, GFont font, GRect text_box, GRect bg_box);

// draws bitmap with currently set fill color as background
void graphics_draw_bitmap_with_background(GContext *ctx, GBitmap *bitmap, GRect bitmap_box,  GRect bg_box);

// converts string to uppercase
void uppercase(char *sPtr);

//sets battery color indicator according to percentage
void change_battery_color(int8_t battery_percentage);