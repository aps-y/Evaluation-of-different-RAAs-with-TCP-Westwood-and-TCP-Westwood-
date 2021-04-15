import os
import matplotlib.pyplot as plt
import numpy as np

f=open("Averages/averages.txt","r")
lines=f.readlines()


cwdir=os.getcwd()

out=[]
for line in lines:
    if len(line.split())==0:
        continue
    else:
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
    print(line)
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

x=[i for i in range(2,8)]
for raa in ['Arf','Aarf','Aarfcd','Minstrel','Onoe']:
    print(x,throughput["TcpWestwood"][raa])
    plt.plot(x,throughput["TcpWestwood"][raa],label="TcpWestwood")
    plt.plot(x,throughput["TcpWestwoodPlus"][raa],label="TcpWestwoodPlus")
    plt.legend()
    plt.xlabel("#nodes")
    plt.ylabel("throughput in Mbps")
    plt.title("Average Throughput vs #nodes where Raa: "+raa)
    plt.savefig("averages/avTH_"+raa)
    plt.close()

x=[i for i in range(2,8)]
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
x=[i for i in range(2,8)]
for raa in ['Arf','Aarf','Aarfcd','Minstrel','Onoe']:
    plt.plot(x,throughput["TcpWestwood"][raa],label="TcpWestwood_"+raa)
    plt.plot(x,throughput["TcpWestwoodPlus"][raa],label="TcpWestwoodPlus_"+raa)
plt.legend()
plt.xlabel("#nodes")
plt.ylabel("throughput in Mbps")
plt.title("Average Throughput vs #nodes")
plt.savefig("averages/average_throughput_all")
plt.close()

plt.figure(figsize=(16,12))
x=[i for i in range(2,8)]
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
