/**********************************************************************
twoPlayerComp.cpp

Two player 10 x 10 five in a row tic-tac-toe where computer can play
one side.
***********************************************************************/

#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cctype>

#include "gmbrd.hpp"

using namespace std;

/***********************************************************************
CONSTANTS
************************************************************************/

static const int LOWDIM = 0,
                 HIGHDIM = 9;

/***********************************************************************
PROTOTYPES
************************************************************************/

void displayBoard(board &b);
void readWeights(ifstream &ifl, vector<double> &w1, vector<double> &w2);
int getGameType();
void outputResult(int rsltCode);
int playGame(int gType, board &brd, char gmChrs[], vector<double> &wt1,
                                                    vector<double> &wt2);
bool isComputer(int gType, int curP);
int humanMove(board &b, int curP, char gCh[]);
void humanRC(int cp, int &r, int &c);
int makeMove(board &brd, vector <double> wgts, char bVals[], int player);
double vHat(vector <double> &w, vector <double> &x, double extraW);
void getXvals(board &brd, vector <double> &varVals, char pc1, char pc2);
void processLane(const string &str, vector <double> &x, char c1, char c2);
int captureCheck(board &brd, int r, int c, char bVals[], int player);

//**********************************************************************

int main (int argc, char *argv[])
{ board brd('.');
  char ic,
       gmChars[] = {'.', 'X', 'O'};
  int tmpInt;
  string ibuf;
  vector <double> wt1, //-> WEIGHTS FOR COMPUTER AS PLAYER 1
  	                wt2; //-> WEIGHTS FOR COMPUTER AS PLAYER 2
  ifstream ifl;

//*** ENSURE THAT WE HAVE THE CORRECT NUMBER OF PARAMETERS
if (argc != 2)
   {
   cout << "USAGE: " << argv[0] << "<[path] name of weight file" << endl;
   exit(0);
   }

//*** OPEN WEIGHT FILE - IF UNSUCCESSFUL OUTPUT ERROR AND QUIT
ifl.open(argv[1]);
if (!ifl)
   {
   cout << "Could not open the input file..." << endl;
   exit(0);
   }

//*** GET WEIGHTS AND CLOSE FILE - ERROR MSG & QUIT IF THERE'S A PROBLEM
readWeights(ifl, wt1, wt2);
ifl.close();

cout << "Weights read in...\n" << endl;

//*** PLAY GAMES UNTIL THE USER TIRES OF THEM
while ((tmpInt = getGameType()) != 4)
   outputResult(playGame(tmpInt, brd, gmChars, wt1, wt2));

return 0;
}

//**********************************************************************

void displayBoard(board &b)
{
cout << " |0|1|2|3|4|5|6|7|8|9|\n";

for (int i = 0; i < DIM; i++)
	{
	cout << i;
	for (int j = 0; j < DIM; j++)
		{
		cout << '|' << b[i][j];
		}
	cout << "|\n";
	}
}

//**********************************************************************

void readWeights(ifstream &ifl, vector<double> &w1, vector<double> &w2)
{ string ibuf;
  double td;
  int num1;

//*** ENSURE VECS START EMPTY & GET THE NUMBER OF WEIGHTS IN THE FIRST VECTOR
w1.clear();
w2.clear();
getline(ifl, ibuf);
num1 = strtol(ibuf.c_str(), NULL, 10);

//*** GET FIRST SET OF VECTORS
for (int i = 0; (i < num1) && getline(ifl, ibuf); i++)
   {
   td = strtod(ibuf.c_str(), NULL);
   w1.push_back(td);
   }

//*** GET SECOND SET OF VECTORS
while(getline(ifl, ibuf))
   {
   td = strtod(ibuf.c_str(), NULL);
   w2.push_back(td);
   }

//*** CHECK FOR IMPROPER NUMBERS - ASSUME IN THIS CASE BOTH SAME
if ((w1.size() != num1) || (w2.size() != num1))
   {
   cout << "Both sets of weight must have " << num1 << "each...\n"
        << "Set 1 size: " << w1.size()
        << "   Set 2 size: " << w2.size() << endl;
   exit(0);
   }
}

//************************************************************

int getGameType()
{ int choice;
  string ibuf;

do
   {
   cout << "CHOOSE TYPE OF GAME:\n"
        << "1. Human vs. Human\n"
        << "2. Computer (Player 1) vs. Human (Player 2)\n"
        << "3. Human (Player 1) vs. Computer (Player 2)\n"
        << "4. Quit\n\n"
        << "Your Choice: ";

   getline(cin, ibuf);
   choice = strtol(ibuf.c_str(), NULL, 10);

   if ((choice < 1) || (choice > 4))
      cout << "Choice must be an integer in the range 1-4... try again\n\n";
   }
while ((choice < 1) || (choice > 4));
return choice;
}

//**************************************************************

void outputResult(int rsltCode)
{
switch (rsltCode)
      {
      case 0:
         cout << "Game is a draw..." << endl;
         break;
      case 1:
         cout << "Player 1 wins!" << endl;
         break;
      case 2:
         cout << "Player 2 wins!" << endl;
         break;
      default:
         cout << "CHError: gameplay returned bad code" << endl;
         break;
      }
}


//**************************************************************

int playGame(int gType, board &brd, char gmChrs[], vector<double> &wt1,
                                                   vector<double> &wt2)
{ int result = -1,
      curPlayer = 1,
      tint,
      caps[2] = {0, 0}; //-- CAPTURES...[0]:p1, [1]:p2

  vector < location > empties;


//*** SET UP FOR THE GAME
brd.fillAll(gmChrs[0]);
displayBoard(brd);

do
	{
	//*** GET NEXT MOVE
	if (isComputer(gType, curPlayer))
	   {
	   if (curPlayer == 1)
	      caps[0] += makeMove(brd, wt1, gmChrs, curPlayer);
	   else
	      caps[1] += makeMove(brd, wt2, gmChrs, curPlayer);
	   }
	else
	   caps[curPlayer - 1] += humanMove(brd, curPlayer, gmChrs);

	//*** DISPLAY NEW BOARD AND CHECK FOR A WINNER
	displayBoard(brd);
	cout << "Captures by Player 1: " << caps[0]
	     << "      Captures by Player 2: "  << caps[1] << endl;
	if ((tint = brd.victoryCheck("XXXXX", "OOOOO")) != 0)
	   result = tint;
	else if (caps[curPlayer - 1] >= 5)
	   result = curPlayer;

	//*** CHECK FOR A DRAW - IF NONE SET NEXT PLAYER TO GO
	brd.getEmptySquares(empties, gmChrs[0]);
	if (!empties.size())
	   result =  0;

	curPlayer = 3 - curPlayer;
	}
while (result < 0);

return result;
}

//*******************************************************************

bool isComputer(int gType, int curP)
{
if ((gType - curP) == 1)
   return true;

return false;
}

//*******************************************************************

int humanMove(board &b, int curP, char gCh[])
{ int r, c;


humanRC(curP, r, c);

while (b[r][c] != gCh[0])
   {
   if (b[r][c] != gCh[0])
		cout << "That space is already taken - "
		     << " please choose one that is open\n" << endl;

	humanRC(curP, r, c);
   }

b[r][c] = gCh[curP];
return captureCheck(b, r, c, gCh, curP);
}

//*******************************************************************

void humanRC(int cp, int &r, int &c)
{ string ibuf;
  bool notValid = true;

do
   {
   cout << "\nPlayer " << cp << ", please enter your next move "
	     << "(row (0-9), then a space, then column (0-9)): ";

   getline(cin, ibuf);

   if (ibuf.size() != 3)
      cout << "You must enter a digit for the row, one space, "
           << "and a digit for the column..." << endl;
   else if (!isdigit(ibuf[0]))
      cout << "The row value should be a single digit..." << endl;
   else if (!isdigit(ibuf[2]))
      cout << "The column value should be a single digit..." << endl;
   else if (ibuf[1] != ' ')
      cout << "There should be one space between the row & column digits..."
           << endl;
   else
      notValid = false;

   if (notValid)
      cout << "    Please try again...\n" << endl;
   else
      {
      r = ibuf[0] - '0';
      c = ibuf[2] - '0';
      }
   }
while (notValid);
}

//*********************************************************************

int makeMove(board &brd, vector <double> wgts, char bVals[], int player)
{  char blank = bVals[0],      //*** CHARACTER REPRESENTING BLANK
        plChr = bVals[player]; //*** CHARACTER REPRESENT CURRENT PLAYER
   int rMx = -1, //*** ROW OF BEST SQUARE SO FAR, -1 ALLOWS TEST FOR FIRST
       cMx; //*** COLUMN OF BEST SQUARE SO FAR

   board tbrd;

	double vhMx, //*** GREATEST VALUE OF VHAT SEEN SO FAR
	       cVH;  //*** vHat of current item

	vector <double> varVals;

//*** FIND THE BEST SQUARE (ONE WITH HIGHEST VHAT VALUE)
for (int r = 0; r < 10; r++)
   for (int c = 0; c < 10; c++)
      if (brd[r][c] == blank) //*** WE'VE GOT ONE TO CHECK
         {
         tbrd = brd;
         tbrd[r][c] = plChr; //*** SUPPOSE WE MOVE HERE
         getXvals(tbrd, varVals, bVals[1], bVals[2]);
         cVH = vHat(varVals, wgts, 0);
         if ((rMx == -1) || (vhMx < cVH)) //*** WE HAVE A NEW MAX
            {
            rMx = r;
            cMx = c;
            vhMx = cVH;
            }
         }

if (rMx == -1) //*** NO BLANKS FOUND - JUST QUIT
   return 0;

//*** OK, USE THE BEST ONE
brd[rMx][cMx] = plChr;
cout << "\nComputer (Player " << player << ") plays to square: "
     << rMx << ", " << cMx << endl;
return captureCheck(brd, rMx, cMx, bVals, player);
}

//***************************************************************

double vHat(vector <double> &w, vector <double> &x, double extraW)
{ double rval = 0.0;

if (w.size() != x.size())
	throw "vHat: weight & x vectors are different sizes";

for (int i = 0; i < w.size(); i++)
	rval += w[i] * x[i];

rval += extraW;

return rval;
}

//***************************************************************

void getXvals(board &brd, vector <double> &varVals, char pc1, char pc2)
{ string tmpLane;

//*** RESET VARIABLES TO ALL ZEROS
varVals.clear();
varVals.resize(10, 0.0);

//*** PROCESS ROW LANES
for (int i = 0; i < 10; i++)
   {
   brd.getRow(i, tmpLane);
   processLane(tmpLane, varVals, pc1, pc2);
   }

//*** PROCESS COLUMN LANES
for (int i = 0; i < 10; i++)
   {
   brd.getCol(i, tmpLane);
   processLane(tmpLane, varVals, pc1, pc2);
   }

//*** PROCESS RD DIAGONAL LANES
for (int i = 0; i < 11; i++)
   {
   brd.getDiagRD(i, tmpLane);
   processLane(tmpLane, varVals, pc1, pc2);
   }

//*** PROCESS LD DIAGONAL LANES
for (int i = 0; i < 11; i++)
   {
   brd.getDiagLD(i, tmpLane);
   processLane(tmpLane, varVals, pc1, pc2);
   }
}

/**********************************************************
countVales: VERY LOCAL HELPER FUNCTION
***********************************************************/

void countVals(const string &s, char c1, char c2, int &cot1, int &cot2)
{
cot1 = cot2 = 0;
for (int i = 0; i < s.size(); i++)
   {
   if (s[i] == c1)
      cot1++;
   else if (s[i] == c2)
      cot2++;
   }
}

//**********************************************************

void processLane(const string &str, vector <double> &x, char c1, char c2)
{ int lim = str.size() - 4,
      count1,
      count2;

for (int i = 0; i < lim; i++)
   {
	countVals(str.substr(i, 5), c1, c2, count1, count2);
	if (count1 && !count2) //*** HAVE SOMETHING FOR PLAYER 1
	   x[count1 - 1]++;
	else if (!count1 && count2) //*** HAVE SOMETHING FOR PLAYER 2
	   x[count2 + 4]++;
	}
}

//**********************************************************



int captureCheck(board &brd, int r, int c, char bVals[], int player)
/*----------------------------------------
Check for capture(s) where r & c are row and column of last
move.  If there is one or more captures, remove captured pieces and
return number of captures.  Otherwise, don't change the board and
return 0.

player is the last player to move.

-------------------------------------------*/
{ char lastC = bVals[player], //-- Character of last piece played
       otherC = bVals[3 - player], //-- Character of other player
       blnk = bVals[0]; //-- The blank character

  string compStr = "@@@", //-- String holding pattern for comparison
         tstr;    //-- String to be tested
  int numCaps = 0, //-- Number of captures discovered
      tr, //-- TEMPORARY ROW VALUE
      tc; //-- TEMPORARY COLUMN VALUE

//--- SET UP THE compStr ARRAY
  compStr[0] = compStr[1] = otherC;
  compStr[2] = lastC;

//--- RUN THROUGH CHECKS - MAKING ADJUSTMENTS AS NECESSARY
for (int rs = -1; rs < 2; rs++) //-- Possible r shifts
   {
   for (int cs = -1; cs < 2; cs++) //-- Possible c shifts
      {
      //--- SKIP IF rs == 0 & cs == 0 B/C THIS PATTERN IS NOT USED
      if (!cs && !rs)
         continue;

      //--- SKIP IF DOING THIS CHECK TAKES US OUTSIDE OF THE BOARD BOUNDARIES
      tr = r + rs * 3;
      tc = c + cs * 3;

      if ((tr < LOWDIM) || (tr > HIGHDIM) || (tc < LOWDIM) || (tc > HIGHDIM))
         continue;

      //--- BUILD tstr USING THE GIVEN OFFSETS
      tstr = "";
      for (int i = 1; i < 4; i++)
         tstr += brd[r + i*rs][c + i*cs];

      //--- HANDLE A CAPTURE IF WE'VE FOUND ONE
      if (tstr == compStr) //-- Handle the capture.
         {
         numCaps++;
         brd[r + 1*rs][c + 1*cs] = brd[r + 2*rs][c + 2*cs] = blnk;
         }
      }
   }
return numCaps;
}