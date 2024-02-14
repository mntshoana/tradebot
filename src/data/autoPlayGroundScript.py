#!/usr/bin/env python3.8
import sys
sys.path = ['', '/Library/Frameworks/Python.framework/Versions/3.8/lib/python38.zip', '/Library/Frameworks/Python.framework/Versions/3.8/lib/python3.8', '/Library/Frameworks/Python.framework/Versions/3.8/lib/python3.8/lib-dynload', '/Library/Frameworks/Python.framework/Versions/3.8/lib/python3.8/site-packages']
import io
import os.path as pathcheck
from PIL import Image
import matplotlib.pyplot as plt
import matplotlib

from multiprocessing import shared_memory
import signal
import pandas as pd

import pandas.io.common
import time
  
  
# Globals
RGBMEMSIZE = 4
IMGLENGTH = 4
IMGMEMSIZE = 10240*4 - IMGLENGTH
MEMSIZE = IMGMEMSIZE + IMGLENGTH + RGBMEMSIZE

# Shared Memory - existing
shm_a = shared_memory.SharedMemory(name='autoPG', create=False)

def writeIntToMem(startIndex, endIndex, value):
    shm_a.buf[startIndex:endIndex] = value.to_bytes(endIndex-startIndex, byteorder=sys.byteorder, signed=True)
    return
    
def readIntFromMem(startIndex, endIndex):
    value = int.from_bytes(shm_a.buf[startIndex:endIndex], byteorder=sys.byteorder, signed=True)
    return value
    
class Autoplayground():
    def __init__(self = None):
        self.buf = io.BytesIO()
        plt.tight_layout()
    
    def load(self):
        if not (pathcheck.isfile(sys.argv[1])):
            print ("File doesn't exist... waiting one second")
            return
        print("Autoplayground: Loading [" + sys.argv[1] + "]")
        try:
            self.data = pd.read_csv(sys.argv[1])
        except pandas.io.common.EmptyDataError:
            print ("File is empty... waiting to load data")
            return
        self.data.columns = ['sequence', 'timestamp', 'price', 'volume', 'Buy?']
        # Remove common garbage data from network errors
        index_names = self.data[ (self.data['price'] <= 50) | (self.data['volume'] == 0)].index
        self.data.drop(index_names, inplace=True)
        #Plot
        fig = plt.figure(figsize=(9.4,2), dpi=100.0)
        axes = fig.add_subplot(111)
        # +1 to ignore alpha channel , read 1 byte r, g and b
        RGB = readIntFromMem(IMGMEMSIZE+IMGLENGTH, MEMSIZE)
        R = (RGB & 0xFF) / 255
        G = ((RGB >> 8) & 0xFF) / 255
        B = ((RGB >> 16) & 0x0FF) / 255
        axes.set_facecolor((R, G, B))
        fig.patch.set_facecolor((R,G,B))
        axes.spines['top'].set_color('none')
        axes.spines['right'].set_color('none')
        axes.spines['bottom'].set_color('none')
        axes.spines['left'].set_color('none')
        axes.tick_params(axis='x', colors=(1-R, 1-G, 1-B))
        axes.tick_params(axis='y', colors=(1-R, 1-G, 1-B))
        
        axes.plot(self.data['price'],'-')
        axes.yaxis.set_ticks_position('both')
    
        self.buf.truncate(0)
        self.buf.seek(0)
        plt.savefig(self.buf, format='png', facecolor=fig.get_facecolor(), edgecolor='none')
        length = self.buf.getbuffer().nbytes

        self.buf.seek(0)
        print("Length of buffer: ", length)
        shm_a.buf[0:length] = self.buf.read()
        writeIntToMem(IMGMEMSIZE, IMGMEMSIZE+IMGLENGTH, length)

        plt.close(fig)


    
if __name__ == "__main__":
    print("\nAutoplayground: Child process initiated")
    writeIntToMem(0, 4, value=1)
    time.sleep(4) #takes some time to make sure connection is well established
    response = readIntFromMem(512, 512+4)
    if (response == 1):
        print("Autoplayground: Respnse received")
    else:
        print("Autoplayground: Unexpected respnse received: " + str(response))
    
    app = Autoplayground()
    i = 0
    while(True):
        app.load()
        i =  i+1
        if (i>7):
            i = 0
        time.sleep(4)

    buf.close()
