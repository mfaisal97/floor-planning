# floor-planning

## Intro
This repo contains the source files for a linear programming algorithm that is used to solve the floor planning problem. It uses the mipcl library to get access to linear programming tools.

## GettingStarted
First, install the mipcl library as instructed on their website and update the test.sh script to reflect the actual location of the installed library.

After installing the mentioned library, run the test.sh from the repo's home directory, using the following command,
            source test.sh

The script will run and compile and run the programm on all the provided test cases. You can then find the output files in the directories Out and Sol.

## Input File Description
The input has to specify the attributes for all the used cells in the design under development. It should striclty contains 3 sections. Each section must have at its beginning the number of entries in the section followed by the specified entries.

### The i/o pads section
It starts with the entries number n, followed by n lines where each lines has 3 values; the width, the length and its count. For this section, regardless how of many different input gates are used, only the first input type will be used in the i/o pads and the remaining are distracted according to the requirments.

### The Hard Core Cells
This is the second section and it starts with the entries number m, followed by m lines where each line has 3 values; the width, the length, and its count. All the entered cells in this section will be used.

### The soft core cells
This is the third section and it starts with the entries number l, followed by  lines where each line has 3 values; the total cell area, the minimum width, and the maximum width.

## Using the program
After, creating an input file meeting the constraints of the previous section and compiling it using the test.sh script, you can run the programm using the following command, where 20000 and 10000 represent the maximum diemensions of the core region:
            ./app input.txt 20000 10000

## Limitations
Although the program manages to solve the problem efficiently, it stills need more optimization to ensure larger utlization factor. Moreover, although the program is expected to behave well under large input files but the program is still undergoing some tests to ensure correctness in this case.

## Acknowledgemnt
This repo was done as a part of the digital design II course, supervised by Dr. Mohamed Shaalan in the American University in Cairo.
