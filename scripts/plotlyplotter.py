# Get this figure: fig = py.get_figure("https://plot.ly/~jeremy.karnowski/6/")
# Get this figure's data: data = py.get_figure("https://plot.ly/~jeremy.karnowski/6/").get_data()
# Add data to this figure: py.plot(Data([Scatter(x=[1, 2], y=[2, 3])]), filename ="plotly-log-axes", fileopt="extend"))
# Get y data of first trace: y1 = py.get_figure("https://plot.ly/~jeremy.karnowski/6/").get_data()[0]["y"]

# Get figure documentation: https://plot.ly/python/get-requests/
# Add data documentation: https://plot.ly/python/file-options/

# You can reproduce this figure in Python with the following code!

# Learn about API authentication here: https://plot.ly/python/getting-started
# Find your api_key here: https://plot.ly/settings/api


if __name__ == '__main__':
    import plotly.plotly as py
    from plotly.graph_objs import *
    py.sign_in('santerim', '471tt4gor6')      #need sign-in info

    resultsCor = []
    resultsInc = []
    
    # recresults.txt has values for correct speakers on the first line and incorrect speakers on the second line
    with open('verificationResults.txt', 'r') as file:
        i = 0
        for line in file:
            if i = 0:
                resultsCor += line.split()
            else:
                resultsInc += line.split()
            i += 1
            i %= 2

    resultsCor = [float(i) for i in resultsCor]
    resultsInc = [float(i) for i in resultsInc]
    cor = float(len(resultsCor))
    inc = float(len(resultsInc))

    i = min(resultsInc)
    fns = []
    fps = []
    while i <= max(resultsCor):
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
        i += 0.01 
    fpr = []
    fnr = []
    for entry in fps:
        fpr.append(100*entry/inc)
    for entry in fns:
        fnr.append(100*entry/cor)

    data = Data([
        Scatter(
            x = fpr,
            y = fnr
        )
    ])
    layout = Layout(
        autosize=True,
        height=454,
        title='Detection Error Tradeoff (DET) Curve',
        width=838,
        xaxis=XAxis(
            autorange=False,
            range=[-2.9999999999999996, 1],
            ticks=[0, 0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 40],
            title='False Positive Rate (%)',
            type='log'
        ),
        yaxis=YAxis(
            autorange=False,
            range=[-2.9999999999999996, 1],
            ticks=[0, 0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 40],
            title='False Negative Rate (%)',
            type='log'
        )
    )
    fig = Figure(data=data, layout=layout)
    plot_url = py.plot(fig)