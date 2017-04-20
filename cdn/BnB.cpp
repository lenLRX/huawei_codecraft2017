#include "BnB.h"
#include "Timer.h"

void BnB::optimize(){
	while(true){
		try{
			step();
		}
		catch(...){
			break;
		}
	}
}

void BnB::init(int cost,vector<int> seed){
	branch first(NodeNum);
	//first.local_solution = seed;
	for(int i = 0;i < NodeNum;i++){
		first.free_mask[i] = true;
		first.local_opt[i] = 0;
	}

	int Esize = optimizer.G.array_Vertex2Edge_len[-1];
	int offset = optimizer.G.array_Vertex2Edge_offset[-1];

	for(int i = 0;i < optimizer.G.VertexNum;i++){
		if(optimizer.G.mem.array_Vertex_consumer_id[i] > 0)
		    first.local_solution[i] = true;
		else
		    first.local_solution[i] = false;
	}

	first.cost = cost;
	Q.push(first);
}

void BnB::step(){
	if(Timer::getInstance().timeout())
		throw "timeout";
	if(Q.size() == 0)
	    throw "done";
	branch present = Q.top();
	//cout << "Q.size() " << Q.size() << endl;
	Q.pop();

    
	int next_pivot = uniform_int_distribution<int>(0,
			present.free_num - 1)(generator);
	int c = 0;
	for(int i = 0;i < NodeNum;i++){
		if(present.free_mask[i]){
			if(c == next_pivot){
				break;
			}
			c++;
		}
		
	}
	
    
	/*
	int max_flow = 10000000;
	int max_pos = -1;

	int Esize = optimizer.G.array_Vertex2Edge_len[-1];
	int offset = optimizer.G.array_Vertex2Edge_offset[-1];
	for(int i = 0;i < Esize;i++){
		int e = optimizer.G.mem.array_Vertex_EdgesOut[offset + i];
		if(e < 0)
		    break;
		if(optimizer.G.mem.array_Edge_x[e] < max_flow && optimizer.G.mem.array_Edge_x[e] > 0){
			max_flow = optimizer.G.mem.array_Edge_x[e];
			max_pos = optimizer.G.mem.array_Edge_to[e];
		}
	}
	

	if(max_pos < 0)
	    cout << "error" << endl;
	*/

	present.free_mask[c] = false;
	present.free_num--;

	branch _1_branch = present;
	_1_branch.local_solution[c] = true;

	CostOfBranch(_1_branch);

	Q.push(_1_branch);

	branch _0_branch = present;
	_0_branch.local_solution[c] = false;

	CostOfBranch(_0_branch);
    Q.push(_0_branch);
}

void BnB::CostOfBranch(branch& _branch){
	optimizer.G.restore();
	unordered_set<int> includeing_set;
	for(size_t i = 0;i < NodeNum;i++){
		if(_branch.local_solution[i])
		    includeing_set.insert(i);
	}
	//cout << "server Num " << includeing_set.size() << endl;
	optimizer.legacy_create_pesudo_source(includeing_set);
	int ret = optimizer.optimize();

	if(ret > 0)
	    _branch.cost = ret;
	else//invalid
	    _branch.cost = numeric_limits<int>::max();
	
	//cout << _branch.cost << endl;
	
	if(_branch.cost < GlobalMin){
		GlobalMin = _branch.cost;
		cout << "new GlobalMin: " 
		     << GlobalMin
			 << endl;
		//result = lr.G.to_String();//slow
		optimizer.G.save_globalmin();
	}
}