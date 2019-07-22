#!/usr/bin/gnuplot

reset
set samples 2000
#set terminal qt size 350,262 enhanced font 'Verdana,10' persist
#set terminal wxt size 350,262 enhanced font 'Verdana,10' persist
set terminal pngcairo size 1280,720 enhanced font 'Verdana,10'
set output 'test.png'
#set terminal svg size 350,262 fname 'Verdana, Helvetica, Arial, sans-serif' #fsize '10'
#set output 'test.svg'

set style line  1 lc rgb '#0060ad' pt 5 ps 0.2 lt 1 lw 2    # blue

#set tmargin at screen 0.95; set bmargin at screen 0.45

set grid linetype 1 linecolor "#cccccc"
set multiplot layout 3,1

#fich="2019-07-19_17h20.txt"
fich="2019-07-19_17h44.txt"
set xrange [66000:76000]

plot fich using 7:1 with lines, fich using 7:3 with lines
#plot fich using 7:2 with lines, fich using 7:4 with lines
plot fich using 7:2 with lines, fich using 7:4 with lines, fich using 7:6 with lines
plot fich using 7:5 with lines

#plot fich using 1 with lines
#plot fich using 2 with lines
#plot fich using (sqrt($1*$1+$2*$2)) with lines
#plot fich using (atan2($1,$2)) with lines

