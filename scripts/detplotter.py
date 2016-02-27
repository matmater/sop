 # -*- coding: utf-8 -*-

# some code from https://jeremykarnowski.wordpress.com/2015/08/07/detection-error-tradeoff-det-curves/

from matplotlib import pyplot as plt
from matplotlib.ticker import ScalarFormatter
import sys
import os

def DETCurve(results, eerDcf, title):
    """
    Given false positive and false negative rates, produce a DET Curve.
    The false positive rate is assumed to be increasing while the false
    negative rate is assumed to be decreasing.
    """
    axis_min = min(min(results))
    fig,ax = plt.subplots()
    for r in range(len(results)):
        plt.plot(results[r][0], results[r][1], linewidth = 2, label = results[r][2])
        if r == len(results)-1:
            plt.plot(eerDcf[r][2][0], eerDcf[r][2][1], 'kD', markersize=6, label = "DCF minimipisteet")
        else:
            plt.plot(eerDcf[r][2][0], eerDcf[r][2][1], 'kD', markersize=6)
    plt.yscale('logit')
    plt.xscale('logit')
    ticks_to_use = [0.01,0.02,0.05,0.1,0.2,0.4,0.6,0.8,0.9]
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.set_xticks(ticks_to_use)
    ax.set_yticks(ticks_to_use)
    plt.axis([0.005,0.9,0.005,0.9])
    #plt.axis('tight')
    plt.title(title)
    plt.xlabel(u"Väärät hyväksymiset (%)")
    plt.ylabel(u"Väärät hylkäykset (%)")
    plt.legend(loc = 0)
    for l in ax.get_xticklabels()[::2]:
        l.set_visible(False)
    #plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,ncol=2, borderaxespad=0.) # legend above the plot
    ax.grid(True)
    #plt.savefig("{}.png".format(title))
    plt.show()
    
def GetResults(files):
    results = []
    
    for f in files:
        resultsCor = []
        resultsInc = []
        with open(f[0], 'r') as file:
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
        
        #print resultsCor
        #print resultsInc 
        
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
            fpr.append(entry/inc)
        for entry in fns:
            fnr.append(entry/cor)
        results.append([fpr,fnr, f[1]])
        
    return results
    
def EERandDCF(results):
    eerDcf = []
    for entry in results:
        minDcf = 109.0
        minDiff = 10.0
        for i in range(len(entry[0])):          
            dcf = 0.1*entry[1][i] + 0.99*entry[0][i]    # 0.1 * P(false negative) + 0.99 * P(false positive)
            if dcf < minDcf:
                minDcf = dcf
                dcfPoint = [entry[0][i],entry[1][i]]
            if abs(entry[0][i] - entry[1][i]) < minDiff:        
                minDiff = abs(entry[0][i] - entry[1][i])
                eer = entry[0][i]
        eerDcf.append([eer, minDcf, dcfPoint])
    
    return eerDcf
 
if __name__ == '__main__':
    if len(sys.argv) > 1:
        folder = sys.argv[1]            
    else:
        folder = "verificationresults"
    files = []
    for file in os.listdir(folder):
        files.append("{}/{}".format(folder, file))
    results = GetResults(files)
    
    eerDcf = EERandDCF(results)
    print eerDcf
    DETCurve(results, eerDcf)
