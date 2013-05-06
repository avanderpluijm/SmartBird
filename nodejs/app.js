////////////////////////////////////////////////////////
// Use the cool library                               //
// git://github.com/voodootikigod/node-serialport.git //
// to read the serial port where arduino is sitting.  //
////////////////////////////////////////////////////////               

// Module dependencies
var request = require('request'), 
	cosm = require('cosm'), 
	com = require("serialport"), 
	winston = require('winston');

// variable for last update
var lastUpdate = Math.round(new Date().getTime() / 1000); 
// 1 minute update frequency
var updateFreq = 60; 

var logger=new (winston.Logger)({
	transports: [
		new (winston.transports.Console)(), 
		new (winston.transports.File)({filename: 'log.log'})
	]
});

logger.info('Started logger'); 

// cosm settings
var client = new cosm.Cosm('******'),
  feed = new cosm.Feed(cosm, {id: '*****'}),
  stream = new cosm.Datastream(client, feed, {id: 1}); 

// Serial port settings
// dev/ttyACM0 is usb arduino MEGA port. 
var serialPort = new com.SerialPort("/dev/ttyACM0", {
  baudrate: 9600,
  parser: com.parsers.readline('\r\n')
});


// Serial port event
serialPort.on('open',function() {
  console.log('Port open');
});

// Serial port event
serialPort.on('data', function(data) {
	
	var n=data.split(",");	
	
	switch(n[0]){
		case 'LOG': 	
		case 'ERROR': 
			//console.log(data); 
			break; 
		case 'FEED': 
			n.shift(); 			// remove log start
			feed.id = n[0]; // save feed-id
			n.shift(); 			// remove feed-id
			var currentTime = Math.round(new Date().getTime() / 1000); 
			if ( (currentTime - lastUpdate) > updateFreq) {
				updateFeed(n); 	
			} else {
				console.log(currentTime - lastUpdate); 
			}
			break; 
		default: //console.log('unrecognised command: ' + data);  
	}
});


function updateFeed(data) {
	//console.log(data); 
	// construct data for request
	var d='{"datastreams":[ '; 
	var i=0; 

	var a = []; 
	data.forEach(function(entry) {
		
		i++; 
		a.push('{"id":"'+ i +'", "current_value":"' + entry + '"}');
	});
	d += a.join(',') + ']}'; 

	// http://cosm.com/docs/v2/feed/update.html
	
	request({
		method: 'PUT',
		uri: 'http://api.cosm.com/v2/feeds/'+feed.id,
		headers: {
    	"X-ApiKey" : client.apiKey
	  },
  	body: d
	}, function(error, response, body){
		if (response.statusCode == 200 ) {
			lastUpdate = Math.round(new Date().getTime() / 1000); 
			logger.log('info', 'Data sent', { feed:feed.id, data: d });
		} 
		//console.log(error); 
		//console.log(body); 
	}); 

}
