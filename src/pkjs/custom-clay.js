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
  
  
  
  
  
  // hides weather section if weather is not selected
  function toggleSecondaryInfo() {
    if (this.get() == "0") { // weather is selected
      Clay.getItemById('weatherHeading').show();      
      Clay.getItemByAppKey('KEY_TEMPERATURE_FORMAT').show();

    } else { // weather is not selected
      Clay.getItemById('weatherHeading').hide();
      Clay.getItemByAppKey('KEY_TEMPERATURE_FORMAT').hide();
      
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
    

    
    // weather secondary info
    var secondaryInfo = Clay.getItemByAppKey('KEY_SECONDARY_INFO_TYPE');
    
    //on aplite there's no health so we're hardwired to weather
    if(!Clay.meta.activeWatchInfo || Clay.meta.activeWatchInfo.platform === 'aplite') {
      secondaryInfo.hide();
    }
    
    toggleSecondaryInfo.call(secondaryInfo);
    secondaryInfo.on('change', toggleSecondaryInfo);
    
    // on aplite hide visual health goal
    if(!Clay.meta.activeWatchInfo || Clay.meta.activeWatchInfo.platform === 'aplite') {
         Clay.getItemByAppKey('KEY_SHOW_VISUAL_HEALTH_GOAL').set(0);
         Clay.getItemByAppKey('KEY_SHOW_VISUAL_HEALTH_GOAL').hide();
    }
    

  });
  


};