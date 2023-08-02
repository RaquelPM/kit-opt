#include <iostream>
#include "Data.h"
#include "MasterProblem.h"
#include "Subproblem.h"
#include <cmath>
#include <ctime>

using namespace std;

struct Node{
    vector<int> lambdasProhibited;
    vector<pair<int, int>> pairsProhibitedTogether;
    vector<pair<int, int>> pairsProhibitedSeparated;
    vector<vector<bool>> itemIJ;
    double bins;
    bool raiz = false;
    bool feasible = false;
};

void isFeasible(MasterProblem* m, Node* n){
	int sum = 0;
    vector<double> l = m->getLambdas();
	
	for(int i = 0; i < l.size() ; i++){
		sum += l[i] > 0.9 ? 1 : l[i];	
	}

	n->feasible = abs(sum - n->bins) <= EPSILON ? true : false;
}

double GC(Node *n, MasterProblem *master, Data* data){
    Subproblem sub(data);

    if(!n->raiz) master->attLambdas(n->lambdasProhibited, n->itemIJ);

    try{
        n->bins = master->solve();
    } catch(const char* msg){
        n->bins = bigM;
        return -1;
    }

    sub.attObjective(master->duals);

    sub.attRestricitionsSeparated(n->pairsProhibitedTogether);
    sub.attRestrictionsTogether(n->pairsProhibitedSeparated);

    double subOpt = sub.solve(n->raiz);
    while(1 - subOpt < -EPSILON){
        master->addColumn(sub.solution);
        n->bins = master->solve();

        sub.attObjective(master->duals);
        subOpt = sub.solve(n->raiz);
    }

    isFeasible(master, n);

    n->itemIJ = master->getItemIJ(); 

    return n->bins;
}

int main(int argc, char** argv){
    clock_t start= clock();
    srand((unsigned)time(NULL));

    Data data(argv[1]);

    double UB = data.getQuantItems();
    vector<Node> tree;

    Node raiz;
    raiz.raiz = true;

    tree.push_back(raiz);

    MasterProblem master(&data);
    
    while(!tree.empty()){
        Node n = tree[0];
        tree.erase(tree.begin());

        GC(&n, &master, &data);

        if(n.bins > UB-1) continue;

        if(n.feasible){
            if(n.bins <= UB-1){
                UB = n.bins;
            }
        }
        else{
            pair<int, int> pairFractionated = master.getPairFractioned();

            vector<int> lambdasPairTogether = master.getLambdasPairTogether(pairFractionated);
            vector<int> lambdasPairSeparated = master.getLambdasPairSeparated(pairFractionated);

            Node newNodeTogether, newNodeSeparated;

            newNodeTogether = newNodeSeparated = n;
            newNodeTogether.raiz = newNodeSeparated.raiz = false;

            newNodeTogether.pairsProhibitedTogether.push_back(pairFractionated);
            newNodeSeparated.pairsProhibitedSeparated.push_back(pairFractionated);

            for(int i = 0; i < lambdasPairTogether.size(); i++){
                newNodeTogether.lambdasProhibited.push_back(lambdasPairTogether[i]);
            }
            for(int i = 0; i < lambdasPairSeparated.size(); i++){
                newNodeSeparated.lambdasProhibited.push_back(lambdasPairSeparated[i]);
            }

            tree.push_back(newNodeTogether);
            tree.push_back(newNodeSeparated);
        }
    }


    clock_t end= clock();
    double time= ((double) (end - start)) / CLOCKS_PER_SEC;

    cout << "UB: " << UB << endl;
    cout << time << endl;

    return 0;
}