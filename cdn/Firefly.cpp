#include "Firefly.h"
#include <chrono>

void Firefly::resize(int n){
	bits.resize(n);
}

void FireflySolver::Randomize(Firefly& fly){

	for(size_t i = 0;i < NodeNum;i++){
		fly.bits[i] = _0_1_distribution(generator);
	}
}

void FireflySolver::UpdateObjectiveAndBestFly(){
	Fmin = numeric_limits<int>::max();
	for(int i = 0;i < population;i++){
		auto& fly = Fireflies[i];
		CostOfFly(fly);
		if(fly.objective < Fmin){
		    Fmin = fly.objective;
			Fminpos = i;
	    }

		if(Fmin < GlobalMin){
			GlobalMin = Fmin;
			cout << "new GlobalMin: " << GlobalMin << endl;
		}
	}
}

void FireflySolver::CostOfFly(Firefly& fly){
	
	lr.refresh();
	unordered_set<int> includeing_set;
	for(size_t i = 0;i < NodeNum;i++){
		if(fly.bits[i])
		    includeing_set.insert(i);
	}

	lr.create_pesudo_source(includeing_set);
	lr.optimize();
	fly.objective = lr.G.total_cost();
}

void FireflySolver::Beta_step(Firefly& fly1,Firefly& fly2){
	int r_ij = fly1.objective - fly2.objective;
	float beta_ij = 100 / (1 + Gamma * r_ij * r_ij);

	cout << beta_ij << endl;
	
	for(size_t i = 0;i < NodeNum;i++){
		if(fly1.bits[i] != fly2.bits[i]){
			float rand_num = _float_distribution(generator);
			if(rand_num < beta_ij){
				fly1.bits[i] = fly2.bits[i];
	        }
		}
	}
}

void FireflySolver::Alpha_step(Firefly& fly){
	int rand_position = _random_cell_distribution(generator);
	fly.bits[rand_position] = _0_1_distribution(generator);
}

void FireflySolver::optimize(){
	auto start_time = chrono::high_resolution_clock::now();
	while(true){
		auto time_now = chrono::high_resolution_clock::now();
		if(chrono::duration_cast<chrono::seconds>(time_now - start_time).count() > 80)
			break;
		UpdateObjectiveAndBestFly();
		for(auto& fly:Fireflies){
			if(fly.objective > Fmin){
				//xj != null
				Beta_step(fly,Fireflies[Fminpos]);
				Alpha_step(fly);
			}
			else{
				//skip
				//Alpha_step(fly);
			}
		}
	}
}