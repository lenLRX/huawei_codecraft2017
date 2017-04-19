#include "optimizer.h"
#include <algorithm>

void Optimizer::create_pesudo_source(unordered_set<int> IncludingVertex){
	int sum = 0;

	for(int i = 0;i < G.VertexNum;i++){
		sum += G.mem.array_Vertex_d[i];
		//cout << pv.first << " : " << pv.second.d << endl;
		if(IncludingVertex.count(i) == 0)
		    continue;
		//cout << "to" << i << endl;
		for(int j = 0;j < G.ServerLvlNum;j++){
			int ability;
			int server_cost;
			if(j == 0){
				ability = G.const_array_Server_Ability[j];
				server_cost = G.const_array_Server_Cost[j];
			}
			else{
				ability = G.const_array_Server_Ability[j] - G.const_array_Server_Ability[j - 1];
				server_cost = G.const_array_Server_Cost[j] - G.const_array_Server_Cost[j - 1];
			}
			G.add_Edge(-1,i,ability,server_cost / ability);
		}
		
		//cout << "e.id " << e.id << endl;
	}

	G.mem.array_Vertex_d[-1] = -sum;

	//cout << "pesudo_source.d " << pesudo_source.d << endl;

	
}

void Optimizer::legacy_create_pesudo_source(unordered_set<int> IncludingVertex){
	int sum = 0;

	int max_ability = 0;

	for(int i = 0;i < G.ServerLvlNum;i++){
		max_ability = max(max_ability,G.const_array_Server_Ability[i]);
	}

	for(int i = 0;i < G.VertexNum;i++){
		sum += G.mem.array_Vertex_d[i];
		//cout << pv.first << " : " << pv.second.d << endl;
		if(IncludingVertex.count(i) == 0)
		    continue;
		//cout << "to" << i << endl;
		G.add_Edge(-1,i,max_ability,0);
		//cout << "e.id " << e.id << endl;
	}

	G.mem.array_Vertex_d[-1] = -sum;

	//cout << "pesudo_source.d " << pesudo_source.d << endl;

	
}


void Optimizer::check(){
	throw "not valid now";
	for(int i = -1;i < G.VertexNum;i++){
		int sum = 0;
		//for(auto edge_no:vp.second.EdgesIn){
		for(int j = 0;j < MaxEdgeNum;j++){
			int e = G.mem.array_Vertex_EdgesIn[i * MaxEdgeNum + j];
			if(e < 0)
			    break;
			if(!G.mem.array_Edge_IsReversEdge[e])
				sum += G.mem.array_Edge_x[e];
		}

		for(int j = 0;j < MaxEdgeNum;j++){
			int e = G.mem.array_Vertex_EdgesOut[i * MaxEdgeNum + j];
			if(e < 0)
			    break;
			if(!G.mem.array_Edge_IsReversEdge[e])
				sum -= G.mem.array_Edge_x[e];
		}

		if(sum != 0){
			//cout << "ID : " << i << " Vertex sum " << sum << endl;
		}

		if(G.mem.array_Vertex_d[i] !=0 )
			cout << "d: " << G.mem.array_Vertex_d[i] << endl;
		
		if(G.mem.array_Vertex_consumer_id[i] > 0 &&
		    G.mem.array_Consumer_requirement[G.mem.array_Vertex_consumer_id[i]] - sum != 0){
			cout << "ID : " << i << "unbalanced" << endl;
		}
	}
}