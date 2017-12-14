# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np

def findLED(image, x_min, x_max, delta=50):

	def sum(y_0):
		return np.sum(image[x_min:x_max,y_0-delta:y_0+delta])

	def pos(y_0):
		y_pos = 0.
		for y in range(y_0-delta, y_0+delta):
			for x in range(x_min, x_max):
				y_pos = y_pos + y*image[x,y]
		return y_pos/sum(y_0)

	def init():
		y_max = 0
		max = 0

		for i in range(1,640//delta-1):
			y_0 = delta*i
			if sum(y_0) > max:
				max = sum(y_0)
				y_max = y_0

	return pos(y_max)
		


# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
#camera.resolution = (1280, 720)
camera.resolution = (640, 480)
camera.framerate = 32
#rawCapture = PiRGBArray(camera, size=(1280, 720))
rawCapture = PiRGBArray(camera, size=(640, 480))

#camera.iso = 100
#camera.shutter_speed = 2000

# allow the camera to warmup
time.sleep(0.1)

# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
	# grab the raw NumPy array representing the image, then initialize the timestamp
	# and occupied/unoccupied text
	image = frame.array

	#print(camera.exposure_speed)

	# extract channels
	b_channel, g_channel, r_channel = cv2.split(image)

	def cross(image, x_0, y_0, delta=20):
		for x in range(x_0-delta, x_0+delta):
			image[x,y_0] = 255

		for y in range(y_0-delta, y_0+delta):
			image[x_0,y] = 255

	#y_pos = findLED(r_channel,100,300)

	#cross(r_channel, 200, int(y_pos))

	#cross(r_channel, 100, 300)
	#cross(r_channel, 300, 300)

	#cross(r_channel, 100, 200)
	#cross(r_channel, 300, 200)

	
	#cv2.imshow("Frame red", r_channel)
	#cv2.imshow("Frame green", g_channel)
	#cv2.imshow("Frame blue", b_channel)

	params = cv2.SimpleBlobDetector_Params()
	#detector = cv2.SimpleBlobDetector_create(params)
	#keypoints = detector.detect(255-r_channel)
	#im_with_keypoints = cv2.drawKeypoints(r_channel, keypoints, np.array([]), (0,0,255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

	#cv2.imshow("Keypoints", im_with_keypoints)
	cv2.imshow("Keypoints", image)

	key = cv2.waitKey(1) & 0xFF

	# clear the stream in preparation for the next frame
	rawCapture.truncate(0)

	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		#print(image.dtype)
		#print(bw_image)
		#print(bw_image[0])
		#print(image[:,:,0].shape)
		#print(bw_image[0].shape)
		cv2.imwrite("test.jpg",image)
		break
