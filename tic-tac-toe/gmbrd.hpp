/********************************************************
gmbrd.hpp

Class definitions for 10 x 10 board for 5 in-a-row 
tic-tac-toe
*********************************************************/

#ifndef GMBRD_HPP
#define GMBRD_HPP

#include <string>
#include <vector>

using namespace std;

static const int DIM = 10,  //*** DIMENSION OF THE SQUARE BOARD
					  NUMDIAGS = 11; //*** NUMBER OF DIAGONALS IN A GIVEN DIRECTION
                 

/****************************************
struct location
*****************************************/

struct location
{
int r, c;
};

/****************************************
class board
*****************************************/

class board
{
string brd[DIM];

public:

board(char c = ' ');
void fillAll(char c);
string &operator[](int idx){return brd[idx];}
void getRow(int row, string &str);
void getCol(int col, string &str);
void getDiagRD(int diagIdx, string &str);
void getDiagLD(int diagIdx, string &str);
int victoryCheck(const string &c1, const string &c2);
void getEmptySquares(vector <location> &eSquares, char eVal);
};

#endif