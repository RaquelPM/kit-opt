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
#include "Kruskal.h"

int countEdges(vector <pair<int, int>> edges, int n){
	int count = 0;
	for (int i = 0; i < edges.size(); i++){
		if (edges[i].first == n || edges[i].second == n){
			count++;
		}
	}
	return count;
}

double vectorSquared(vector <double> v){
	int sum = 0;
	for (int i = 0; i < v.size(); i++){
		sum += v[i]*v[i];
	}
	return sum;
}

//  vector <pair<int, int>> RL(double *new_UB, double UB, vector<double*> best_lambdas, Data& data, vector<vector<double>> dist, Kruskal& kruskal){
// 	int k = 0;
// 	int step = 1;

// 	vector<double*> lambdas(data.getDimension());
// 	vector<double> subgradiente(data.getDimension());
// 	vector <pair<int, int>> edges, best_edges;

// 	while(step >= 0.0001){	
// 		// solução
// 		double cost = kruskal.MST(data.getDimension(), dist);
// 		edges = kruskal.getEdges();

// 		if(*new_UB > cost){
// 			*new_UB = cost;
// 			best_edges = edges;
// 			best_lambdas = lambdas;
// 		}
// 		else{
// 			k++;
// 			if(k >= 100) step = step/2;
// 		}

// 		// subgradiente e custo
// 		for(int i = 0; i < data.getDimension(); i++){
// 			subgradiente[i] = (2 - countEdges(edges, i));
// 			cost += 2*(*lambdas[i]);
// 		}

// 		// atualiza lambdas
// 		for(int i = 0; i < data.getDimension(); i++){
// 			double e = (step*(UB - cost))/vectorSquared(subgradiente);
// 			*lambdas[i] = (*lambdas[i]) + e*subgradiente[i];
// 		}

// 		// atualiza distancias
// 		for (int i = 0; i < data.getDimension(); i++){
// 			for (int j = 0; j < data.getDimension(); j++){
// 				dist[i][j] = data.getDistance(i,j) - (*lambdas[i]) - (*lambdas[j]);
// 			}
// 		}

// 	}

// 	return best_edges;
// }


int main(int argc, char** argv) {
	Data * data = new Data(argc, argv[1]);
	data->readData();

	double UB = stod(argv[2])+1;

	vector <vector<double>> dist(data->getDimension(), vector<double>(data->getDimension()));
	for (int i = 0; i < data->getDimension(); i++){
		for (int j = 0; j < data->getDimension(); j++){
			dist[i][j] = data->getDistance(i,j);
		}
	}

	int k = 0;
	int step = 1;
	double new_UB = UB;

	vector<double> best_lambdas(data->getDimension());
	vector <pair<int, int>> best_edges;

	Kruskal *kruskal = new Kruskal();

	// best_edges = RL(&new_UB, UB, best_lambdas, *data, dist, *kruskal);

	vector<double> lambdas(data->getDimension());
	vector<double> subgradiente(data->getDimension());
	vector <pair<int, int>> edges;
	double cost;

	while(step >= 0.0001){
		// solução
		cost = kruskal->MST(data->getDimension(), dist);
		edges = kruskal->getEdges();

		cout << cost << endl;
		if(new_UB > cost){
			new_UB = cost;
			best_edges = edges;
			best_lambdas = lambdas;
		}
		else{
			k++;
			if(k >= 1000) step = step/2;
		}

		// subgradiente e custo
		for(int i = 0; i < data->getDimension(); i++){
			subgradiente[i] = (2 - countEdges(edges, i));
			cost += 2*lambdas[i];
		}

		// atualiza lambdas
		for(int i = 0; i < data->getDimension(); i++){
			//if(vectorSquared(subgradiente) == 0) {cout << k << endl;break;};
			double e = (step*(UB - cost))/vectorSquared(subgradiente);
			lambdas[i] = lambdas[i] + e*subgradiente[i];
		}

		// atualiza distancias
		for (int i = 0; i < data->getDimension(); i++){
			for (int j = 0; j < data->getDimension(); j++){
				dist[i][j] = data->getDistance(i,j) - lambdas[i] - lambdas[j];
			}
		}

	}

	cout << "Custo: " << new_UB << endl;
	cout << "Caminho: " << best_edges.size() << endl;
	for (int i = 0; i < best_edges.size(); i++){
		cout << best_edges[i].first << " " << best_edges[i].second << endl;
	}


	delete kruskal;
	delete data;

    return 0;    
}
