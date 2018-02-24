from openframeworks import *
from protopixel import Content
from protopixel.core import (getContents, getWorkSpaceItems,getConfContent,setConfContent)

size = 255

# color is the current color displayed,
# color2 is the last color received
color = ofColor(255, 255, 255)
color2 = ofColor(255, 255, 255)

content = Content('Custom OSC Script')
content.FBO_SIZE = (size, size)


def update():
    """
    As we receive these messages at a lower rate than the framerate, we use a running
    average to interpolate between values.
    """
    pass


def draw():
    """
    We use r,g,b to draw the three bars.
    """
    pass


def normalize(c):
    """
    Map the values from the accelerometer to 0-255
    """
    pass


@content.OSC('/protopixel/0')
def acceleration(x):
    """
    This function handles OSC user input in address "/acc", with 3 arguments: x,y,z.
    Use pl.OSC decorator to define handles like this. Multiple scripts can listen to
    the same address simultaneously.
    """
    if x < 10:
	    setConfContent('video','enabled',True)
    
