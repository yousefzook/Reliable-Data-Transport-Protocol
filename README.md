# TCP simple network using UDP


## Problem statement   
you can find the problem statement [here](https://drive.google.com/open?id=16c20BTRi0JgU10rpqbw1O12refBeh7mC)

## Description
This code is an implementation to the `Stop-and-wait`, `Selective repeat` and `GBN` algorithms for reliable data transfer.
Congestion control is implemented by increasing and decreasing the window size `cwnd` by keeping track of packets timer.

## Build
```C
cd cmake-build-debug
make
```
## Run
```C
cd cmake-build-debug
./Server 
```
open new rerminal
```C
cd cmake-build-debug
./Client 
```
## Contributors
- [Nada Ayman](https://github.com/N-Einstein)
- [Yassmin](https://github.com/yassmin96)
