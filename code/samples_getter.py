import feature_extractor as fe
from sys import argv
import os
	
if __name__ == '__main__':
    '''
    command line arguments:
    name of output file without .txt, test or train
    type (mfcc or lpcc)
    amount of deltas (0, 1 or 2)
    '''
    
    outfile = "train"
    type = "mfcc"
    delta_amount = 2

    if len(argv) > 1:
        outfile = argv[1]
    if len(argv) > 2:
        type = argv[2]
    if len(argv) > 3:
        delta_amount = int(argv[3])
	
    if outfile == "train":
        #which files are chosen in train set?
        ubm_x = 100
        xstart = 50
        xend = 55
        ystart = 1
        yend = 11
    elif outfile == "test":
        #which files are chosen in test set?
        xstart = 50
        xend = 80
        ystart = 50
        yend = 54
    
    for x in range(xstart,xend):
        if (outfile == "train" and x > ubm_x):
            name = ".ubm"
            pathname = str(x - xend + xstart + 225)
        else:
            pathname = str(x + 225)
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
                fe.extract_and_save(filename, name, outfile, type, delta_amount)