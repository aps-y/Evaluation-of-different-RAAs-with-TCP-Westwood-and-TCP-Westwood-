# Evaluation of different RAAs with TCP Westwood and TCP Westwood+

## Files included 

1. **twoWayFlowTest.cc** - the testbed for the evaluation
    * The file contains the testbed that is used for the evaluation
    * It uses a dumbell topology where there are equal number of wifi nodes on both sides of the p2p link. Half of the pairs from these have a TCP Westwood/Westwood+ flow and the rest of the pairs have a TCP NewReno flow in the opposite direction.
    * Relevant Commandline Arguments :-
        * nWifi (int) :- Number of wireless nodes(2-7)
        * raa (string) :- RAA variants to be used  (Arf/ Aarf/ Aarfcd/ Onoe/ Minstrel etc.)
        * tcp (string) :- TCP variant to be used (TcpWestwood / TcpWestwoodPlus)
        * p2pRate (int) :- point to point link data rate in Mbps
        * p2pDelay (int) :- point to point link delay duration in Milliseconds

2. **graphPlotter.py** - script to generate graphs of throughput and delay from results obtained 
    * The testbed evaluates avg Throughput and avg Delay for the whole network in intervals of 0.2 seconds the graphPlotter script uses the data to plot graphs
3. **runExperiments.sh** - shell script to run tests by varying number of wifi nodes from 2 to 7 and using the above mentioned RAAs along with TCP Westwood and TCP Westwood+ then plot the graphs for the results obtained.

## Output for 4 wifi nodes on each side of the p2p link and 2 Westwood/Westwood+ flows and 2 TCP NewReno flows in opposite direction

### Minstrel
   #### Delay
![TCPs Delay Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/delay%204nodes%20Minstrel.png)
   #### Throughput
![TCPs Throughput Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/throughput%204nodes%20Minstrel.png)

### Aarfcd
   #### Delay
![TCPs Delay Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/delay%204nodes%20Aarfcd.png)
   #### Throughput
![TCPs Throughput Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/throughput%204nodes%20Aarfcd.png)

### Aarf
   #### Delay
![TCPs Delay Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/delay%204nodes%20Aarf.png)
   #### Throughput
![TCPs Throughput Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/throughput%204nodes%20Aarf.png)

### Arf
   #### Delay
![TCPs Delay Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/delay%204nodes%20Arf.png)
   #### Throughput
![TCPs Throughput Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/throughput%204nodes%20Arf.png)

### Onoe
   #### Delay
![TCPs Delay Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/delay%204nodes%20Onoe.png)
   #### Throughput
![TCPs Throughput Comparison Minstrel](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/blob/main/Results/Combined4_Raa_and_nodes/throughput%204nodes%20Onoe.png)

#### For more detailed look at the results please go into the [Results](https://github.com/aps-y/Evaluation-of-different-RAAs-with-TCP-Westwood-and-TCP-Westwood-/tree/main/Results) directory
