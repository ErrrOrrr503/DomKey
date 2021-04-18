import serial
import sys
import signal
from threading import Thread

ser = serial.Serial (sys.argv[1], 115200)

def reader ():
    while True:
        tmp = ser.readline ()
        print (tmp.decode ('utf-8'), end = '')

reader_thread = Thread(target=reader, daemon=True)
reader_thread.start ()

def signal_handler(sig, frame):
    print('Aborted')
    ser.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)
if __name__ == '__main__':
    while True:
        tmp1 = input()
        ser.write(bytes (tmp1, 'utf-8'))