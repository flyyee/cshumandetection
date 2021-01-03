import os

dirname, filename = os.path.split(os.path.abspath(__file__))
with open(os.path.join(dirname, "screentomouseX.txt")) as f:
    anglesX = f.readlines()
with open(os.path.join(dirname, "screentomouseY.txt")) as f:
    anglesY = f.readlines()

movemouseLocation = os.path.join(dirname, "movemouse.exe")
movemouseCMD = "\"" + movemouseLocation + "\" "

myaw = 2 / 3
mpitch = 2 / 3
sens = 2.5
scopedConv = 8.2 / 3
anglesX = [int(angle) * myaw / sens for angle in anglesX]
anglesY = [int(angle) * myaw / sens for angle in anglesY]
anglesX.insert(0, 0)
anglesY.insert(0, 0)
maxIndexX = len(anglesX) - 1
maxIndexY = len(anglesY) - 1

pixelgapX = 10
pixelgapY = 10

def move(deltaX, deltaY):
    if deltaX == 0:
        deltaX = 1
    if deltaY == 0:
        deltaY = 1
        
    sgnX = deltaX / abs(deltaX)
    deltaX = abs(deltaX)
    sgnY = deltaY / abs(deltaY)
    deltaY = abs(deltaY)

    idxX = deltaX // pixelgapX
    idxY = deltaY // pixelgapY
    resX = deltaX % pixelgapX
    resY = deltaY % pixelgapY

    if resX in range(3, 8):
        moveX = (anglesX[idxX] + anglesX[min(idxX + 1, maxIndexX)]) / 2
    else:
        moveX = anglesX[idxX]

    if resY in range(3, 8):
        moveY = (anglesY[idxY] + anglesY[min(idxY + 1, maxIndexY)]) / 2
    else:
        moveY = anglesY[idxY]

    scoped = False
    if scoped:
        moveX *= scopedConv
        moveY *= scopedConv

    # time.sleep(2)
    # start = time.time()

    print("Moving mouse")
    os.system(movemouseCMD + str(sgnX * moveX) + " " + str(sgnY * moveY))
    # duration = (time.time() - start) * 1000
    # print("Duration: " + str(duration) + "ms")

# move(140, 133)