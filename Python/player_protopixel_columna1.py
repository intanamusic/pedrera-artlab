from openframeworks import *
from protopixel import Content
import numpy as np


# We neeed access to low level API calls for communication with core
from protopixel.core import setConfContent, callParameterContent

content = Content("Video Script")



def setup():
    print "setup player"

def draw():
    pass


@content.OSC('/waves1P/1')
def video_play(action):
    """
    This function handles OSC user input in address "/acc", with 3 arguments: x,y,z.
    Use pl.OSC decorator to define handles like this. Multiple scripts can listen to
    the same address simultaneously.
    """
    print ("WORKING!")
    print action
    
    if   action==True:
        callParameterContent("columna1", "params:resume")
        print "Play"
    else:
        callParameterContent("columna1", "params:pause")
        print "Pause"

