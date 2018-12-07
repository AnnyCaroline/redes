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

a=[1,2,3,4,5,6];
pdrs=[];
atrs=[];

filePdr = open("pdr.dat", "w")
fileAtr = open("atr.dat", "w")

for arvore in a:

    if arvore <=2:
        filePdr.write("5");
        fileAtr.write("5");
    elif arvore <=4:
        filePdr.write("17");
        fileAtr.write("17");
    elif arvore <=6:
        filePdr.write("30");
        fileAtr.write("30");        

    for i in range(1,101):    
        filename = "Anny_{0:d}_300_5000.000000_{1:d}".format(arvore,i);
        file = open(filename, 'r');
        pdrs.append(float(file.readline()));
        atrs.append(float(file.readline()));
        file.close();

    pa,pb,pc = mean_confidence_interval(pdrs,0.99)
    aa,ab,ac = mean_confidence_interval(atrs,0.99)

    filePdr.write(", {}, {}, {}".format(pa,pb,pc))
    fileAtr.write(", {}, {}, {}".format(aa,ab,ac))

    if arvore%2==0:
        filePdr.write("\n")
        fileAtr.write("\n")

