var YahooWeather = function() {
  
  var that = this;
  
  this.temperatureFormat = 0;
  
  // getting weather
  this.getWeather = function(woeid ) {  
  
      var temperature;
      var icon;
      var city;
      
      var query = 'select item.condition, location.city from weather.forecast where woeid =  ' + woeid + ' and u="' + (that.temperatureFormat === 0? 'f' : 'c') + '"';
      //console.log ("\n++++ I am inside of 'getWeather()' preparing query:" + query);
      
      var url = 'https://query.yahooapis.com/v1/public/yql?q=' + encodeURIComponent(query) + '&format=json&env=store://datatables.org/alltableswithkeys';
      //console.log ("\n++++ I am inside of 'getWeather()' preparing url:" + url);
        
      // Send request to Yahoo
      var xhr = new XMLHttpRequest();
      xhr.onload = function () {
        
        //console.log  ("\n++++ I am inside of 'getWeather()' callback. Raw JSON is " + this.responseText);
        
        var json = JSON.parse(this.responseText);
        temperature = parseInt(json.query.results.channel.item.condition.temp);
        //console.log  ("\n++++ I am inside of 'getWeather()' callback. Temperature is " + temperature);
        
        icon = parseInt(json.query.results.channel.item.condition.code);
        //console.log  ("\n++++ I am inside of 'getWeather()' callback. Icon code: " + icon);
        
        city = json.query.results.channel.location.city;
               
        var dictionary = {
          'KEY_WEATHER_CODE': icon,
          'KEY_WEATHER_TEMP': temperature,
          'KEY_CITY_NAME': city
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
  
  
     // on location success querying woeid and getting weather
    var locationSuccess = function(pos) {
      // We neeed to get the Yahoo woeid first
      var woeid;
    
      /* YG 2016-01-25  !!! This query no longer works due to Yahoo bug. Using the one below it !!!  */  
      // var query = 'select * from geo.placefinder where text="' +
      //     pos.coords.latitude + ',' + pos.coords.longitude + '" and gflags="R"';
       var query = 'select locality1 from geo.places where text="(' + 
           pos.coords.latitude + ',' + pos.coords.longitude + ')" limit 1';
    
      //console.log ("\n++++ I am inside of 'locationSuccess()' preparing query:" + query);
      var url = 'https://query.yahooapis.com/v1/public/yql?q=' + encodeURIComponent(query) + '&format=json';
      //console.log ("\n++++ I am inside of 'locationSuccess()' preparing URL: " + url);
     
      // Send request to Yahoo
      var xhr = new XMLHttpRequest();
      xhr.onload = function () {
        var json = JSON.parse(this.responseText);
        
        /* YG 2016-01-25  !!! This result no longer works due to Yahoo bug. Using the one below it !!!  */  
        // woeid = json.query.results.Result.woeid;
        woeid = json.query.results.place.locality1.woeid;
        
        //console.log ("\n++++ I am inside of 'locationSuccess()', woeid received:" + woeid);
        that.getWeather(woeid);
      };
      
      xhr.open('GET', url);
      xhr.send();
    
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

module.exports = YahooWeather;
