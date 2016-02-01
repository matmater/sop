import math
import scipy.io.wavfile as wav
import numpy as np

TRESHOLD = 30000 #Minimum energy of a frame, anything below this is considered silent
FRAMELENGTH = 0.025 #Length of frame

def remove_silence(filename):
    (rate,sig) = wav.read(filename)
    
    framelength = int(round(FRAMELENGTH * rate))
    
    frameamount = int(math.ceil(len(sig) / framelength))
    
    newsig = np.array([])
    
    for i in xrange(0,frameamount+1):
        start = i * framelength
        end =  start + framelength
        print end
        if (end > len(sig)):
            end = len(sig)
        if (start >= len(sig)):
            start = len(sig) - 1
        
        length = end - start
        energy = 0.0
        for j in xrange(start,end):
            energy = energy + pow(float(sig[j]), 2)
        energy = energy / length
        if (energy >= TRESHOLD):
            newsig = np.concatenate((newsig,sig[start:end]))
            
    newsig = newsig.astype(sig.dtype)
    print "silence removed, saving: "+ filename+".sr"
    wav.write(filename+".sr", rate, newsig)
    (rate,sig) = wav.read(filename+".sr")

    
if __name__ == '__main__':
    remove_silence("p334_229.wav")