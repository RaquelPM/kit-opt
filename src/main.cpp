#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <queue>
using namespace std;

#include "data.h"
#include "RL.h"

struct Node{
	vector<pair<int, int>> forbidden_edges;
	vector<pair<int, int>> edges_to_branch;
	vector<double> lambdas;
	double lower_bound;
	bool feasible;
};

void attNode(Node *node, vector<vector<double>> cost_matrix, Data& data, double UB){
	RL *rl = new RL();

	int n = rl->doRL(cost_matrix, data, UB, node->lambdas);
	//cout << "NO " << n <<endl;
	node->lower_bound = rl->getNewUB();
	//cout << node->lower_bound << endl;

	node->feasible = n == -1 ? true : false;
	vector<pair<int, int>> edges = rl->getBestEdges();

	for(int i =0; i < edges.size(); i++){
		//cout << edges[i].first << " " << edges[i].second << endl;
		if (edges[i].first == n || edges[i].second == n){
			node->edges_to_branch.push_back(edges[i]);
		}
	}
	
	delete rl;
}

void attNodeSolution(Node *n, Data& data, double UB){
	vector<vector<double>> cost(data.getDimension(), vector<double>(data.getDimension()));
	for (int i = 0; i < data.getDimension(); i++){
		for (int j = 0; j < data.getDimension(); j++){
			cost[i][j] = data.getDistance(i,j);
		}
	}
	//cout << n->forbidden_edges.size() << endl;

	for(int i =0; i < n->forbidden_edges.size(); i++){
		//cout << "FORBIDDEN: ";
		//cout << n->forbidden_edges[i].first << " " << n->forbidden_edges[i].second << endl;
		cost[n->forbidden_edges[i].first][n->forbidden_edges[i].second] = INFINITE;
		cost[n->forbidden_edges[i].second][n->forbidden_edges[i].first] = INFINITE;
	}

	attNode(n,cost, data, UB);
};


 bool operator<(const Node& i, const Node& j){
 	return i.lower_bound > j.lower_bound;
 }


int main(int argc, char** argv) {
	clock_t start= clock();
	Data * data = new Data(argc, argv[1]);
	data->readData();

	double UB = stod(argv[2])+1;
	double upper_bound = UB;

	Node raiz;
	raiz.lambdas = vector<double>(data->getDimension());
	attNodeSolution(&raiz, *data, UB);
	
	//priority_queue<Node> tree;
	//tree.push(raiz);
	vector<Node> tree;
 	tree.push_back(raiz);

	while(!tree.empty()){
		//cout << "TREE: " << tree.size() << endl;
 		Node node = tree[0]; 
 		tree.erase(tree.begin());
		//Node node = tree.top();
		//tree.pop();

 		if (node.feasible){
			upper_bound = min(upper_bound, node.lower_bound);
			//cout << "FEASIBLE !!!" << node.lower_bound << endl;
		}
 		else {
 			for(int i =0; i< node.edges_to_branch.size();i++){
				Node n;
 				n.forbidden_edges = node.forbidden_edges;
				n.lambdas = node.lambdas;

 				n.forbidden_edges.push_back(node.edges_to_branch[i]);

 				attNodeSolution(&n, *data, UB);
 				if(n.lower_bound <= upper_bound){
					tree.push_back(n);
					//tree.push(n);
					//cout << "LB: " << n.lower_bound << endl;
					//cout << "UB: " << upper_bound << endl;
				} 
 			}
 		}
 	}


	cout << upper_bound << endl;
	

	clock_t end= clock();
    double time= ((double) (end - start)) / CLOCKS_PER_SEC;

	cout << time << endl;
	delete data;

    return 0;    
}
