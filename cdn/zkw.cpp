#include "zkw.h"
#include <limits>
#include <algorithm>
using namespace std;

int zkw::optimize(){
	counter++;
	pi = 0;
	cost = 0;
	do 
		do 
		    memset(raw_v_accessed,0,sizeof(raw_v_accessed));
		while(G.mem.array_Vertex_d[-1] > 0 && aug(-1,G.mem.array_Vertex_d[-1]));
    while(G.mem.array_Vertex_d[-1] > 0 && modifylabel());
	vector<int> actual_flow(G.VertexNum,0);
	int E_size = G.array_Vertex2Edge_len[-1];
	for(int i = 0;i < E_size;i++){
		int e = G.mem.array_Vertex_EdgesOut[G.array_Vertex2Edge_offset[-1] + i];
		if(e < 0)
		    break;
		if(G.mem.array_Edge_x[e] > 0)
		    actual_flow[G.mem.array_Edge_to[e]] += G.mem.array_Edge_x[e];
		    //local_cost += G.get_ServerCost(G.mem.array_Edge_to[e],G.mem.array_Edge_x[e]);
	}

	if(G.mem.array_Vertex_d[-1] != 0)
	    return -1;

	for(int i = 0;i < G.VertexNum;i++){
		if(actual_flow[i] > 0)
		    cost += G.get_ServerCost(i,actual_flow[i]);
	}
	return cost;
}

int zkw::aug(int v,int flow){
	v_accessed[v] = true;
	if(G.mem.array_Vertex_d[v] < 0){
		int actual_flow;
		actual_flow = min(-G.mem.array_Vertex_d[v],flow);
		G.mem.array_Vertex_d[v] += actual_flow;
		G.mem.array_Vertex_d[-1] -= actual_flow;
		cost += pi * actual_flow;
		return actual_flow;
	}

	
	int l = flow;
	int offset = G.array_Vertex2Edge_offset[v];
	int Esize = G.array_Vertex2Edge_len[v];
	for(int j = 0;j < Esize;j ++){
		int e = G.mem.array_Vertex_EdgesOut[offset + j];
		if(e < 0) break;
		int u = G.mem.array_Edge_to[e];
		if(!v_accessed[u] && G.mem.array_Edge_cost[e] == 0){
			int nextflow;
			if(G.mem.array_Edge_IsReversEdge[e]){
				nextflow = min(l,G.mem.array_Edge_bandwidth[e]);
				if(nextflow <= 0)
				    continue;
				int d = aug(u,nextflow);
				G.mem.array_Edge_bandwidth[e] -= d;
				G.mem.array_Edge_x[G.mem.array_Edge_ResidualEdgeNo[e]] -= d;
			}
			else{
				nextflow = min(l,G.mem.array_Edge_bandwidth[e] - G.mem.array_Edge_x[e]);
				if(nextflow <= 0)
				    continue;
				int d = aug(u,nextflow);
				G.mem.array_Edge_x[e] += d;
				G.mem.array_Edge_bandwidth[G.mem.array_Edge_ResidualEdgeNo[e]] += d;
			}
			if(l == 0)
				return flow;
		}
	}
	return flow - l;
}

bool zkw::modifylabel(){
	int d = numeric_limits<int>::max();
	for(int v = -1;v < G.VertexNum;v++){
		if(v_accessed[v]){
			int offset = G.array_Vertex2Edge_offset[v];
			int Esize = G.array_Vertex2Edge_len[v];
			for(int j = 0;j < Esize;j ++){
				int e = G.mem.array_Vertex_EdgesOut[offset + j];
				if(e < 0) break;
				int u = G.mem.array_Edge_to[e];
				if(!v_accessed[u]){
					if(G.mem.array_Edge_IsReversEdge[e]){
						continue;
						if(G.mem.array_Edge_cost[e] < d 
						    && G.mem.array_Edge_bandwidth[e] > 0)
						    d = G.mem.array_Edge_cost[e];
					}else{
						if(G.mem.array_Edge_cost[e] < d 
						    && G.mem.array_Edge_bandwidth[e] - G.mem.array_Edge_x[e] > 0)
						    d = G.mem.array_Edge_cost[e];
							if(d < 0)
							    throw "error";
					}
				}
			}
		}
	}

	if(d == numeric_limits<int>::max())return false;

	for(int v = -1;v < G.VertexNum;v++){
		if(v_accessed[v]){
			int offset = G.array_Vertex2Edge_offset[v];
			int Esize = G.array_Vertex2Edge_len[v];
			for(int j = 0;j < Esize;j ++){
				int e = G.mem.array_Vertex_EdgesOut[offset + j];
				if(e < 0) break;
				int u = G.mem.array_Edge_to[e];
				if(!v_accessed[u]){
					if(G.mem.array_Edge_IsReversEdge[e])
						G.mem.array_Edge_cost[e] += d;
					else
					    G.mem.array_Edge_cost[e] -= d;
				}
			}
		}
	}
	pi += d;
	return true;
}