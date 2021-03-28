# Evaluation of different RAAs with TCP Westwood and TCP Westwood+

### Files included 

1. **assignment.cc** - the testbed for the evaluation
    * The file contains the testbed that is used for the evaluation
    * Relevant Commandline Arguments :-
        * nWifi (int) :- Number of wireless nodes
        * raa (string) :- RAA variants to be used  (Arf/ Aarf/ Aarfcd/ Onoe/ Minstrel etc.)
        * tcp (string) :- TCP variant to be used (TcpWestwood / TcpWestwoodPlus)
        * p2pRate (int) :- point to point link data rate in Mbps
        * p2pDelay (int) :- point to point link delay duration in Milliseconds

2. **graphPlotter.py** - script to generate graphs of throughput and delay from results obtained 
    * The testbed evaluates avg Throughput and avg Delay for the whole network in intervals of 0.2 seconds the graphPlotter script uses the data to plot graphs
3. **runExperiments.sh** - shell script to run tests by varying number of wifi nodes from 1 to 10 and using the above mentioned RAAs along with TCP Westwood and TCP Westwood+ then plot the graphs for the results obtained.
