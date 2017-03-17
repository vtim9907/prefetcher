reset

set datafile separator ","
set title "transpose runtime"
set xlabel 'N (array size = N * N)'
set ylabel 'time(sec * 10^{-6})'
set key left top
set terminal png size 1000,800 enhanced font 'Verdana,14'
set output 'plot.png'

plot "test_time.csv" u 1:2  w lp title "Naive",\
"test_time.csv" u 1:3 w lp title "SSE",\
"test_time.csv" u 1:4 w lp title "SSE prefetch",\
 "test_time.csv" u 1:5 w lp title "AVX" ,\
 "test_time.csv" u 1:6 w lp  title "AVX using SSE prefetch" ,\
