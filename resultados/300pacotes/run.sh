for i in `seq 1 100`;
        do
./waf --run="scratch/redes-adhoc --pacotes=300 --simulationTime=5000 --arvore=1 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=300 --simulationTime=5000 --arvore=2 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=300 --simulationTime=5000 --arvore=3 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=300 --simulationTime=5000 --arvore=4 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=300 --simulationTime=5000 --arvore=5 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=300 --simulationTime=5000 --arvore=6 --round=$i"
done
