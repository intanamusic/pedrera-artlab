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


@content.OSC('/protopixel/0')
def video_play(action):
    """
    This function handles OSC user input in address "/acc", with 3 arguments: x,y,z.
    Use pl.OSC decorator to define handles like this. Multiple scripts can listen to
    the same address simultaneously.
    """
    #print action
    
    if   20 < action < 40:
        callParameterContent("vv0", "params:resume")
        print "Resume!!"
    else:
        callParameterContent("vv0", "params:pause")
        print "Pause!!"


@content.OSC('/protopixel/1')
def video_play(action):
    """
    This function handles OSC user input in address "/acc", with 3 arguments: x,y,z.
    Use pl.OSC decorator to define handles like this. Multiple scripts can listen to
    the same address simultaneously.
    """
    
    print action
    if   10 < action < 30:
        callParameterContent("vv2", "params:resume")
        print "Resume!!"
    else:
        callParameterContent("vv2", "params:pause")
        print "Pause!!"

@content.OSC('/protopixel/2')
def video_play(action):
    """
    This function handles OSC user input in address "/acc", with 3 arguments: x,y,z.
    Use pl.OSC decorator to define handles like this. Multiple scripts can listen to
    the same address simultaneously.
    """
    if 10 < action < 30:
        callParameterContent("vv3", "params:resume")
        print "Resume!!"
    else:
        callParameterContent("vv3", "params:pause")
        print "Pause!!"