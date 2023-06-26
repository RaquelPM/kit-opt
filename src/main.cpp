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
	cout << "NO" << n <<endl;
	node->lower_bound = rl->getNewUB();

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
	cout << n->forbidden_edges.size() << endl;

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
	Data * data = new Data(argc, argv[1]);
	data->readData();

	vector <vector<double>> dist(data->getDimension(), vector<double>(data->getDimension()));
	for (int i = 0; i < data->getDimension(); i++){
		for (int j = 0; j < data->getDimension(); j++){
			dist[i][j] = data->getDistance(i,j);
		}
	}

	RL *rl = new RL();

	double UB = stod(argv[2])+1;
	vector<double> lambdas(data->getDimension());

	int n = rl->doRL(dist, *data, UB, lambdas);
	cout << "n: " << n << endl;

	cout << "Custo: " << rl->getNewUB() << endl;
	cout << "Caminho: " << rl->getBestEdges().size() << endl;
	for (int i = 0; i < rl->getBestEdges().size(); i++){
		cout << rl->getBestEdges()[i].first << " " << rl->getBestEdges()[i].second << endl;
	}

	Node raiz;
	raiz.lambdas = vector<double>(data->getDimension());
	attNodeSolution(&raiz, *data, UB);
	
	priority_queue<Node> tree;
 	tree.push(raiz);

	while(!tree.empty()){
		cout << tree.size() << endl;
 		Node node = tree.top();
 		tree.pop();

		cout << "branchs" << node.edges_to_branch.size() << endl;
		cout << "forbidden edges" << node.forbidden_edges.size() << endl;

 		if (node.feasible){
			UB = min(UB, node.lower_bound);
			cout << "FEASIBLE!!!!" << endl;
		}
 		else {
 			for(int i =0; i< node.edges_to_branch.size();i++){
				Node n;
 				n.forbidden_edges = node.forbidden_edges;
				n.lambdas = node.lambdas;

 				n.forbidden_edges.push_back(node.edges_to_branch[i]);

				cout << "forbidden edges novo n: " << n.forbidden_edges.size() << endl;

 				attNodeSolution(&n, *data, UB);
 				if(n.lower_bound <= UB) tree.push(n);
 			}
 		}
 	}


	cout << UB << endl;

	delete data;

    return 0;    
}
