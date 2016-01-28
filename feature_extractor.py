from scikits.talkbox.linpred.levinson_lpc import levinson, lpc
from features import sigproc
from features import mfcc
from features import logfbank
from array import array
from struct import pack
from sys import byteorder
from sys import argv
import os
import numpy
import scipy.io.wavfile as wav
import wave
    
def get_mfcc(filename):
    '''
    gets mfccs for each frame in a wav file
    filename: name of wav file with .wav
    returns the mfcc features in each frame as a list of lists
    '''
    print "Getting MFCC"
    (rate,sig) = wav.read(filename)
    
    mfcc_feat = mfcc(sig,rate)

    print mfcc_feat
    return mfcc_feat
    
def get_lpcc(filename):
    '''
    gets lpccs for each frame in a wav file
    filename: name of wav file with .wav
    returns the lpcc features in each frame as a list of lists
    '''
    print "Getting LPCC"
    (rate,sig) = wav.read(filename)
    frames = sigproc.framesig(sig, 0.025*rate, 0.01*rate)
    lpccs = [[]]*len(frames)
    for x in xrange(0,len(frames)):
        lpcc_feat = lpc(frames[x], 12)
        for feature in lpcc_feat[0]:
            feature = float(feature)
        lpccs[x] = lpcc_feat[0]

    return numpy.asarray(lpccs)

def append_deltas(original, deltas):
    '''
    appends the delta features after the original features in each frame
    returns the new features which include the deltas
    '''
    print str(len(original)) + " " + str(len(deltas))
    newframes = [[]]*len(original)
    for t in xrange(0,len(original)):
        newframes[t] = numpy.concatenate([original[t],deltas[t]])

    return newframes


def get_all_deltas(all_frames):
    '''
    gets deltas for each frame in all_frames
    returns the deltas in each frame
    '''
    deltas = [0] * len(all_frames)
    for t in xrange(0,len(all_frames),1):
        deltas[t] = get_frame_deltas(t, all_frames)
    return deltas

def get_frame_deltas(t, all_frames):
    '''
    calculates deltas for a frame at index t in all_frames
    delta(t) = Sigma(n=1 to N)[n*(c(t+n)-c(t-n))] / (2 * Sigma(n=1 to N)[n^2])
    returns the deltas in the frame
    '''
        
    deltas = [0] * len(all_frames[0]);
    
    denom = 0.0
        
    for n in xrange(1, 3, 1):
        denom += n * n

    if denom == 0.0:
        denom = 1.0
		
    denom = 1.0 / denom
         
    for i in xrange(len(deltas)):
        numer = 0.0
            
        for n in xrange(1, 3, 1):
            if (t-n >=0):
                c0 = all_frames[t - n][i]
            else:
                c0= all_frames[0][i]
            if (t+n >= len(all_frames)):
                c1 = all_frames[len(all_frames)-1][i]
            else:
                c1 = all_frames[t+n][i]
            numer += n * (c1 - c0)

        deltas[i] = 0.5 * (numer * denom)
        
    return deltas
    
def save_coefficients(feat, ofile, labelname):
    '''
    saves given feature vectors in a new row in a file
    feat: a list of lists
    ofile: name of output file with .txt
    labelname: name in the beginning of the row
    '''
    file = open(ofile, 'a')
    file.write("{} ".format(labelname))
    for frame in feat:
        for feature in frame:
            file.write(str(feature))
            file.write(' ')
        file.seek(-1, os.SEEK_END)
        file.truncate()
        file.write(',')
    file.seek(-1, os.SEEK_END)
    file.truncate()  
    file.write('\n')	
    file.close()
	
def extract_and_save(wavname, name, outfile, type = "mfcc", delta_amount = 2):
    '''
    gets the wanted feature vectors from a wav file and saves them in a given file
    type: "mfcc" or "lpcc"
    wavname: name of the wav file
    name: name of speaker
    outfile: name of the txt file without .txt ending
    delta_amount: how many deltas are calculated. 0 for none, 1 for deltas, 2 for deltas+delta-deltas.
    
    returns true if successful, false if not
    '''
    try:
        if type == "mfcc":
            feats = get_mfcc(wavname)
        elif type == "lpcc":
            feats = get_lpcc(wavname)
    
        if (delta_amount > 0):
            deltas = get_all_deltas(feats)
        if (delta_amount > 1):
            delta_deltas = get_all_deltas(deltas)
        if (delta_amount > 0):
            feats = append_deltas(feats,deltas)
        if (delta_amount > 1):
            feats = append_deltas(feats, delta_deltas)
        save_coefficients(feats, "{}.txt".format(outfile), name)
        
        print "Finished, samples added to {}.txt".format(outfile)
        return True
        
    except:    
        return False
    