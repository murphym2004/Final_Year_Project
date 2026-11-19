import cv2
import cvzone
import math
import time
import serial
from ultralytics import YOLO
from picamera2 import Picamera2

cap = Picamera2()
config = cap.create_preview_configuration(main={"format": "BGR888", "size": (320, 240)}, buffer_count=2, queue=False)
cap.configure(config)
cap.start()

model = YOLO('yolov8n_ncnn_model')


with open('classes.txt', 'r') as f:
    classNames = f.read().splitlines()

previous_center_y = None
possible_fall = False
fall_start_time = None

esp32 = serial.Serial(
    '/dev/ttyUSB0',
    115200,
    timeout=0.2
)

time.sleep(2)

last_command = None

def send_command(command):
    global last_command

    if command == last_command:
        return

    print("Sending command:", command)
    esp32.write((command + '\n').encode())
    last_command = command

while True:
    frame = cap.capture_array()
    if frame is None:
        print("Could not read from the camera.")
        break


    results = model(frame, classes=[0], conf=0.5,imgsz=320)


    annotated_frame = results[0].plot(labels=False)


    for info in results:
        for box in info.boxes:
            x1, y1, x2, y2 = box.xyxy[0]
            x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)

            width = x2 - x1
            height = y2 - y1

            center_y = (y1 + y2) / 2

            person_centre_x = (x1 + x2) / 2
            frame_centre_x = frame.shape[1] / 2
            dead_zone = 60                                               //allows for a dead zone of 60 pixels around the center of the frame
            left_zone = frame_centre_x - dead_zone
            right_zone = frame_centre_x + dead_zone


            if person_centre_x < left_zone:
                person_position = "LEFT"

            elif person_centre_x > right_zone:
                person_position = "RIGHT"

            else:
                person_position = "CENTRE"



            if person_position == "LEFT":
                send_command("LEFT")

            elif person_position == "RIGHT":
                send_command("RIGHT")

            elif person_position == "CENTRE":
                send_command("FORWARD")


            print("Person centre:", person_centre_x)
            print("Frame centre:", frame_centre_x)
            print("Person position:", person_position)

            if height > 0:
                aspect_ratio = width / height
            else:
                aspect_ratio = 0

            if previous_center_y is not None:
                downward_movement = center_y - previous_center_y
            else:
                downward_movement = 0

            previous_center_y = center_y

            if downward_movement > 20 and aspect_ratio > 1.0:
                print("Possible fall")
                possible_fall = True

            if possible_fall and aspect_ratio > 1.0:
                if fall_start_time is None:
                    fall_start_time = time.time()

            if possible_fall:

                if aspect_ratio > 1.0:

                    if fall_start_time is not None:

                        time_down = time.time() - fall_start_time


                        if time_down > 2.0:
                            print("POTENTIAL FALL DETECTED")
                            cv2.putText(annotated_frame,"POTENTIAL FALL DETECTED",(10, 25),cv2.FONT_HERSHEY_SIMPLEX,0.6,(0, 0, 255),2)
                else:
                    possible_fall = False
                    fall_start_time = None



            confidence = float(box.conf[0])
            confidence_percentage = math.ceil(confidence * 100)
            class_id = int(box.cls[0])
            class_name = classNames[class_id]


            if confidence_percentage > 50:
                cvzone.cornerRect(
                    annotated_frame,
                    (x1, y1, x2 - x1, y2 - y1),
                    l=9,
                    t=5,
                    rt=1,
                    colorR=(255, 0, 255)
                )

                cvzone.putTextRect(
                    annotated_frame,
                    f'{class_name} {confidence_percentage}%',
                    (max(0, x1), max(35, y1)),
                    scale=0.8,
                    thickness=1,
                    offset=3
                )


    cv2.imshow("YOLO Camera", annotated_frame)


    if cv2.waitKey(1) & 0xFF == ord('q'):
        break


cap.stop()
esp32.close()
cv2.destroyAllWindows()