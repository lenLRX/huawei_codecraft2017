#include "SSPA2.h"
#include "heap.h"
#include <assert.h>
#include <algorithm>

bool SSPA2::optimize(){
	counter++;
	vector<int> deficit_set;

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
		    return false;//no solution

		for(int dest:deficit_set){
			if(G.array_Vertex_distance[dest] == numeric_limits<int>::max())
			    break;
			else{
				augment_flow(dest);
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

	check();

	for(int i = -1;i < G.VertexNum;i++){
		if(G.array_Vertex_d[i] != 0)
		    cout << "error! " << i << " not balanced" << endl;
	}
	        
	return true;
}
//#define USE_BINARY_HEAP
//#define USE_FIBONACCI_HEAP
#define NOHEAP

#ifdef USE_FIBONACCI_HEAP

#include "FibonacciHeap.h"

void SSPA2::dijkstra(Vertex* source){

	for(auto& v:G.V){
		v.second.distance = numeric_limits<int>::max();
	}

	size_t v_size = G.V.size();

	vector<fibonacci_node> nodes(v_size);
	vector<fibonacci_node*> p_nodes(v_size);
	for(size_t i = 0;i < v_size;i++){
		p_nodes[i] = &nodes[i];
	}

	source->distance = 0;

	fibonacci_heap _heap;

	for(auto& v:G.V){
		p_nodes[v.second.id + 1]->node_index = &v.second;
		fib_heap_insert(&_heap,p_nodes[v.second.id + 1],v.second.distance);
	}

	while(true){
		int min_distance = numeric_limits<int>::max();
		//Extracting the min distant node.
		fibonacci_node *min_node = fib_heap_extract_min(&_heap);

		Vertex* v = nullptr;

		if (min_node != nullptr) {
			v = min_node->node_index;
			if(v->distance == numeric_limits<int>::max())
			    break;
			//Updating the distances for the adjacent vertices of min distant unmarked node.
			for(auto e:v->EdgesOut){
				//if(excluding_set[e->to->id + 1])
				//    continue;
				Vertex* u = e->to;
				if(e->bandwidth > 0 && e->bandwidth - e->x > 0 
					&& u->distance > v->distance + e->cost){
					u->distance = v->distance + e->cost;
					u->from_edge = e;
					fib_heap_decrease_key(&_heap, p_nodes[u->id + 1],
								u->distance);
				}
			}
		}
		else{
			break;
		}
	}

	auto cmp = [](Vertex* v1,Vertex* v2)->bool{
		return v1->distance > v2->distance;
	};
}

#endif//USE_FIBONACCI_HEAP

#ifdef USE_BINARY_HEAP

void SSPA2::dijkstra(Vertex* source){

	auto cmp = [](Vertex* v1,Vertex* v2)->bool{
		return v1->distance > v2->distance;
	};


	for(auto& v:G.V){
		v.second.distance = numeric_limits<int>::max();
	}

	heap<Vertex*,decltype(cmp)> _heap(cmp);
	source->distance = 0;

	_heap.push(source);

	size_t v_size = G.V.size();

	//set<Vertex*> excluding_set;
	vector<int> in_heap(G.V.size(),false);
	
	while(!_heap.empty()){

		Vertex* v = _heap.top();
		_heap.pop();
		in_heap[v->id + 1] = false;
		

		int my_distance = v->distance;

		for(auto e:v->EdgesOut){
			Vertex* u = e->to;
			if(e->bandwidth > 0 && e->bandwidth - e->x > 0 
			    && u->distance > v->distance + e->cost){
				u->distance = v->distance + e->cost;
				u->from_edge = e;
				if(in_heap[u->id + 1]){
					_heap.reheapify();
				}
				else{
					in_heap[u->id + 1] = true;
					_heap.push(u);
				}
			}
		}
	}
}
#endif//USE_BINARY_HEAP

#ifdef NOHEAP

void SSPA2::dijkstra(int source){
	vector<int> Q;
	for(int i = -1;i < G.VertexNum;i++){
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

void SSPA2::augment_flow(int dest){
	int source = -1;

	int min_value = numeric_limits<int>::max();
	
	min_value = min(min_value,G.array_Vertex_d[-1]);
	min_value = min(min_value,-G.array_Vertex_d[dest]);

	int pVertex = dest;

	while(true){
		//cout << "id: " << pVertex->id << endl;
		int e = G.array_Vertex_from_edge[pVertex];
		if(!G.array_Edge_IsReversEdge[e])
	        min_value = min(min_value,G.array_Edge_bandwidth[e] - G.array_Edge_x[e]);
		else
			min_value = min(min_value,G.array_Edge_bandwidth[e]);
		pVertex = G.array_Edge_from[G.array_Vertex_from_edge[pVertex]];
		if(pVertex == source)
		    break;
	}

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
}