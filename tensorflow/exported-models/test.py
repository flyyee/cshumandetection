import pyautogui
import time
pyautogui.FAILSAFE = False

time.sleep(2)
x, y = pyautogui.position()
print(x, y)
# pyautogui.moveTo(x + 100, y)
# exit()


import numpy as np
import cv2
from PIL import ImageGrab
import time
import win32gui

screen = 'Counter-Strike: Global Offensive'
hwnd = win32gui.FindWindow(None, screen)
rect = win32gui.GetWindowRect(hwnd)
zoomscale = 1
offsetx = 3
offsety = 3
brx = rect[2] * zoomscale - offsetx
bry = rect[3] * zoomscale - offsety
width = 800
height = 600
rect = (brx - width, bry - height, brx, bry)
print(rect)
coords = [156.34055, 286.3577, 229.01222, 429.44604]
tox = (coords[0] + coords[2]) / 2 + rect[0]
toy = (coords[1] + coords[3]) / 2 + rect[1]

print(tox, toy)

sens = 1
mox = (tox - x) / 6.949098 / 2.2 / sens * 100 * 0.92
moy = (toy - y) / 5.236522 / 2.2 / sens * 100 * 0.6

print(mox, moy)
# pyautogui.dragTo(tox, toy)
# print(tox - x, toy - y)
# for a in range(1329):
#     pyautogui.move(-1, 0)
# for b in range(560):
#     pyautogui.move(0, 1)

pyautogui.move(mox/4, moy/4)
pyautogui.move(mox/4, moy/4)
pyautogui.move(mox/4, moy/4)
pyautogui.move(mox/4, moy/4)