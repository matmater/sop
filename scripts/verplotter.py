from matplotlib import pyplot as plt
from matplotlib.ticker import ScalarFormatter
import detplotter as dp
import sys
import os

"""
WIP
Plots min DCF values as the function of the number of trained sample files for verification results
"""

def DrawCurve(results):

    fig,ax = plt.subplots()
    results.sort()
    print results
    plt.plot([5,10], results, 'bD')
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.set_xticks([5*x for x in range(5)])
    ax.set_yticks([0.02*x for x in range(21)])
    plt.axis([0, 15, 0, 0.2])
    #plt.axis('tight')
    plt.ylabel('min DCF')
    plt.xlabel('Sample files(esim?)')
    #plt.legend(loc = 0)
    #plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,ncol=2, borderaxespad=0.) # legend above the plot
    ax.grid(True)
    plt.show()    

if __name__ == '__main__':
    if len(sys.argv) > 1:
        folder = sys.argv[1]            
    else:
        folder = "verificationresults"
    files = []
    for file in os.listdir(folder):
        files.append("{}/{}".format(folder, file))
    results = dp.GetResults(files)
    EerDcf = dp.EERandDCF(results)
    print EerDcf
    DrawCurve([i[1] for i in EerDcf])