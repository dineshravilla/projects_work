#include <iostream>
#include <fstream>
#include <algorithm>

#include <math.h> // for the square root
#include <vector> // for weighted sort
#include <map>

using namespace std;

class node {
        public:
        int status; // 1:vacant, 2: same as present location 3:locked 4: occupied and not locked
        int cell; //cell number
        //node();
        };

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
        //void mydisplay2darray(int [][2],int);
        void mydisplay1dVector(Vectors&);
        void sortcells();
        void mydisplayLayoutCell(layout&,int);
        void mydisplayLayoutStatus(layout&,int);
        //node** mypointer;
        //void mydisplayLayoutCell(node [][],int);
        void ForceDirectedPlacement(layout&,int,int);
        void targetxy(int seed,circuit& myvector,int *tx,int *ty);
        void mydisplay2dVector(circuit& myvacantlist);
        void NearVacant(layout&,circuit&,int,int,int *VacX,int *VacY);
        void unlockAll(layout&,int);

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
        cout << endl << "the sorted list is ";
        mydisplay1dVector(desc_cells);


        layout mylayout;
        //form the class objects
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

        //initialize the layout
        k=0;
        for(i=0;i<rootnCells;i++)
        {
            for(j=0;j<rootnCells;j++)
            {
                mylayout[i][j].cell=desc_cells[k];
                //mylayout[i][j].cell=k;
                myvector[desc_cells[k]].push_back(j);
                myvector[desc_cells[k]].push_back(i);
                //initialize the status of cell as occupied
                mylayout[i][j].status=4; // 4 is for occupied and not locked
                k++;
                if(k>=numCells)
                    break;
            }
        }
        mydisplayLayoutCell(mylayout,rootnCells);
        mydisplayLayoutStatus(mylayout,rootnCells);
        mydisplayCircuit(myvector,numCells);

        //find the vacant position
        circuit myvacantlist;
        k=0;
        for(i=0;i<rootnCells;i++)
        {
            vector<int> temprrow;
            for(j=0;j<rootnCells;j++)
            {
                temprrow.clear();
                if(mylayout[i][j].status==-2)
                    {
                    temprrow.push_back(i);
                    temprrow.push_back(j);
                    k++;
                    myvacantlist.push_back(temprrow);
                    }
            }
        }

        mydisplay2dVector(myvacantlist);
        int VacX,VacY;
        NearVacant(mylayout,myvacantlist,5,5,&VacX,&VacY);

        int itr_count=0;
        int abrt_count=0;
        int itr_limit=20;
        int abrt_limit=30;
        int seed;
        int x,y;
        int tx,ty;
        bool end_ripple=false;

        cout << "ForceDirectedPlacement begins here";
        i=0;
        vector<int>::reverse_iterator itr;
        while(itr_count<itr_limit)
        {
            seed=desc_cells[i];
            itr=myvector[seed].rbegin();
            x=myvector[seed].back();
            y=*(itr+1);
            //cout << endl << "position of seed is i = " << myvector[seed].back() << "and j = " << myvector[seed][myvector[seed].size()-1] ;
            //cout << endl << "position of seed is i = " << myvector[seed].back() << "and j = " << *(itr+1);
            mylayout[x][y].status=1; //vacant


                    while(end_ripple==false)
                    {
                        targetxy(seed,myvector,&tx,&ty);
                        cout << endl << "target x and target y" << tx << " , " << ty;
                        cout << endl << "The status of cell is " << mylayout[tx][ty].status;
                        if(tx==x && ty==y)
                                mylayout[x][y].status=2; // same as present location
                        //if(mylayout[tx][ty].status=)

                            switch(mylayout[tx][ty].status)
                            {
                            case 1 : //vacant
                                cout << "This is case 1" ;
                                //move seed to target point and lock
                                mylayout[tx][ty].status=3; //lock
                                mylayout[tx][ty].cell=seed; //move seed
                                myvector[seed].pop_back();
                                myvector[seed].pop_back();
                                myvector[seed].push_back(ty);
                                myvector[seed].push_back(tx);

                                end_ripple = true;
                                abrt_count = 0;

                                i++;
                                break;
                            case 2 : //same as present location
                                cout << "This is case 2" ;
                                end_ripple = true;
                                abrt_count = 0;

                                i++;
                                break;

                             case 3 : //locked
                                cout << "This is case 3" ;
                                //find nearest vacant position
                                NearVacant(mylayout,myvacantlist,tx,ty,&VacX,&VacY);
                                myvector[seed].pop_back();
                                myvector[seed].pop_back();
                                myvector[seed].push_back(VacX);
                                myvector[seed].push_back(VacY);

                                end_ripple = true;
                                abrt_count++;
                                if(abrt_count > abrt_limit)
                                {
                                    unlockAll(mylayout,rootnCells);
                                    itr_count++;
                                }


                                break;

                             case 4 : //occupied and not locked
                                cout << "This is case 4" ;
                                //select cell at target point for next move
                                if(mylayout[tx][ty].status==4)
                                {
                                    seed = mylayout[tx][ty].cell;
                                    mydisplay1dVector(desc_cells);

                                    desc_cells.erase(remove(desc_cells.begin(),desc_cells.end(),seed));
                                    cout << "The selected seed is deleted :" << seed << endl;
                                    mydisplay1dVector(desc_cells);
                                }
                                else
                                    i++;
                                //move seed cell to target point and lock the target
                                mylayout[tx][ty].cell=seed;
                                mylayout[tx][ty].status=3;
                                myvector[seed].pop_back();
                                myvector[seed].pop_back();
                                myvector[seed].push_back(ty);
                                myvector[seed].push_back(tx);

                                end_ripple = false;
                                abrt_count = 0;

                                break;

                            default :
                                cout << "This is case default" ;
                            }

                        break;
                    }


        break;
        }//while ends





    return 0;
}
/*void ForceDirectedPlacement(layout& mylayout,int rootnCells,int numCells)
{
        int itr_count=0;
        int abrt_count=0;
        int itr_limit=20;
        int abrt_limit=30;
        int seed;
        int i;

        cout << "ForceDirectedPlacement begins here";
        i=0;
        while(itr_count<itr_limit)
        {
            seed=desc_cells[i];
            //find the position of seed

        }
}*/
void targetxy(int seed,circuit& myvector,int *tx,int *ty)
{
    int numeratorx=0,numeratory=0;
    int i;
    int c,xx,yy;
    vector<int>::reverse_iterator itr;

    int denominator=myvector[seed].size()-2;

    for(i=0;i<denominator;i++)
    {
            c=myvector[seed][i];
            itr=myvector[c].rbegin();
            xx=myvector[c].back();
            yy=*(itr+1);

            numeratorx=numeratorx + xx;
            numeratory=numeratory + yy;
            cout << " xx is " << xx << " yy is " << yy;
    }
    cout << endl << "weight is " << denominator;
    *tx=numeratorx/denominator;
    *ty=numeratory/denominator;
    cout << endl << "target x and target y" << *tx << " , " << *ty;

}

void NearVacant(layout& mylayout,circuit& myvacantlist,int tx,int ty,int *VacX,int *VacY)
{
    unsigned int i,
    node=0;
    int manhattan=0,minDist=0;

    cout << endl << "Size of vacant list is " <<myvacantlist.size() ;
    for(i=0;i<myvacantlist.size();i++)
    {
        manhattan=abs(myvacantlist[i].front()-tx)+abs(myvacantlist[i].back()-ty);
        if(manhattan<minDist || i==0)
        {
            minDist=manhattan;
            node=i;
        }

    }
    cout << endl << "The minimum manhattan distance is "<< minDist << " for position i " << node ;
    *VacX=myvacantlist[node].front();
    *VacY=myvacantlist[node].back();
    cout << endl << "The nearest vacant location is "<< *VacX << "," << *VacY;
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
/*void mydisplay2darray(int weightedarray[][2],int numCells)
{
        int i,j;
        cout << endl;
            for(i=0;i<numCells;i++)
            {   //cout<< "cell "<<i<< " is connected to ";
                for(j=0; j< 2;j++)
                    cout << " " <<weightedarray[i][j];
                cout << endl;
            }
}*/
void sortcells()
{
    for(Maps::reverse_iterator rit = weights.rbegin(); rit != weights.rend(); rit++)
        desc_cells.push_back(rit->second);
}
void mydisplay1dVector(Vectors& vecPrint)
{
    cout << " Elements in 1-D vector are " << endl;
    for(Vectors::iterator it = vecPrint.begin(); it != vecPrint.end(); it++)
        cout << *it << " ";
}

void mydisplay2dVector(circuit& myvacantlist)
{
    unsigned int i,j;
    cout << endl << "The vacant list is " << endl;
    for(i=0;i<myvacantlist.size();i++)
    {
        cout << " ( ";
        for(j=0;j<myvacantlist[i].size();j++)
            cout << myvacantlist[i][j] << "," ;
        cout << " ) ";
    }

}
void unlockAll(layout& mylayout,int rootnCells)
{
    int i,j;
    for(i=0; i<rootnCells; i++)
    {
        for(j=0; j<rootnCells; j++)
        {
            if(mylayout[i][j].status==3)
            {
              mylayout[i][j].status=4;
            }//if ends
        }//j for ends
    }//i for ends
}

