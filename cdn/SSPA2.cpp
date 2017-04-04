#include "SSPA2.h"
#include "heap.h"
#include "array_heap.h"
#include <assert.h>
#include <algorithm>

int SSPA2::optimize(){
	counter++;
	vector<int> deficit_set;

	int local_cost = 0;

	for(int i = 0;i < G.VertexNum;i++){
		if(G.array_Vertex_d[i] < 0)
		    deficit_set.push_back(i);
	}

    while(true){
		if(deficit_set.size() == 0)
		    break;//done!

		dijkstra(-1);

		sort(deficit_set.begin(),deficit_set.end(),
		[this](int lhs,int rhs)->bool{
			return G.array_Vertex_distance[lhs] <  G.array_Vertex_distance[rhs];
		});

		if(G.array_Vertex_distance[deficit_set[0]] == numeric_limits<int>::max())
		    return -1;//no solution

		for(int dest:deficit_set){
			if(G.array_Vertex_distance[dest] == numeric_limits<int>::max())
			    break;
			else{
				local_cost += augment_flow(dest);
			}
		}
        
		for(auto it = deficit_set.begin();it != deficit_set.end();){
			if(G.array_Vertex_d[(*it)] == 0){
				it = deficit_set.erase(it);
			}
			else{
				++it;
			}
		}
	}
	for(int i = 0;i < MaxEdgeNum;i++){
		int e = G.array_Vertex_EdgesOut[-1 * MaxEdgeNum + i];
		if(G.array_Edge_x[e] > 0)
		    local_cost += G.ServerCost;
	}
	return local_cost;
}


void SSPA2::dijkstra(int source){
	for(int i = 0;i < G.VertexNum;i++){
		G.array_Vertex_distance[i] = numeric_limits<int>::max();
	}
	array_heap _heap(G.VertexNum +1,G);

	size_t v_size = G.VertexNum + 1;

	//set<Vertex*> excluding_set;
	vector<int> excluding_set(v_size,false);
	
	for(size_t i = 0;i < v_size - 1;i++){
		int v = _heap.extract_min();
		int distance = G.array_Vertex_distance[v];

		if(distance == numeric_limits<int>::max())
		    return;

		excluding_set[v + 1] = true;

		//for(auto e:v->EdgesOut){
		for(int j = 0;j < MaxEdgeNum;j ++){
			int e = G.array_Vertex_EdgesOut[v * MaxEdgeNum + j];
			if(e < 0)
			    break;
			int u = G.array_Edge_to[e];
			if(excluding_set[u + 1])
			    continue;
			
			if(G.array_Edge_bandwidth[e] > 0 && G.array_Edge_bandwidth[e] - G.array_Edge_x[e] > 0 
			    && G.array_Vertex_distance[u] > G.array_Vertex_distance[v] + G.array_Edge_cost[e]){
				G.array_Vertex_distance[u] = G.array_Vertex_distance[v] + G.array_Edge_cost[e];
				G.array_Vertex_from_edge[u] = e;
				_heap.decrease_key(u);
			}
		}
	}
}


//#define USE_BINARY_HEAP
//#define USE_FIBONACCI_HEAP
//#define NOHEAP

#ifdef NOHEAP

void SSPA2::dijkstra(int source){
	vector<int> Q;
	for(int i = 0;i < G.VertexNum;i++){
		G.array_Vertex_distance[i] = numeric_limits<int>::max();
		Q.push_back(i);
	}
	G.array_Vertex_distance[source] = 0;

	size_t v_size = G.VertexNum + 1;

	//set<Vertex*> excluding_set;
	vector<int> excluding_set(v_size,false);
	
	for(size_t i = 0;i < v_size - 1;i++){
		int min_element_pos = -1;
		int min_value = numeric_limits<int>::max();
		for(size_t j = i;j < v_size;j++){
			if(G.array_Vertex_distance[Q[j]] < min_value){
				min_value = G.array_Vertex_distance[Q[j]];
				min_element_pos = j;
			}
		}

		if(min_element_pos < 0)
		    return;

		swap(Q[i],Q[min_element_pos]);

		excluding_set[Q[i] + 1] = true;

		int v = Q[i];

		//for(auto e:v->EdgesOut){
		for(int j = 0;j < MaxEdgeNum;j ++){
			int e = G.array_Vertex_EdgesOut[v * MaxEdgeNum + j];
			if(e < 0)
			    break;
			int u = G.array_Edge_to[e];
			if(excluding_set[u + 1])
			    continue;
			
			if(G.array_Edge_bandwidth[e] > 0 && G.array_Edge_bandwidth[e] - G.array_Edge_x[e] > 0 
			    && G.array_Vertex_distance[u] > G.array_Vertex_distance[v] + G.array_Edge_cost[e]){
				G.array_Vertex_distance[u] = G.array_Vertex_distance[v] + G.array_Edge_cost[e];
				G.array_Vertex_from_edge[u] = e;
			}
		}
	}
}

#endif//NOHEAP

int SSPA2::augment_flow(int dest){
	int cost = 0;
	int source = -1;

	int min_value = numeric_limits<int>::max();
	
	min_value = min(min_value,G.array_Vertex_d[-1]);
	min_value = min(min_value,-G.array_Vertex_d[dest]);

	int pVertex = dest;

	while(true){
		//cout << "id: " << pVertex->id << endl;
		int e = G.array_Vertex_from_edge[pVertex];
		cost += G.array_Edge_cost[e];
		if(!G.array_Edge_IsReversEdge[e])
	        min_value = min(min_value,G.array_Edge_bandwidth[e] - G.array_Edge_x[e]);
		else
			min_value = min(min_value,G.array_Edge_bandwidth[e]);
		pVertex = G.array_Edge_from[G.array_Vertex_from_edge[pVertex]];
		if(pVertex == source)
		    break;
	}

	cost = cost * min_value;

	G.array_Vertex_d[-1] -= min_value;
	G.array_Vertex_d[dest] += min_value;

	pVertex = dest;
	
	while(true){
		int r = G.array_Vertex_from_edge[pVertex];
		//cout << r->from->id << "=>" << r->to->id << " min: " << min_value << endl;
		if(!G.array_Edge_IsReversEdge[r]){
			G.array_Edge_x[r] += min_value;
			G.array_Edge_bandwidth[G.array_Edge_ResidualEdgeNo[r]] = G.array_Edge_x[r];
		}
		else{
			G.array_Edge_x[G.array_Edge_ResidualEdgeNo[r]] -= min_value;
			G.array_Edge_bandwidth[r] = G.array_Edge_x[G.array_Edge_ResidualEdgeNo[r]];
		}
		pVertex = G.array_Edge_from[G.array_Vertex_from_edge[pVertex]];
		if(pVertex == source)
			break;
	}

	return cost;
}