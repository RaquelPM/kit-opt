echo "--TSP Benchmark--"

make

k=1
for instance in instances-table-mlp/*; do
	echo $instance >> ./outputMLP.txt

	echo "Running $instance"
	echo "Instance $k of 22" 

	./tsp ${instance} | awk "{print $1}" >> ./outputMLP.txt

	k=$(($k + 1))
done

echo "-" >> ./output.txt