#!/bin/bash

## start arduino and connect to correct port and board
## start scren session to monitor serial port with loggin enabled
##
##    screen -s arduino -L  /dev/ttyACM0 9600
##
## detach from the screen session
##
## CTRL-a d
##
## plot the temperature
## 
gnuplot -persist -e 'set yrange [30:50]; set xlabel "hours since 23/2/2023 20:00" ; plot "/home/lhcb/g-2/workdir/screenlog.0" every 2::2400 using (20+$0/3600):2 with linespoints'
## gnuplot -persist -e 'set xdata time; set timefmt "%s"; set yrange [40:50]; set xlabel "approx time" ; plot "/home/lhcb/g-2/workdir/screenlog.0" every 2::0 using ($0+1677181308):2 with linespoints'
##

##

