#include <cmip.h>
#include <except.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct HardReactangle{
        int length;
        int width;
        int inputOrder;
        int count;
        long x_position;
        long y_position;
};


struct SoftRectangle{
        int area;
        int predicted_length;
        int predicted_width;
        long x_position;
        long y_position;
};

bool parseForVector(vector<HardReactangle>& Rects, ifstream& inputFile){
        int count = 0;
        inputFile >> count;
        Rects.resize(count);

        int offset = 0;
        for (int i =0; i < count; i++){
                inputFile >> dieHarRectsdRectangles[i + offset].length >> Rects[i + offset].width >> Rects[i + offset].count;
                Rects[i + offset].inputOrder = i;

                if(Rects[i + offset].count > 1){
                        Rects.resize(Rects.size() + Rects[i + offset].count - 1);
                        for (int j = 1; j <  Rects[i + offset].count; j++){
                                Rects[i + offset + j].length = Rects[i + offset].length;
                                Rects[i + offset + j].width = Rects[i + offset].width;
                                Rects[i + offset + j].count = Rects[i + offset].count;
                                Rects[i + offset + j].inputOrder = Rects[i + offset].inputOrder;                                        
                        }

                        offset += (Rects[i + offset].count  - 1);
                }

        }

        return true;
}


bool parseInputConstraints(vector<HardReactangle>& dieRectangles, vector<HardReactangle>& coreRectangles, vector<SoftRectangle>& softRectangles, string& fileName){
        ifstream inputFile (fileName);
        
        if (inputFile.is_open()){
                //reading first rectangles type
                parseForVector(dieRectangles, inputFile);

                //reading second rectangles type
                parseForVector(coreRectangles, inputFile);

                //reading third rectangles type
                int softCount = 0;
                inputFile >> softCount;
                for (int i =0; i < softCount; i++){
                        inputFile >> softRectangles[i].area;
                }


                inputFile.close();
        }else{
                cout << "Error Opening Specified file";
                return false;
        }

        return true;
}



bool solveProblem(){
        try {
		CMIP prob("MIPCLtest"); // 1
		prob.openMatrix(2,2,4); // 2
		prob.addVar(0,CMIP::VAR_INT, 100.0,0.0, CLP::VAR_INF); // 3
		prob.addVar(1,CMIP::VAR_INT,  64.0,0.0, CLP::VAR_INF); // 4
		prob.addCtr(0,0,-CLP::INF,250); // 5
		prob.addCtr(1,0,-CLP::INF,4); // 6
		prob.addEntry(50.0,0,0); // 7.1
		prob.addEntry(31.0,0,1); // 7.2
		prob.addEntry(-3.0,1,0); // 7.3
		prob.addEntry( 2.0,1,1); // 7.4
		prob.closeMatrix(); // (8)
		prob.optimize(); // (9)
		prob.printSolution("primer.sol"); // (10)
	}
	catch(CException* pe) {
		std::cerr << pe->what() << std::endl;
		delete pe;
		return false;
	}
	return true;
}



bool visualizeProblem(){
        return true;
}

int main(int argc, char *argv[]){
        vector<HardReactangle> dieHardRectangles;
        vector<HardReactangle> coreHardRectangles;
        vector<SoftRectangle> softRectangles;

        string fileName = argv[1];

        if(!parseInputConstraints(dieHardRectangles, coreHardRectangles, softRectangles, fileName)){
                cout << "Input File doesnot meet the specified requirements\n";
                exit(-1);
        }

        solveProblem(dieHardRectangles, coreHardRectangles, softRectangles);

        

        return 0;
}
