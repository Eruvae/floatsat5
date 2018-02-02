import serial
import RPi.GPIO as GPIO
import time

def setCamPos(index):
    if (index == 1):
        p.ChangeDutyCycle(4.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(5)
        time.sleep(0.1)
        p.ChangeDutyCycle(5.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(6)
        time.sleep(0.1)
        p.ChangeDutyCycle(6.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(7)
        time.sleep(0.1)
        p.ChangeDutyCycle(7.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(8)
        time.sleep(0.1)
        p.ChangeDutyCycle(0)
    elif (index == 2):
        p.ChangeDutyCycle(7.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(7)
        time.sleep(0.1)
        p.ChangeDutyCycle(6.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(6)
        time.sleep(0.1)
        p.ChangeDutyCycle(5.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(5)
        time.sleep(0.1)
        p.ChangeDutyCycle(4.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(4)
        time.sleep(0.1)
        p.ChangeDutyCycle(0)

ser = serial.Serial('/dev/ttyAMA0', 115200)  # open serial port
#print(ser.name)
servoPIN = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(servoPIN, GPIO.OUT)

p = GPIO.PWM(servoPIN, 50)
p.start(0)
camPosition = 1 # -1 - undefined; 1 - ST; 2 - OT
state = 0
received = ''
st_enable = False
ot_enable = False
rd_enable = False
while True:

    while(ser.inWaiting()):
        c = ser.read()
        if (state == 0):
            if (c == b'$'):
                state = 1
        elif (state == 1):
            if (c == b'\n'):
                print('Received: ' + received)
                if (received[0:2] == 'ST'):
                    if (received[2] == '1'):
                        ot_enable = False
                        st_enable = True
                        if (camPosition != 1):
                            setCamPos(1)
                            camPosition = 1
                    elif (received[2] == '0'):
                        st_enable = False
                elif (received[0:2] == 'OT'):
                    if (received[2] == '1'):
                        st_enable = False
                        ot_enable = True
                        if (camPosition != 2):
                            setCamPos(2)
                            camPosition = 2
                    elif (received[2] == '0'):
                        ot_enable = False
                elif (received[0:2] == 'RD'):
                    if (received[2] == '1'):
                        rd_enable = True
                    elif (received[2] == '0'):
                        rd_enable = False
                state = 0
                received = ''
            else:
                received += c.decode()
        

    stX = 1.5
    stY = 3.452
    stRho = 1.4563
    toSend = '$ST:' + str(stX) + ':' + str(stY) + ':' + str(stRho) + '\n'
    ser.write(toSend.encode())

    
        
    time.sleep(0.1)
ser.close()
p.stop()
GPIO.cleanup()
