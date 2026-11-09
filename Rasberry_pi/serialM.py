import serial
import time

esp32 = serial.Serial(
    '/dev/ttyUSB0',
    115200,
    timeout=1
)

time.sleep(2)


def send_command(command):
    print(f"Sending: {command}")

    esp32.write((command + '\n').encode())

    end_time = time.time() + 2

    while time.time() < end_time:
        response = esp32.readline().decode(errors='ignore').strip()

        if response:
            print("Received:", response)

        if response == "PONG" or response.startswith("OK"):
            return

    print("No command response received")


send_command("PING")

time.sleep(1)

send_command("FORWARD")
time.sleep(2)

send_command("STOP")
time.sleep(1)

send_command("REVERSE")
time.sleep(2)

send_command("STOP")

esp32.close()