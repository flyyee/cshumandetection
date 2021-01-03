import csv
p = "C:\\Users\\Gerrard Tai\\Documents\\code\\cshumandetection\\boy.txt"
with open(p, newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
    for row in spamreader:
        print(row[3])