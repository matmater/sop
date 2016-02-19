from matplotlib import pyplot as plt
from matplotlib.ticker import ScalarFormatter
import sys
import os

"""
Plots results of recognition tests.
Currently plots reliability on y-axis and amount of trained speakers on x-axis.
"""

def GetResults(files):
    allresults = []
    
    for f in files:
        results = []
        with open(f, 'r') as file:
            for line in file:
                result = line.split()
                result = [int(i) for i in result]
                results.append([result[0], 100 * result[1] / float((result[1] + result[2]))])
        results.sort()
        allresults.append([[i[0] for i in results], [i[1] for i in results]])
    
    return allresults
    
def DrawCurve(results):

    fig,ax = plt.subplots()
    for entry in results:
        plt.plot(entry[0], entry[1], linewidth = 2)
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.set_xticks([10*x for x in range(11)])
    ax.set_yticks([10*x for x in range(21)])
    plt.axis([max(0,min(entry[0])-10), min(100,max(entry[0])+10), max(0,min(entry[1])-10), min(100,max(entry[1])+10)])
    #plt.axis('tight')
    plt.ylabel('Recognition rate (%)')
    plt.xlabel('Trained Speakers')
    #plt.legend(loc = 0)
    #plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,ncol=2, borderaxespad=0.) # legend above the plot
    ax.grid(True)
    plt.show()    

if __name__ == '__main__':
    if len(sys.argv) > 1:
        folder = sys.argv[1]            
    else:
        folder = "recognitionresults"
    files = []
    for file in os.listdir(folder):
        files.append("{}/{}".format(folder, file))
    results = GetResults(files)
    print results
    DrawCurve(results)
