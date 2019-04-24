#include <iostream>
#include <fstream>

#include <math.h> // for the square root
#include <vector> // for weighted sort
#include <map>

using namespace std;

class node {
    	public:
    	int status; // lock or occupied or vacant;
    	int cell; //cell number
    	//node();
    	};
/*node::node()
{
	status=-1;
	cell=-1;
*/

typedef vector< vector<int> > circuit;
typedef multimap< int, int > Maps;
typedef vector<int> Vectors;
Vectors desc_cells;
Maps weights;
typedef vector<node> row;
typedef vector< vector<node> > layout;


int main(int argc,char *argv[2])
{
    	int numCells,numNets,rootnCells;
    	int t,c1,c2; //variables for reading the bench mark file
    	int i,j,k;

    	void mydisplayCircuit(circuit&,int);
    	void mydisplay2darray(int [][2],int);
    	void mydisplay1dVector(Vectors&);
    	void sortcells();
    	void mydisplayLayoutCell(layout&,int);
    	void mydisplayLayoutStatus(layout&,int);
    	//node** mypointer;
    	//void mydisplayLayoutCell(node [][],int);

    	cout << "Reading the file " << argv[1] << " ...... " ;
    	//open file in read mode and read the data
    	ifstream infile;
    	infile.open(argv[1]);
    	infile >> numCells;
    	infile >> numNets;

    	cout << "no.of cells " << numCells << " and ";
    	cout << "no.of nets " << numNets <<  endl;

    	vector <vector <int> > myvector(numCells);
    	for(i=0;i<numCells;i++)
    	{
        	infile >> t;
        	infile >> c1;
        	infile >> t;
        	infile >> c2;
        	infile >> t;
        	//cout << n1 << " " << n2 << endl;
        	if(c1!=c2)
        	{
            	myvector[c1-1].push_back(c2-1);
            	myvector[c2-1].push_back(c1-1);
        	}
    	}

    	infile.close();
    	mydisplayCircuit(myvector,numCells);

    	rootnCells=sqrt(numCells);
    	if(rootnCells < sqrt(numCells))
       	rootnCells++;

    	cout << endl << "square root is " << sqrt(numCells) << " the rootnCells is " << rootnCells;



    	//int weightedarray[numCells][2];
    	//initialize the array
    	for(i=0;i<numCells;i++)
        	weights.insert(std::pair<int,int>(myvector[i].size(),i));

    	sortcells();
    	//mydisplay1dVector(desc_cells);


    	layout mylayout;
    	//initialize the layout
    	node tempnode;
    	for(i=0;i<rootnCells;i++)
        	{
        	row temprow;
        	for(j=0;j<rootnCells;j++)
            	{
                	tempnode.cell=-1;
                	tempnode.status=-2;
                	temprow.push_back(tempnode);
            	}
        	mylayout.push_back(temprow);
        	}

    	k=0;
    	for(i=0;i<rootnCells;i++)
    	{
        	for(j=0;j<rootnCells;j++)
        	{
            	mylayout[i][j].cell=desc_cells[k];
            	k++;
        	}
    	}
    	mydisplayLayoutCell(mylayout,rootnCells);
    	mydisplayLayoutStatus(mylayout,rootnCells);

	return 0;
}
void mydisplayLayoutCell(layout& mylayout,int rootnCells)
{
	int i,j;
    	cout << endl;
    	for(i=0;i<rootnCells;i++)
    	{
        	for(j=0;j<rootnCells;j++)
            	cout << "\t" <<mylayout[i][j].cell;
            	cout << endl;
    	}
}

void mydisplayLayoutStatus(layout& mylayout,int rootnCells)
{
	int i,j;
    	cout << endl;
    	for(i=0;i<rootnCells;i++)
    	{
        	for(j=0;j<rootnCells;j++)
            	cout << "\t" <<mylayout[i][j].status;
            	cout << endl;
    	}
}


void mydisplayCircuit(circuit& myvector,int numCells)
{
    	int i;
    	unsigned int j;
    	cout << endl;
        	for(i=0;i<numCells;i++)
        	{   cout<< "cell "<<i<< " is connected to ";
            	for(j=0; j< myvector[i].size();j++)
                	cout << " " <<myvector[i][j];
            	cout << endl;
        	}
}
void mydisplay2darray(int weightedarray[][2],int numCells)
{
    	int i,j;
    	cout << endl;
        	for(i=0;i<numCells;i++)
        	{   //cout<< "cell "<<i<< " is connected to ";
            	for(j=0; j< 2;j++)
                	cout << " " <<weightedarray[i][j];
            	cout << endl;
        	}
}
void sortcells()
{
	for(Maps::reverse_iterator rit = weights.rbegin(); rit != weights.rend(); rit++)
    	desc_cells.push_back(rit->second);
}
void mydisplay1dVector(Vectors& vecPrint)
{
	for(Vectors::iterator it = vecPrint.begin(); it != vecPrint.end(); it++)
    	cout << *it << " ";
}



