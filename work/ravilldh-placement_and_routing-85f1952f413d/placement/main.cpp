#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

class node {
        public:
        int status; // lock or occupied or vacant;
        int cell; //cell number
        };

typedef vector< vector<int> > circuit;
typedef multimap< int, int > Maps;

Maps weights;

int main(int argc,char *argv[2])
{
        int numCells,numNets;
        int t,c1,c2; //variables for reading the bench mark file
        int i;

        void mydisplayCircuit(circuit&,int);
        void mydisplay2darray(int [][2],int);
        void mysort(int*);

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

        node mylayout[numCells][numCells];
        int weightedarray[numCells][2];
        //initialize the array
        for(i=0;i<numCells;i++)
        {
	    weights.insert(std::pair<int,int>(myvector[i].size(),i));
        }

	for(Maps::iterator it = weights.begin(); it != weights.end(); it++)
            cout << it -> first << " " << it -> second;
        return 0;
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
/*void mysort(int* weightedarray)
{

}*/
