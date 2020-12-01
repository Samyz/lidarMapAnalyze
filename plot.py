import numpy as np
import matplotlib.pyplot as plt
import math
for i in range(0,1):#range(9,24):
    filename = str(i + 1)
    # readFile = open('dataDynamic/'+filename+'.txt', 'r')
    # filename = str(i)
    # readFile = open('dataraw/'+filename+'.txt', 'r')
    readFile = open('out.txt', 'r')
    txt = readFile.readlines()
    readFile.close()
    x = []
    y = []
    xy = []
    fig = plt.figure()
    for i in range(len(txt)):
        if i%2 == 0:
            tmp = txt[i]
            r = int(tmp.split(' ')[0])
            angle = int(tmp.split(' ')[1])
            x.append(r*math.sin(math.radians(angle)))
            y.append(r*math.cos(math.radians(angle)))
            xy.append((int(r*math.sin(math.radians(angle))), int(r*math.cos(math.radians(angle)))))

    def select_x(tmp):
        return tmp[0]

    xy.sort(key = select_x)
    # print(xy)

    def plot():
        ax = fig.add_subplot(1, 1, 1)
        ax.grid(True)
        ax.plot(x, y, 'ro', markersize = 1)
        ax.spines['left'].set_position('zero')
        ax.spines['right'].set_color('none')
        ax.spines['bottom'].set_position('zero')
        ax.spines['top'].set_color('none')
        ax.spines['left'].set_smart_bounds(True)
        ax.spines['bottom'].set_smart_bounds(True)
        ax.xaxis.set_ticks_position('bottom')
        ax.yaxis.set_ticks_position('left')
        yabs_max = abs(max(ax.get_ylim(), key=abs))
        ax.set_ylim(ymin=-yabs_max, ymax=yabs_max)
        xabs_max = abs(max(ax.get_xlim(), key=abs))
        ax.set_xlim(xmin=-xabs_max, xmax=xabs_max)
        plt.show()
        x_ticks = np.arange(-2000, 2000, 100)
        plt.xticks(x_ticks,fontsize=6, rotation=90)
        y_ticks = np.arange(-2000, 2000, 100)
        plt.yticks(y_ticks,fontsize=6)
        # plt.savefig('./datarawDynamic/'+filename+'.png')
        
    plot()
    # filename = str(int(filename)+1)

# print(txt)
# x_val = [x[0] for x in data]
# y_val = [x[1] for x in data]