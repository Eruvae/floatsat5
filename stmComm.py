import serial
import RPi.GPIO as GPIO
import time
import cv2
import picamera
import picamera.array
import numpy
import radioPositioning
import starTracker
from led_recognition import detect_leds

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
        p.ChangeDutyCycle(8.5)
        time.sleep(0.1)
        p.ChangeDutyCycle(0)
    elif (index == 2):
        p.ChangeDutyCycle(8)
        time.sleep(0.1)
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

def setup_camera_st(camera, stream):
    camera.resolution = (320, 240)
    camera.brightness = 30
    camera.contrast = 95
    camera.framerate = 30
    camera.exposure_mode = "auto"
    camera.shutter_speed = 0
    time.sleep(0.1)

def setup_camera_ot(camera, stream):
    camera.resolution = (640, 480)
    camera.brightness = 50
    camera.contrast = 0
    camera.framerate = 32
    camera.exposure_mode = "off"
    camera.shutter_speed = 1000
    time.sleep(0.1)

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

camera = picamera.PiCamera()
stream = picamera.array.PiRGBArray(camera)
print(camera.resolution)
print(camera.brightness)
print(camera.contrast)
print(camera.framerate)
print(camera.exposure_mode)
print(camera.shutter_speed)

debug = False
# Read ST catalogs into memory
star_catalog = starTracker.read_catalog()
# Open Pi camera stream
try:
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
                            setup_camera_st(camera, stream)
                            if (camPosition != 1):
                                setCamPos(1)
                                camPosition = 1
                        elif (received[2] == '0'):
                            st_enable = False
                    elif (received[0:2] == 'OT'):
                        if (received[2] == '1'):
                            st_enable = False
                            ot_enable = True
                            setup_camera_ot(camera, stream)
                            print(camera.shutter_speed)
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

        ser.write(("$RS:" + str(int(st_enable)) + ":" + str(int(ot_enable)) + ":" + str(int(rd_enable)) + "\n").encode())
        # Radio positioning
        if (rd_enable):
            data = radioPositioning.get_position()
            if data:
                ser.write(("$RD:%4.2f:%4.2f:%4.2f:%4.2f\n" % (data[0].x, data[0].y, data[1].x, data[1].y)).encode())
        if (st_enable):
            # Get camera stream for ST/OT
            camera.capture(stream, 'bgr', use_video_port=True)

            # Convert frame to greyscale
            frame = stream.array
            image = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            data = starTracker.track_stars(image, star_catalog, False, debug)
            if data:
                # convert mm to m
                x = numpy.round(data['x'] / 1000.0, 4)
                y = numpy.round(data['y'] / 1000.0, 4)
                theta = data['theta']
                ser.write(("$ST:%4.2f:%4.2f:%4.2f\n" % (x, y, theta)).encode())

                # reset camera stream for next frame
            stream.seek(0)
            stream.truncate()
        elif (ot_enable):
            #TODO
            camera.capture(stream, 'bgr', use_video_port=True)
            frame = stream.array

            data = detect_leds(frame, False)

            toSend = '$OT:' + str(data[0][0]) + ':' + str(data[0][1]) + ':' + str(data[0][2]) + ':' + str(int(data[1])) + '\n'
            ser.write(toSend.encode())

            stream.seek(0)
            stream.truncate()
        else:
            time.sleep(0.1)
                    
        ##    stX = 1.5
        ##    stY = 3.452
        ##    stRho = 1.4563
        ##    toSend = '$ST:' + str(stX) + ':' + str(stY) + ':' + str(stRho) + '\n'
        ##    ser.write(toSend.encode())

except KeyboardInterrupt:
    ser.close()
    p.stop()
    GPIO.cleanup()
