#!/usr/bin/env python3
import os.path
import time
import serial
import sys

device = '/dev/ttyUSB0'
if len(sys.argv) > 1:
    device = sys.argv[1]

def main():
    while True:
        if os.path.exists(device):
            try:
                # Use a breakpoint in the code line below to debug your script.
                ser = serial.Serial(device, baudrate=115200, timeout=None)
                print("Opened:"+device)
                while ser.isOpen():
                    try:
                        line = ser.readline().decode('ASCII').strip()
                        print(line)
                    except KeyboardInterrupt:
                        print()
                        return
            except SerialException as e:
                pass
        time.sleep(0.1)

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
