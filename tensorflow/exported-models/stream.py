import numpy as np
import cv2
from PIL import ImageGrab
import win32gui
import time

screen = 'Counter-Strike: Global Offensive'
hwnd = win32gui.FindWindow(None, screen)
rect = win32gui.GetWindowRect(hwnd)
zoomscale = 1.5
offsetx = 3
offsety = 3
brx = rect[2] * zoomscale - offsetx
bry = rect[3] * zoomscale - offsety
width = 800
height = 600
rect = (brx - width, bry - height, brx, bry)
# rect[0] = rect[2] - width
# rect[1] = rect[3] - height

print(rect)
# input("here")

last_time = time.time()
cont = True

while cont:
    try:
        start = time.time()
        print_screen = np.array(ImageGrab.grab(rect))
        # Load np array into ml model
        print(str(int((time.time() - start) * 1000)) + "ms")


        # print("loop took {} seconds".format(time.time() - last_time))
        # last_time = time.time()
        # cv2.imshow('window',cv2.cvtColor(print_screen, cv2.COLOR_BGR2RGB))
        # cv2.waitKey(0)
        # cv2.destroyAllWindows()
    except Exception as e:
        print("error", e)
    # input()