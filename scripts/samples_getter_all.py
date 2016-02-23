import feature_extractor as fe
from sys import argv
import os

"""
Put all samples (mfcc, deltas and delta deltas) of speaker n to file samples_n.txt. Repeat for all speakers.
By uncommenting the lines with i, number skipping will be avoided in resulting files,
but the file names will not match the correct speaker. 
"""
	
if __name__ == '__main__':
    i = 1
    for x in range(152):
        print "Speaker {} in progress.".format(x + 225) 
        pathname = str(x + 225)
        #outfile = "samples_{}".format(pathname)
        outfile = "samples_{}".format(i)        
        for y in range(505):
            if y < 10:
                z = "00"
            elif y < 100:
                z = "0"
            else:
                z = ""
            name = pathname + "_" + str(y)
            filename = "VCTK-Corpus\wav48\p"+pathname+"\p"+pathname+"_"+z+str(y)+".wav"
            if os.path.exists(filename):
                fe.extract_and_save(filename, name, outfile, "mfcc", 2)
                i += 1                
    
    
