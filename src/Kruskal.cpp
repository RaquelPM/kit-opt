#include "Kruskal.h"

Kruskal::Kruskal(){}

void Kruskal::initGraph(vvi dist){
	for(int i = 1; i < dist.size(); ++i){
		for(int j = 1; j < dist[i].size(); ++j){
			graph.push( make_pair(-dist[i][j], make_pair(i, j)) );
		}	
	}
}

void Kruskal::initDisjoint(int n, vvi dist){
	initGraph(dist);
	edges.clear();
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

double Kruskal::MST(int nodes, vvi dist){
	initDisjoint(nodes, dist);

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

	for (int j = 0; j<2;j++){
		int distance = 9999999;
		int best = -1;
		for(int i = 1; i < dist[0].size(); i++){
			if(dist[0][i] < distance){
				distance = dist[0][i];
				best = i;
			}
		}
		edges.push_back(make_pair(0, best));
		cost += distance;
		dist[0][best] = 999999;
	}

	return cost;
}