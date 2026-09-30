import cv2
import cvzone
import math
from ultralytics import YOLO

cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

model = YOLO('yolov8s.pt')

classNames = []
with open('classes.txt', 'r') as f:
    classNames = f.read().splitlines()

while True:
    ret, frame = cap.read()
    if not ret or frame is None:
        print("Could not read from the camera.")
        break

    frame = cv2.resize(frame, (640, 480))

    results = model(frame, classes=[0])

    annotated_frame = results[0].plot()
    cv2.imshow("Annotated Frame", annotated_frame)

    for info in results:
        parameters = info.boxes
        for box in parameters: 
            x1, y1, x2, y2 = box.xyxy[0]
            x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
            cofidence = box.conf[0]
            class_detect = box.cls[0]
            class_detect = int(class_detect)
            class_detect = classNames[class_detect]
            confidence = math.ceil(cofidence * 100)

    cv2.imshow("Annotated Frame", annotated_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

