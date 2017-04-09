#include "Relax.h"
#include "deploy.h"
#include <assert.h>
#include <algorithm>
#include <string>

int Relax::optimize(){

	G.restore();

	unordered_set<int> including_set;
	for(int i = 0;i < G.ConsumerNum;i++){
		including_set.insert(G.array_Consumer_fromVertex[i]);
	}
	create_pesudo_source(including_set);

	vector<int> deficit_set;

	for(int i = 0;i < MaxEdgeNum;i++){
		int e = G.array_Vertex_EdgesOut[-1 * MaxEdgeNum + i];
		if(e < 0)
		    break;
		G.array_Edge_x[e] = -G.array_Vertex_d[G.array_Edge_to[e]];
		deficit_set.push_back(G.array_Edge_to[e]);
	}
	
	vector<int> deficit_set_original = deficit_set;

	bool b = true;

	bool b_vertex = false;

    for(int id = 0;;id++){
		size_t size = deficit_set.size();
		//cout << "cost " << G.total_cost() << endl;
		//check();

		if(id%size == 0){
			if(!b)
			    break;
			sort(deficit_set.begin(),deficit_set.end(),
			[this](const int lhs,const int rhs)->bool{
				return G.array_Vertex_distance[lhs] > G.array_Vertex_distance[rhs];
			});
			b = false;
		}

		b_vertex = false;

		deficit_set = deficit_set_original;

		swap(deficit_set[0],deficit_set[id % size]);

		dijkstra(deficit_set.front());

		sort(deficit_set.begin() + 1,deficit_set.end(),
		[this](const int lhs,const int rhs)->bool{
			return G.array_Vertex_distance[lhs] < G.array_Vertex_distance[rhs];
		});

		for(size_t i = 1;i < size;i++){
			int dest = deficit_set[i];
			if(G.array_Vertex_distance[dest] == numeric_limits<int>::max())
			    break;
			else{
				bool ret = augment_flow(deficit_set[0],dest);
				b_vertex = b_vertex || ret;
			}
		}
		

		if(false && !b_vertex){
			int source = deficit_set[0];
			reverse_dijkstra(source);
			deficit_set.erase(deficit_set.begin());
			bool ret = reverse_augment_flow(source,deficit_set);
			b_vertex = b_vertex || ret;
		}



		
		for(auto it = deficit_set_original.begin();it != deficit_set_original.end();){
			int source_edge;

			//for(auto e:(*it)->EdgesIn){
			for(int j = 0;j < MaxEdgeNum;j++){
				int e = G.array_Vertex_EdgesIn[(*it) * MaxEdgeNum + j];
				if(G.array_Edge_from[e] == -1){
					source_edge = e;
					break;
				}
			}

			if(G.array_Edge_x[source_edge] == 0){
				it = deficit_set_original.erase(it);
			}
			else{
				++it;
			}
		}
		b = b || b_vertex;
	}
	cout << "cost " << G.total_cost() << endl;
	return true;
}

void Relax::dijkstra(int source){

	vector<int> Q;
	for(int i = -1;i < G.VertexNum + 1;i++){
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
			if(excluding_set[u + 1] || u < 0)
			    continue;
			
			if(G.array_Edge_bandwidth[e] > 0 && G.array_Edge_bandwidth[e] - G.array_Edge_x[e] > 0 
			    && G.array_Vertex_distance[u] > G.array_Vertex_distance[v] + G.array_Edge_cost[e]){
				G.array_Vertex_distance[u] = G.array_Vertex_distance[v] + G.array_Edge_cost[e];
				G.array_Vertex_from_edge[u] = e;
			}
		}
	}
}

bool Relax::augment_flow(int source,int dest){

	int dest_edge,source_edge;

	for(int i = 0;i < MaxEdgeNum;i++){
		int e = G.array_Vertex_EdgesIn[dest * MaxEdgeNum + i];
		if(G.array_Edge_from[e] == -1){
			dest_edge = e;
			break;
		}
	}

	for(int i = 0;i < MaxEdgeNum;i++){
		int e = G.array_Vertex_EdgesIn[source * MaxEdgeNum + i];
		if(G.array_Edge_from[e] == -1){
			source_edge = e;
			break;
		}
	}
	int min_value = numeric_limits<int>::max();
	

	int pVertex = dest;

	int cost = 0;

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

/*
    if(!G.array_Edge_IsReversEdge[G.array_Vertex_from_edge[dest]]){
		if(cost * min_value < 0){
			//可以抵消一部分流量
			//cout << "type1 : source : " << source->id << " dest: " << dest->id << endl;
			pVertex = dest;

			G.array_Edge_x[dest_edge] -= min_value;
			G.array_Edge_x[source_edge] += min_value;
		
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
			return true;
		}
	}
	else*/{
		if(G.array_Edge_x[dest_edge] <= min_value){
				min_value = min(min_value,G.array_Edge_x[dest_edge]);
				if(cost * min_value < G.get_ServerCost(dest,min_value)){
				//cout << "type2 : source : " << source->id << " dest: " << dest->id << endl;
				//流量可以替代这个服务器
				pVertex = dest;
			
				G.array_Edge_x[dest_edge] = 0;
				G.array_Edge_x[source_edge] += min_value;
			
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
				return true;
			}
		}
	}
	
	return false;
}

bool Relax::reverse_augment_flow(int source,vector<int> deficit_set){
	/*
	vector<pair<Edge*,Edge>> undo;
	Edge* source_edge;

	for(auto e:source->EdgesIn){
		if(e->from->id == -1){
			source_edge = e;
			break;
		}
	}

	sort(deficit_set.begin(),deficit_set.end(),
	[](const Vertex* lhs,const Vertex* rhs)->bool{
		return lhs->distance < rhs->distance;
	});

	int total_cost = 0;

	for(Vertex* v:deficit_set){
		Edge* dest_edge;

		for(auto e:v->EdgesIn){
			if(e->from->id == -1){
				dest_edge = e;
				break;
			}
		}

		

		int min_value = numeric_limits<int>::max();
		

		Vertex* pVertex = v;

		int cost = 0;

		while(true){
			//cout << "id: " << pVertex->id << endl;
			const Edge *const pE = pVertex->from_edge;
			cost += pE->cost;
			if(pE->id > 0)
				min_value = min(min_value,pE->bandwidth - pE->x);
			else{
				min_value = min(min_value,pE->bandwidth);
			}
				
			pVertex = pVertex->from_edge->to;
			if(pVertex == source)
				break;
		}

		if(source_edge->x <= min_value){
			pVertex = v;

			min_value = min(min_value,source_edge->x);

			total_cost += min_value * cost;

			dest_edge->x += min_value;
			source_edge->x -= min_value;
				
			while(true){
				Edge* r = pVertex->from_edge;
				Edge* r_r = &G.E.at(-r->id);
				undo.push_back(pair<Edge*,Edge>(r,*r));
				undo.push_back(pair<Edge*,Edge>(r_r,*r_r));
				//cout << r->from->id << "=>" << r->to->id << " min: " << min_value << endl;
				if(r->id > 0){
					r->x += min_value;
					r_r->bandwidth = r->x;
				}
				else{
					r_r->x -= min_value;
					r->bandwidth = G.E.at(-r->id).x;
				}
				pVertex = pVertex->from_edge->to;
				if(pVertex == source)
					break;
			}
		}

		if(source_edge->x == 0){
			if(total_cost < G.ServerCost){
				cout << "success" << endl;
			    return true;
			}
			else{
				for(auto& p:undo){
					*p.first = p.second;
				}
				return false;
			}
		}

	}

	if(source_edge->x != 0){
		for(auto& p:undo){
			*p.first = p.second;
		}
		return false;
	}
	*/
	return false;
}

void Relax::reverse_dijkstra(int source){
	/*
	vector<Vertex*> Q;
	for(auto& v:G.V){
		v.second.distance = numeric_limits<int>::max();
		Q.push_back(&v.second);
	}
	source->distance = 0;

	size_t v_size = G.V.size();

	//set<Vertex*> excluding_set;
	vector<int> excluding_set(G.V.size(),false);
	
	for(size_t i = 0;i < v_size - 1;i++){
		int min_element_pos = -1;
		int min_value = numeric_limits<int>::max();
		for(size_t j = i;j < v_size;j++){
			if(Q[j]->distance < min_value){
				min_value = Q[j]->distance;
				min_element_pos = j;
			}
		}

		if(min_element_pos < 0)
		    return;

		swap(Q[i],Q[min_element_pos]);

		excluding_set[Q[i]->id + 1] = true;

		Vertex* v = Q[i];

		int my_distance = v->distance;

		for(auto e:v->EdgesIn){
			// for EdgesIn fix it!!
			if(excluding_set[e->from->id + 1] || e->from->id < 0)
			    continue;
			Vertex* u = e->from;
			int _cost;
			if(e->id > 0){
				_cost = e->cost;
			}
			else{
				if(e->bandwidth > 0)
				    _cost = -e->cost;
				else
				    _cost = e->cost;
			}
			if(e->bandwidth > 0 && e->bandwidth - e->x > 0 
			    && u->distance > v->distance + _cost){
				u->distance = v->distance + _cost;
				u->from_edge = e;
			}
		}
	}
	*/
}

vector<int> Relax::get_result(){
	vector<int> ret(G.VertexNum,false);
	
	//for(auto e:pesudoSource->EdgesOut){
	for(int i = 0;i < MaxEdgeNum;i++){
		int e = G.array_Vertex_EdgesOut[-1 * MaxEdgeNum + i];
		if(G.array_Edge_x[e] > 0)//has some flow
		    ret[G.array_Edge_to[e]] = true;
	}
	return ret;
}

vector<int> Relax::postOptimize(){
	/*
	cout << "starting postOptimize" << endl;
	Vertex* pesudoSource = &G.V.at(-1);
	vector<int> ret;
	for(Edge* e:pesudoSource->EdgesOut){
		Vertex* v = e->to;
		const Consumer& c = G.C.at(v->consumer_id);//assume all server at consumer nodes
		if(c.requirement < e->x - c.requirement){
			cout << "v: " << v->id <<  endl;
			//move server forward
			for(Edge* _e:v->EdgesOut){
				if(_e->x > 0){
					cout << "=> " << _e->to->id << " x " << _e->x << endl;
					ret.push_back(_e->to->id);
				}
			}
		}//if
	}
	return ret;
	*/
}

void Relax::save_topo(){
}