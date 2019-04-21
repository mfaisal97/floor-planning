#include <cmip.h>
#include <except.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

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
        int wMin;
        int wMax;
        int hMin;
        int hMax;
        double delta;
        double c;
        int width;
        int length;
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
                        inputFile >> softRectangles[i].area >> softRectangles[i].wMin >> softRectangles[i].wMax;
                        softRectangles[i].hMax = int (ceil(1.0 * softRectangles[i].area / softRectangles[i].wMin));
                        softRectangles[i].hMin = int (ceil(1.0 * softRectangles[i].area / softRectangles[i].wMax));
                        softRectangles[i].delta = 1.0 * ((1.0 * softRectangles[i].area / softRectangles[i].wMax) - (1.0 * softRectangles[i].area / softRectangles[i].wMin)) / (softRectangles[i].wMax - softRectangles[i].wMin);
                        softRectangles[i].c = (1.0 * softRectangles[i].area / softRectangles[i].wMin) - softRectangles[i].delta * softRectangles[i].wMin;
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
        return i_count + k - 1;
}

void checkThis(CMIP& prob){
        if(false){
                prob.printMatrix("matrix.txt");
                system("read -p \'Press Enter to continue...\' var");
        }
}

// checkThis(prob);


int CoreWidth;
int CoreLength;
bool freePadsPlacement = false;
bool solveProblem(vector<HardReactangle>& dieRectangles, vector<HardReactangle>& coreRectangles, vector<SoftRectangle>& softRectangles, int w, int h){
        try {
		CMIP prob("Floor-Planning");

                //General Paramters
                int W = w;
                int H = h;
                int hVarCount = 2;


                //Adding type One Constraints Count
                int dieCount = 0;


                //Adding type Two Constraints Count
                int coreCount = coreRectangles.size();
                

                //Adding type Three Constraints Count
                int softCount = softRectangles.size();


                int totalShapesCount = dieCount + coreCount + softCount;

                int permCount = (totalShapesCount*(totalShapesCount-1))/2;
                int permIndexBase = hVarCount + 2 * totalShapesCount + softCount;

                int totalVarsCount = permIndexBase + 2*permCount;
                
                int totalEquationsCount = 2*totalShapesCount + 4*permCount;
                prob.openMatrix(totalEquationsCount,totalVarsCount,totalEquationsCount*totalVarsCount);
                prob.addVar(0,CMIP::VAR_INT, -1, 0.0,CLP::VAR_INF);
                prob.addVar(1,CMIP::VAR_INT, -1, 0.0,CLP::VAR_INF);


                //Adding type One Constraints

                
                //Adding variables
                //Adding main variables
                //typeTwo
                for(int i = 0; i< coreCount; i++){
                        prob.addVar(hVarCount + 2 * dieCount + 2*i,CMIP::VAR_INT, 0.0, 0.0,CLP::VAR_INF);
                        prob.addVar(hVarCount + 2 * dieCount + 2*i + 1,CMIP::VAR_INT, 0.0 , 0.0,CLP::VAR_INF);
                }

                //Type Three
                for(int i = 0; i < softCount; i++ ){
                        prob.addVar(hVarCount + 2 * dieCount + 2 * coreCount + 3*i,CMIP::VAR_INT, 0.0, 0.0,CLP::VAR_INF);
                        prob.addVar(hVarCount + 2 * dieCount + 2 * coreCount + 3*i + 1,CMIP::VAR_INT, 0.0 , 0.0,CLP::VAR_INF);
                        prob.addVar(hVarCount + 2 * dieCount + 2 * coreCount + 3*i + 2,CMIP::VAR_INT, 0.0 , softRectangles[i].wMin, softRectangles[i].wMax);
                }


                //adding assistant variables
                for(int i = 0; i< totalShapesCount; i++){
                        for(int k = i + 1; k < totalShapesCount; k++){
                                prob.addVar(permIndexBase + 2* getikRelation(i,k),CMIP::VAR_BIN, 0.0,false,true );
                                prob.addVar(permIndexBase + 2* getikRelation(i,k) + 1,CMIP::VAR_BIN, 0.0,false,true);
                        }
                }


                //Adding constraints
                //Type Two
                for(int i =0; i < coreCount; i++){
                        //cells size constraints
                        prob.addCtr(2 * dieCount + 2*i,0, -CLP::VAR_INF , 0 - coreRectangles[i].width);
                        prob.addEntry(1, 2 * dieCount + 2*i, hVarCount + 2 * dieCount + 2*i);
                        prob.addEntry(-1, 2 * dieCount +2*i, 1);

                        prob.addCtr(2 * dieCount + 2*i+1,0, -CLP::VAR_INF , 0 - coreRectangles[i].length);                        
                        prob.addEntry(1, 2 * dieCount +2*i+1, hVarCount + 2 * dieCount +  2*i + 1);
                        prob.addEntry(-1, 2 * dieCount +2*i+1, 0);
                }

                //Type Three
                for(int i =0; i < softCount; i++){
                        //cells size constraints
                        prob.addCtr(2 * dieCount + 2 * coreCount + 2 * i,0, -CLP::VAR_INF , 0);
                        prob.addEntry(1, 2 * dieCount + 2 * coreCount + 2 * i, hVarCount + 2 * dieCount + 2 * coreCount + 3*i);
                        prob.addEntry(1, 2 * dieCount + 2 * coreCount + 2 * i, hVarCount + 2 * dieCount + 2 * coreCount + 3*i + 2);
                        prob.addEntry(-1, 2 * dieCount + 2 * coreCount + 2 * i, 1);

                        prob.addCtr(2 * dieCount + 2 * coreCount + 2 * i + 1,0, -CLP::VAR_INF , 0 - softRectangles[i].c);                        
                        prob.addEntry(1, 2 * dieCount + 2 * coreCount + 2 * i + 1, hVarCount + 2 * dieCount + 2 * coreCount + 3*i + 1);
                        prob.addEntry(softRectangles[i].delta, 2 * dieCount + 2 * coreCount + 2 * i + 1, hVarCount + 2 * dieCount + 2 * coreCount + 3*i + 2);
                        prob.addEntry(-1, 2 * dieCount + 2 * coreCount + 2 * i + 1, 0);
                }

                checkThis(prob);



                for(int i = dieCount; i < dieCount + coreCount; i++){
                        //small modules overlapping constraints
                        for (int k = i +1; k < dieCount + coreCount; k++){
                                //relation One
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k), 0, -CLP::VAR_INF,- coreRectangles[i-dieCount].width);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k), hVarCount + 2*i);                    //normal
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k), hVarCount + 2*k);                   //normal right

                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k), permIndexBase + 2* getikRelation(i,k));
                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k), permIndexBase + 2* getikRelation(i,k) + 1);


                                //relation Two
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 1, 0, -CLP::VAR_INF, H - coreRectangles[i-dieCount].length);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 1, hVarCount + 2*i + 1);
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 1, hVarCount + 2*k + 1);

                                prob.addEntry(-H, 2*totalShapesCount + 4*getikRelation(i,k) + 1, permIndexBase + 2* getikRelation(i,k) );
                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 1, permIndexBase + 2* getikRelation(i,k) + 1);


                                //relation Three
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 2, 0, -CLP::VAR_INF, W - coreRectangles[k-dieCount].width);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 2, hVarCount + 2*k );
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 2, hVarCount + 2*i );

                                prob.addEntry(W, 2*totalShapesCount + 4*getikRelation(i,k) + 2, permIndexBase + 2* getikRelation(i,k) );
                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k) + 2, permIndexBase + 2* getikRelation(i,k) +1);


                                //relation Four
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 3, 0, -CLP::VAR_INF, 2*H - coreRectangles[k-dieCount].length);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 3, hVarCount + 2*k + 1);
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 3, hVarCount + 2*i + 1);

                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 3, permIndexBase + 2* getikRelation(i,k));
                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 3, permIndexBase + 2* getikRelation(i,k) +1);
                                checkThis(prob);
                        }


                        for (int k = dieCount + coreCount; k < dieCount + coreCount + softCount; k++){
                                //relation One
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k), 0, -CLP::VAR_INF,- coreRectangles[i-dieCount].width);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k), hVarCount + 2*i);                    //normal
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k), hVarCount + 3*k - dieCount - coreCount);                   //normal right

                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k), permIndexBase + 2* getikRelation(i,k));
                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k), permIndexBase + 2* getikRelation(i,k) + 1);


                                //relation Two
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 1, 0, -CLP::VAR_INF, H - coreRectangles[i-dieCount].length);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 1, hVarCount + 2*i + 1);
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 1, hVarCount + 3*k - dieCount - coreCount + 1);

                                prob.addEntry(-H, 2*totalShapesCount + 4*getikRelation(i,k) + 1, permIndexBase + 2* getikRelation(i,k) );
                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 1, permIndexBase + 2* getikRelation(i,k) + 1);


                                //relation Three
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 2, 0, -CLP::VAR_INF, W - softRectangles[k- dieCount - coreCount].width);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 2, hVarCount + 3*k - dieCount - coreCount );
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 2, hVarCount + 2*i );

                                prob.addEntry(W, 2*totalShapesCount + 4*getikRelation(i,k) + 2, permIndexBase + 2* getikRelation(i,k) );
                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k) + 2, permIndexBase + 2* getikRelation(i,k) +1);


                                //relation Four
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 3, 0, -CLP::VAR_INF, 2*H - softRectangles[k- dieCount - coreCount].c);
                                // - coreRectangles[k].length
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 3, hVarCount + 3*k - dieCount - coreCount + 1);
                                prob.addEntry(softRectangles[k- dieCount - coreCount].delta, 2*totalShapesCount + 4*getikRelation(i,k) + 3, hVarCount + 3*k - dieCount - coreCount + 2);
                                
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 3, hVarCount + 2*i + 1);

                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 3, permIndexBase + 2* getikRelation(i,k));
                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 3, permIndexBase + 2* getikRelation(i,k) +1);

                                checkThis(prob);
                        }
                }

                for(int i = dieCount + coreCount; i < dieCount + coreCount + softCount; i++){
                        for (int k = i + 1; k < dieCount + coreCount + softCount; k++){
                                //relation One
                                //cout << "ana hena y john \t" << 2*totalShapesCount + 4*getikRelation(i,k) << "\n";
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k), 0, -CLP::VAR_INF,0);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k), hVarCount + 3*i - dieCount - coreCount);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k), hVarCount + 3*i - dieCount - coreCount+2);                    //normal
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k), hVarCount + 3*k - dieCount - coreCount);                   //normal right

                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k), permIndexBase + 2* getikRelation(i,k));
                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k), permIndexBase + 2* getikRelation(i,k) + 1);


                                //relation Two
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 1, 0, -CLP::VAR_INF, H - softRectangles[i-dieCount-coreCount].c);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 1, hVarCount + 3*i - dieCount - coreCount + 1);
                                prob.addEntry(softRectangles[i- dieCount - coreCount].delta, 2*totalShapesCount + 4*getikRelation(i,k) + 1, hVarCount + 3*k - dieCount - coreCount + 2);
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 1, hVarCount + 3*k - dieCount - coreCount + 1);

                                prob.addEntry(-H, 2*totalShapesCount + 4*getikRelation(i,k) + 1, permIndexBase + 2* getikRelation(i,k) );
                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 1, permIndexBase + 2* getikRelation(i,k) + 1);


                                //relation Three
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 2, 0, -CLP::VAR_INF, W);
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 2, hVarCount + 3*k - dieCount - coreCount );
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 2, hVarCount + 3*k - dieCount - coreCount + 2);
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 2, hVarCount + 3*i - dieCount - coreCount );

                                prob.addEntry(W, 2*totalShapesCount + 4*getikRelation(i,k) + 2, permIndexBase + 2* getikRelation(i,k) );
                                prob.addEntry(-W, 2*totalShapesCount + 4*getikRelation(i,k) + 2, permIndexBase + 2* getikRelation(i,k) +1);


                                //relation Four
                                prob.addCtr(2*totalShapesCount + 4*getikRelation(i,k) + 3, 0, -CLP::VAR_INF, 2*H - softRectangles[k- dieCount - coreCount].c);
                                // - coreRectangles[k].length
                                prob.addEntry(1, 2*totalShapesCount + 4*getikRelation(i,k) + 3, hVarCount + 3*k - dieCount - coreCount + 1);
                                prob.addEntry(softRectangles[k- dieCount - coreCount].delta, 2*totalShapesCount + 4*getikRelation(i,k) + 3, hVarCount + 3*k - dieCount - coreCount + 2);
                                
                                prob.addEntry(-1, 2*totalShapesCount + 4*getikRelation(i,k) + 3, hVarCount + 3*i - dieCount - coreCount + 1);

                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 3, permIndexBase + 2* getikRelation(i,k));
                                prob.addEntry(H, 2*totalShapesCount + 4*getikRelation(i,k) + 3, permIndexBase + 2* getikRelation(i,k) +1);
                                checkThis(prob);
                        }

                }                


		prob.closeMatrix();
		prob.optimize();
		prob.printSolution("floor-planning.sol");


                //Now doing part one
                int* indicies;
                double* vals;
                prob.getSolution(vals,indicies);

                for(int i =0; i < totalVarsCount; i++){
                        if(indicies[i] == 0){
                                CoreWidth = vals[i];
                        }else if (indicies[i] == 1){
                                CoreLength = vals[i];
                        }else if (indicies[i] < hVarCount + 2* coreCount){
                                if (indicies[i] % 2 ==0){
                                        coreRectangles[(indicies[i] -2) / 2].x_position = vals[i];
                                }else {
                                        coreRectangles[(indicies[i] - 2) / 2].y_position = vals[i];
                                }
                        }else if (indicies[i] < hVarCount + 2* coreCount + 3* softCount){
                                if ((indicies[i]- 2 - 2 * coreCount) % 3 ==0){
                                        softRectangles[(indicies[i] - 2 - 2 * coreCount) / 3].x_position = vals[i];
                                }else if ((indicies[i]- 2 - 2 * coreCount) % 3 ==1){
                                        softRectangles[(indicies[i] - 2 - 2 * coreCount) / 3].y_position = vals[i];
                                }else {
                                        softRectangles[(indicies[i] - 2 - 2 * coreCount) / 3].width = vals[i];
                                        softRectangles[(indicies[i] - 2 - 2 * coreCount) / 3].length = softRectangles[(indicies[i] - 2 - 2 * coreCount) / 3].delta * softRectangles[(indicies[i] - 2 - 2 * coreCount) / 3].width + softRectangles[(indicies[i] - 2 - 2 * coreCount) / 3].c;
                                }
                        }
                }



                cout << "Finally\t" << CoreWidth << "\t" << CoreLength << endl;
	}
	catch(CException* pe) {
		std::cerr << pe->what() << std::endl;
		delete pe;
		return false;
	}
	return true;
}



bool visualizeProblem(vector<HardReactangle>& dieRectangles, vector<HardReactangle>& coreRectangles, vector<SoftRectangle>& softRectangles, string outputFileName){
        
        int CoreWidthStart = dieRectangles[0].width;
        int CoreLengthStart = dieRectangles[0].length;

        std::ofstream outFile (outputFileName);
        outFile << "Solving Part One (i/o pads)\n";

        int minWidthCount = floor(1.0 * CoreWidth / dieRectangles[0].width);
        int minLengthCount = floor(1.0 * CoreLength / dieRectangles[0].length);
        freePadsPlacement = (((minWidthCount + minLengthCount) * 2) >= dieRectangles[0].count);

        int ind = 0;
        if(freePadsPlacement){
                outFile << "The die is core constrained ...\n";
                
                for(int i =1; i <= minWidthCount; i++){
                        dieRectangles[i-1].x_position = dieRectangles[0].width * i;
                        dieRectangles[i-1].y_position = 0;
                }

                for(int i =minWidthCount + 1; i <= minWidthCount *2; i++){
                        dieRectangles[i-1].x_position = dieRectangles[0].width * (i - minWidthCount);
                        dieRectangles[i-1].y_position = dieRectangles[0].length + CoreLength;
                        
                }

                ind  = minWidthCount * 2;
        }else {

                outFile << "The die is i/o pads constrained ...\n";
                int remainingCells = dieRectangles[0].count - ((minWidthCount + minLengthCount) * 2);
                int halfRemainingCells = remainingCells /2;
                int minWidthCountOne = ((minWidthCount + minLengthCount) * 2) + remainingCells - halfRemainingCells;

                for(int i =1; i <= minWidthCountOne; i++){
                        dieRectangles[i-1].x_position = dieRectangles[0].width * i;
                        dieRectangles[i-1].y_position = 0;
                }

                CoreWidth = minWidthCountOne * dieRectangles[0].width;

                
                int minWidthCountTwo = minWidthCount + halfRemainingCells;

                for(int i =minWidthCountOne + 1; i <= minWidthCountOne + minWidthCountTwo; i++){
                        dieRectangles[i-1].x_position = dieRectangles[0].width * (i - minWidthCountOne);
                        dieRectangles[i-1].y_position = dieRectangles[0].length + CoreLength;
                        
                        }

                ind  = minWidthCountOne + minWidthCountTwo;
        }

        for (int i = ind + 1; i < ind + minLengthCount; i++){
                dieRectangles[i-1].x_position = 0;
                dieRectangles[i-1].y_position = dieRectangles[0].length  * (i - ind) ;
        }

        for (int i = ind + minLengthCount + 1; i < dieRectangles[0].count; i++){
                dieRectangles[i-1].x_position = dieRectangles[0].width + CoreWidth;
                dieRectangles[i-1].y_position = dieRectangles[0].length  * (i -ind - minLengthCount);
        }

        int usedAreadFromPads = 0;
        outFile << "Cell Number \t\t\t" << "X_Position\t\t\t" << "Y_Position\t\t\t" << "Width\t\t\t" << "Length\t\t\t\n";
        for (int i = 0; i < dieRectangles[0].count; i++){
                outFile << i << "\t\t\t\t\t\t" <<  dieRectangles[i].x_position << "\t\t\t\t\t\t" <<dieRectangles[i].y_position << "\t\t\t\t" << dieRectangles[i].width << "\t\t\t\t\t" << dieRectangles[i].length << endl;
                usedAreadFromPads += (coreRectangles[i].width * coreRectangles[i].length);
        }
        int dieWidth = CoreWidth + 2 * coreRectangles[0].width;
        int dieLength = CoreLength + 2 * coreRectangles[0].length;
        int totalArea = CoreWidth * CoreLength;
        int totalusedArea = 0;
        outFile << "-------------------------------\n";
        outFile << "After placing i/o pads:\nTotal Area: " << dieWidth * dieLength  << "\t\t\tTotal Used Area: " << totalusedArea + usedAreadFromPads << "\t\t\tUtilization Factor of Area:" << 1.0* (totalusedArea + usedAreadFromPads) / (dieWidth * dieLength);
        outFile << "\ntotalCoreArea: " << totalArea << "\t\t\ttotalUsedCoreArea: " << totalusedArea << "\t\t\tUtilization Factor of Core Area:" << 1.0* totalusedArea / totalArea;
        outFile << "\n-------------------------------\n\n";

        


        
        outFile << "\n\n\nSolving Part Two (Hard Core Cells)\n";
        outFile << "Cell Number \t\t\t" << "X_Position\t\t\t" << "Y_Position\t\t\t" << "Width\t\t\t" << "Length\t\t\t\n";
        for (int i = 0; i < coreRectangles.size(); i++){
                outFile << i << "\t\t\t\t\t\t" << CoreWidthStart + coreRectangles[i].x_position << "\t\t\t\t\t\t" << CoreLengthStart + coreRectangles[i].y_position << "\t\t\t\t" << coreRectangles[i].width << "\t\t\t\t\t" << coreRectangles[i].length << endl;
                totalusedArea += (coreRectangles[i].width * coreRectangles[i].length);
        }
        outFile << "-------------------------------\n";
        outFile << "After placing Hard Core Cells:\nTotal Area: " << dieWidth * dieLength  << "\t\t\tTotal Used Area: " << totalusedArea + usedAreadFromPads << "\t\t\tUtilization Factor of Area:" << 1.0* (totalusedArea + usedAreadFromPads) / (dieWidth * dieLength);
        outFile << "\ntotalCoreArea: " << totalArea << "\t\t\ttotalUsedCoreArea: " << totalusedArea << "\t\t\tUtilization Factor of Core Area:" << 1.0* totalusedArea / totalArea;
        outFile << "\n-------------------------------\n\n";



        outFile << "\n\n\n\nSolving Part Three (Soft Area Cells)\n";
        outFile << "Cell Number \t\t\t" << "X_Position\t\t\t" << "Y_Position\t\t\t" << "Width\t\t\t" << "Length\t\t\t\n";
        for (int i = 0; i < softRectangles.size(); i++){
                outFile << i << "\t\t\t\t\t\t" << CoreWidthStart + softRectangles[i].x_position << "\t\t\t\t\t\t" << CoreLengthStart + softRectangles[i].y_position << "\t\t\t\t" << softRectangles[i].width << "\t\t\t\t\t" << softRectangles[i].length << endl;
                totalusedArea += (softRectangles[i].width * softRectangles[i].length);
        }
        outFile << "-------------------------------\n";
        outFile << "After placing Soft Area Cells:\nTotal Area: " << dieWidth * dieLength  << "\t\t\tTotal Used Area: " << totalusedArea + usedAreadFromPads << "\t\t\tUtilization Factor of Area:" << 1.0* (totalusedArea + usedAreadFromPads) / (dieWidth * dieLength);
        outFile << "\ntotalCoreArea: " << totalArea << "\t\t\ttotalUsedCoreArea: " << totalusedArea << "\t\t\tUtilization Factor of Core Area:" << 1.0* totalusedArea / totalArea;
        outFile << "\n-------------------------------\n\n";


        outFile.close();
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

        if(!parseInputConstraints(dieHardRectangles, coreHardRectangles, softRectangles, "./In/" + fileName)){
                cout << "Input File doesnot meet the specified requirements\n";
                exit(-1);
        }

        solveProblem(dieHardRectangles, coreHardRectangles, softRectangles, stoi(argv[2]) , stoi(argv[3]));

        visualizeProblem(dieHardRectangles, coreHardRectangles, softRectangles, "./Out/" + fileName);

        

        return 0;
}
