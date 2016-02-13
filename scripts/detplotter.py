# some code from https://jeremykarnowski.wordpress.com/2015/08/07/detection-error-tradeoff-det-curves/
from matplotlib import pyplot as plt
from matplotlib.ticker import ScalarFormatter
import sys

def DETCurve(results):
    """
    Given false positive and false negative rates, produce a DET Curve.
    The false positive rate is assumed to be increasing while the false
    negative rate is assumed to be decreasing.
    """

    axis_min = min(min(results))
    fig,ax = plt.subplots()
    for entry in results:
        plt.plot(entry[0], entry[1], linewidth = 2)
    plt.yscale('log')
    plt.xscale('log')
    ticks_to_use = [0.01,0.02,0.05,0.1,0.2,0.5,1,2,5,10,20,50]
    #ticks_to_use = [0.001,0.05,1,10,50]
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.set_xticks(ticks_to_use)
    ax.set_yticks(ticks_to_use)
    #plt.axis([0.001,100,0.001,100])
    plt.axis('tight')
    plt.xlabel('false positive rate (%)')
    plt.ylabel('false negative rate (%)')
    plt.show()
    
def GetResults(files):
    results = []
    
    for j in range(len(files)):
        resultsCor = []
        resultsInc = []
        with open(files[j], 'r') as file:
            i = 0
            for line in file:
                if i == 0:
                    resultsCor += line.split()
                else:
                    resultsInc += line.split()
                i += 1
                i %= 2
           
        resultsCor = [float(i) for i in resultsCor]
        resultsInc = [float(i) for i in resultsInc]
        cor = float(len(resultsCor))
        inc = float(len(resultsInc))
        
        print resultsCor
        print resultsInc 
        
        i = max(resultsCor)
        fns = []
        fps = []
        while i >= min(resultsInc):
            irej = 0
            irec = 0
            for entry in resultsCor:
                if entry <= i:
                    irej += 1
            for entry in resultsInc:
                if entry > i:
                    irec += 1
            fns.append(irej)
            fps.append(irec)
            i -= 0.01
            
        fpr = []
        fnr = []
        for entry in fps:
            fpr.append(100*entry/inc)
        for entry in fns:
            fnr.append(100*entry/cor)
        results.append([fpr,fnr])
        
    return results

if __name__ == '__main__':
    files = []
    if len(sys.argv) > 1:
        for j in range(len(sys.argv) - 1):
            files.append(sys.argv[j+1])                
    else:
        files.append("verificationresults.txt")
    results = GetResults(files)
           
    DETCurve(results)