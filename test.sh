rm *.sol main.o app *.txt ./Out/*

g++ -c -Imipcl-2.1.3/mipcl/headers -m64 -std=c++11 -DMIP_API= main.cpp
g++ main.o -Lmipcl-2.1.3/lib -lmipcl  -o app -std=c++11


./app input.txt 200000 200000

./app 01padsTestOne.txt 200000 200000
./app 02padsTestTwo.txt 200000 200000
./app 03padsTestThree.txt 200000 200000
./app 04HardCoreCellsOne.txt 200000 200000
./app 05HardCoreCellsTwo.txt 200000 200000
./app 06HardCoreCellsThree.txt 200000 200000
./app 07PadsAndCoreCellsOne.txt 200000 200000
./app 08PadsAndCoreCellsTwo.txt 200000 200000
./app 09SoftCellsOne.txt 200000 200000
./app 10SoftCellsTwo.txt 200000 200000
./app 11AllCellsOne.txt 200000 200000
./app 12AllCellsTwo.txt 200000 200000

