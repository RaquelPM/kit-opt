#include "RL.h"

int rlCountEdges(vector <pair<int, int>> edges, int n){
	int count = 0;
	for (int i = 0; i < edges.size(); i++){
		if (edges[i].first == n || edges[i].second == n){
			count++;
		}
	}
	return count;
}

int rlVectorSquared(vector <int> v){
	int sum = 0;
	for (int i = 0; i < v.size(); i++){
		sum += v[i]*v[i];
	}
	return sum;
}

RL::RL() {}

int RL::doRL(vector<vector<double>> dist, Data& data, double UB, vector<double>& lambdas){
	int k = 0;
	double step = 1;
	new_UB = 0;

	int n = -1;
	int maior_grau = 0;

	Kruskal *kruskal = new Kruskal();
	
	vector<int> subgradiente(data.getDimension());
	vector <pair<int, int>> edges;
	double cost;

	vector<vector<double>> dist_copy = dist;

	while(step >= 0.0005){
		// solução
		cost = kruskal->MST(data.getDimension(), dist_copy);
		edges = kruskal->getEdges();

		// subgradiente
		for(int i = 0; i < data.getDimension(); i++){
			subgradiente[i] = (2 - rlCountEdges(edges, i));
			cost += 2.0*lambdas[i];
		}	


		if(cost > new_UB){
			k = 0;
			new_UB = cost;
			best_edges = edges;
			best_lambdas = lambdas;
		}
		else{
			k++;
			if(k >= 30){
				step = step/2;
				k = 0;
			} 
		}

		//tamanho do passo
		if(rlVectorSquared(subgradiente) == 0) break;
		double e = (step*(UB - cost))/rlVectorSquared(subgradiente);

		for(int i = 0; i < data.getDimension(); i++){
			lambdas[i] += e*subgradiente[i];
		}

		// atualiza distancias
		for (int i = 0; i < data.getDimension(); i++){
			for (int j = 0; j < data.getDimension(); j++){
				dist_copy[i][j] = dist[i][j] - lambdas[i] - lambdas[j];
			}
		}

	}
	for(int i =0; i< best_edges.size(); i++){
		int grau = rlCountEdges(best_edges, i);
		if(grau >= maior_grau){
			maior_grau = grau;
			n = i;
		}
	}
	n = maior_grau == 2 ? -1 : n;

	delete kruskal;
	return n;
}

vector <pair<int, int>> RL::getBestEdges(){
    return best_edges;
}

vector <double> RL::getBestLambdas(){
    return best_lambdas;
}

double RL::getNewUB(){
    return new_UB;
}