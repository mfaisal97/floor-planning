rm *.sol main.o app *.txt

g++ -c -Imipcl-2.1.3/mipcl/headers -m64 -std=c++11 -DMIP_API= main.cpp
g++ main.o -Lmipcl-2.1.3/lib -lmipcl  -o app -std=c++11


./app input.in 50 100