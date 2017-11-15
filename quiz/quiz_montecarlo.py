#!/usr/bin/python
####################
#     QUIZ 10      #
# Logan Wan (lwan) #
####################

##########
# Task 1 #
##########

import numpy as np


def guessPi():
    NUM_COCOS = [1, 10, 100, 1000, 10000, 100000, 1000000]

    for num in NUM_COCOS:
        in_circle = 0.0
        # assuming circle radius 1, square side 2
        for coco in range(num):
            x = np.random.uniform(-1.0, 1.0)
            y = np.random.uniform(-1.0, 1.0)
            #import ipdb; ipdb.set_trace()
            if (np.sqrt(np.square(x) + np.square(y)) <= 1.0):
                in_circle += 1.0

        mypi = 4.0 * in_circle/num
        print 'NUM_COCOS  =', num
        print 'Pi equals: ', mypi
        print 'Error: ', np.absolute(1 - mypi / np.pi)

    return mypi


if __name__ == '__main__':
    guessPi()

'''
##########
# Task 2 #
##########

NUM_COCOS  = 1
Pi equals:  4.0
Error:  0.273239544735
NUM_COCOS  = 10
Pi equals:  3.2
Error:  0.0185916357881
NUM_COCOS  = 100
Pi equals:  3.48
Error:  0.10771840392
NUM_COCOS  = 1000
Pi equals:  3.14
Error:  0.000506957382897
NUM_COCOS  = 10000
Pi equals:  3.1508
Error:  0.00293078938789
NUM_COCOS  = 100000
Pi equals:  3.14468
Error:  0.000982732884443
NUM_COCOS  = 1000000
Pi equals:  3.140324
Error:  0.000403824979774


##########
# Task 3 #
##########
Monte carlo does not seems like an efficient means to approximate pi, since we
need a large number of samples to accurately approximate it. To get an accuracy
within 0.001, it looks like we'll need at least 100,000 samples. That's a lot
of computation. 

Perhaps we can rasterize a circle on top of a square with some finite 
resolution? We can then find the fraction of the square's area covered by the 
circle by simply dividing the number of pixels in the circle by the square, and
multiply it by the area of the square. This also seems like a lot of 
computation still, but would probably yield more accurate results more quickly. 

'''
