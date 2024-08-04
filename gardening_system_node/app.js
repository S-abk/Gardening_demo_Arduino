const express = require('express'); 
const http = require('http'); 
const socketIo = require('socket.io'); 
const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

// Serve static files from the public directory
app.use(express.static('public'));

// Setup serial port connection
const port = new SerialPort('/dev/ttyACM0', { baudRate: 9600 });
const parser = port.pipe(new Readline({ delimiter: '\n' }));

// Handle serial data
parser.on('data', (line) => {
    console.log(`Received: ${line}`);
    try {
        const [temperature, humidity, moisture] = line.split(',').map(Number);
        io.emit('sensor_update', { temperature, humidity, moisture });
    } catch (error) {
        console.error('Error parsing data:', error);
    }
});

// Handle socket connection
io.on('connection', (socket) => {
    console.log('Client connected');
    socket.on('toggle_watering', () => {
        console.log('Toggling watering system');
        port.write('T\n');  // Send 'T' command to Arduino
    });

    socket.on('disconnect', () => {
        console.log('Client disconnected');
    });
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => console.log(`Server running on port ${PORT}`));
