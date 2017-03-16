#include "Firefly.h"

#include <algorithm>

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
		auto time_now = chrono::high_resolution_clock::now();
		if(chrono::duration_cast<chrono::seconds>(time_now - start_time).count() > 80)
			throw "timeout";
		auto& fly = Fireflies[i];
		CostOfFly(fly);
		if(fly.objective < Fmin){
		    Fmin = fly.objective;
			Fminpos = i;
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
	bool b = lr.optimize();
	if(b)
	    fly.objective = lr.G.total_cost();
	else//invalid
	    fly.objective = numeric_limits<int>::max();
	
	if(fly.objective < GlobalMin){
		GlobalMin = fly.objective;
		cout << "new GlobalMin: " << GlobalMin << endl;
		result = lr.G.to_String();
	}
}

void FireflySolver::Beta_step(Firefly& fly1,Firefly& fly2){
	int r_ij = 0;

	for(size_t i = 0;i < NodeNum;i++){
		if(fly1.bits[i] != fly2.bits[i])
		    r_ij++;
	}

	float beta_ij = 1 / (1 + Gamma * r_ij * r_ij);

	//cout << beta_ij << endl;
	
	for(size_t i = 0;i < NodeNum;i++){
		if(fly1.bits[i] != fly2.bits[i]){
			float rand_num = _float_distribution(generator);
			if(rand_num < beta_ij){
				fly1.newbits[i] = fly2.bits[i];
	        }
		}
	}
}

void FireflySolver::Alpha_step(Firefly& fly){
	int rand_position = _random_cell_distribution(generator);
	fly.newbits[rand_position] = !fly.newbits[rand_position];
}

void FireflySolver::optimize(){
	start_time = chrono::high_resolution_clock::now();
	while(true){
		try{
			UpdateObjectiveAndBestFly();
		}
		catch(...){
			break;
		}
		
		sort(Fireflies.begin(),Fireflies.end(),
		[](const Firefly& f1,const Firefly& f2)->bool
		{
			return f1.objective < f2.objective;
		});
		for(size_t i = 0;i < population;i++){
			auto& fly = Fireflies[i];
			fly.newbits = fly.bits;
			if(i > 0){
				//xj != null
				Beta_step(fly,Fireflies[i - 1]);
				Alpha_step(fly);
			}
			else{
				//skip
				Alpha_step(fly);
			}
		}

        //update
		for(size_t i = 0;i < population;i++){
			Fireflies[i].bits = Fireflies[i].newbits;
		}
	}
}