const express = require('express');
const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');

const app = express();
const port = new SerialPort('/dev/ttyACM0', { baudRate: 9600 });
const parser = port.pipe(new Readline({ delimiter: '\n' }));

let recentData = {
    humidity: 0,
    temperature: 0,
    moisture: 0,
    uvIndex: 0,
    warning: 0
};

parser.on('data', (line) => {
    const [humidity, temperature, moisture, uvIndex, warning] = line.split(',').map(Number);
    recentData = { humidity, temperature, moisture, uvIndex, warning };

    console.log('Updated Data:', recentData);
});

app.use(express.static('public'));

// Endpoint to trigger watering
app.post('/control/water', (req, res) => {
    port.write('WATER\n', (err) => {
        if (err) {
            return res.status(500).json({ status: 'Error sending command' });
        }
        res.json({ status: 'Watering command sent' });
    });
});

// Serve the most recent data to clients
app.get('/data', (req, res) => {
    res.json(recentData);
});

// Start the web server
app.listen(3000, () => {
    console.log('Web server running on http://localhost:3000');
});
