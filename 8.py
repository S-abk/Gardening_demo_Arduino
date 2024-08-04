from flask import Flask, jsonify, render_template
import threading
import serial

app = Flask(__name__)

# Global variables to store sensor data
sensor_data = {
    'temperature': 0.0,
    'humidity': 0.0,
    'moisture': 0
}

def read_serial():
    ser = serial.Serial('/dev/ttyAMA0', 9600)  # Adjust the port as necessary
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            try:
                temperature, humidity, moisture = map(float, line.split(','))
                sensor_data['temperature'] = temperature
                sensor_data['humidity'] = humidity
                sensor_data['moisture'] = moisture
            except ValueError:
                print("Error parsing line:", line)

@app.route('/')
def index():
    # Serve the index.html template
    return render_template('index.html')

@app.route('/data')
def get_data():
    # Provide sensor data as JSON
    return jsonify(sensor_data)

if __name__ == '__main__':
    serial_thread = threading.Thread(target=read_serial)
    serial_thread.start()
    app.run(host='0.0.0.0', port=5000)
