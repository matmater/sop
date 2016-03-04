 # -*- coding: utf-8 -*-

from matplotlib import pyplot as plt
from matplotlib.ticker import ScalarFormatter
from matplotlib.ticker import FuncFormatter
import sys
import os

def PlotDET(results, eerDcf, title):
    """
    Plots a DET curve with false positive and false negative rates (results).
    marks minDCF value (from eerDcf) for each curve.
    Some code based on https://jeremykarnowski.wordpress.com/2015/08/07/detection-error-tradeoff-det-curves/
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
    majorTicks = [0.01,0.02,0.05,0.1,0.2,0.4,0.6,0.8,0.9]
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.set_xticks(majorTicks)
    ax.set_yticks(majorTicks)
    plt.axis([0.005,0.9,0.005,0.9])
    plt.title(title)
    plt.xlabel(u"Väärät hyväksymiset (%)")
    plt.ylabel(u"Väärät hylkäykset (%)")
    plt.legend(loc = 0)
    plt.minorticks_off()
    
    def ToPercent(y, position):
        return int(y * 100)
        
    formatter = FuncFormatter(ToPercent)
    plt.gca().yaxis.set_major_formatter(formatter)
    plt.gca().xaxis.set_major_formatter(formatter)
    ax.grid(True)
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
        
        start = max(resultsInc)
        end = min(resultsCor)
        step = (start - end) / 500
        print "range: {} ... {}, step: {}".format(start, end, step)
        fns = []
        fps = []
        while start >= end:
            irej = 0
            irec = 0
            for entry in resultsCor:
                if entry <= start:
                    irej += 1
            for entry in resultsInc:
                if entry > start:
                    irec += 1
            fns.append(irej)
            fps.append(irec)
            start -= step
            
        fpr = []
        fnr = []
        for entry in fps:
            fpr.append(entry/inc)
        for entry in fns:
            fnr.append(entry/cor)
        results.append([fpr,fnr, f[1]])
        
    return results
    
def GetEERandDCF(results):
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
