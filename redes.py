import numpy as np
import scipy as sp
import scipy.stats
import os
import sys

def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    return m, m-h, m+h

def tail(f, n):
	stdin,stdout = os.popen2("tail -n "+str(n)+" "+f.name)
	stdin.close()
	lines = stdout.readlines(); stdout.close()
	return lines[0].replace('\n', '')

t = ["WestwoodPlus","NewReno","Vegas"]
e = [0.0001,0.001,0.01]
f2 = open("resultadosSimu1.dat", "w")

for error in e:
	f2.write(str(error))
	for tcp in t:
		dados = []
		for i in range(1,101):
			filename ="simu1_resultados/Resultado_Tcp{0:s}_{1:.6f}_60.000000_{2:d}".format(tcp,error,i)
			rodada = float(tail(open(filename, "r"),1))		
			dados.append(rodada)
		a,b,c = mean_confidence_interval(dados,0.99)
		f2.write(", {}, {}, {}".format(a,b,c))
	f2.write("\n")

f.close()


