#include "optimizer.h"

void Optimizer::create_pesudo_source(unordered_set<int> IncludingVertex){
	int sum = 0;

	for(int i = 0;i < G.VertexNum;i++){
		sum += G.array_Vertex_d[i];
		//cout << pv.first << " : " << pv.second.d << endl;
		if(IncludingVertex.count(i) == 0)
		    continue;
		//cout << "to" << i << endl;
		G.add_Edge(-1,i,numeric_limits<int>::max(),0);
		//cout << "e.id " << e.id << endl;
	}

	G.array_Vertex_d[-1] = -sum;

	//cout << "pesudo_source.d " << pesudo_source.d << endl;

	
}

void Optimizer::check(){
	for(int i = -1;i < G.VertexNum;i++){
		int sum = 0;
		//for(auto edge_no:vp.second.EdgesIn){
		for(int j = 0;j < MaxEdgeNum;j++){
			int e = G.array_Vertex_EdgesIn[i * MaxEdgeNum + j];
			if(e < 0)
			    break;
			if(!G.array_Edge_IsReversEdge[e])
				sum += G.array_Edge_x[e];
		}

		for(int j = 0;j < MaxEdgeNum;j++){
			int e = G.array_Vertex_EdgesOut[i * MaxEdgeNum + j];
			if(e < 0)
			    break;
			if(!G.array_Edge_IsReversEdge[e])
				sum -= G.array_Edge_x[e];
		}

		if(sum != 0){
			//cout << "ID : " << i << " Vertex sum " << sum << endl;
		}

		if(G.array_Vertex_d[i] !=0 )
			cout << "d: " << G.array_Vertex_d[i] << endl;
		
		if(G.array_Vertex_consumer_id[i] > 0 &&
		    G.array_Consumer_requirement[G.array_Vertex_consumer_id[i]] - sum != 0){
			cout << "ID : " << i << "unbalanced" << endl;
		}
	}
}