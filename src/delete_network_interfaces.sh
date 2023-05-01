for veth in $(ip a | grep veth | cut -d' ' -f 2 | rev | cut -c2- | rev | cut -d '@' -f 1 )
do
	sudo ip link set $veth down
done


for veth in $(ip a | grep veth | cut -d' ' -f 2 | rev | cut -c2- | rev | cut -d '@' -f 1 )
do
	sudo ip link delete $veth
done
