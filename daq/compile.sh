#!/bin/bash
cp MUconfig.c mudaq.c Makefile.mudaq test.c ~/CAEN/Wavedump/wavedump-3.5.3/src/
cp mudaq.h ~/CAEN/Wavedump/wavedump-3.5.3/inc/
cd ~/CAEN/Wavedump/wavedump-3.5.3/src/
make -f Makefile.mudaq mudaq 
make -f Makefile.mudaq test
cp mudaq test ~/g-2/daq/
cd -
