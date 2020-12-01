import serial
import threading

connected = False
rpSerial = serial.Serial('COM4', 115200)

def read_from_port(ser):

    global connected
    lastAngle = 0
    num = 0
    writeFile = open('data/' + str(num) + '.txt', 'w')
    while not connected:
        #serin = ser.read()
        connected = True
        while True:
        #    print("test")
            read = ser.readline().decode('utf-8')
            # print(tmp)
            length = int(read.split(' ')[0])
            angle = int(read.split(' ')[1][:-1])
            if angle < lastAngle:
                writeFile.close()
                num += 1
                lastAngle = 0
                writeFile = open('data/' + str(num) + '.txt', 'w')
                writeFile.write(read)
            else:
                writeFile.write(read)
            lastAngle = angle

        #    reading = ser.readline().decode()
        #    handle_data(reading)

thread = threading.Thread(target=read_from_port, args=(rpSerial,))
thread.start()