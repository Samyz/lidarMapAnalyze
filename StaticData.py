import math
for i in range(0,20): #range(0,1):#range(9,24):
    filename = str(int(i / 2 + 1)) + '_' + str(i % 2 + 1)
    # print(filename)
    readFile = open('dataDynamic/'+filename+'.txt', 'r')
    # print('eiei')
    # filename = str(i)
    # readFile = open('dataraw/'+filename+'.txt', 'r')
    # readFile = open('out.txt', 'r')
    txt = readFile.readlines()
    readFile.close()
    x = []
    y = []
    # xy = []
    for j in range(len(txt)):
        if j % 2 == 0:
            tmp = txt[j]
            r = int(tmp.split(' ')[0])
            angle = int(tmp.split(' ')[1])
            
            x.append(r*math.sin(math.radians((angle + 90 if i % 2 == 0 else angle + 270) % 360)))
            y.append(r*math.cos(math.radians((angle + 90 if i % 2 == 0 else angle + 270) % 360)))
            # x.append(r*math.sin(math.radians(angle)))
            # y.append(r*math.cos(math.radians(angle)))
            # xy.append((int(r*math.sin(math.radians(angle))), int(r*math.cos(math.radians(angle)))))
            # xy.append((int(r*math.sin(math.radians((angle + 90 if i % 2 == 0 else 270) / 360))), int(r*math.cos(math.radians((angle + 90 if i % 2 == 0 else 270) / 360)))))

    def select_x(tmp):
        return tmp[0]

    # xy.sort(key = select_x)
    # print(xy)

    # filename = str(int(filename)+1)

# print(txt)
# x_val = [x[0] for x in data]
# y_val = [x[1] for x in data]