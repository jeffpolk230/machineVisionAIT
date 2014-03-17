import cv2.cv as cv
import time

cv.NamedWindow("camera", cv.CV_WINDOW_NORMAL)
# anyone knows where's CV_WINDOW_OPENGL? it's in the documentation
# cv2 stuff!
capture = cv.CaptureFromCAM(0)
i=0
while True:
    img = cv.QueryFrame(capture)
#    cv.putText(img, "Press s for snap, Esc to quit")
    cv.ShowImage("camera", img)
    if cv.WaitKey(1) == ord('s'):
    	print "snaped!"
    	cv.SaveImage('pic{:>03}.jpg'.format(i), img)
    	i +=1
    if cv.WaitKey(1) == 27: # Esc
        break
#    time.sleep(1)
