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

int main(int argc, char** argv) {
	Data * data = new Data(argc, argv[1]);
	data->readData();

    double **cost = new double*[data.getDimension()];
	for (int i = 0; i < data.getDimension(); i++){
		cost[i] = new double[data.getDimension()];
		for (int j = 0; j < data.getDimension(); j++){
			cost[i][j] = data.getDistance(i,j);
		}
	}

    Kruskal *kruskal = new Kruskal(cost);


    return 0;    
}
