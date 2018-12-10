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

set terminal postscript eps
set output "pdr.eps"

set title "Taxa média de entrega de pacotes"
set ylabel "Packets Delivery Rate (%)"
set yrange [0 : 1]
plot 'pdr.dat' \
		   using 2:3:4:xtic(1) ti "Altura máxima" linecolor rgb "#FF0000" fs pattern 2, \
        '' using 5:6:7 ti "Completa" lt 1 lc rgb "#0000FF" fs pattern 6

set terminal pngcairo
set output "pdr.png"
replot

set title "Atraso médio"
set terminal postscript eps
set output "atr.eps"
set ylabel "Atraso (segundos)"
set yrange [0 : 0.6]
plot 'atr.dat' \
		   using 2:3:4:xtic(1) ti "Altura máxima" linecolor rgb "#FF0000" fs pattern 2, \
        '' using 5:6:7 ti "Completa" lt 1 lc rgb "#0000FF" fs pattern 6

set terminal pngcairo
set output "atr.png"
replot