#!/bin/bash
mkdir -p build 
qmake6 -project -o DoItNow.pro -after "QT += core gui widgets"
cd build
make clean || true
qmake6 ../DoItNow.pro
make
./DoItNow
