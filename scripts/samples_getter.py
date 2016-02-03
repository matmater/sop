import feature_extractor as fe
from sys import argv
import os
import silence_remover as sr
	
if __name__ == '__main__':
    '''
    command line arguments:
    name of output file without .txt, test or train
    type (mfcc or lpcc)
    amount of deltas (0, 1 or 2)
    remove silence (0 (no), 1 (yes))
    '''
    
    outfile = "train"
    type = "mfcc"
    delta_amount = 2
    
    remove_silence = 1

    if len(argv) > 1:
        outfile = argv[1]
    if len(argv) > 2:
        type = argv[2]
    if len(argv) > 3:
        delta_amount = int(argv[3])
    if len(argv) > 4:
        remove_silence = int(argv[4])
	
    if outfile == "train":
        #which files are chosen in train set?
        ubm_x = 5
        xstart = 0
        xend = 10
        ystart = 1
        yend = 5
    elif outfile == "test":
        #which files are chosen in test set?
        xstart = 0
        xend = 2
        ystart = 50
        yend = 52
    
    for x in range(xstart,xend):
        pathname = str(x + 225)
        if (outfile == "train" and x > ubm_x):
            name = ".ubm"
        else:
            name = pathname
        for y in range(ystart, yend):
            if y < 10:
                z = "00"
            elif y < 100:
                z = "0"
            else:
                z = ""
            if (outfile == "test"):
                name = pathname + "_" + str(y)
            filename = "VCTK-Corpus\wav48\p"+pathname+"\p"+pathname+"_"+z+str(y)+".wav"
            if os.path.exists(filename):
                if (remove_silence != 0):
                    sr.remove_silence(filename)
                    filename = filename + ".sr"
                    
                    fe.extract_and_save(filename, name, outfile, type, delta_amount)
                    print filename
                    os.remove(filename)
                    
                else:
                    print filename
                    fe.extract_and_save(filename, name, outfile, type, delta_amount)