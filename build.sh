#!/bin/bash
mkdir -p build 
qmake -project -o DoItNow.pro -after "QT += core gui widgets"
cd build
make clean
qmake ../DoItNow.pro
make
./DoItNow
