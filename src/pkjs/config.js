module.exports = [

  {
    "type": "heading",
    "id": "main-heading",
    "defaultValue": "Future Time Config",
    "size": 3
  },
  
  //***************** END TRIAAL, BUY NOW *****************
    {
    "type": "section",
    "items": [
  
        {
            "type": "toggle",
            "id": "KEY_BUY_NOW",  // To end trial earlier and initiate purchase righ away
            "appKey": "KEY_BUY_NOW",
            "defaultValue": 0,
            "label": "END TRIAL, BUY NOW",
            "description": "Toggle to end your trial initiate purchase. After saving config follow instructions on your watch"
        }
      
     ]
    },
  
  
  //***************** APPEARANCE *****************
  {
    "type": "section",
    "items": [
    {
      "type": "heading",
      "defaultValue": "Appearance"
    },
    {
      "type": "select",
      "id": "KEY_MAIN_CLOCK",        // Main Clock
      "appKey": "KEY_MAIN_CLOCK",
      "defaultValue": "1",
      "label": "Main Clock",
      "options": [
        { 
          "label": "Analog", 
          "value": "0" 
        },
        { 
          "label": "Digital",
          "value": "1" 
        },
      ]
    },
    {
      "type": "toggle",
      "id": "KEY_SECOND_HAND",  // Second Hand
      "appKey": "KEY_SECOND_HAND",
      "defaultValue": 0,
      "label": "Enable Second Hand"
    },
      {
      "type": "select",
      "id": "KEY_TIME_SEPARATOR",        // Digital time separator
      "appKey": "KEY_TIME_SEPARATOR",
      "defaultValue": "0",
      "label": "Time Separator",
      "options": [
        { 
          "label": "Colon", 
          "value": "0" 
        },
        { 
          "label": "Dot",
          "value": "1" 
        }
       ]
    }
   ]
  },
      

  //***************** Behavior *****************
  {
    "type": "section",
    "items": [
    {
      "type": "heading",
      "defaultValue": "Behavior"
    },
    
      {
      "type": "select",
      "id": "KEY_BLUETOOTH_ALERT",        // Bluetooth
      "appKey": "KEY_BLUETOOTH_ALERT",
      "defaultValue": "2",
      "label": "Bluetooth Alert",
      "options": [
        { 
          "label": "Disabled",
          "value": "0" 
        },
        { 
          "label": "Silent",
          "value": "1" 
        },
        { 
          "label": "Weak",
          "value": "2" 
        },
        { 
          "label": "Normal",
          "value": "3" 
        },
        { 
          "label": "Strong",
          "value": "4" 
        },
        { 
          "label": "Double",
          "value": "5" 
        }
      ]
    },
      
    {
      "type": "select",
      "id": "KEY_SECONDARY_INFO_TYPE",        // Secondary info
      "appKey": "KEY_SECONDARY_INFO_TYPE",
      "defaultValue": "0",
      "label": "Additional info",
      "options": [
        { 
          "label": "Weather",
          "value": "0" 
        },
        { 
          "label": "Step Counter",
          "value": "1" 
        },
        { 
          "label": "Distance, meters",
          "value": "2" 
        },
        { 
          "label": "Distance, yards",
          "value": "3" 
        },
        { 
          "label": "Sleep, hours",
          "value": "4" 
        },
        { 
          "label": "Deep Sleep, hours",
          "value": "5" 
        }
      ]
    },  
      
    {
      "type": "toggle",
      "id": "KEY_SHOW_VISUAL_HEALTH_GOAL",
      "appKey": "KEY_SHOW_VISUAL_HEALTH_GOAL",
      "defaultValue": 0,
      "label": "Show fitness goals",
      "description": "When ON inner line shows graphical step goal"
    }  

  ]
},    
      
    
 //***************** COLORS *****************
 {     
  "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Colors"
    },
    {
      "type": "select",
      "id": "KEY_COLOR_SELECTION",        // Color selection: custom/automatic
      "appKey": "KEY_COLOR_SELECTION",
      "defaultValue": "0",
      "label": "Watchface Colors",
      "options": [
        { 
          "label": "Automatic", 
          "value": "0" 
        },
        { 
          "label": "Custom",
          "value": "1" 
        }
      ]
    },
        
   {
    "type": "color",                    // custom colors
    "appKey": "KEY_OUTER_COLOR",
    "id": "KEY_OUTER_COLOR",   
    "defaultValue": "005500",
    "label": "Outer Color",
    "sunlight": false
  }, 
  {
    "type": "color",                    
    "appKey": "KEY_MIDDLE_COLOR",
    "id": "KEY_MIDDLE_COLOR",   
    "defaultValue": "00aa55",
    "label": "Middle Color",
    "sunlight": false
  },
  {
    "type": "color",                    
    "appKey": "KEY_INNER_COLOR",
    "id": "KEY_INNER_COLOR",   
    "defaultValue": "55ff55",
    "label": "Inner Color",
    "sunlight": false
  },
  {
    "type": "color",                    
    "appKey": "KEY_MAIN_COLOR",
    "id": "KEY_MAIN_COLOR",   
    "defaultValue": "000000",
    "label": "Text/icons Color",
    "sunlight": false
  }
]
},
  
  
{     
  "type": "section",
  "items": [
    {
      "type": "heading",
      "id": "colorThemesHeader",
      "defaultValue": "Color Themes"
    },  
    
    {
      "type": "select",
      "id": "KEY_COLOR_THEME",        // Predefined color theme
      "appKey": "KEY_COLOR_THEME",
      "defaultValue": " ",
      "label": "Color Theme",
      "options": [
        { 
          "label": "-select theme-", 
          "value": " " 
        },
        { 
          "label": "GREEN",
          "value": "005500,00aa55,55ff55,000000,z"  // z = not a gradient theme
        },
        { 
          "label": "RED",
          "value": "550000,aa0000,ff5555,000000,z" 
        },
        { 
          "label": "BLUE",
          "value": "000055,0055ff,00aaff,000000,z" 
        },
        { 
          "label": "YELLOW",
          "value": "555500,aaaa00,ffff00,000000,z" 
        },
        { 
          "label": "PINK",
          "value": "aa0055,ff00aa,ffaaff,000000,z" 
        },
        { 
          "label": "TURQUOISE",
          "value": "005555,00AAAA,AAFFFF,000000,z" 
        },        
        { 
          "label": "GREY",
          "value": "000000,AAAAAA,FFFFFF,000000,z" 
        },
        { 
          "label": "BLUE-WHITE",
          "value": "000055,55AAFF,FFFFFF,000000,z" 
        },
        { 
          "label": "BLACK-RED-WHITE",
          "value": "000000,AA0000,FFFFFF,000000,z" 
        },
        { 
          "label": "GREEN-YELLOW",
          "value": "005500,00AA55,FFFF55,000000,z" 
        },
        { 
          "label": "PURPLE-GREEN-YELLOW",
          "value": "550055,00AA55,FFFF55,000000,z" 
        },
        { 
          "label": "BLUE-ORANGE-YELLOW",
          "value": "0055AA,FF5500,FFFF55,000000,z" 
        },
        { 
          "label": "PINK-BLUE-YELLOW",
          "value": "AA0055,55AAFF,FFFF55,000000,z" 
        },
        { 
          "label": "PINK-ORANGE-WHITE",
          "value": "AA0055,FFAA00,FFFFFF,000000,z" 
        },
        { 
          "label": "PURPLE-PINK-WHITE",
          "value": "550055,AA0055,FFFFFF,000000,z" 
        },
        { 
          "label": "BLUE-RED-WHITE",
          "value": "000055,AA0000,FFFFFF,000000,z" 
        },
         { 
          "label": "RED-DARKYELLOW-YELLOW",
          "value": "AA0000,AAAA00,FFFFAA,000000,z" 
        },
         { 
          "label": "BLACK-GREEN-WHITE",
          "value": "000000,00AA55,FFFFFF,000000,z" 
        },
         { 
          "label": "GREEN-ORANGE-YELLOW",
          "value": "005500,FF5500,FFFF55,000000,z" 
        },
         { 
          "label": "BLACK-TURQUOISE-CYAN",
          "value": "000000,55AAAA,55FFFF,000000,z" 
        },
        
        { 
          "label": "GREEN GRADIENT",
          "value": "005500,00aa55,55ff55,000000,0" // last digit - gradient theme number
        },

        { 
          "label": "RED GRADIENT",
          "value": "550000,aa0000,ff5555,000000,1" 
        },
                
        { 
          "label": "GREY GRADIENT",
          "value": "000000,AAAAAA,FFFFFF,000000,2" 
        },
        
        { 
          "label": "PINK GRADIENT",
          "value": "aa0055,ff00aa,ffaaff,000000,3" 
        },
        
        { 
          "label": "YELLOW GRADIENT",
          "value": "555500,aaaa00,ffff00,000000,4" 
        },
        
        { 
          "label": "BLUE GRADIENT",
          "value": "000055,0055ff,00aaff,000000,5" 
        },
        { 
          "label": "BLACK-RED-WHITE GRADIENT",
          "value": "000000,AA0000,FFFFFF,000000,6" 
        },
        { 
          "label": "GREEN-YELLOW GRADIENT",
          "value": "005500,00AA55,FFFF55,000000,7" 
        },
         { 
          "label": "PINK-BLUE-YELLOW GRADIENT",
          "value": "AA0055,55AAFF,FFFF55,000000,8" 
        },
        { 
          "label": "BLACK-GREEN-WHITE GRADIENT",
          "value": "000000,00AA55,FFFFFF,000000,9" 
        },
        { 
          "label": "BLUE-RED-WHITE GRADIENT",
          "value": "000055,AA0000,FFFFFF,000000,:" 
        },
        { 
          "label": "BLUE-WHITE GRADIENT",
          "value": "000055,55AAFF,FFFFFF,000000,;" 
        },
         { 
          "label": "RED-DARKYELLOW-WHITE GRADIENT",
          "value": "AA0000,AAAA00,FFFFAA,000000,<" 
        }
        
      ]
    },
        
    {
      "type": "input",
      "id": "KEY_COLOR_THEME_GRADIENT",        // hidden, stores Gradient theme number
      "appKey": "KEY_COLOR_THEME_GRADIENT",
      "defaultValue": "z", // dec 122 - no gradient theme
      "attributes": {
        "style": "display:none"
      }
   }    
  
  ]
},
  
  

  //***************** WEATHER *****************
 {     
  "type": "section",
  "items": [
    {
      "type": "heading",
      "id": "weatherHeading",
      "defaultValue": "Weather"
    },
    
    {
      "type": "input",                    //forecast.io API KEY
      "appKey": "KEY_FORECAST_IO_API",
      "id":  "KEY_FORECAST_IO_API",
      "defaultValue": "",
      "label": "Forecast.io API KEY",
      "description": "Weather powered by forecast.io. Register at https://developer.forecast.io/ to receive API Key. This is a one time procedure, no billing information required there."
    },
    
   {
      "type": "select",
      "id": "KEY_WEATHER_INTERVAL",        //Weather update interval
      "appKey": "KEY_WEATHER_INTERVAL",
      "defaultValue": "P",
      "label": "Update Interval",
      "options": [
        { 
          "label": "15 min", 
          "value": "#" // #35 (- 20) = 15 
        },
        { 
          "label": "30 min", 
          "value": "2" // #50 (- 20) =  30
        },
        { 
          "label": "1 hour",
          "value": "P" // #80 (- 20) = 60
        }
      ]
    },
      
    {
      "type": "select",
      "id": "KEY_TEMPERATURE_FORMAT",        //Temperature format
      "appKey": "KEY_TEMPERATURE_FORMAT",
      "defaultValue": "0",
      "label": "Temperature format",
      "options": [
        { 
          "label": "Fahrenheit", 
          "value": "0" 
        },
        { 
          "label": "Celsius", 
          "value": "1"
        }
      ]
    },
      
    {
      "type": "select",
      "id": "KEY_LOCATION_SERVICE",        //weather location: automatic or manual
      "appKey": "KEY_LOCATION_SERVICE",
      "defaultValue": "0",
      "label": "Location",
      "options": [
        { 
          "label": "Automatic", 
          "value": "0" 
        },
        { 
          "label": "Manual", 
          "value": "1"
        }
      ]
    },  
    
    {
      "type": "input",
      "appKey": "KEY_WOEID",
      "id":  "KEY_WOEID",
      "defaultValue": "",
      "label": "Location coordinates: latitude,longitude",
      "description": "You can lookup coordinates using form below"
    },
      
    {
      "type": "text",
      "id":  "woeidLookupResult", 
      "defaultValue": "",
    },
      
    {
      "type": "input",
      "id":  "woeidLookupName", 
      "defaultValue": "",
      "label": "Location Name",
      "description": "Enter town or city name"
    },  
      
      
      {
        "type": "button",
        "id":  "woeidLookupButton",
        "defaultValue": "Coordinates Lookup"
      }


      ]
    },
    

      
{
  "type": "submit",
  "defaultValue": "Save"
}    

];