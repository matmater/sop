#from https://jeremykarnowski.wordpress.com/2015/08/07/detection-error-tradeoff-det-curves/
from matplotlib import pyplot as plt
def DETCurve(fps,fns):
    """
    Given false positive and false negative rates, produce a DET Curve.
    The false positive rate is assumed to be increasing while the false
    negative rate is assumed to be decreasing.
    """
    axis_min = min(fps[0],fns[-1])
    fig,ax = plt.subplots()
    plt.plot(fps,fns)
    plt.yscale('log')
    plt.xscale('log')
    ticks_to_use = [0.001,0.002,0.005,0.01,0.02,0.05,0.1,0.2,0.5,1,2,5,10,20,50]
    #ticks_to_use = [1,2,5,10,20,50]
    #ax.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
    #ax.get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
    ax.set_xticks(ticks_to_use)
    ax.set_yticks(ticks_to_use)
    plt.axis([0.001,100,0.001,100])
    plt.xlabel('false positive rate (%)')
    plt.ylabel('false negative rate (%)')
    plt.show()
    
    
if __name__ == '__main__':
    file = open('recresults.txt', 'r')
    content = file.readlines()
    resultsCor = content[0].split()
    resultsInc = content[1].split()
    resultsCor = [float(i) for i in resultsCor]
    resultsInc = [float(i) for i in resultsInc]
    cor = float(len(resultsCor))
    inc = float(len(resultsInc))
    
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
     
    DETCurve(fpr,fnr)