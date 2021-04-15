import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os


def plotGraph(raa,tcp,nWifi,graph_type):
    throughputPath = "Throughput_" + raa + "_" + tcp + "_" + str(nWifi) + ".csv"
    delayPath = "Delay_" + raa + "_" + tcp + "_" + str(nWifi) + ".csv"
    thrputsPd = pd.read_csv(throughputPath,header=None)
    delaysPd = pd.read_csv(delayPath, header=None)
    pathToResult = "Results/"+str(nWifi)+"nodes/"+tcp
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
    
    if graph_type=="delay":
        time = [0.2*i for i in range(1,len(delays)+1)]
        plt.plot(time,delays,label=tcp)
        # plt.xlabel("Time (seconds)")
        # plt.ylabel("Delay (seconds)")
        # plt.title(raa)
        # plt.savefig(pathToResultDelay+raa+".png")
        # plt.close()
    
    if graph_type=="throughput":
        time = [0.2*i for i in range(1,len(delays)+1)]
        plt.plot(time,thrputs,label=tcp)
        # plt.xlabel("Time (seconds)")
        # plt.ylabel("Throughput (Mb/s)")
        # plt.title(raa)
        # plt.savefig(pathToResultThroughput+raa+".png")
        # plt.close()


raas = ["Arf", "Aarf","Aarfcd", "Onoe", "Minstrel"]
tcps = ["TcpWestwood","TcpWestwoodPlus"]
# for nWifi in range(2,8):
#     for raaNum in range(0,5):
#         plotGraph(raas[raaNum],tcps[0],nWifi)
#         plotGraph(raas[raaNum],tcps[1],nWifi)

for graph_type in ['throughput','delay']:
    for nWifi in range(2,8):
        for raa in raas:
            plt.figure(figsize=(20,12))
            plt.xlabel("Time (seconds)")
            if graph_type=="delay":
                plt.ylabel("Delay (seconds)")
                plt.title("Delay "+str(nWifi)+"nodes "+raa)
            else:
                plt.title("Throughput "+str(nWifi)+"nodes "+raa)
                plt.ylabel("Throughput (Mb/s)")
            for tcp in tcps:
                plotGraph(raa,tcp,nWifi,graph_type)
            plt.legend()
            plt.savefig("Results/"+graph_type+" "+str(nWifi)+"nodes"+" "+raa+".png")
            plt.close()