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
        cout << "Parsing new vector...\n";
        int count = 0;
        inputFile >> count;
        Rects.resize(count);

        int offset = 0;
        for (int i =0; i < count; i++){
                inputFile >> Rects[i + offset].width >> Rects[i + offset].length >> Rects[i + offset].count;
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


bool parseInputConstraints(vector<HardReactangle>& dieRectangles, vector<HardReactangle>& coreRectangles, vector<SoftRectangle>& softRectangles, string fileName){
        cout << fileName << "\n";
        ifstream inputFile (fileName);
        
        if (inputFile.is_open()){
                //reading first rectangles type
                parseForVector(dieRectangles, inputFile);

                //reading second rectangles type
                parseForVector(coreRectangles, inputFile);

                //reading third rectangles type
                int softCount = 0;
                inputFile >> softCount;
                softRectangles.resize(softCount);
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

int getikRelation(int i, int k){
        int c = 0;

        int i_count = 0;
        for (i_count = 0; i_count < i; i_count++){
                c+= i_count;
        }
        return i_count + k-1;
}

void checkThis(){
        prob.printMatrix("matrix.txt");
        system("read -p \'Press Enter to continue...\' var");
}

bool solveProblem(vector<HardReactangle>& dieRectangles, vector<HardReactangle>& coreRectangles, vector<SoftRectangle>& softRectangles, int w, int h){
        try {
		CMIP prob("Floor-Planning");

                //General Paramters
                int W = w;
                int H = h;
                int hVarCount = 1;



                //Adding type One Constraints Count





                //Adding type Two Constraints Count
                int coreCount = coreRectangles.size();
                int permCoreCount = (coreCount*(coreCount-1))/2;

                





                //Adding type Three Constraints Count


                int totalVarsCount = hVarCount + 2*coreCount + 2*permCoreCount;
                int totalEquationsCount = 2*coreCount + 4*permCoreCount;
                prob.openMatrix(totalEquationsCount,totalVarsCount,totalEquationsCount*totalVarsCount);
                prob.addVar(0,CMIP::VAR_INT, -1, 0.0,CLP::VAR_INF);


                //Adding type One Constraints


                //Adding type Two Constraints
                //Adding variables
                for(int i = 0; i< coreCount; i++){
                        //Variables constraints
                        prob.addVar(1 + 2*i,CMIP::VAR_INT, 0.0, 0.0,CLP::VAR_INF);
                        prob.addVar(1 + 2*i + 1,CMIP::VAR_INT, 0.0 , 0.0,CLP::VAR_INF);
                }


                for(int i = 0; i< coreCount; i++){
                        //adding assistant variables
                        for(int k = i + 1; k < coreCount; k++){
                                prob.addVar(1 + 2*coreCount + 2* getikRelation(i,k),CMIP::VAR_BIN, 0.0,false,true );
                                prob.addVar(1 + 2*coreCount + 2* getikRelation(i,k) + 1,CMIP::VAR_BIN, 0.0,false,true);
                        }
                }


                cout << coreCount << endl;
                cout << permCoreCount << endl;
                //Adding constraints
                
                for(int i =0; i < coreCount; i++){
                        //small modules size constraints

                        prob.addCtr(2*i,0, -CLP::VAR_INF , W - coreRectangles[i].width);
                        prob.addEntry(1, 2*i, 1 + 2*i);

                        prob.addCtr(2*i+1,0, -CLP::VAR_INF , 0 - coreRectangles[i].length);                        
                        prob.addEntry(1, 2*i+1, 1 + 2*i + 1);
                        prob.addEntry(-1, 2*i+1, 0);
                }


                for(int i =0; i < coreCount; i++){
                        //small modules overlapping constraints
                        for (int k = i +1; k < coreCount; k++){
                                //relation One
                                prob.addCtr(2 * coreCount + 4*getikRelation(i,k), 0, -CLP::VAR_INF,- coreRectangles[i].width);
                                prob.addEntry(1, 2 * coreCount + 4*getikRelation(i,k), 1 + 2*i);                    //normal
                                prob.addEntry(-1, 2 * coreCount + 4*getikRelation(i,k), 1 + 2*k);                   //normal right

                                prob.addEntry(-W, 2 * coreCount + 4*getikRelation(i,k) + 0, 1 + 2*coreCount + 2* getikRelation(i,k));
                                prob.addEntry(-W, 2 * coreCount + 4*getikRelation(i,k) + 0, 1 + 2*coreCount + 2* getikRelation(i,k) + 1);


                                //relation Two
                                prob.addCtr(2 * coreCount + 4*getikRelation(i,k) + 1, 0, -CLP::VAR_INF, H - coreRectangles[i].length);
                                prob.addEntry(1, 2 * coreCount + 4*getikRelation(i,k) + 1, 1 + 2*i + 1);
                                prob.addEntry(-1, 2 * coreCount + 4*getikRelation(i,k) + 1, 1 + 2*k + 1);

                                prob.addEntry(-H, 2 * coreCount + 4*getikRelation(i,k) + 1, 1 + 2*coreCount + 2* getikRelation(i,k) );
                                prob.addEntry(H, 2 * coreCount + 4*getikRelation(i,k) + 1, 1 + 2*coreCount + 2* getikRelation(i,k) + 1);


                                //relation Three
                                prob.addCtr(2 * coreCount + 4*getikRelation(i,k) + 2, 0, -CLP::VAR_INF, W - coreRectangles[k].width);
                                prob.addEntry(1, 2 * coreCount + 4*getikRelation(i,k) + 2, 1 + 2*k );
                                prob.addEntry(-1, 2 * coreCount + 4*getikRelation(i,k) + 2, 1 + 2*i );

                                prob.addEntry(W, 2 * coreCount + 4*getikRelation(i,k) + 2, 1 + 2*coreCount + 2* getikRelation(i,k) );
                                prob.addEntry(-W, 2 * coreCount + 4*getikRelation(i,k) + 2, 1 + 2*coreCount + 2* getikRelation(i,k) +1);


                                //relation Four
                                prob.addCtr(2 * coreCount + 4*getikRelation(i,k) + 3, 0, -CLP::VAR_INF, 2*H - coreRectangles[k].length);
                                prob.addEntry(1, 2 * coreCount + 4*getikRelation(i,k) + 3, 1 + 2*k + 1);
                                prob.addEntry(-1, 2 * coreCount + 4*getikRelation(i,k) + 3, 1 + 2*i + 1);

                                prob.addEntry(H, 2 * coreCount + 4*getikRelation(i,k) + 3, 1 + 2*coreCount + 2* getikRelation(i,k));
                                prob.addEntry(H, 2 * coreCount + 4*getikRelation(i,k) + 3, 1 + 2*coreCount + 2* getikRelation(i,k) +1);
                        }
                }


                //Adding type Three Constraints


		prob.closeMatrix();
		prob.optimize();
		prob.printSolution("floor-planning.sol");
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
        string fileName;

        if (argc < 2){
                cout << "Error couldnot find input file name\n";
                return 1;
        }else {
                cout << "Using input file named \"" << argv[1] <<"\"\n";
                fileName = string(argv[1]);
                cout << fileName << "\n";
        }

        if(!parseInputConstraints(dieHardRectangles, coreHardRectangles, softRectangles, fileName)){
                cout << "Input File doesnot meet the specified requirements\n";
                exit(-1);
        }

        solveProblem(dieHardRectangles, coreHardRectangles, softRectangles, stoi(argv[2]) , stoi(argv[3]));

        

        return 0;
}
