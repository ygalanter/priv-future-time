var Clay = require('./clay');
var clayConfig = require('./config');
var customClay = require('./custom-clay');
var clay; //constructor moved to JS Ready

var version = "1.53";

// var YahooWeather = require('YahooWeather.js');
// var yahooWeather = new YahooWeather();
var ForecastIoWeather = require('./ForecastIoWeather.js');
var forecastIoWeather = new ForecastIoWeather();


/* === KIEZELPAY === SET TO false BEFORE RELEASING === */
var KIEZELPAY_LOGGING = false;
/* === KIEZELPAY === SET TO false BEFORE RELEASING === */


/**********************************************************************/
/* === KIEZELPAY === GENERATED CODE BEGIN === DO NOT MODIFY BELOW === */
/**********************************************************************/
var kiezelPayAppMessageHandler = null;

function kiezelPayInit(appMessageHandler) {
  kiezelPayLog("kiezelPayInit() called");
  kiezelPayAppMessageHandler = appMessageHandler;
  
  var msg = {
    "KIEZELPAY_READY": 1
  };
  Pebble.sendAppMessage(msg, 
                        function(e) {
                          kiezelPayLog("KiezelPay Ready msg successfully sent.");
                        },
                        function(e) {
                          kiezelPayLog("KiezelPay Ready msg failed.");
                        });
}

function kiezelPayLog(msg) {
  if (KIEZELPAY_LOGGING) {
    console.log(msg);
  }
}

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

function kiezelPayOnAppMessage(appmsg) {
  if (appmsg && appmsg.payload && 
      appmsg.payload.hasOwnProperty('KIEZELPAY_STATUS_CHECK') && 
      appmsg.payload.KIEZELPAY_STATUS_CHECK > 0) {
    //its a status check message, handle it
    var devicetoken = appmsg.payload.KIEZELPAY_DEVICE_TOKEN;
    var appId = appmsg.payload.KIEZELPAY_APP_ID;
    var random = appmsg.payload.KIEZELPAY_RANDOM;
    var accounttoken = Pebble.getAccountToken();
    var platform = getPlatform();
    
    //build url
    var kiezelpayStatusUrl = 'https://www.kiezelpay.com/api/v1/status?';
    kiezelpayStatusUrl += 'appid=' + encodeURIComponent(appId);
    kiezelpayStatusUrl += '&devicetoken=' + encodeURIComponent(devicetoken);
    kiezelpayStatusUrl += '&rand=' + encodeURIComponent(random);
    kiezelpayStatusUrl += '&accounttoken=' + encodeURIComponent(accounttoken);
    kiezelpayStatusUrl += '&platform=' + encodeURIComponent(platform);
    kiezelpayStatusUrl += '&flags=' + encodeURIComponent(appmsg.payload.KIEZELPAY_STATUS_CHECK);
    kiezelpayStatusUrl += '&nocache=' + encodeURIComponent(Math.round(new Date().getTime()));
    kiezelPayLog(kiezelpayStatusUrl);
    
    //perform the request
    xhrRequest(kiezelpayStatusUrl, "GET", 
      function(responseText) {
        kiezelPayLog("KiezelPay status response: " + responseText);
        var response = JSON.parse(responseText);
        var status = 0;
        var trialDuration = 0;
        var paymentCode = 0;
        var purchaseStatus = 0;
        if (response.status === 'unlicensed') {
          status = 0;
          paymentCode = Number(response.paymentCode);
          if (response.purchaseStatus == 'waitForUser') {
            purchaseStatus = 0;
          }
          else if (response.purchaseStatus == 'inProgress') {
            purchaseStatus = 1;
          }
        } else if (response.status == 'trial') {
          status = 1;
          trialDuration = Number(response.trialDurationInSeconds);
        } else if (response.status == 'licensed') {
          status = 2;
        }

        var msg = {
          KIEZELPAY_STATUS_RESULT: status,
          KIEZELPAY_STATUS_TRIAL_DURATION: trialDuration,
          KIEZELPAY_PURCHASE_CODE: paymentCode,
          KIEZELPAY_PURCHASE_STATUS: purchaseStatus,
          KIEZELPAY_STATUS_VALIDITY_PERIOD: response.validityPeriodInDays,
          KIEZELPAY_STATUS_CHECKSUM: kiezelpay_toByteArray(response.checksum)
        };
        kiezelPayLog("KiezelPay watch status msg: " + JSON.stringify(msg));
        Pebble.sendAppMessage(msg,
                              function (e) {
                                kiezelPayLog('KiezelPay status msg successfully sent to watch');
                              },
                              function (e) {
                                kiezelPayLog('KiezelPay status msg failed sending to watch');
                              });
      },
      function (error) {
        kiezelPayLog('KiezelPay status request failed: ' + JSON.stringify(error));
        kiezelpay_sendInternetFailedMsg();
      }, 5000);
    return true;    //its our message, we handled it
  }
  
  return false;    //not a kiezelpay message
}

function kiezelpay_sendInternetFailedMsg() {
  var msg = {
    KIEZELPAY_INTERNET_FAIL: 1
  };
  Pebble.sendAppMessage(msg,
                        function (e) {
                          kiezelPayLog('KiezelPay internet fail successfully sent');
                        },
                        function (e) {
                          kiezelPayLog('KiezelPay internet fail not sent');
                        });
}

function kiezelpay_toByteArray(hexStringValue) {
  var bytes = [];
  for (var i = 0; i < hexStringValue.length; i += 2) {
    bytes.push(parseInt(hexStringValue.substr(i, 2), 16));
  }
  return bytes;
}

function getPlatform() {
  if (Pebble.getActiveWatchInfo) {
    return Pebble.getActiveWatchInfo().platform;
  }
  else {
    return "aplite";
  }
}

Pebble.addEventListener("appmessage", function (e) {
  if (!kiezelPayOnAppMessage(e) && kiezelPayAppMessageHandler) {
    kiezelPayAppMessageHandler(e);
  }
});

/********************************************************************/
/* === KIEZELPAY === GENERATED CODE END === DO NOT MODIFY ABOVE === */
/********************************************************************/



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
    
    //init kiezelpay and register your own handler to receive appMessages
    kiezelPayInit(onAppMessageReceived);
        
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

