# -*- coding: utf-8 -*-
 
from matplotlib import pyplot as plt
from matplotlib.ticker import ScalarFormatter
import sys
import os

"""
Plots results of recognition tests.
Plots reliability on y-axis and amount of trained speakers on x-axis.
"""

def GetResults(files):
    allresults = []
    
    for f in files:
        results = []
        with open(f[0], 'r') as file:
            for line in file:
                result = line.split()
                result = [int(i) for i in result]
                results.append([result[0], 100 * result[1] / float((result[1] + result[2]))])
        results.sort()
        allresults.append([[i[0] for i in results], [i[1] for i in results], f[1]])
    
    return allresults
    
def DrawCurve(results, title):

    fig,ax = plt.subplots()
    xpadding = 10
    ypadding = 0.5
    xmin = min([result[0][0] for result in results]) - xpadding
    xmax = max([result[0][-1] for result in results]) + xpadding
    ymin = min([min(result[1]) for result in results]) - ypadding
    ymax = max([max(result[1]) for result in results]) + ypadding
    
    for entry in results:
        plt.plot(entry[0], entry[1], linewidth = 2, label = entry[2])
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.set_xticks([10*x for x in range(1,11)])
    ax.set_yticks([x for x in range(101)])
    plt.axis([max(0,xmin),xmax,max(0,ymin),min(100.2,ymax)])
    plt.title(title)
    plt.ylabel('Tunnistuksen tarkkuus (%)')
    plt.xlabel('Opetetut puhujat')
    plt.legend(loc = 0)
    ax.grid(True)
    plt.show()
    
