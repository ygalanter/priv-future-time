var OpenMetroWeather = function() {
  
 // converts open-metri weather icon code to Yahoo weather icon code (to reuse current bitmap with icon set)
var OpenMetroCodeToYahooIcon = function (weather_code, is_day) {
  var yahoo_icon = 3200; //initially not defined

  if (weather_code === 0) {
    yahoo_icon = is_day === 1 ? 32 : 31; // sunny or clear night
  } else if ([51, 53, 55, 61, 63, 65, 80, 81, 82, 95, 96, 99].includes(weather_code)) {
    yahoo_icon = 11; //showers
  } else if ([71, 73, 75, 77, 85, 86].includes(weather_code)) {
    yahoo_icon = 16; //snow
  } else if ([56, 57, 66, 67].includes(weather_code)) {
    yahoo_icon = 18; //sleet
  } else if ([45, 48].includes(weather_code)) {
    yahoo_icon = 20; //foggy
  } else if (weather_code === 3) {
    yahoo_icon = 26; //cloudy
  } else if ([1, 2].includes(weather_code)) {
    yahoo_icon = is_day === 1 ? 30 : 29; //partly cloudy day or night
  }

  return yahoo_icon;

};
  
  var that = this;
  
  this.temperatureFormat = 0;
  
  // getting weather
  this.getWeather = function(coords ) {  

    var temperature;
    var code;
    var is_day;

    var url = `https://api.open-meteo.com/v1/forecast?latitude=${coords.latitude}&longitude=${coords.longitude}&current=temperature_2m,weather_code,is_day&temperature_unit=${this.temperatureFormat === 0 ? 'fahrenheit' : 'celsius'}`;
    //console.log ("++++ I am inside of 'getWeather()' preparing url:" + url);

    // ** Send request to Yahoo
    //Send request to Forecast.io
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {

      //console.log  ("++++ I am inside of 'getWeather()' callback. responseText is " + this.responseText);

      var json = JSON.parse(this.responseText);

      temperature = json.current.temperature_2m;
      console.log("++++ I am inside of 'getWeather()' callback. Temperature is " + temperature);

      code = json.current.weather_code;
      console.log("++++ I am inside of 'getWeather()' callback. Icon code: " + code);

      is_day = json.current.is_day;
      console.log("++++ I am inside of 'getWeather()' callback. Is day: " + is_day);
          
               
        var dictionary = {
          'KEY_WEATHER_CODE': OpenMetroCodeToYahooIcon(code, is_day),
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
      that.getWeather(pos.coords);
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

module.exports = OpenMetroWeather;
