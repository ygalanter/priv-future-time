#include <pebble.h>
#pragma once

#define KEY_JSREADY 6

#define KEY_MAIN_CLOCK 0

#define MAIN_CLOCK_ANALOG 48  // because clay is passing "0" from select
#define MAIN_CLOCK_DIGITAL 49 // because clay is passing "1"  from select

#define KEY_SECOND_HAND 1

#define SECOND_HAND_DISABLED 0
#define SECOND_HAND_ENABLED 1

#define KEY_TIME_SEPARATOR 12

#define TIME_SEPARATOR_COLON 48 // because clay is passing "0" from select
#define TIME_SEPARATOR_DOT 49   // because clay is passing "1"  from select

#define KEY_BLUETOOTH_ALERT 2

#define BLUETOOTH_ALERT_DISABLED 48 // because clay is passing "0" from select
#define BLUETOOTH_ALERT_SILENT 49
#define BLUETOOTH_ALERT_WEAK 50
#define BLUETOOTH_ALERT_NORMAL 51
#define BLUETOOTH_ALERT_STRONG 52
#define BLUETOOTH_ALERT_DOUBLE 53

#define KEY_COLOR_SELECTION 15

#define COLOR_SELECTION_AUTOMATIC 48 // because clay is passing "0" from select
#define COLOR_SELECTION_CUSTOM 49    // because clay is passing "1"  from select

#define KEY_OUTER_COLOR 16
#define KEY_MIDDLE_COLOR 17
#define KEY_INNER_COLOR 18
#define KEY_MAIN_COLOR 19

#define KEY_WEATHER_INTERVAL 5
#define KEY_WEATHER_CODE 3
#define KEY_WEATHER_TEMP 4
#define KEY_CITY_NAME 9
#define KEY_TEMPERATURE_FORMAT 8

#define TEMPERATURE_FORMAT_FARENHEIT 48 // because clay is passing "0" from select
#define TEMPERATURE_FORMAT_CELCIUS 49   // because clay is passing "1"  from select

#define KEY_LOCATION_SERVICE 7

#define LOCATION_AUTOMATIC 48 // because clay is passing "0" from select
#define LOCATION_MANUAL 49    // because clay is passing "1"  from select
#define LOCATION_DISABLED 50

#define KEY_WOEID 998

#define KEY_SECONDARY_INFO_TYPE 10
#define SECONDARY_INFO_WEATHER 48
#define SECONDARY_INFO_DISTANCE_STEPS 49
#define SECONDARY_INFO_DISTANCE_METERS 50
#define SECONDARY_INFO_DISTANCE_YARDS 51
#define SECONDARY_INFO_SLEEP 52
#define SECONDARY_INFO_DEEP_SLEEP 53

#define ICON_WIDTH 21
#define ICON_HEIGHT 17

#define KEY_SHOW_VISUAL_HEALTH_GOAL 20

#define KEY_COLOR_THEME 999

#ifdef PBL_COLOR // only using gradients on color watches7

#define KEY_COLOR_THEME_GRADIENT 1001

int GRADIENTS[] = {
    RESOURCE_ID_GRADIENT_GREEN, RESOURCE_ID_GRADIENT_RED, RESOURCE_ID_GRADIENT_GREY, RESOURCE_ID_GRADIENT_PINK, RESOURCE_ID_GRADIENT_YELLOW, RESOURCE_ID_GRADIENT_BLUE,
    RESOURCE_ID_GRADIENT_BLACK_RED_GREY, RESOURCE_ID_GRADIENT_GREEN_YELLOW, RESOURCE_ID_GRADIENT_PINK_BLUE_YELLOW, RESOURCE_ID_GRADIENT_BLACK_GREEN_GREY,
    RESOURCE_ID_GRADIENT_BLUE_RED_GREY, RESOURCE_ID_GRADIENT_BLUE_GREY, RESOURCE_ID_GRADIENT_RED_YELLOW_GREY};

#endif

// bluetooth vibe patterns
const VibePattern VIBE_PATTERN_WEAK = {
    .durations = (uint32_t[]){100},
    .num_segments = 1};

const VibePattern VIBE_PATTERN_NORMAL = {
    .durations = (uint32_t[]){300},
    .num_segments = 1};

const VibePattern VIBE_PATTERN_STRONG = {
    .durations = (uint32_t[]){500},
    .num_segments = 1};

const VibePattern VIBE_PATTERN_DOUBLE = {
    .durations = (uint32_t[]){500, 100, 500},
    .num_segments = 3};
