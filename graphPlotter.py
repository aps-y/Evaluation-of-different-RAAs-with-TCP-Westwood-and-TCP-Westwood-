# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os


# %%
raas = ["Arf", "Aarf","Aarfcd", "Onoe", "Minstrel"]
tcps = ["TcpWestwood","TcpWestwoodPlus"]


# %%
def plotGraph(raa,tcp,nWifi):
    throughputPath = "../Throughput_" + raa + "_" + tcp + "_" + str(nWifi) + ".csv"
    delayPath = "../Delay_" + raa + "_" + tcp + "_" + str(nWifi) + ".csv"
    thrputsPd = pd.read_csv(throughputPath,header=None)
    delaysPd = pd.read_csv(delayPath, header=None)
    pathToResult = "../Results/"+str(nWifi)+"nodes/"+tcp
    pathToResultDelay = pathToResult + "/Delay/"
    pathToResultThroughput = pathToResult + "/Throughput/"
    print(pathToResultDelay)
    print(pathToResultThroughput)
    if not os.path.isdir(pathToResultDelay):

        os.makedirs(pathToResultDelay)
    if not os.path.isdir(pathToResultThroughput):
        os.makedirs(pathToResultThroughput)
    thrputs = thrputsPd.to_numpy()
    delays = delaysPd.to_numpy()
    thrputs = thrputs[:,0]
    delays = delays[:,0]
    
    time = [0.2*i for i in range(1,len(delays)+1)]
    plt.plot(time,delays)
    plt.xlabel("Time (seconds)")
    plt.ylabel("Delay (seconds)")
    plt.title(raa)
    plt.savefig(pathToResultDelay+raa+".png")
    plt.close()
    
    plt.plot(time,thrputs)
    plt.xlabel("Time (seconds)")
    plt.ylabel("Throughput (Mb/s)")
    plt.title(raa)
    plt.savefig(pathToResultThroughput+raa+".png")
    plt.close()


# %%
for nWifi in range(1,11):
    for raaNum in range(0,5):
        plotGraph(raas[raaNum],tcps[0],nWifi)
        plotGraph(raas[raaNum],tcps[1],nWifi)


