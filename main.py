import cv2
import cvzone
import math
from ultralytics import YOLO
from picamera2 import Picamera2

cap = Picamera2()
config = cap.create_preview_configuration(main={"format": "BGR888", "size": (640, 480)})
cap.configure(config)
cap.start()

model = YOLO('yolov8s.pt')


with open('classes.txt', 'r') as f:
    classNames = f.read().splitlines()

while True:
    frame = cap.capture_array()
    if frame is None:
        print("Could not read from the camera.")
        break

    
    frame = cv2.resize(frame, (640, 480))

    results = model(frame, classes=[0], conf=0.5)

    annotated_frame = results[0].plot(labels=False)

    for info in results:
        for box in info.boxes:
            x1, y1, x2, y2 = box.xyxy[0]
            x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
            confidence = float(box.conf[0])
            confidence_percentage = math.ceil(confidence * 100)
            class_id = int(box.cls[0])
            class_name = classNames[class_id]

            if confidence_percentage > 50:
                cvzone.cornerRect(annotated_frame, (x1, y1, x2 - x1, y2 - y1), l=9, t=5, rt=1, colorR=(255, 0, 255))
            cvzone.putTextRect(annotated_frame, f'{class_name} {confidence_percentage}%', (max(0, x1), max(35, y1)), scale=0.8, thickness=1, offset=3)

    cv2.imshow("YOLO Camera", annotated_frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.stop()
cv2.destroyAllWindows()

