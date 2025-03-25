var Clay = require('./clay');
var clayConfig = require('./config');
var customClay = require('./custom-clay');
var clay; //constructor moved to JS Ready

var version = "1.54";

var xhrRequest = function (url, type, callback, errorCallback, timeout) {
  try {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(this.responseText);
    };
    xhr.open(type, url);
    if (timeout) {
      xhr.timeout = timeout;
    }
    xhr.send();
  }
  catch (ex) {
    if (errorCallback) {
      errorCallback(ex);
    }
  }
};


function getPlatform() {
  if (Pebble.getActiveWatchInfo) {
    return Pebble.getActiveWatchInfo().platform;
  }
  else {
    return "aplite";
  }
}



function onAppMessageReceived(appMsg) {
    //console.log ("\n++++ I am inside of 'Pebble.addEventListener('appmessage'): AppMessage received");
    
    // reading stored config data
    var claySettings = JSON.parse(localStorage.getItem('clay-settings'));
    
    //passing config (0 or 1; F or C) to weather
    forecastIoWeather.temperatureFormat = claySettings? parseInt(claySettings.KEY_TEMPERATURE_FORMAT) : 0;
    //console.log(forecastIoWeather.temperatureFormat);
  
    //passing Forecast IO API KEY
    forecastIoWeather.apiKey = claySettings? claySettings.KEY_FORECAST_IO_API : '';
    //console.log(forecastIoWeather.apiKey);
    
    
    if ((claySettings? parseInt(claySettings.KEY_LOCATION_SERVICE) : 0) == 1) { // for manual location - request weather right away
        forecastIoWeather.getWeather(claySettings.KEY_WOEID); //passing "lat,long" via woeid kwy
    } else 
    {
       forecastIoWeather.getLocation();  // for automatic location - get 
    }
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    
    // initiating Clay with custom functions & account info
    clay = new Clay(clayConfig, customClay, {userData: {appid: '1237976080', accounttoken: Pebble.getAccountToken()}});
        
    //console.log ("\n++++ I am inside of 'Pebble.addEventListener('ready'): PebbleKit JS ready!");
    var dictionary = {
      "KEY_JSREADY": 1
    };

    // Send to Pebble, so we can load units variable and send it back
    
        //console.log ("\n++++ I am inside of 'Pebble.addEventListener('ready') about to send Ready message to phone");
        Pebble.sendAppMessage(dictionary,
          function(e) {
            //console.log ("\n++++ I am inside of 'Pebble.sendAppMessage() callback: Ready notice sent to phone successfully!");
          },
          function(e) {
            //console.log ("\n++++ I am inside of 'Pebble.sendAppMessage() callback: Error ready notice to Pebble! = " +  JSON.stringify(e));
          }
        ); 
    
  }
);

