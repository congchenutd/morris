Depends
-----------------------
Qt >= 4.7

Install
-----------------------
Windows:
cd Source
qmake
nmake release
nmake clean

Linux:
cd Source
qmake CONFIG+=release
make
make clean

Mac:
cd Source
qmake -spec macx-g++ CONFIG+=release
make
make clean