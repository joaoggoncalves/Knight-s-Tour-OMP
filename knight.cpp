#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <omp.h>
#include <chrono>
#include <stdio.h>

using namespace std;

const int movimento[8][2]={1,2, 2,1, 2,-1, 1,-2, -1,-2, -2,-1, -2,1, -1,2};
auto start = chrono::steady_clock::now();

class tour {
  vector< vector< int > > board;
  int sx, sy, size;

public:
  bool findtour(tour& , int );

  // Constructor
  tour(int s = 5, int startx = 0, int starty = 0) :sx(startx), sy(starty), size(s)
  {
    // Get the board to size x size
    board.resize(size);
    for(int i = 0; i < size; ++i)
      board[i].resize(size);

    // Fill the board with -1s
    for(int i = 0; i < size; ++i)
      for(int j = 0; j < size; ++j)
        board[i][j] = -1;

    // Move 0
    board[sx][sy] = 0;

    // Solve the problem
    #pragma omp parallel num_threads(8)
    {
      #pragma omp single
      {
        if(!findtour(*this, 0)) {
          cout << "No solutions found\n";
        }
      }
    }
  }

  // Copy constructor
  tour(const tour& T): sx(T.sx), sy(T.sy), size(T.size) {
    this->board.resize(size);
    for(int i = 0; i < size; ++i)
      board[i].resize(size);

    // Copy the board
    for(int i = 0; i < size; ++i)
      for(int j = 0; j < size; ++j)
        board[i][j] = T.board[i][j];
  }

  // Function to output class to ostream
  friend std::ostream& operator<<
  (std::ostream& os, const tour& T);
};


std::ostream& operator<<(std::ostream& os, const tour& T) {
  int size = T.size;

  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j)
      os <<  setw(2) << T.board[i][j] << " ";
    os << endl;
  }

  return os;
}

// A recursive function to find the knight tour.
bool tour::findtour(tour& T, int imove) {
  if(imove == (size*size - 1)) return true;
  // make a move
  int cx = T.sx;
  int cy = T.sy;
  int cs = T.size;
  //int tnum = omp_get_thread_num();
  //int nivel = omp_get_level();
  for (int i = 0; i < 8; ++i) {
    int tcx = cx + movimento[i][0];
    int tcy = cy + movimento[i][1];
    //cout << "Thread: " << tnum << endl << "Tour: " << T << endl;
    //cout << "Thread: " << tnum << endl;
    // Is this a valid move?
    // Has this place been visited yet
    if ((tcx >= 0) &&  (tcy >= 0)  &&  (tcx < cs) &&  (tcy < cs) && (T.board[tcx][tcy] == -1)) {
      tour temp(T);
      temp.board[tcx][tcy] = imove+1;
      temp.sx = tcx;
      temp.sy = tcy;
      #pragma omp task if (i < 2)
      {
        if(findtour(temp, imove+1)) {
          cout << temp << endl;
          auto end = chrono::steady_clock::now();
          auto diff = end - start;
          cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
          exit(1);
        }
      }
    }
  }
  #pragma omp taskwait
  return false;
}


int main(void) {
  int table_size, start_x, start_y;

  cin >> table_size >> start_x >> start_y;

  start = chrono::steady_clock::now();
  tour T(table_size, start_x, start_y);
  return 0;
}

// Adapted with permission from the original implementation by
// prof. Piyush Kumar (Florida State University)
// http://www.compgeom.com/~piyush/
