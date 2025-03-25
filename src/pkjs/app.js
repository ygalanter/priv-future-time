var Clay = require('./clay');
var clayConfig = require('./config');
var customClay = require('./custom-clay');
var clay; //constructor moved to JS Ready

var version = "1.55";

// var YahooWeather = require('YahooWeather.js');
// var yahooWeather = new YahooWeather();
var OpenMetroWeather = require('./OpenMetroWeather.js');
var openMetroWeather = new OpenMetroWeather();


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

Pebble.addEventListener("appmessage", onAppMessageReceived);

function onAppMessageReceived(appMsg) {
    //console.log ("\n++++ I am inside of 'Pebble.addEventListener('appmessage'): AppMessage received");
    
    // reading stored config data
    var claySettings = JSON.parse(localStorage.getItem('clay-settings'));
    
    //passing config (0 or 1; F or C) to weather
    openMetroWeather.temperatureFormat = claySettings? parseInt(claySettings.KEY_TEMPERATURE_FORMAT) : 0;
    //console.log(forecastIoWeather.temperatureFormat);
    
    openMetroWeather.getLocation();  // for automatic location - get 
    
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

