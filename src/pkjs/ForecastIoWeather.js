var ForecastIoWeather = function() {
  
  // converts forecast.io weather icon code to Yahoo weather icon code (to reuse current bitmap with icon set)
  var ForecastIoIconToYahooIcon = function(forecsat_io_icon) {
    var yahoo_icon = 3200; //initialy not defined
    
    switch (forecsat_io_icon){
      case "clear-day":
        yahoo_icon = 32; // sunny
        break;
      case "clear-night":
        yahoo_icon = 31; // clear night
        break;
      case "rain":
        yahoo_icon = 11; // showers
        break;
      case "snow":
        yahoo_icon = 16; // snow
        break;
      case "sleet": 
        yahoo_icon = 18; // sleet
        break;
      case "wind": 
        yahoo_icon = 24; // windy
        break;
      case "fog": 
        yahoo_icon = 20; // foggy
        break;
      case "cloudy":
        yahoo_icon = 26; // cloudy
        break;
      case "partly-cloudy-day":
        yahoo_icon = 30; // partly cloudy day
        break;
      case "partly-cloudy-night":
        yahoo_icon = 29; // partly cloudy night
        break;
    }
    
    return yahoo_icon;
    
  };
  
  var that = this;
  
  this.temperatureFormat = 0;
  this.apiKey = ''; //2a646b5313191005958df09cc5f61e6e'; // test. replace with user's
  
  // getting weather
  this.getWeather = function(coords ) {  
    
      if (that.apiKey === '') {
        //console.log ("\n++++ I am inside of 'getWeather()' API KEY NOT DEFINED");
        return;
      }
  
      var temperature;
      var icon;

      var url = 'https://api.forecast.io/forecast/' + that.apiKey + '/' + coords + '?exclude=minutely,hourly,daily,alerts,flags&units=' + (that.temperatureFormat === 0? 'us' : 'si');
      //console.log ("\n++++ I am inside of 'getWeather()' preparing url:" + url);
        
      // Send request to Forecasr.IO
      var xhr = new XMLHttpRequest();
      xhr.onload = function () {
        
        //console.log  ("\n++++ I am inside of 'getWeather()' callback. Raw JSON is " + this.responseText);
        var json = JSON.parse(this.responseText);
        
        temperature = json.currently.temperature;
        //console.log  ("\n++++ I am inside of 'getWeather()' callback. Temperature is " + temperature);
        
        icon = json.currently.icon;
        //console.log  ("\n++++ I am inside of 'getWeather()' callback. Icon code: " + icon);
        
               
        var dictionary = {
          'KEY_WEATHER_CODE': ForecastIoIconToYahooIcon(icon),
          'KEY_WEATHER_TEMP': temperature
        };
        
        // Send to Pebble
        //console.log  ("\n++++ I am inside of 'getWeather()' callback. About to send message to Pebble");
        Pebble.sendAppMessage(dictionary,
        function(e) {
          //console.log ("\n++++ I am inside of 'Pebble.sendAppMessage()' callback. Weather info sent to Pebble successfully!");
        },
        function(e) {
          //console.log ("\n++++ I am inside of 'Pebble.sendAppMessage()' callback. Error sending weather info to Pebble! = " + JSON.stringify(e));
        }
        );
      };
      xhr.open('GET', url);
      xhr.send();
  };
  
  
     // on location success querying passing coordinates to weather function
    var locationSuccess = function(pos) {
      that.getWeather(pos.coords.latitude + ',' + pos.coords.longitude);
    };
  
   // on location error
   var locationError = function(err) {
     //console.log ("\n++++ I am inside of 'locationError: Error requesting location!");
   };
  
  
    // Get Location lat+lon
   this.getLocation = function () {
      navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 15000, maximumAge: 60000}
     );
   };  
      
  
  
};

module.exports = ForecastIoWeather;
