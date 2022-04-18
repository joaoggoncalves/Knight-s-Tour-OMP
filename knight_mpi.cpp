#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <stdio.h>
#include <mpi.h>

using namespace std;

const int movimento[8][2]={1,2, 2,1, 2,-1, 1,-2, -1,-2, -2,-1, -2,1, -1,2};
int numtasks, idprocesso;
auto start = chrono::steady_clock::now();

class tour {
    vector< vector< int > > board;
    int sx, sy, size;

public:
    bool separatrabalho(tour& , int );
    bool findtour_completo(tour&, int );

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
        if(!separatrabalho(*this, 0)) {
            cout << "No solutions found by process of rank: " << idprocesso << endl;
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
bool tour::separatrabalho(tour& T, int imove) {
    if(imove == (size*size - 1)) return true;
    // make a move
    int cx = T.sx;
    int cy = T.sy;
    int cs = T.size;
    if (numtasks >= 8) {
        for (int i = 0; i < 8; ++i) {
            int tcx = cx + movimento[i][0];
            int tcy = cy + movimento[i][1];
            if ((tcx >= 0) &&  (tcy >= 0)  &&  (tcx < cs) &&  (tcy < cs) && (T.board[tcx][tcy] == -1)) {
                tour temp(T);
                temp.board[tcx][tcy] = imove+1;
                temp.sx = tcx;
                temp.sy = tcy;
                if (idprocesso == i) {
                    findtour_completo(temp, imove+1);
                }
            }
        }
    } else if (numtasks < 8 && numtasks >= 4) {
        switch (idprocesso) {
            case 0:
                for (int i = 0; i < 2; ++i) {
                    int tcx = cx + movimento[i][0];
                    int tcy = cy + movimento[i][1];
                    if ((tcx >= 0) &&  (tcy >= 0)  &&  (tcx < cs) &&  (tcy < cs) && (T.board[tcx][tcy] == -1)) {
                        tour temp(T);
                        temp.board[tcx][tcy] = imove+1;
                        temp.sx = tcx;
                        temp.sy = tcy;
                        findtour_completo(temp, imove+1);
                    }
                }
            case 1:
                for (int i = 2; i < 4; ++i) {
                    int tcx = cx + movimento[i][0];
                    int tcy = cy + movimento[i][1];
                    if ((tcx >= 0) &&  (tcy >= 0)  &&  (tcx < cs) &&  (tcy < cs) && (T.board[tcx][tcy] == -1)) {
                        tour temp(T);
                        temp.board[tcx][tcy] = imove+1;
                        temp.sx = tcx;
                        temp.sy = tcy;
                        findtour_completo(temp, imove+1);
                    }
                }
            case 2:
                for (int i = 4; i < 6; ++i) {
                    int tcx = cx + movimento[i][0];
                    int tcy = cy + movimento[i][1];
                    if ((tcx >= 0) &&  (tcy >= 0)  &&  (tcx < cs) &&  (tcy < cs) && (T.board[tcx][tcy] == -1)) {
                        tour temp(T);
                        temp.board[tcx][tcy] = imove+1;
                        temp.sx = tcx;
                        temp.sy = tcy;
                        findtour_completo(temp, imove+1);
                    }
                }
            case 3:
                for (int i = 6; i < 8; ++i) {
                    int tcx = cx + movimento[i][0];
                    int tcy = cy + movimento[i][1];
                    if ((tcx >= 0) &&  (tcy >= 0)  &&  (tcx < cs) &&  (tcy < cs) && (T.board[tcx][tcy] == -1)) {
                        tour temp(T);
                        temp.board[tcx][tcy] = imove+1;
                        temp.sx = tcx;
                        temp.sy = tcy;
                        findtour_completo(temp, imove+1);
                    }
                }
        }
    }
    
    return false;
}

bool tour::findtour_completo(tour& T, int imove) {
    if(imove == (size*size - 1)) return true;
    // make a move
    int cx = T.sx;
    int cy = T.sy;
    int cs = T.size;
    for (int i = 0; i < 8; ++i) {
        int tcx = cx + movimento[i][0];
        int tcy = cy + movimento[i][1];
        // Is this a valid move?
        // Has this place been visited yet
        if ((tcx >= 0) &&  (tcy >= 0)  &&  (tcx < cs) &&  (tcy < cs) && (T.board[tcx][tcy] == -1)) {
            tour temp(T);
            temp.board[tcx][tcy] = imove+1;
            temp.sx = tcx;
            temp.sy = tcy;
            if(findtour_completo(temp, imove+1)) {
                cout << temp << endl;
                auto end = chrono::steady_clock::now();
                auto diff = end - start;
                cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
                cout << "Solution found by proccess of rank: " << idprocesso << endl;
                MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS);
                MPI_Finalize();
            }
        }
    }
    return false;
}


int main(int argc, char *argv[]) {
    int table_size, start_x, start_y;

    int result = MPI_Init(&argc, &argv);
    if (result != MPI_SUCCESS) {
        fprintf(stderr,"Erro iniciando MPI: %d\n",result);
        MPI_Abort(MPI_COMM_WORLD, result);
    }
    
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &idprocesso);

    if (idprocesso == 0) {
        cin >> table_size >> start_x >> start_y;
        for (int i = 1; i < numtasks; i++) {
            MPI_Send(&table_size, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&start_x, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&start_y, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        }
        start = chrono::steady_clock::now();
        tour T(table_size, start_x, start_y);
    } else {
        MPI_Recv(&table_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&start_x, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&start_y, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        tour T2(table_size, start_x, start_y);
    }
    return 0;
}

// Adapted with permission from the original implementation by
// prof. Piyush Kumar (Florida State University)
// http://www.compgeom.com/~piyush/
