#/!/bin/bash

#File: testscript
#Author: Andy Sayler
#Project: CSCI 3753 Programming Assignment 3
#Create Date: 2012/03/09
#Modify Date: 2012/03/21
#Description:
#	A simple bash script to run a signle copy of each test case
#	and gather the relevent data.

ITERATIONS=100000000
BYTESTOCOPY=102400
BLOCKSIZE=1024
TIMEFORMAT="wall=%e user=%U system=%S CPU=%P i-switched=%c v-switched=%w"
MAKE="make -s"

echo Building code...
$MAKE clean
$MAKE

echo Starting test runs...

echo Calculating pi over $ITERATIONS iterations using SCHED_OTHER with 1 simultaneous process...
/usr/bin/time -f "$TIMEFORMAT" ./pi-sched $ITERATIONS SCHED_OTHER > /dev/null

echo Calculating pi over $ITERATIONS iterations using SCHED_FIFO with 1 simultaneous process...
/usr/bin/time -f "$TIMEFORMAT" sudo ./pi-sched $ITERATIONS SCHED_FIFO > /dev/null

echo Calculating pi over $ITERATIONS iterations using SCHED_RR with 1 simultaneous process...
/usr/bin/time -f "$TIMEFORMAT" sudo ./pi-sched $ITERATIONS SCHED_RR > /dev/null

echo Copying $BYTESTOCOPY bytes in blocks of $BLOCKSIZE from rwinput to rwoutput
echo using SCHED_OTHER with 1 simultaneous process...
/usr/bin/time -f "$TIMEFORMAT" ./rw $BYTESTOCOPY $BLOCKSIZE > /dev/null
