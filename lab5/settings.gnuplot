set title "Comparing execution time with/out mutex"
set xlabel "# of threads"
set ylabel "time to execute in seconds"
set grid
set output 'plot.png'
set terminal pngcairo enhanced font "Times New Roman,12.0" size 1500,1100
set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5
set style line 2 \
    linecolor rgb '#dd181f' \
    linetype 1 linewidth 2 \
    pointtype 5 pointsize 1.5
plot 'plot.dat' u 1:2 with linespoints linestyle 1 title "With mutex", '' u 1:3 with linespoints linestyle 2 title "without mutex
