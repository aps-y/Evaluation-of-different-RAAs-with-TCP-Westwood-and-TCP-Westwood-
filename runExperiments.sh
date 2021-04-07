num_nodes=("5")
#  "2" "3" "4" "5" "6" "7")
#  "8" "9" "10")
raa_list=("Arf" "Aarf" "Aarfcd" "Onoe" "Minstrel")
tcp_list=("TcpWestwood" "TcpWestwoodPlus")

for i in "${Raa_list[@]}"; do   # The quotes are necessary here
    echo "$i"
done

touch ../averages.txt
echo "">../averages.txt

for nwifi in "${num_nodes[@]}"; do
    for tcp in "${tcp_list[@]}"; do
        for raa in "${raa_list[@]}"; do

            echo "$nwifi $tcp $raa"
            ../waf --run "assignment2 --tcp=$tcp --raa=$raa --nWifi=$nwifi --logAverages=true"

        done
    done
done

python3 graphPlotter.py

