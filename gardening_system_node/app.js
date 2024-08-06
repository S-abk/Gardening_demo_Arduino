const express = require('express');
const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');

const app = express();
const port = new SerialPort('/dev/ttyUSB0', { baudRate: 9600 }); // Adjust the port name as necessary
const parser = port.pipe(new Readline({ delimiter: '\n' })); // Set up the parser with a newline delimiter

let recentData = {
    humidity: 0,
    temperature: 0,
    moisture: 0,
    uvIndex: 0,
    warning: 0
};

// Process incoming data
parser.on('data', (line) => {
    try {
        const [humidity, temperature, moisture, uvIndex, warning] = line.split(',').map(Number);
        recentData = { humidity, temperature, moisture, uvIndex, warning };

        console.log('Updated Data:', recentData);
        // Further processing or alert generation
    } catch (error) {
        console.error('Error parsing data:', error);
    }
});

// Serve the most recent data to clients
app.get('/data', (req, res) => {
    res.json(recentData);
});

// Start the web server
app.listen(3000, () => {
    console.log('Web server running on http://localhost:3000');
});
