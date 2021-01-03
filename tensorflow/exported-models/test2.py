import pyautogui, time, os
time.sleep(2)

print("Moving mouse")
os.system("movemouse.exe -477 278")
# currentMouseX, currentMouseY = pyautogui.position()

# pyautogui.move(100, 100)
# pyautogui.move(100, 100)
# pyautogui.click()
exit()

# screenDeltaX = (detectionCoords[0] + detectionCoords[2]) / 2 + rect[0]
# # toy = (coords[1] + coords[3]) / 2 + rect[1]
# screenDeltaY = detectionCoords[1] + 0.4 * (detectionCoords[3] - detectionCoords[1]) + rect[1]

screenDeltaX = (773 - 960)
screenDeltaY = 0

angX = screenDeltaX / 800 * 90
print(angX)
moveX = angX / 2.2 * 100

# print(tox, toy)

sens = 1
# moveX = (screenDeltaX) / 6.949098 / 2.2 / sens * 100 #* 0.95
moveY = (screenDeltaY) / 5.236522 / 2.2 / sens * 100 * 0.65

# print(mox, moy)

pyautogui.move(moveX / 4, moveY)
pyautogui.move(moveX / 4, moveY)
pyautogui.move(moveX / 4, moveY)
pyautogui.move(moveX / 4, moveY)