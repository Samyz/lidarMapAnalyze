import serial
import time

rpSerial = serial.Serial('COM4', 115200)
time.sleep(1)
print("Start")
rpSerial.write(b'start\r\n')
get = rpSerial.readline()
print(get)
