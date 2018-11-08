let SerialPort = require('serialport')
let Readline = SerialPort.parsers.Readline

let mqtt = require('mqtt')
let mqttLocal = mqtt.connect('mqtt://localhost')

let mqttMaster = mqtt.connect('mqtt://192.168.0.210')

let buttonMappings = {"2": "DR", "3": "UR", "4": "SR"}

mqttMaster.on('connect', function() {
	console.log("Connected to Master MQTT server");
})



mqttLocal.on('connect', function() {
	mqttLocal.subscribe('arduino/out', function(err) {

	})
})

mqttLocal.on('message', function(topic, message) {
	// message is Buffer
	console.log(message.toString())
	serialPort.write(message.toString(), function(err) {
	  if (err) {
	    return console.log('Error on write: ', err.message)
	  }
	  console.log('message written')
	})
	//mqttLocal.end()
})


let serialPort = new SerialPort('/dev/ttyACM0', {
	baudRate: 115200
})

let parser = new Readline()
serialPort.pipe(parser)
parser.on('data', function(data) {
	console.log('data received: ' + data)
	parseData(data);
	mqttLocal.publish("arduino/in", data);
})

serialPort.on('open', function() {
	console.log('Serial port open!')
})
// Open errors will be emitted as an error event
serialPort.on('error', function(err) {
  console.log('Error: ', err.message)
})

function parseData(data) {
	let tokens = data.split(",");
	//console.log(tokens); 

	switch (tokens[0]) {
		case "Ready":
			console.log("Ready");
			//mqttLocal.publish("status/DR", "Ready");
			
			break;
		case "i":
			btn = tokens[1].trim()

			mqttLocal.publish("arduino/button", btn)
			//topic = "room/status/" + buttonMappings[btn];
			//mqttMaster.publish(topic, "STANDBY")
			console.log("btn", btn);
			break;
		default:
			console.log(tokens);
	}
}