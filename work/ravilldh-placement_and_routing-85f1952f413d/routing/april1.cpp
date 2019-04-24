#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <math.h>

using namespace std;

#define R 5
#define Cell_Width 6

typedef vector<int> Vectors;
typedef vector< vector<int> > Vectors2D;
typedef vector<int>::iterator Vec_Iter;

Vectors row;
Vectors2D nets, grid, col;

int grows, gcols, numCells;

struct gridPoint {
    gridPoint(int x, int y) : x(x), y(y) { };
    int x;
    int y;
};

struct coord {
    int x;
    int y;
};

typedef vector<coord> route_coord;
route_coord m1;
route_coord m2;

struct route_wire {
    route_wire(int xbot, int ybot, int xtop, int ytop) : xbot(xbot), ybot(ybot), xtop(xtop), ytop(ytop) { };
    int xbot, ybot, xtop, ytop;
};

std::vector<route_wire> metal1;
std::vector<route_wire> metal2;

void create_grid(int cols, int rows, Vectors2D &grid) {
    for(int i = 1; i <= cols; i++) {
        Vectors cells;
        for(int j = 1; j <= rows; j++) {
	    cells.push_back(0);	    
	}
	cout << " start" << cells.size();
	grid.push_back(cells);
    }       
    cout << " " << grid.size() << "ENd";
}

void reset_grid(int ncells, int cols, int rows, Vectors2D &grid) {
    int r = ceil(sqrt(ncells));	
    for(int i = 1; i <= r; i ++) {
        int r1 = (R+6)*i - 6;
	for(int j = 1; j <= r; j ++) {
	    int c1 = (R+6)*j - 6;
	    for(int ri = 0; ri < 6; ri++) {
	        for(int ci = 0; ci < 6; ci++) {
		    grid[r1+ri][c1+ci] = -1;
		    grid[r1-1][c1+ci] = -3;
		    grid[r1+Cell_Width][c1+ci] = -3;
		    grid[r1+ri][c1-1] = -3;
		    grid[r1+ri][c1+Cell_Width] = -3;
		}
	    }

	    grid[r1-1][c1-1] = -3;            // boundary around the cell
	    grid[r1-1][c1+Cell_Width] = -3;
    	    grid[r1+Cell_Width][c1-1] = -3;
	    grid[r1+Cell_Width][c1+Cell_Width] = -3;

    	    // Terminals
            grid[r1][c1+1] = -2;     // 1   
	    grid[r1][c1+4] = -2;     // 2
            grid[r1+5][c1+1] = -2;   // 3
	    grid[r1+5][c1+4] = -2;   // 4
	    grid[r1-1][c1+1] = 0;           // via at terminals
	    grid[r1-1][c1+4] = 0;
	    grid[r1+Cell_Width][c1+1] = 0;
	    grid[r1+Cell_Width][c1+4] = 0;
	}
    }
}

void placed_cells() {
    ifstream plcmntbench;
    plcmntbench.open("P1");    
    int k, rows, cols;
    plcmntbench >> rows;
    cols = rows;
    for(int i = 0; i < cols; i++) {
	Vectors row;    
        for(int j = 0; j < rows; j++) {
	    if(plcmntbench >> k) {	
                row.push_back(k);	    		 
	    }	
	}
	col.push_back(row);
    }
    cout << endl;
    for(int i = 0; i < cols; i++) {
        for(int j = 0; j < col[i].size(); j++) {
	    cout << setw(3) << col[i][j] << " ";
	}
	cout << endl;
    }    
}

void print_grid(int gcols, int grows, Vectors2D &grid) {
    for(int i = 0; i < gcols; i++) {
    	for(int j = 0; j < grows; j++) {
	    cout << setw(2) << grid[i][j] << " " ;
	}
	cout << endl;
    }
}


int canPropogate(int x, int y, int m, Vectors2D &grid) {
    if((x >= gcols) || (y >= grows) || (x <= 0) || (y <= 0))
	return 0;
    if((grid[x][y] == -1) || (grid[x][y] == -2) || (grid[x][y] == -3))
	return 0;
    if((m == 1) && ((grid[x][y] == 5) || (grid[x][y] == 6)))
	return 0;
    if((m == 2) && ((grid[x][y] == 7) || (grid[x][y] == 8)))
	return 0;
    if((m == 1) && ((grid[x][y] == 7) || (grid[x][y] == 8)))
	return 1;
    if((m == 2) && ((grid[x][y] == 5) || (grid[x][y] == 6)))
	return 1;
    if(grid[x][y] == 0)
	return 1;    
}

gridPoint nextPoint(int x, int y, int vert, Vectors2D &grid, int val) {
    int next = val - 1;
    if(val == 1) {
	next = 3;
    }    

    if(grid[x][y+1] == next) 
        return gridPoint(x, y+1);
    if(grid[x][y-1] == next)
        return gridPoint(x, y-1);
    if(grid[x+1][y] == next)
        return gridPoint(x+1, y);
    if(grid[x-1][y] == next)
        return gridPoint(x-1, y);
}

int Lee_Algorithm(int x1, int y1, int x2, int y2, int m, Vectors2D &grid) {

    int foundtgtxp1, foundtgtxm1;
    foundtgtxp1 = 0;
    foundtgtxm1 = 0;
    int vert = 1;
    std::vector<gridPoint> CCS;
    std::vector<gridPoint> NCS;
    int wavefront = 1;
    bool foundtgt = 0;
    CCS.push_back(gridPoint(x1, y1));
    grid[x1][y1] = wavefront;

    while(!foundtgt) {
        wavefront ++;
	if(wavefront == 4) { 
	    wavefront = 1; 
	}

	for(long int i = 0; i < CCS.size(); i++) {
	    if(canPropogate(CCS[i].x, CCS[i].y+1, m, grid)) 
		NCS.push_back(gridPoint(CCS[i].x, CCS[i].y+1)); 	    
    	    if(canPropogate(CCS[i].x, CCS[i].y-1, m, grid))
		NCS.push_back(gridPoint(CCS[i].x, CCS[i].y-1)); 
	    if(canPropogate(CCS[i].x+1, CCS[i].y, m, grid))
		NCS.push_back(gridPoint(CCS[i].x+1, CCS[i].y)); 
	    if(canPropogate(CCS[i].x-1, CCS[i].y, m, grid)) 
		NCS.push_back(gridPoint(CCS[i].x-1, CCS[i].y)); 		
	}


	if(NCS.size() == 0) {
	    for(int i = 0; i < gcols; i++) {
		for(int j = 0; j < grows; j++) {
	            grid[i][j] = 0;    
	        }
            } 
            reset_grid(numCells, gcols, grows, grid);	    
	    if(m == 1) {
 	        for(int i = 0; i < m1.size(); i++) {
                    grid[m1[i].x][m1[i].y] = 5; 			
                    if(grid[m1[i].x+1][m1[i].y] == 0)			// 4 - metal 1 wire
                        grid[m1[i].x+1][m1[i].y] = 6;
	            if(grid[m1[i].x-1][m1[i].y] == 0)
	                grid[m1[i].x-1][m1[i].y] = 6;			// 5 - boundary around metal1 wire
	            if(grid[m1[i].x][m1[i].y+1] == 0)
	                grid[m1[i].x][m1[i].y+1] = 6;
	            if(grid[m1[i].x][m1[i].y-1] == 0)
	                grid[m1[i].x][m1[i].y-1] = 6;	     
                }     
            }

            if(m == 2) {
            	for(int i = 0; i < m2.size(); i++) {
            	    grid[m2[i].x][m2[i].y] = 7;	    	
                    if(grid[m2[i].x+1][m2[i].y] == 0)			// 6 - metal 2 wire
		        grid[m2[i].x+1][m2[i].y] = 8;
	            if(grid[m2[i].x-1][m2[i].y] == 0)
		        grid[m2[i].x-1][m2[i].y] = 8;			// 7 - boundary around metal2 wire
		    if(grid[m2[i].x][m2[i].y+1] == 0)
     	    	        grid[m2[i].x][m2[i].y+1] = 8;
	            if(grid[m2[i].x][m2[i].y-1] == 0)
	      	        grid[m2[i].x][m2[i].y-1] = 8;
        	}
    	    }
	    return 0;
	}

	for(long int i = 0; i < NCS.size(); i++) {	    
	    grid[NCS[i].x][NCS[i].y] = wavefront;
	    if((NCS[i].x+1 == x2) && (NCS[i].y == y2)) {
		    cout << "Target Found x+1, y" << endl;
		    foundtgtxp1 = 1;	    
		    foundtgt = 1;
		    break;
	    }
	    if((NCS[i].x-1 == x2) && (NCS[i].y == y2)) {
		    cout << "Target Found x-1, y" << endl;	
		    foundtgtxm1 = 1;
	    	    foundtgt = 1;
		    break;
	    }	    
	}    

	CCS.clear();

	for(long int i = 0; i < NCS.size(); i++) {
	    CCS.push_back(NCS[i]);
	}
	
	NCS.clear();
    }
    cout << x2 << ", " <<y2;
    int foundroute = 0;
    
    gridPoint cur = gridPoint(x2, y2);

    if(foundtgtxp1) 
	cur = gridPoint(x2-1, y2);
    if(foundtgtxm1)
        cur = gridPoint(x2+1, y2);

    gridPoint btrace = cur;   

    while(!foundroute) {
        gridPoint next = nextPoint(cur.x, cur.y, vert, grid, grid[cur.x][cur.y]);

	if(((cur.x == next.x) && (vert == 1)) || ((cur.y == next.y) && (vert == 0))) {
            if(m == 1)
	        metal1.push_back(route_wire(btrace.x, btrace.y, cur.x, cur.y));
	    if(m == 2)
	        metal2.push_back(route_wire(btrace.x, btrace.y, cur.x, cur.y));
            btrace = cur;
    	}

	if(cur.x == next.x) { vert = 0; }
	if(cur.y == next.y) { vert = 1; }

	if(m == 1) {
	    coord r1;
	    r1.x = cur.x;
	    r1.y = cur.y;
	    m1.push_back(r1);
	}
	if(m == 2) {
	    coord r2;
    	    r2.x = cur.x;
	    r2.y = cur.y;
    	    m2.push_back(r2);	    
	}

	cur = next;

	if((cur.x == x1) && (cur.y == y1)) {
	    if(m == 1)
		metal1.push_back(route_wire(btrace.x, btrace.y, cur.x, cur.y));
	    if(m == 2)
		metal2.push_back(route_wire(btrace.x, btrace.y, cur.x, cur.y));
	    foundroute = 1;
	}
    }

    for(int i = 0; i < gcols; i++) {
        for(int j = 0; j < grows; j++) {
	    grid[i][j] = 0;    
	}
    }       
    reset_grid(numCells, gcols, grows, grid);
    
    if(m == 1) {
        for(int i = 0; i < m1.size(); i++) {
            grid[m1[i].x][m1[i].y] = 5;
            if(grid[m1[i].x+1][m1[i].y] == 0)			// 4 - metal 1 wire
                grid[m1[i].x+1][m1[i].y] = 6;
            if(grid[m1[i].x-1][m1[i].y] == 0)
                grid[m1[i].x-1][m1[i].y] = 6;			// 5 - boundary around metal1 wire
	    if(grid[m1[i].x][m1[i].y+1] == 0)
                grid[m1[i].x][m1[i].y+1] = 6;
            if(grid[m1[i].x][m1[i].y-1] == 0)
                grid[m1[i].x][m1[i].y-1] = 6;
	}
    }

    if(m == 2) {
        for(int i = 0; i < m2.size(); i++) {
            grid[m2[i].x][m2[i].y] = 7;
	    if(grid[m2[i].x+1][m2[i].y] == 0)			// 6 - metal 2 wire
	        grid[m2[i].x+1][m2[i].y] = 8;
	    if(grid[m2[i].x-1][m2[i].y] == 0)
	        grid[m2[i].x-1][m2[i].y] = 8;			// 7 - boundary around metal2 wire
	    if(grid[m2[i].x][m2[i].y+1] == 0)
                grid[m2[i].x][m2[i].y+1] = 8;
	    if(grid[m2[i].x][m2[i].y-1] == 0)
	        grid[m2[i].x][m2[i].y-1] = 8;
        }
    }
    return 1;
}


int main() {
    int numNets;    
    char filename[20];
    int n1, c1, c2, t1, t2;
    int r;
    ifstream benchmark;
    cin.getline(filename,20);
    benchmark.open(filename);

    if(!benchmark.is_open()) {
        cout << " No such file exists" << endl;
	return 0; 
    }

    benchmark >> numCells;
    benchmark >> numNets;
    int i = 0;
    Vectors r1;        
    while(benchmark >> n1 >> c1 >> t1 >> c2 >> t2) {

        r1.push_back(c1);
	r1.push_back(t1);
	r1.push_back(c2);
	r1.push_back(t2);
	r1.push_back(0);
	nets.push_back(r1);
	r1.clear();	
    }

    r = ceil(sqrt(numCells));

    grows = r * Cell_Width + (r+1) * R;
    gcols = grows;

    create_grid(gcols, grows, grid);

    placed_cells();

    reset_grid(numCells, gcols, grows, grid);  

    print_grid(gcols, grows, grid);


    int source; int target;
    int xsrc, ysrc, xtgt, ytgt;
    int termsrc, termtgt;

    for(int i = 0; i < nets.size(); i++) {
	source = nets[i][0];
	target = nets[i][2];
	termsrc = nets[i][1];
	termtgt = nets[i][3];
	for(int j = 0; j < r; j++) {
	    for(int k = 0; k < col[j].size(); k++) {
	        if(source == col[j][k]) {
		    if(termsrc == 1) {
		        ysrc = (R+6)*(k+1) - Cell_Width + 1;
			xsrc = (R+6)*(j+1) - 1 - Cell_Width + 1;
		    } else if(termsrc == 2) {
		        ysrc = (R+6)*(k+1) - Cell_Width + 4;
			xsrc = (R+6)*(j+1) - 1 - Cell_Width + 1;
		    } else if(termsrc == 3) {
		        ysrc = (R+6)*(k+1) - Cell_Width + 1;
			xsrc = (R+6)*(j+1) - 1;
		    } else {
		        ysrc = (R+6)*(k+1) - Cell_Width + 4;
			xsrc = (R+6)*(j+1) - 1;
		    }
		}
		if(target == col[j][k]) {
		    if(termtgt == 1) {
		        ytgt = (R+6)*(k+1) - Cell_Width + 1;
			xtgt = (R+6)*(j+1) - 1 - Cell_Width + 1;
		    } else if(termtgt == 2) {
		        ytgt = (R+6)*(k+1) - Cell_Width + 4;
			xtgt = (R+6)*(j+1) - 1 - Cell_Width + 1;
		    } else if(termtgt == 3) {
		        ytgt = (R+6)*(k+1) - Cell_Width + 1;
			xtgt = (R+6)*(j+1) - 1;
		    } else {
		        ytgt = (R+6)*(k+1) - Cell_Width + 4;
			xtgt = (R+6)*(j+1) - 1;
		    }
		}		
	    }
	}

	// Lee Algorithm
        cout << "Source:" << source << "; Target:" << target << endl;
	if(Lee_Algorithm(xsrc, ysrc, xtgt, ytgt, 1, grid)) {
	    nets[i][4] = 1;    
	    print_grid(gcols, grows, grid);	    
	}
    } 
    

    for(int i = 0; i < nets.size(); i++) {
        if(nets[i][4] == 0) {
	source = nets[i][0];
	target = nets[i][2];
	termsrc = nets[i][1];
	termtgt = nets[i][3];
	for(int j = 0; j < r; j++) {
	    for(int k = 0; k < col[j].size(); k++) {
	        if(source == col[j][k]) {
		    if(termsrc == 1) {
		        ysrc = (R+6)*(k+1) - Cell_Width + 1;
			xsrc = (R+6)*(j+1) - 1 - Cell_Width + 1;
		    } else if(termsrc == 2) {
		        ysrc = (R+6)*(k+1) - Cell_Width + 4;
			xsrc = (R+6)*(j+1) - 1 - Cell_Width + 1;
		    } else if(termsrc == 3) {
		        ysrc = (R+6)*(k+1) - Cell_Width + 1;
			xsrc = (R+6)*(j+1) - 1;
		    } else {
		        ysrc = (R+6)*(k+1) - Cell_Width + 4;
			xsrc = (R+6)*(j+1) - 1;
		    }
		}
		if(target == col[j][k]) {
		    if(termtgt == 1) {
		        ytgt = (R+6)*(k+1) - Cell_Width + 1;
			xtgt = (R+6)*(j+1) - 1 - Cell_Width + 1;
		    } else if(termtgt == 2) {
		        ytgt = (R+6)*(k+1) - Cell_Width + 4;
			xtgt = (R+6)*(j+1) - 1 - Cell_Width + 1;
		    } else if(termtgt == 3) {
		        ytgt = (R+6)*(k+1) - Cell_Width + 1;
			xtgt = (R+6)*(j+1) - 1;
		    } else {
		        ytgt = (R+6)*(k+1) - Cell_Width + 4;
			xtgt = (R+6)*(j+1) - 1;
		    }
		}		
	    }
	}

	// Lee Algorithm
        cout << "Source:" << source << "; Target:" << target << endl;
	if(Lee_Algorithm(xsrc, ysrc, xtgt, ytgt, 2, grid)) {
	    nets[i][4] = 1;
    	    print_grid(gcols, grows, grid);	    
	}
    }
    }    
}
