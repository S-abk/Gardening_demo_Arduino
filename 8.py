

from flask import Flask, render_template, jsonify
from flask_socketio import SocketIO, emit
import threading
import serial

app = Flask(__name__)
socketio = SocketIO(app)

# Global variables to store sensor data
sensor_data = {
    'temperature': 0.0,
    'humidity': 0.0,
    'moisture': 0,
    'watering_status': False
}

def read_serial():
    ser = serial.Serial('/dev/ttyACM0', 9600)  # Adjust the port as necessary
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            # Skip non-numeric lines and initial messages
            if ',' in line and all(char.isdigit() or char in '.,' for char in line):
                try:
                    temperature, humidity, moisture = map(float, line.split(','))
                    sensor_data['temperature'] = temperature
                    sensor_data['humidity'] = humidity
                    sensor_data['moisture'] = moisture
                    socketio.emit('sensor_update', sensor_data)
                except ValueError:
                    print("Error parsing line:", line)
            else:
                print("Ignoring line:", line)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/toggle_watering', methods=['POST'])
def toggle_watering():
    # Send 'T' to Arduino to toggle the watering system
    ser.write(b'T')  # Send the 'T' character over serial
    # Update local status (assumed it matches the device's new state)
    sensor_data['watering_status'] = not sensor_data['watering_status']
    socketio.emit('sensor_update', sensor_data)
    return jsonify({'status': 'On' if sensor_data['watering_status'] else 'Off'})

if __name__ == '__main__':
    serial_thread = threading.Thread(target=read_serial)
    serial_thread.start()
    socketio.run(app, host='0.0.0.0', port=5000, use_reloader=False, debug=False)
