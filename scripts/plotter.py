from matplotlib import pyplot as plt
from matplotlib.ticker import ScalarFormatter
import detplotter as dp
import recplotter as rp
import sys
import os

def PlotVerification(tests):
    return
    
def PlotRecognition():
    return
    
if __name__ == '__main__':
    if len(sys.argv) > 1:
        f = sys.argv[1]            
    else:
        f = "tests.txt"

    files = []
    firstLine = True
    with open(f, 'r') as file:
        for line in file:
            if firstLine: 
                test = []
                testInfo = line.split("|")
                firstLine = False
                continue
            files.append(line.split("|"))
            
    testInfo[-1] = testInfo[-1].rstrip()
    for file in files:
        file[1] = file[1].rstrip()
    print testInfo
    print files
    
    if testInfo[1] == "ver":
        results = dp.GetResults(files)
        eerDcf = dp.EERandDCF(results)
        dp.DETCurve(results, eerDcf, testInfo[0])
    else:
        results = rp.GetResults(files)
        print results
        rp.DrawCurve(results, testInfo[0])
        
        