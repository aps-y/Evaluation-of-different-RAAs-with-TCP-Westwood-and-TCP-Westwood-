
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os


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


raas = ["Arf", "Aarf","Aarfcd", "Onoe", "Minstrel"]
tcps = ["TcpWestwood","TcpWestwoodPlus"]
for nWifi in range(1,11):
    for raaNum in range(0,5):
        plotGraph(raas[raaNum],tcps[0],nWifi)
        plotGraph(raas[raaNum],tcps[1],nWifi)


f=open("../averages1.txt","r")
lines=f.readlines()

cwdir=os.getcwd()
os.chdir("..")
os.chdir(os.getcwd()+"/Results")

out=[]
for line in lines:
    if len(line.split())==0:
        continue
    w1=line.split()[0]
    if w1.isnumeric() or w1=="Average":
        out.append(line)

throughput={}
delay={}
throughput['TcpWestwood']={}
throughput['TcpWestwoodPlus']={}
delay['TcpWestwood']={}
delay['TcpWestwoodPlus']={}
for raa in ['Arf','Aarf','Aarfcd','Minstrel','Onoe']:
    throughput['TcpWestwood'][raa]=[]
    throughput['TcpWestwoodPlus'][raa]=[]
    delay['TcpWestwood'][raa]=[]
    delay['TcpWestwoodPlus'][raa]=[]

nodes=1
tcp="TcpWestwood"
raa='Arf'
for i in range(len(out)):
    line=out[i]
    if len(line.split(' '))==3 and line.split(' ')[1].startswith('Tcp'):
        print(line)
        l=out[i].split()
        nodes=int(l[0])
        tcp=l[1]
        raa=l[2]
    if line.startswith("Average"):
        if line.split(' ')[1]=="Throughput:":
            throughput[tcp][raa].append(float(line.split()[2]))
        elif line.split(' ')[1]=="Delay:":
            delay[tcp][raa].append(float(line.split()[2][:-2]))

x=[i for i in range(1,11)]
for raa in ['Arf','Aarf','Aarfcd','Minstrel','Onoe']:
    plt.plot(x,throughput["TcpWestwood"][raa],label="TcpWestwood")
    plt.plot(x,throughput["TcpWestwoodPlus"][raa],label="TcpWestwoodPlus")
    plt.legend()
    plt.xlabel("#nodes")
    plt.ylabel("throughput in Mbps")
    plt.title("Average Throughput vs #nodes where Raa: "+raa)
    plt.savefig("averages/avTH_"+raa)
    plt.close()

x=[i for i in range(1,11)]
for raa in ['Arf','Aarf','Aarfcd','Minstrel','Onoe']:
    plt.plot(x,delay["TcpWestwood"][raa],label="TcpWestwood")
    plt.plot(x,delay["TcpWestwoodPlus"][raa],label="TcpWestwoodPlus")
    plt.legend()
    plt.xlabel("#nodes")
    plt.ylabel("Delay in ms")
    plt.title("Average Delay vs #nodes where Raa: "+raa)
    plt.savefig("averages/avDelay_"+raa)
    plt.close()

plt.figure(figsize=(16, 12))
x=[i for i in range(1,11)]
for raa in ['Arf','Aarf','Aarfcd','Minstrel','Onoe']:
    plt.plot(x,throughput["TcpWestwood"][raa],label="TcpWestwood_"+raa)
    plt.plot(x,throughput["TcpWestwoodPlus"][raa],label="TcpWestwoodPlus_"+raa)
plt.legend()
plt.xlabel("#nodes")
plt.ylabel("throughput in Mbps")
plt.title("Average Throughput vs #nodes")
plt.savefig("averages/average_throughput_all")
plt.close()

plt.figure(figsize=(16, 12))
x=[i for i in range(1,11)]
for raa in ['Arf','Aarf','Aarfcd','Minstrel','Onoe']:
    plt.plot(x,delay["TcpWestwood"][raa],label="TcpWestwood_"+raa)
    plt.plot(x,delay["TcpWestwoodPlus"][raa],label="TcpWestwoodPlus_"+raa)
plt.legend()
plt.xlabel("#nodes")
plt.ylabel("Delay in ms")
plt.title("Average delay vs #nodes")
plt.savefig("averages/average_delay_all")
plt.close()

os.chdir(cwdir)
