#include "Kruskal.h"

Kruskal::Kruskal(vii dist){
	for(int i = 1; i < dist.size(); ++i){
		for(int j = 1; j < dist[i].size(); ++j){
			graph.push(make_pair(-dist[i][j], make_pair(i, j)) );
		}	
	}
}

void Kruskal::initDisjoint(int n){
	pset.resize(n);
	for (int i = 0; i < n; ++i){
		pset[i] = i;
	}
}

int Kruskal::findSet(int i){
	return (pset[i] == i) ? i : (pset[i] = findSet(pset[i]));
}

void Kruskal::unionSet(int i, int j){
	pset[findSet(i)] = findSet(j);
}

bool Kruskal::isSameSet(int i, int j){
	return (findSet(i) == findSet(j))? true:false;
}

vii Kruskal::getEdges(){
	return edges;
}

double Kruskal::MST(int nodes){
	initDisjoint(nodes);

	double cost = 0;

	while(!graph.empty()){
		pair<double, ii> p = graph.top();
		graph.pop();

		if(!isSameSet(p.second.first, p.second.second)){
			edges.push_back(make_pair(p.second.first, p.second.second));
			cost += (-p.first);
			unionSet(p.second.first, p.second.second);
		}
	}

	int best1, best2 = -1;
	int distance = 9999999;

	for (int j = 0; j<2;j++){
		for (int i = 1; i< data.getDimension(); i++){
			if(data.getDistance(0,i) < distance){
				distance = data.getDistance(0,i);
				if(!j) best1 = i;
				else best2 = i;
			}
		}
		if(!j) edges.push_back(make_pair(0, best1));
		else edges.push_back(make_pair(0, best2));
	}

	return cost;
}