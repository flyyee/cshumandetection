from PIL import Image, ImageDraw, ImageShow
import sys
import math

filename = sys.argv[1]
p1 = float(sys.argv[2])
p2 = float(sys.argv[3])
p3 = float(sys.argv[4])
p4 = float(sys.argv[5])

# fo = open(".\\foobar.txt", "r")
with Image.open(".\\imgs\\" + filename + ".jpg") as im:
    draw = ImageDraw.Draw(im)
    # INFO = fo.readline()
    # l = INFO.split(",")
    # p1 = float(l[0])
    # p2 = float(l[1])
    # INFO = fo.readline()
    # l = INFO.split(",")
    # p3 = float(l[0])
    # p4 = float(l[1])

    h = p4 - p2
    # w = 32 / 72 * h
    w = h / 2 # 2 for standing, 1.5 for crouch
    y = (p2 + p4) / 2
    x1 = (p1 + p3) / 2 - w/2 #-2
    x2 = (p1 + p3) / 2 + w/2 #+2

    # print(p1, p2)
    draw.rectangle([p1, p2, p3, p4], fill=None, width=3)
    ImageShow.show(im)
    # input()