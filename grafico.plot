reset
set encoding utf8
fontsize = 18
set term postscript enhanced eps color fontsize
set boxwidth 0.9
set style fill solid 1.00 border 0
set style histogram errorbars gap 3 lw 1
set style data histograms
set xlabel "Quantidade de nós"
set bars 0.6
set datafile separator ","

set output "pdr.eps"
set ylabel "PDR"
set yrange [0 : 1]
plot 'pdr.dat' \
		   using 2:3:4:xtic(1) ti "Desbalanceada" linecolor rgb "#FF0000" fs pattern 2, \
        '' using 5:6:7 ti "Balanceada" lt 1 lc rgb "#0000FF" fs pattern 6

set output "atr.eps"
set ylabel "Atraso"
set yrange [0 : 0.045]
plot 'atr.dat' \
		   using 2:3:4:xtic(1) ti "Desbalanceada" linecolor rgb "#FF0000" fs pattern 2, \
        '' using 5:6:7 ti "Balanceada" lt 1 lc rgb "#0000FF" fs pattern 6
