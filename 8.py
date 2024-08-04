from flask import Flask, jsonify
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
    ser = serial.Serial('/dev/ttyACM0', 9600)
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            temperature, humidity, moisture = map(float, line.split(','))
            sensor_data['temperature'] = temperature
            sensor_data['humidity'] = humidity
            sensor_data['moisture'] = moisture

@app.route('/data')
def get_data():
    return jsonify(sensor_data)

if __name__ == '__main__':
    serial_thread = threading.Thread(target=read_serial)
    serial_thread.start()
    app.run(host='0.0.0.0', port=5000)
