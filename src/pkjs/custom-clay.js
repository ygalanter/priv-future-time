var Clay;
module.exports = function(minified) {
  Clay = this;
  var _ = minified._;
  var $ = minified.$;
  var HTML = minified.HTML;

  // clock appearance
  function toggleClockAppearance() {
    if (this.get() == '0') { // Analog
      Clay.getItemByAppKey('KEY_SECOND_HAND').show();
      Clay.getItemByAppKey('KEY_TIME_SEPARATOR').hide();
    } else { // digital
      Clay.getItemByAppKey('KEY_SECOND_HAND').hide();
      Clay.getItemByAppKey('KEY_SECOND_HAND').set(0);
      Clay.getItemByAppKey('KEY_TIME_SEPARATOR').show();
    }
  }
  
  // colors
  function toggleColors() {
    if (this.get() == '0') { // Automatic
      Clay.getItemByAppKey('KEY_OUTER_COLOR').hide();
      Clay.getItemByAppKey('KEY_MIDDLE_COLOR').hide();
      Clay.getItemByAppKey('KEY_INNER_COLOR').hide();
      Clay.getItemByAppKey('KEY_MAIN_COLOR').hide();
      Clay.getItemByAppKey('KEY_COLOR_THEME').hide();
      Clay.getItemById('colorThemesHeader').hide();
      
    } else { // custom
      Clay.getItemByAppKey('KEY_OUTER_COLOR').show();
      Clay.getItemByAppKey('KEY_MIDDLE_COLOR').show();
      Clay.getItemByAppKey('KEY_INNER_COLOR').show();
      Clay.getItemByAppKey('KEY_MAIN_COLOR').show();
      Clay.getItemByAppKey('KEY_COLOR_THEME').show();
      Clay.getItemById('colorThemesHeader').show();
    }
  }
  
  // color themes
  var need_to_reset_themes = true; // whether need to reset predefined theme selection
  function toggleColorTheme() {
    if (this.get() != ' ') { // theme is set
      
      var colors = this.get().split(","); // getting colors from SELECT value
      
      need_to_reset_themes = false; // on automatic color set - don't reset theme
      Clay.getItemByAppKey('KEY_OUTER_COLOR').set(colors[0]);
      Clay.getItemByAppKey('KEY_MIDDLE_COLOR').set(colors[1]);
      Clay.getItemByAppKey('KEY_INNER_COLOR').set(colors[2]);
      Clay.getItemByAppKey('KEY_MAIN_COLOR').set(colors[3]);
      
      Clay.getItemByAppKey('KEY_COLOR_THEME_GRADIENT').set(colors[4]); // also setting gradient theme #
      
      need_to_reset_themes = true;
      
    }
  }
  
  function resetColorTheme() {
    if (need_to_reset_themes) Clay.getItemByAppKey('KEY_COLOR_THEME').set(' ');
  }
  
  
  //location coords lookup
  function woeidLookup() {
    
    $.request('get', 'http://nominatim.openstreetmap.org/search?q=' + Clay.getItemById('woeidLookupName').get() + '&format=json').then(function (str_data) {
       var result;
       var resultText='';
       var data = JSON.parse(str_data);
      
       //console.log(data);

       if (data.length === 0) { // no results
          resultText = '<div>No Results</div>';
       } else { //looping thru results
          resultText = '<div><i>Tap location below to get coordinates</i></div><hr/>';

         for (var i = 0; i < data.length; i++) {
            result = data[i];
            resultText += '<div onclick = "Clay.getItemById(\'KEY_WOEID\').set(\'' + result.lat + ',' + result.lon + '\');">' +
                 (result.display_name.length > 37 ? result.display_name.substring(0, 34) + '...' : result.display_name) +
            '</div>';
         }
       }
      
       Clay.getItemById('woeidLookupResult').set(resultText);
       Clay.getItemById('woeidLookupName').set('');
      
       });
  
  }
  
  //weather location
  function changeWeatherLocation() {
    if (this.get() == '0') { // Automatic
      Clay.getItemById('KEY_WOEID').hide();
      Clay.getItemById('woeidLookupResult').hide();
      Clay.getItemById('woeidLookupName').hide();
      Clay.getItemById('woeidLookupButton').hide();
      
      Clay.getItemById('KEY_WOEID').$manipulatorTarget.set('required', '');
    } else { // Manual
      Clay.getItemById('KEY_WOEID').show();
      Clay.getItemById('woeidLookupResult').show();
      Clay.getItemById('woeidLookupName').show();
      Clay.getItemById('woeidLookupButton').show();
      
      Clay.getItemById('KEY_WOEID').$manipulatorTarget.set('required', 'required');
     }
  }
  
  
  // hides weather section if weather is not selected
  function toggleSecondaryInfo() {
    if (this.get() == "0") { // weather is selected
      Clay.getItemById('weatherHeading').show();
      
      Clay.getItemByAppKey('KEY_LOCATION_SERVICE').show();
      Clay.getItemByAppKey('KEY_FORECAST_IO_API').show();
      
      Clay.getItemByAppKey('KEY_TEMPERATURE_FORMAT').show();
      Clay.getItemByAppKey('KEY_WEATHER_INTERVAL').show();
      
      if (Clay.getItemByAppKey('KEY_LOCATION_SERVICE').get() == "1") { // if we're using custom weather - show custom stuff too
         Clay.getItemByAppKey('KEY_WOEID').show();
         Clay.getItemById('woeidLookupResult').show();
         Clay.getItemById('woeidLookupName').show();
         Clay.getItemById('woeidLookupButton').show();
      }
      
    } else { // weather is not selected
      Clay.getItemById('weatherHeading').hide();
      Clay.getItemByAppKey('KEY_WEATHER_INTERVAL').hide();
      Clay.getItemByAppKey('KEY_TEMPERATURE_FORMAT').hide();
      
      Clay.getItemByAppKey('KEY_LOCATION_SERVICE').hide();
      Clay.getItemByAppKey('KEY_FORECAST_IO_API').hide();
      
      if (Clay.getItemByAppKey('KEY_LOCATION_SERVICE').get() == "1") { // if we're using custom weather - hide custom stuff too
         Clay.getItemByAppKey('KEY_WOEID').hide();
         Clay.getItemById('woeidLookupResult').hide();
         Clay.getItemById('woeidLookupName').hide();
         Clay.getItemById('woeidLookupButton').hide();
      }
      
    }
  }
  
  
  
  //on Aplite and Diorite hide unavailable colors
  Clay.on(Clay.EVENTS.BEFORE_BUILD, function() {
  if(!Clay.meta.activeWatchInfo || Clay.meta.activeWatchInfo.platform === 'aplite' || Clay.meta.activeWatchInfo.platform === 'diorite') {
      Clay.config[4].items[2].layout = [['ffffff', 'aaaaaa', '000000']];
      Clay.config[4].items[3].layout = [['ffffff', 'aaaaaa', '000000']];
      Clay.config[4].items[4].layout = [['ffffff', 'aaaaaa', '000000']];
      Clay.config[4].items[5].layout = [['ffffff', 'aaaaaa', '000000']];
    
      Clay.config[4].items[2].defaultValue = '000000';
      Clay.config[4].items[3].defaultValue = 'aaaaaa';
      Clay.config[4].items[4].defaultValue = 'ffffff';
      Clay.config[4].items[5].defaultValue = '000000';
    }
  });
  
  

  Clay.on(Clay.EVENTS.AFTER_BUILD, function() {
    
   // initially hiding BUY NOW 
   var buynow = Clay.getItemByAppKey('KEY_BUY_NOW');
   buynow.set(0); 
   buynow.disable();
   buynow.hide(); 

    
    //detecting whether "BUY NOW" section should be shown
    $.request('get', "https://kiezelpay.com/api/v1/status?appid=" + Clay.meta.userData.appid + "&accounttoken=" + Clay.meta.userData.accounttoken).then(function (str_data) {
      
       console.log(str_data);
      
        var data = JSON.parse(str_data);
      
        // if user is unlicensed - show BUY NOW
        if (data.status != "licensed") { 
            buynow.enable();
            buynow.show(); 
        }
      
    });
    
    // clock appearance
    var clockAppearance = Clay.getItemByAppKey('KEY_MAIN_CLOCK');
    toggleClockAppearance.call(clockAppearance);
    clockAppearance.on('change', toggleClockAppearance);
    
    
    // colors
    var colorSelection = Clay.getItemByAppKey('KEY_COLOR_SELECTION');
    
    // on aplite or diorite color selection hardwired to "custom"
    if(!Clay.meta.activeWatchInfo || Clay.meta.activeWatchInfo.platform === 'aplite' || Clay.meta.activeWatchInfo.platform === 'diorite') {
        colorSelection.set("1");
        colorSelection.hide();
    }
    
    toggleColors.call(colorSelection);
    colorSelection.on('change', toggleColors);
    
    
    // colorThemes
    var colorThemeSelection = Clay.getItemByAppKey('KEY_COLOR_THEME');
    
     //on aplite or Diorite there's no color themes
     if(!Clay.meta.activeWatchInfo || Clay.meta.activeWatchInfo.platform === 'aplite' || Clay.meta.activeWatchInfo.platform === 'diorite') {
        colorThemeSelection.hide();
        Clay.getItemById('colorThemesHeader').hide();
     }
    
    toggleColorTheme.call(colorThemeSelection);
    colorThemeSelection.on('change', toggleColorTheme);
    
    // resetting theme on custom color select
    Clay.getItemByAppKey('KEY_OUTER_COLOR').on('change', resetColorTheme);
    Clay.getItemByAppKey('KEY_MIDDLE_COLOR').on('change', resetColorTheme);
    Clay.getItemByAppKey('KEY_INNER_COLOR').on('change', resetColorTheme);
    Clay.getItemByAppKey('KEY_MAIN_COLOR').on('change', resetColorTheme); 
    
    
    // weather location service
    var weatherLocation = Clay.getItemByAppKey('KEY_LOCATION_SERVICE');
    changeWeatherLocation.call(weatherLocation);
    weatherLocation.on('change', changeWeatherLocation);
    
    // weather secondary info
    var secondaryInfo = Clay.getItemByAppKey('KEY_SECONDARY_INFO_TYPE');
    
    //on aplite there's no health so we're hardwired to weather
    if(!Clay.meta.activeWatchInfo || Clay.meta.activeWatchInfo.platform === 'aplite') {
      secondaryInfo.hide();
    }
    
    toggleSecondaryInfo.call(secondaryInfo);
    secondaryInfo.on('change', toggleSecondaryInfo);
    
    // on clicking WOEID lookup button - perform WOEID lookup
    Clay.getItemById('woeidLookupButton').on('click', woeidLookup);
    
    // on aplite hide visual health goal
    if(!Clay.meta.activeWatchInfo || Clay.meta.activeWatchInfo.platform === 'aplite') {
         Clay.getItemByAppKey('KEY_SHOW_VISUAL_HEALTH_GOAL').set(0);
         Clay.getItemByAppKey('KEY_SHOW_VISUAL_HEALTH_GOAL').hide();
    }
    

  });
  


};