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

    xmin = min([result[0][0] for result in results]) - 10
    xmax = max([result[0][-1] for result in results]) + 10
    ymin = min([min(result[1]) for result in results]) - 10
    ymax = max([max(result[1]) for result in results]) + 10
    
    for entry in results:
        plt.plot(entry[0], entry[1], linewidth = 2, label = entry[2])
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.yaxis.set_major_formatter(ScalarFormatter())
    ax.set_xticks([10*x for x in range(11)])
    ax.set_yticks([10*x for x in range(21)])
    #plt.axis([0,100,0,100])
    plt.axis([max(0,xmin),xmax,max(0,ymin),min(100,ymax)])
    #plt.axis('tight')
    plt.title(title)
    plt.ylabel('Tunnistuksen tarkkuus (%)')
    plt.xlabel('Opetetut puhujat')
    plt.legend(loc = 0)
    #plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,ncol=2, borderaxespad=0.) # legend above the plot
    ax.grid(True)
    #plt.savefig("{}.png".format(title))
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
