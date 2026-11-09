import serial
import time

esp32 = serial.Serial(
    '/dev/ttyUSB0',
    115200,
    timeout=2
)

time.sleep(2)

print("Sending PING...")

esp32.write(b'PING\n')

while True:
    response = esp32.readline().decode(errors='ignore').strip()

    if response:
        print("Received:", response)

    if response == "PONG":
        print("SUCCESS: ESP32 replied PONG")
        break

esp32.close()