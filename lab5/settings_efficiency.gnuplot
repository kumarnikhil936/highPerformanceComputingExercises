set title "Comparing efficiency in Task 1 and Task 2"
set xlabel "# of threads"
set ylabel "Efficiency"
set grid
set output 'plot2.png'
set terminal pngcairo enhanced font "Times New Roman,12.0" size 1500,1100
set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5
set style line 2 \
    linecolor rgb '#dd181f' \
    linetype 1 linewidth 2 \
    pointtype 5 pointsize 1.5
plot 'plot.dat' u 1:4 with linespoints linestyle 1 title "Task 1", '' u 1:5 with linespoints linestyle 2 title "Task 2"
