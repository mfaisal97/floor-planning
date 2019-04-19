export  MIPDIR="C:\Users\mfaisal\Desktop\MickeyFiles\DigitalDesign\MiniProject\mipcl-2.1.3.linux-x86_64\mipcl-2.1.3"

g++ -I\$MIPDIR/mipcl/headers main.cpp\\
g++ -shared-libgcc testMIP.o \$MIPDIR/bin/mipcl.dll -o main.exe

./main.exe 1