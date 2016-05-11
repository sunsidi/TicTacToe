#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <math.h> 
#include <algorithm>

using namespace std;
/**************************************
structure for holding training examples
**************************************/
struct trainExample
{
vector<double> varVals; //-> VALUES FOR THE VARIABLES
};
struct exampleSet
{
vector<trainExample> examples; //-> THE EXAMPLES THEMSELVES
double startTarget; //-> STARTING TARGET (TARGET FOR LAST BOARD GENERATED)
};
/**************************************
Global Variable for holding new weights
**************************************/
vector<double> newwgts1;
vector<double> newwgts2;
/**************************************
prototype
**************************************/
void training(ofstream &trace, vector<exampleSet> &ts1, vector<exampleSet> &ts2, vector<double> &wt1, vector<double> &wt2, vector<double> &nwt1, vector<double> &nwt2, double, int);
double trainRun(vector<exampleSet> &tests, vector<double> &weights, vector<double> &nwt);
double trainOnSet(exampleSet &set, vector<double> &wts, vector<double> &nwt);
double updateWeights(vector<double> &wt, trainExample &var, double, vector<double> &nwt);
double vHat(vector <double> &w, vector <double> &x, double);
void readTrainingSets(ifstream &ifl, vector<exampleSet> &sets1, vector<exampleSet> &sets2, int, int);
int readSet(ifstream &ifl, exampleSet &theSet, int, int);
void getExample(ifstream &ifl, trainExample &example, int);

/**************************************
Functions for reading in proto-tests
**************************************/
void readTrainingSets(ifstream &ifl, vector<exampleSet> &sets1, vector<exampleSet> &sets2, int numVars1, int numVars2) 
{ 
	string ibuf; //-> GENERAL INPUT BUFFER
	int owner; //-> KEEPS TRACK OF WHO THE CURRENT SET APPLIES TO
	exampleSet tempSet; //-> USED TO GET THE CURRENT SET
	//*** ENSURE THAT TRAINING SET VECTORS START EMPTY
	sets1.clear();
	sets2.clear();
	
	//*** OVERALL READ LOOP... TAKES ADVANTAGE OF EMPTY LINE PRECEEDING EACH SET
	// IN THE FILE... THIS PART IS READ IN THE WHILE CONDITION, THE REST IS READ
	// INSIDE THE LOOP
	while (getline(ifl, ibuf))
	{
		//*** MAKE SURE WE’RE READING SEPARATOR (EMPTY) LINE... ERROR IF NOT
		if (ibuf != "")
		{
			cout << "Error! Missing Separator Line!" << endl;
			exit(EXIT_FAILURE);
		}
	
		//*** GET THE SET AND PUSH IT ONTO THE CORRECT VECTOR
		owner = readSet(ifl, tempSet, numVars1, numVars2);
		
		if (owner == 1)
		sets1.push_back(tempSet);
		else
		sets2.push_back(tempSet);
	}
}

int readSet(ifstream &ifl, exampleSet &theSet, int nv1, int nv2) 
{
	string ibuf; //-> GENERAL INPUT BUFFER
	trainExample tex; //-> TEMP TRAINING EXAMPLE FOR PUSHING ONTO SET
	int pNum, //-> PLAYER (1 OR 2)
	rslt, //-> NUMBER GIVING RESULT
	numVars; //-> NUMBER OF VARIABLE VALUES TO READ FOR EACH BOARD
	char ichar; //-> INPUT BUFFER FOR A SINGLE CHARACTER
	const int WINTARGET = 100, //-> TARGET VALUE FOR WIN
			  LOSSTARGET = -100, //-> TARGET VALUE FOR LOSS
			  DRAWTARGET = 0; //-> TARGET VALUE FOR DRAW
			  
	//*** ENSURE THAT THE EXAMPLE SET IS EMPTY (HAS NO EXAMPLES)
	theSet.examples.clear();
	//*** GET FIRST LINE - PLAYER & RESULT SEPARATED BY SINGLE BLANK
	
	getline(ifl, ibuf, ' '); //-> READ TO THE SPACE - THROWING SPACE AWAY
	pNum = strtol(ibuf.c_str(), NULL, 10);
	
	getline(ifl, ibuf); //-> READ TO END OF LINE
	rslt = strtol(ibuf.c_str(), NULL, 10);
	
	//*** DETERMINE IF WIN, LOSS, OR DRAW AND SET THE PROPER TARGET VALUE
	if (rslt == 0)
	theSet.startTarget = DRAWTARGET;
	else if (rslt == pNum)
	theSet.startTarget = WINTARGET;
	else
	theSet.startTarget = LOSSTARGET;
	//*** SET PROPER VALUE FOR THE NUMBER OF VARIABLE VALUES PER LINE
	if (pNum == 1)
	numVars = nv1;
	else
	numVars = nv2;
	
	//*** GET SETS AND ADD TO THE LIST
	ifl.get(ichar);	
	while (ichar != '@')
	{	//cout<<ichar;

		getExample(ifl, tex, numVars);
		//*** PUT EXAMPLE INTO THE EXAMPLE SET & START THE NEXT EXAMPLE (LINE)
		theSet.examples.push_back(tex);			
		ifl.get(ichar); //-> START NEXT LINE
		
	}
	
	getline(ifl, ibuf); //-> FLUSH ’\n’ LEFT ON LINE WITH ’@’
	return pNum;
}

void getExample(ifstream &ifl, trainExample &example, int numVars) 
//*** RETURN TRUE IF
// RETURNS THE NUMBER OF THE PLAYER TO WHOM THE SET BELONGS
{
	string ibuf; //-> GENERAL INPUT BUFFER
	trainExample tex; //-> TEMP TRAINING EXAMPLE FOR PUSHING ONTO SET
	int lim = numVars - 1; //-> LIMIT FOR for LOOP THAT READS IN VARIABLE VALUES (numVars - 1)
	double tDoub; //-> TEMPORARY DOUBLE
	//*** ENSURE THAT THE VARIABLE VALUE LIST STARTS EMPTY
	example.varVals.clear();
	for (int i = 0; i < lim; i++) //-> ALL BUT THE LAST (LAST HAS ’\n’ AFTER)
	{
		getline(ifl, ibuf, ' ');
		tDoub = strtod(ibuf.c_str(), NULL);
		example.varVals.push_back(tDoub);
	}
	//*** DO THE LAST VARIABLE VALUE
	getline(ifl, ibuf);
	tDoub = strtod(ibuf.c_str(), NULL);
	example.varVals.push_back(tDoub);
}

/**************************************
Functions for training weights
**************************************/


void training(ofstream &trace, vector<exampleSet> &ts1, vector<exampleSet> &ts2, vector<double> &wt1, vector<double> &wt2, vector<double> &nwt1,vector<double> &nwt2, double aveErrLim, int iterMax) 
{ 
	double p1Err = aveErrLim + 1, //-> AVE SQ ERR FOR PLAYER 2 - START HIGH
		   p2Err = aveErrLim + 1; //-> AVE SQ ERR FOR PLAYER 2 - START HIGH
	int numUnder = 0; //-> NUMBER OF PLAYERS UNDER THE ERROR LIMIT
	
	for (int i = 0; (numUnder < 2) && (i < iterMax); i++)
	{
		//*** DO A RUN ON PLAYER 1 DATA IF NECESSARY
		if (p1Err >= aveErrLim) //-> ERROR IS STILL TOO HIGH SO TRAIN
		{
			p1Err = trainRun(ts1, wt1, nwt1);
			if (p1Err < aveErrLim) //-> DONE WITH THIS ONE
			numUnder = numUnder + 1;
		}
		//*** DO A RUN ON PLAYER 2 DATA IF NECESSARY
		if (p2Err >= aveErrLim) //-> ERROR IS STILL TOO HIGH SO TRAIN
		{
			p2Err = trainRun(ts2, wt2, nwt2);
			if (p1Err < aveErrLim) //-> DONE WITH THIS ONE
			numUnder = numUnder + 1;
		}
		//*** OUTPUT DATA TO TRACE FILE
		trace << "Iteration: " << i+1 << " P1 Error: " << p1Err << " P2 Error: " << p2Err <<'\n';
	} // end of for loop
}

double trainRun(vector<exampleSet> &tests, vector<double> &weights, vector<double> &newwgts)
//*** RETURNS THE AVERAGE OF THE SQUARED ERROR
{ 
	double sumSqErr = 0.0, //-> SUM OF SQUARED ERROR
		   totNumExamples = 0; //-> TOTAL NUMBER OF EXAMPLES
	
	for (int i = 0; i < tests.size(); i++)
	{
		sumSqErr += trainOnSet(tests[i], weights, newwgts);
		totNumExamples += tests[i].examples.size();
	}
	return sumSqErr/totNumExamples;
}

double trainOnSet(exampleSet &set, vector<double> &wts, vector<double> &nwt) 
//*** RETURNS THE SUM OF THE SQUARED ERRORS OF THE EXAMPLES IN THE SET
{ 
	double sumSqErr = 0.0, //-> SUM OF SQUARED ERROR
		   target = set.startTarget; //-> TARGET FOR NEXT ITEM
		   
	for (int i = 0; i < set.examples.size(); i++)
	{
		//*** MODIFY THE WEIGHTS AND GET NEW ERROR TERM
		sumSqErr += updateWeights(wts, set.examples[i], target, nwt);
		//*** TARGET FOR NEXT ITERATION IS vhat VALUE USING NEW WEIGHTS
		target = vHat(wts, set.examples[i].varVals, 0);
		
	}
	return sumSqErr;
}


double updateWeights(vector<double> &wt, trainExample &var, double tar, vector<double>& newwgts)
{
	double err,
		   sqerr,
		   newwgt,
		   rate = 1.0,
		   extraW = 0.0;
	
	
	//check if the number of weights and number of factors are the same
	if (wt.size() != var.varVals.size())
	throw "vHat: weight & x vectors are different sizes";
	
	newwgts.clear();//empty the new weight vector
	//calculate error term
	err = tar - vHat(wt, var.varVals, extraW);
	
	//square of err
	sqerr = pow(err, 2.0);
	//calculate new weights and push into vector newwgts
	for (int i = 0; i < wt.size(); i++)
	{
		newwgt = wt[i] + (var.varVals[i] * rate * err);
		newwgts.push_back(newwgt);
	}
	
	return sqerr;
		   
}

double vHat(vector <double> &w, vector <double> &x, double extraW)
{ 
	double rval = 0.0;

	if (w.size() != x.size())
		throw "vHat: weight & x vectors are different sizes";
	
	for (int i = 0; i < w.size(); i++)
		rval += w[i] * x[i];
	
	rval += extraW;
	
	return rval;
}
int main ( int argc, char *argv[] )
{	
	// check if there are 2 arguments
	if ( argc != 2 ) 
    //print argv[0] assuming it is the program name
    cout<<"usage: "<< argv[0] <<" <filename>\n";
	
	else {
    	//argv[1] is a filename to open
    	ifstream ifile ( argv[1] );
    	//check to see if file opening succeeded
    	if ( !ifile.is_open())
      	cout<<"Could not open input file\n";
      	//if file is open successfully
    	else 
    	{
    		string startingwgts,//name of the starting weights file
    			   revisedwgts,//revised weights file name
    			   proto,//training file name
    			   trace,//trace file name
    			   ibuff;//buffer
    		double AVT;// average square error term
    		int MAX;//max loop
    		int numofwgts;//number of weights fro each player
    		vector<exampleSet> tests1;//hold the training values for player 1
    		vector<exampleSet> tests2;//hold the training values for player 2
    		vector<double>player1;//hold wgts for player1
      		vector<double>player2;//hold wgts for player2
    		
    		//read file into variables
    		getline(ifile, startingwgts);//starting weights file
    		getline(ifile, revisedwgts);//revised weights file
    		getline(ifile, proto);//proto-example file
    		getline(ifile, trace);//trace file
    		//get the AVT & convert to double
    		getline(ifile, ibuff);
    		AVT = strtod(ibuff.c_str(), NULL);
    		//get the max training number & convert to int
    		getline(ifile, ibuff);
    		MAX = strtol(ibuff.c_str(), NULL, 10);
    		//close the control file
    		ifile.close();
    		
    		//open the starting weights file
    		ifstream ifile2;
    		ifile2.open(startingwgts.c_str());
    		if (!ifile2.is_open())
      		cout<<"Could not open start weights file\n";
      		else
      		{
      			
      			string line;
      			double weight;
      			getline(ifile2,line);
      			numofwgts = strtol(line.c_str(), NULL, 10);
      			//read in the weights for player 1
      			for(int i = 0; i < 10; i++)
      			{	
      				getline(ifile2,line);
      				weight = strtod(line.c_str(), NULL);
      				player1.push_back(weight);
      			}
      			//read in the weights for player 2
      			for(int i = 0; i < 10; i++)
      			{	
      				getline(ifile2,line);
      				weight = strtod(line.c_str(), NULL);
      				player2.push_back(weight);
      			}
      			ifile2.close();
      		}
      		//open the proto file
      		ifstream ifile1;
      		ifile1.open(proto.c_str());
    		if (!ifile1.is_open())
      		cout<<"Could not open proto file\n";
      		else
      		{
    			readTrainingSets(ifile1, tests1, tests2, 10, 10);
    			ifile1.close();
      		}
      		//open the trace file
      		ofstream ofile1;
      		ofile1.open(trace.c_str());
      		if (!ofile1.is_open())
      		cout<<"Could not open trace file\n";
      		else
      		{
      			training(ofile1, tests1, tests2, player1, player2, newwgts1, newwgts2, 		AVT, MAX);
      			ofile1.close();
      		}
      		//open the revised weights file
      		ofstream ofile2;
      		ofile2.open(revisedwgts.c_str());
      		if (!ofile2.is_open())
      		cout<<"Could not open revised weights file\n";
      		else
      		{	
      			ofile2 << numofwgts << endl;
      			for(int i = 0; i < newwgts1.size(); i++)
      			ofile2 << newwgts1[i] << endl;
      			for(int i = 0; i < newwgts2.size(); i++)
      			ofile2 << newwgts2[i] << endl;
      			ofile2.close();
      		}
      		
      		cout << "Training Done!" << endl;
      		
    	}
    }
}
    	