import cv2
import cvzone
import math
from ultralytics import YOLO

cap = cv2.VideoCapture(0)

model = YOLO('yolov8s.pt')

classNames = []
with open('classes.txt', 'r') as f:
    classNames = f.read().splitlines()

while True:
    ret, frame = cap.read()
    frame = cv2.resize(frame, (640, 480))

    results = model(frame, classes=[0])
    


