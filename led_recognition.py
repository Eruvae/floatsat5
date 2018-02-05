# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np
from scipy.optimize import fsolve
import sys
import serial

ser = serial.Serial('/dev/ttyAMA0', 115200) # open serial port

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
#camera.resolution = (1280, 720)
camera.resolution = (640, 480)
camera.framerate = 32
#rawCapture = PiRGBArray(camera, size=(1280, 720))
rawCapture = PiRGBArray(camera, size=(640, 480))

stream = picamera.array.PiRGBArray(camera)

camera.exposure_mode = "off"
#camera.iso = 100
camera.shutter_speed = 10000

# allow the camera to warmup
time.sleep(0.1)

a = 9.3e-2
c = 1.91e-3

def cross(image, x_0, y_0, delta=10):
    for x in range(x_0-delta, x_0+delta):
        image[x,y_0] = 255

    for y in range(y_0-delta, y_0+delta):
        image[x_0,y] = 255

def f(x, *args):
    alpha, G_0, g_0 = x
    z_1, z_2, z_3 = args
    return (c*z_1-(G_0+a*np.sin(alpha))/(g_0-a*np.cos(alpha)), c*z_2-(G_0+a*np.sin(alpha+2./3.*np.pi))/(g_0-a*np.cos(alpha+2./3.*np.pi)), c*z_3-(G_0+a*np.sin(alpha+4./3.*np.pi))/(g_0-a*np.cos(alpha+4./3.*np.pi)))

def detect_leds(image, x_0):

    def reduce_noise(mask):
        mask = cv2.erode(mask, None)
        mask = cv2.dilate(mask, None)
        return mask

    def centeroid(mask):
        im2, contours, hierachy = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        # search for the spot with largest area

        if len(contours) == 0:
            return -1
        else:
            cont = contours[0]

        for cnt in contours:
            if cv2.contourArea(cnt) > cv2.contourArea(cont):
                cont = cnt

            #cv2.drawContours(image, cont, -1, (0,255,0), 3)

            x_pos = 0

            num = 0
            for x in cont[:,0,0]:
                x_pos += x
                num += 1

                if num == 0:
                    return -1
                else:
                    return x_pos//num

    def calculate_pos(x_0, x_r, x_g, x_b):
        x_values = (x_r, x_g, x_b)

        for x in x_values:
            if x == -1:
                return x_0, False		
 
        args_0 = (x_r-320,x_g-320,x_b-320)
        return fsolve(f, x_0, args=args_0), True

    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    r_mask = cv2.inRange(hsv_image, np.array([170,50,50]), np.array([180,255,255]))
    g_mask = cv2.inRange(hsv_image, np.array([32,50,50]), np.array([52,255,255]))
    b_mask = cv2.inRange(hsv_image, np.array([102,50,50]), np.array([122,255,255]))

    #r_mask = reduce_noise(r_mask)
    #g_mask = reduce_noise(g_mask)
    #b_mask = reduce_noise(b_mask)

    x_r = centeroid(r_mask)
    x_g = centeroid(g_mask)
    x_b = centeroid(b_mask)

    #cross(image, 100, x_r)
    #cross(image, 100, x_g)
    #cross(image, 100, x_b)

    #cv2.imshow("mask", r_mask)

    (alpha, G_0, g_0), trusted = calculate_pos(x_0, x_r, x_g, x_b)

    #print(alpha*180./np.pi, G_0, g_0)
    return (-alpha, g_0, G_0), trusted

def generate_stats(image,x,y):
    return image[x-10:x+10,y-10:y+10]

xpos = 100
ypos = 100

x_0 = (0.35, 0.05, 0.3)

# capture frames from the camera
#for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
while True:
    # grab the raw NumPy array representing the image, then initialize the timestamp
    # and occupied/unoccupied text
    #image = np.copy(frame.array)
    camera.capture(stream, 'bgr', use_video_port=True)
    image = stream.array

    #print(camera.exposure_speed)

    x = detect_leds(image, x_0)

    print(x)
    toSend = '$OT:' + str(x[0][0]) + ':' + str(x[0][1]) + ':' + str(x[0][2]) + ':' + str(int(x[1])) + '\n'
    ser.write(toSend.encode())

    #cv2.imshow("Keypoints", im_with_keypoints)
    #cv2.imshow("image", image)
    #cv2.imshow("mask", b_mask)

    key = cv2.waitKey(1) & 0xFF

    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)

    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break

    if key == ord("e"):
        cv2.imwrite("test.jpg",image)

    if key == ord("g"):
        hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        np.save("hsv",hsv_image)

    if key == ord("p"):
        print(image[xpos,ypos,:])
        print(hsv_image[xpos,ypos,:])

    cv2.imshow("image", image)
    stream.seek(0)
    stream.truncate()
