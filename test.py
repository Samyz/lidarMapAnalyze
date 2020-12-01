import serial
import time
import threading
import matplotlib.pyplot as plt
import math
writeFile = open('text.txt', 'w')

connected = False
rpSerial = serial.Serial('COM4', 115200)

# a = 0xA520
# b = a.to_bytes(2, 'big')
# rpSerial.write(b)
# print('sended')

def handle_data(data):
    print(data)

def read_from_port(ser):

    global connected
    first = True
    flag = 0
    x = []
    y = []

    plt.ion()
    while not connected:
        #serin = ser.read()
        connected = True
        while True:
        #    print("test")
            read = ser.read(7 if first else 5).hex()
            # print(tmp)
            writeFile.write(read)
            writeFile.write('\n')
            if not first:
                if int(read[1], 16) & 1 == 1:
                    if flag != 0:
                        plt.clf()
                        plt.plot(x, y, 'bo', markersize = 1)
                        plt.pause(0.5)
                    flag = 1 if flag == 0 or flag == 2 else 2
                    x = []
                    y = []
                else:
                    if read[6:10] != '0000':
                        angle0 = int(read[2:4], 16)
                        angle1 = int(read[4:6], 16)
                        angle = int(((angle1 << 7) | (angle0 >> 1))/64)
                        r = int(((int(read[6:8], 16) << 8) | (int(read[8:10], 16)))/4)
                        x.append(r*math.sin(math.radians(angle)))
                        y.append(r*math.cos(math.radians(angle)))
            first = False
        #    reading = ser.readline().decode()
        #    handle_data(reading)

thread = threading.Thread(target=read_from_port, args=(rpSerial,))
thread.start()

a = 0xA520
b = a.to_bytes(2, 'big')
rpSerial.write(b)
print('sended')


# print(rpSerial.write(a))
# time.sleep(0.1)
# b = rpSerial
# print(b)