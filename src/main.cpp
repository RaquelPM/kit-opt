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
#include "hungarian.h"

struct Node {
	vector<pair<int, int>> forbidden_arcs;
	vector<vector<int>> subtours;
	double lower_bound;
	int chosen;
	bool feasible;
};

vector<vector<int>> Subtours(hungarian_problem_t& p){
	vector<vector<int>> subtours = vector<vector<int>>(p.num_rows+1);
	vector<int> rows;

	int w, r;
	w = r = 0;

	for(int i =0; i < p.num_rows; i++){
		rows.push_back(i);
	};
	while(!rows.empty()){
		bool exists = std::find(rows.begin(), rows.end(), r) != rows.end();
		if(!exists){
			subtours[w].push_back(r+1);
			w++;
			r = rows[0];
			continue;
		}
		rows.erase(std::find(rows.begin(), rows.end(), r));
		for(int j =0; j< p.num_cols; j++){
			if(p.assignment[r][j]){
				subtours[w].push_back(r+1);
				r = j;
				break;
			}
		}
	}

	subtours[w].push_back(r+1);
	subtours[p.num_rows].push_back(w);

	return subtours;
}

void attNode(Node *node, double** cost_matrix, Data& data, bool raiz){
	hungarian_problem_t p;

	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	hungarian_init(&p, cost_matrix, data.getDimension(),data.getDimension(), mode);
	node->lower_bound = hungarian_solve(&p);

	vector<vector<int>> subtours = Subtours(p); 
	node->subtours = subtours;
	int n = subtours[p.num_rows][0];

	int m = 0, size = subtours[0].size();
	for(int i =0; i <= n; i++){
		if(subtours[i].size() < size) {
			m = i;
			size = subtours[i].size();
		};
	}

	node->chosen = m;

	if(n > 0) node->feasible = false;
	else node->feasible = true;

	hungarian_free(&p);
	for (int i = 0; i < data.getDimension(); i++) delete [] cost_matrix[i];
	delete [] cost_matrix;
}

void attNodeSolution(Node *n, Data& data){
	double **cost = new double*[data.getDimension()];
	for (int i = 0; i < data.getDimension(); i++){
		cost[i] = new double[data.getDimension()];
		for (int j = 0; j < data.getDimension(); j++){
			cost[i][j] = data.getDistance(i,j);
		}
	}

	for(int i =0; i < n->forbidden_arcs.size(); i++){
		cost[n->forbidden_arcs[i].first-1][n->forbidden_arcs[i].second-1] = INFINITE;
	}

	attNode(n,cost, data, false);
};

bool operator<(const Node& i, const Node& j){
	return i.lower_bound > j.lower_bound;
}


int main(int argc, char** argv) {
	Data * data = new Data(argc, argv[1]);
	data->readData();

	Node raiz;
	attNodeSolution(&raiz, *data);

	priority_queue<Node> tree;
	tree.push(raiz);

	double upper_bound = stod(argv[2])+1;

	while(!tree.empty()){
		Node node = tree.top();
		tree.pop();

		if (node.feasible)
			upper_bound = min(upper_bound, node.lower_bound);
		else {
			for(int i =0; i< node.subtours[node.chosen].size()-1;i++){
				Node n;
				n.forbidden_arcs = node.forbidden_arcs;

				std::pair<int,int> forbidden_arc = {
					node.subtours[node.chosen][i],
					node.subtours[node.chosen][i+1],
				};

				n.forbidden_arcs.push_back(forbidden_arc);
				attNodeSolution(&n, *data);
				if(n.lower_bound <= upper_bound) tree.push(n);
			}
		}
	}

	cout << upper_bound << endl;

	delete data;

	return 0;
}
