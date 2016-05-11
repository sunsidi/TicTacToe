/********************************************************
gmbrd.cpp

External function definitions for class definitions for 
10 x 10 board for 5 in-a-row tic-tac-toe
*********************************************************/

#include <iostream>

#include "gmbrd.hpp"

using namespace std;

/****************************************
Local definitions
*****************************************/

struct diagData
{
int rstart,
    cstart,
    len;
};

/****************************************
Local Data
*****************************************/

const diagData 
lftDn[] = { {0, 4, 5}, {0, 5, 6}, {0, 6, 7}, {0, 7, 8}, {0, 8, 9}, {0, 9, 10},
          {1, 9, 9}, {2, 9, 8}, {3, 9, 7}, {4, 9, 6}, {5, 9, 5} },
          
rgtDn[] = { {5, 0, 5}, {4, 0, 6}, {3, 0, 7}, {2, 0, 8}, {1, 0, 9}, {0, 0, 10},
          {0, 1, 9}, {0, 2, 8}, {0, 3, 7}, {0, 4, 6}, {0, 5, 5} };
          

          
/****************************************
Local Helper Functions
*****************************************/



/****************************************
class board
*****************************************/

board::board(char c)
{
for (int i = 0; i < DIM; i++)
	brd[i].resize(DIM, c);
}

//***************************************

void board::fillAll(char c = ' ')
{
for (int i = 0; i < DIM; i++)
	for (int j = 0; j < DIM; j++)
		brd[i][j] = c;
}

/****************************************
getRow
Returns a full row as a string
*****************************************/

void board::getRow(int row, string &str)
{ 
if ((row < 0) || (row >= DIM))
	throw "board::getRow> row value out of bounds";
	
str = brd[row];
}

/****************************************
getCol
Returns a full column as a string
*****************************************/

void board::getCol(int col, string &str)
{ 
if ((col < 0) || (col >= DIM))
	throw "board::getCol> column value out of bounds";
	
str = "";

for (int row = 0; row < DIM; row++)
	str += brd[row][col];
}

/****************************************
getDiagRD
Returns a full right-and-down diagonal
as a string... index should be in range 0-10
*****************************************/

void board::getDiagRD(int diagIdx, string &str)
{ int row,
      col,
      lim;

if ((diagIdx < 0) || (diagIdx >= NUMDIAGS))
	throw "board::getDiagRD> diagonal index out of bounds";

//*** SET UP INITIAL VALUES DEFORE BUILDING THE STRING	
str = "";
lim = rgtDn[diagIdx].len;
row = rgtDn[diagIdx].rstart;
col = rgtDn[diagIdx].cstart;

//*** CREATE THE STRING
for (int i = 0; i < lim; i++)
	{
	str += brd[row + i][col + i];
	}
}

/****************************************
getDiagLD
Returns a full left-and-down diagonal
as a string... index should be in range 0-10
*****************************************/

void board::getDiagLD(int diagIdx, string &str)
{ int row,
      col,
      lim;

//*** VALIDATE THE DIAGONAL INDEX
if ((diagIdx < 0) || (diagIdx >= NUMDIAGS))
	throw "board::getDiagLD> diagonal index out of bounds";

//*** SET UP INITIAL VALUES DEFORE BUILDING THE STRING	
str = "";
lim = lftDn[diagIdx].len;
row = lftDn[diagIdx].rstart;
col = lftDn[diagIdx].cstart;

//*** CREATE THE STRING
for (int i = 0; i < lim; i++)
	str += brd[row + i][col - i];
}

/***************************************
Searches for victory where v1Pat is the pattern
for player 1's victory and v2Pat is the pattern
for player 2's victory.  Note that each of these
should be a string of length 5.  For instance,
v1Pat could be 5 x's and v2Pat 5 o's.

Returns 0 if no victor is discovered.  Returns
1 if player 1 is victorious and 2 if player 2 is 
victorious.  It stops at the first victory found.
****************************************/

int board::victoryCheck(const string &v1Pat, const string &v2Pat)
{ string tstr;
  int rlim,
      clim;

//*** ENSURE THAT PATTERNS ARE THE RIGHT SIZE      
if ((v1Pat.size() != 5) || (v2Pat.size() != 5))
   return 0; //*** Pattern(s) wrong size - no victor

//*** CHECK RIGHT
for (int i = 0; i < DIM; i++)
	{
	getRow(i, tstr);
	if (tstr.find(v1Pat, 0) != string::npos)
		return 1;
	if (tstr.find(v2Pat, 0) != string::npos)
		return 2;
	}

//*** CHECK DOWN
for (int i = 0; i < DIM; i++)
	{
	getCol(i, tstr);
	if (tstr.find(v1Pat, 0) != string::npos)
		return 1;
	if (tstr.find(v2Pat, 0) != string::npos)
		return 2;
	}

//*** CHECK DIAGONALLY RIGHT AND DOWN
for (int i = 0; i < NUMDIAGS; i++)
	{
	getDiagRD(i, tstr);
	if (tstr.find(v1Pat, 0) != string::npos)
		return 1;
	if (tstr.find(v2Pat, 0) != string::npos)
		return 2;
	}
	   
//*** CHECK DIAGONALLY LEFT AND DOWN
for (int i = 0; i < NUMDIAGS; i++)
	{
	getDiagLD(i, tstr);
	if (tstr.find(v1Pat, 0) != string::npos)
		return 1;
	if (tstr.find(v2Pat, 0) != string::npos)
		return 2;
	}

return 0;
}

/***************************************
Adds to eSquares r,c coords of every square
holding the value eVal.
(eVal stands for the value that means an 
empty square)
****************************************/
void board::getEmptySquares(vector <location> &eSquares, char eVal)
{ location tLoc;

eSquares.clear();

for (int r = 0; r < DIM; r++)
	for (int c = 0; c < DIM; c++)
		{
		if (brd[r][c] == eVal)
			{
		   tLoc.r = r;
		   tLoc.c = c;
		   eSquares.push_back(tLoc);
		   }
		}
}