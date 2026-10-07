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

    annotated_frame = results[0].plot(labels=False)

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

            height= y2 - y1
            width = x2 - x1
            threshold = height - width

            if confidence > threshold:
                cvzone.cornerRect(annotated_frame, (x1, y1, x2 - x1, y2 - y1), l=9, t=5, rt=1, colorR=(255, 0, 255))
            cvzone.putTextRect(annotated_frame, f'{class_detect} {confidence}%', (max(0, x1), max(35, y1)), scale=0.8, thickness=1, offset=3)

    cv2.imshow("Annotated Frame", annotated_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

