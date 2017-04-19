#include "Firefly.h"
#include "Timer.h"

#include <algorithm>

void Firefly::resize(int n){
	bits.resize(n);
}

void FireflySolver::Randomize(Firefly& fly){

	for(size_t i = 0;i < NodeNum;i++){
		fly.bits[i] = _0_1_distribution(generator);
	}
}

bool FireflySolver::UpdateObjectiveAndBestFly(){
	bool ret = false;
	Fmin = numeric_limits<int>::max();
	for(int i = 0;i < population;i++){
		if(Timer::getInstance().timeout())
			throw "timeout";
		auto& fly = Fireflies[i];
		if(tabu.count(fly.bits) && i > 0){
			fly.objective = numeric_limits<int>::max();
			//continue;
		}
		else{
			tabu.insert(fly.bits);
			bool b = CostOfFly(fly,i);
			ret = b || ret;
		}
		
		if(fly.objective < Fmin){
		    Fmin = fly.objective;
			Fminpos = i;
	    }
	}
	return ret;
}

bool FireflySolver::CostOfFly(Firefly& fly,int i){
	//cout << fly.count() << endl;

	bool ret = false;
	
	lr.G.restore();
	unordered_set<int> includeing_set;
	for(size_t i = 0;i < NodeNum;i++){
		if(fly.bits[i])
		    includeing_set.insert(i);
	}
    if(Timer::getInstance().get() < 80000)
	    lr.legacy_create_pesudo_source(includeing_set);
	else
	    lr.create_pesudo_source(includeing_set);
	int b = lr.optimize();
	if(b > 0)
	    fly.objective = b;
	else//invalid
	    fly.objective = numeric_limits<int>::max();
	
	if(b > 0){
		int Esize = lr.G.array_Vertex2Edge_len[-1];
		int offset = lr.G.array_Vertex2Edge_offset[-1];
		fly.bits = vector<int>(lr.G.VertexNum,false);
		for(int i = 0;i < Esize;i++){
			int e = lr.G.mem.array_Vertex_EdgesOut[offset + i];
			if(e < 0)
			    break;
			if(lr.G.mem.array_Edge_x[e] > 0)
			    fly.bits[lr.G.mem.array_Edge_to[e]] = true;
		}
	}
	
	if(fly.objective < GlobalMin){
		//lr.check();
		minServerNum = 0;
		for(auto b:fly.bits){
			if(b)
			    minServerNum++;
		}
		GlobalMin = fly.objective;
		cout << "new GlobalMin: " 
		     << GlobalMin << " ServerNum " 
			 << minServerNum << " position: "
			 << i << endl;
		//result = lr.G.to_String();//slow
		lr.G.save_globalmin();
		//cout << result << endl;
		ret = true;
	}
	return ret;
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
	if(fly.objective == numeric_limits<int>::max()){
		//the fly is invalid open a new facility
		int rand_position = _random_cell_distribution(generator);
	    fly.newbits[rand_position] = 1;
		return;
	}
	if(fly.count() > minServerNum){

		while(fly.count() > minServerNum){
			//too many facility close one
			int delta = fly.count() - minServerNum;

			uniform_int_distribution<int> d(0,delta - 1);

			int todelete = d(generator);

			//cout << delta << "todelete " << todelete << endl;

			size_t bits_size = fly.newbits.size();
			int c = 0;
			for(size_t i = 0;i < bits_size;i++){
				if(fly.newbits[i]){
					if(c == todelete){
						//cout << "delete" << endl;
						fly.newbits[i] = false;
						break;
					}
					c++;
				}
					
			}
		}
			
	}
	{
		//the fly is valid close a facility 
		float rand_num = _0_1_distribution(generator);
		if(rand_num < 0.0){
			int empty_consumer_node_num = 0;
			size_t bits_size = fly.newbits.size();
			for(size_t i = 0;i < bits_size;i++){
				if(consumer_map[i] && !fly.newbits[i]){
					empty_consumer_node_num++;
				}
			}

			int c_to_open = uniform_int_distribution<int>(0,
			empty_consumer_node_num - 1)(generator);

			int c = 0;

			for(size_t i = 0;i < bits_size;i++){
				if(consumer_map[i] && !fly.newbits[i]){
					if(c == c_to_open){
						fly.newbits[i] = true;
						break;
					}
					c++;
				}
			}
		}

		else if(rand_num < 0.5){
			/*
			int rand_position = _random_cell_distribution(generator);
	        fly.newbits[rand_position] = 0;
			*/

			int delta = fly.count();

			uniform_int_distribution<int> d(0,delta - 1);

			int todelete = d(generator);

			//cout << delta << "todelete " << todelete << endl;

			size_t bits_size = fly.newbits.size();
			int c = 0;
			for(size_t i = 0;i < bits_size;i++){
				if(fly.newbits[i]){
					if(c == todelete){
						//cout << "delete" << endl;
						fly.newbits[i] = false;
						break;
					}
					c++;
				}
					
			}
			
		}
		else{
			int count = fly.count();
				//cout << "count : " << count << endl;
				uniform_int_distribution<int> d(0,count - 1);

				int to_swap = d(generator);

				int pos1 = -1;

				size_t bits_size = fly.newbits.size();
				int c = 0;
				for(size_t i = 0;i < bits_size;i++){
					if(fly.newbits[i]){
						if(c == to_swap){
							pos1 = i;
							break;
						}
						c++;
					}	
				}

				c = 0;
				int pos2 = -1;

				int empty_consumer_node_num = 0;
				for(size_t i = 0;i < bits_size;i++){
					if(consumer_map[i] && !fly.newbits[i]){
						empty_consumer_node_num++;
					}
				}

				if(empty_consumer_node_num <= 0){
					uniform_int_distribution<int> d2(0,NodeNum - count - 1);
					int to_swap2 = d2(generator);
					for(size_t i = 0;i < bits_size;i++){
						if(!fly.newbits[i]){
							if(c == to_swap2){
								pos2 = i;
								break;
							}
							c++;
						}	
					}

				}else{
					int c_to_open = uniform_int_distribution<int>(0,
					empty_consumer_node_num - 1)(generator);

					for(size_t i = 0;i < bits_size;i++){
						if(consumer_map[i] && !fly.newbits[i]){
							if(c == c_to_open){
								pos2 = i;
								break;
							}
							c++;
						}
					}
				}

				if(pos1 < 0)
					cout << "error! " << pos1 << " to_swap: " << to_swap << " count " << count << endl;
				
				if(pos2 < 0)
					cout << "error " << pos2 << " " << empty_consumer_node_num << endl;

				swap(fly.newbits[pos1],fly.newbits[pos2]);
		}
		#ifdef shit
		else{
			float rand_num2 = _0_1_distribution(generator);
			if(Timer::getInstance().get() > 40000 && rand_num2 < 0.5){
				int count = fly.count();
				//cout << "count : " << count << endl;
				uniform_int_distribution<int> d(0,count - 1);

				int to_swap = d(generator);

				int pos1 = -1;

				size_t bits_size = fly.newbits.size();
				int c = 0;
				for(size_t i = 0;i < bits_size;i++){
					if(fly.newbits[i]){
						if(c == to_swap){
							pos1 = i;
							break;
						}
						c++;
					}	
				}

				c = 0;

				int near_by_count = lr.G.V.at(pos1).EdgesIn.size() + lr.G.V.at(pos1).EdgesIn.size();

				uniform_int_distribution<int> d2(0,near_by_count - 1);

				int to_swap2 = d2(generator);

				int pos2 = -1;

				size_t insize = lr.G.V.at(pos1).EdgesIn.size();

				if(to_swap2 < insize){
					pos2 = lr.G.V.at(pos1).EdgesIn[to_swap2]->to->id;
				}
				else{
					pos2 = lr.G.V.at(pos1).EdgesOut[to_swap2 - insize]->to->id;
				}

				if(pos1 < 0)
					cout << "error! " << pos1 << " to_swap: " << to_swap << " count " << count << endl;
				
				if(pos2 < 0)
					pos2 = _random_cell_distribution(generator);

				swap(fly.newbits[pos1],fly.newbits[pos2]);

			}//if(Timer::getInstance().get() > 40 && rand_num2 < 0.5)
			else{
				int count = fly.count();
				//cout << "count : " << count << endl;
				uniform_int_distribution<int> d(0,count - 1);

				int to_swap = d(generator);

				int pos1 = -1;

				size_t bits_size = fly.newbits.size();
				int c = 0;
				for(size_t i = 0;i < bits_size;i++){
					if(fly.newbits[i]){
						if(c == to_swap){
							pos1 = i;
							break;
						}
						c++;
					}	
				}

				c = 0;
				int pos2 = -1;

				int empty_consumer_node_num = 0;
				for(size_t i = 0;i < bits_size;i++){
					if(consumer_map[i] && !fly.newbits[i]){
						empty_consumer_node_num++;
					}
				}

				if(empty_consumer_node_num <= 0){
					uniform_int_distribution<int> d2(0,NodeNum - count - 1);
					int to_swap2 = d2(generator);
					for(size_t i = 0;i < bits_size;i++){
						if(!fly.newbits[i]){
							if(c == to_swap2){
								pos2 = i;
								break;
							}
							c++;
						}	
					}

				}else{
					int c_to_open = uniform_int_distribution<int>(0,
					empty_consumer_node_num - 1)(generator);

					for(size_t i = 0;i < bits_size;i++){
						if(consumer_map[i] && !fly.newbits[i]){
							if(c == c_to_open){
								pos2 = i;
								break;
							}
							c++;
						}
					}
				}

				if(pos1 < 0)
					cout << "error! " << pos1 << " to_swap: " << to_swap << " count " << count << endl;
				
				if(pos2 < 0)
					cout << "error " << pos2 << " " << empty_consumer_node_num << endl;

				swap(fly.newbits[pos1],fly.newbits[pos2]);
			}
		}
#endif
	}
}

//亮的萤火虫，向暗的反方向随机移动一位
void FireflySolver::Alpha2_step(Firefly& fly_dull,Firefly& fly){
		//the fly is invalid open a new facility
		int rand_position = _random_cell_distribution(generator);
	    if(fly.newbits[rand_position]==fly_dull.bits[rand_position])
			fly.newbits[rand_position]=!fly.bits[rand_position];
}

//获取最k近的萤火虫
int FireflySolver::Get_Closer(vector<Firefly> Fireflies,int site,int k){
	if(k>site)
		k=site;
	priority_queue<MyDistance> Kmin;
	
	for(size_t i=0;i < site;i++){
		int r_ij = 0;
		for(size_t j = 0;j < NodeNum;j++){
			if(Fireflies[site].bits[j] != Fireflies[i].bits[j])
				r_ij++;
		}
		if(Kmin.size()<k){
			Kmin.push(MyDistance(i,r_ij));
		}else{
			if(r_ij<Kmin.top().d_ij){
				Kmin.pop();
				Kmin.push(MyDistance(i,r_ij));
			}
		}
	}
	return Kmin.top().site;
}

void FireflySolver::optimize(int stop_round){
	/*
	high_resolution_clock::time_point last_global_min = high_resolution_clock::now();
	while(duration_cast<milliseconds>(high_resolution_clock::now()
	 - last_global_min).count() < 10000){
		try{
			if(UpdateObjectiveAndBestFly()){
				last_global_min = high_resolution_clock::now();
			}
		}
	*/
	cout << "starting optimize" << endl;
	int count = 0;
	int last_global_min = 0;
	while(count - last_global_min < stop_round){
		count++;
		try{
			if(UpdateObjectiveAndBestFly()){
				last_global_min = count;
			}
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
			if(false && i > Fireflies.size()/2){
				//xj != null
				//int c_site = Get_Closer(Fireflies,i,1);
				//Beta_step(fly,Fireflies[c_site]);
				//Alpha2_step(fly,Fireflies[c_site]);
				Beta_step(fly,Fireflies[i - 1]);
				
				//Alpha_step(fly);
			}
			else if(i > 0){
				fly = Fireflies[0];
				//skip
				Alpha_step(fly);
			}
		}

        /*
		size_t bits_size = Fireflies[0].newbits.size();
		for(size_t i = 0;i < bits_size;i++){
			if(Fireflies[0].newbits[i]){
				cout << i << " ";
			}
		}

		cout << " objective " << Fireflies[0].objective << endl;
		*/

        //update
		for(size_t i = 0;i < population;i++){
			Fireflies[i].bits = Fireflies[i].newbits;
		}
	}
}