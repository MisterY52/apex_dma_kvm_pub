set logscale xy 2

set terminal pngcairo color solid
set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb"#888888" behind
set output "rwperf.png"

set title "Read performance (K - kernel mapped, E - external)"

set xlabel "Chunk size"
set ylabel "Read speed (MB/s)"

set xtics (0x8,0x10,0x100,0x1000,0x10000)
set format x "0x%x"
set key left top

set colorsequence classic

plot "read_kmod" using 1:2 title "K32 chunks" with lines,\
     "read_kmod" using 1:4 title "K8 chunks" with lines dashtype '_.-',\
     "read_kmod" using 1:6 title "K1 chunk" with lines dashtype '-  ',\
     "read_external" using 1:2 title "E32 chunks" with lines,\
     "read_external" using 1:4 title "E8 chunks" with lines dashtype '_.-',\
     "read_external" using 1:6 title "E1 chunk" with lines dashtype '-  '
