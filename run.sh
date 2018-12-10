for i in `seq 1 200`;
        do
./waf --run="scratch/redes-adhoc --pacotes=600 --simulationTime=20000 --arvore=1 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=600 --simulationTime=20000 --arvore=2 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=600 --simulationTime=20000 --arvore=3 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=600 --simulationTime=20000 --arvore=4 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=600 --simulationTime=20000 --arvore=5 --round=$i"
./waf --run="scratch/redes-adhoc --pacotes=600 --simulationTime=20000 --arvore=6 --round=$i"
done
