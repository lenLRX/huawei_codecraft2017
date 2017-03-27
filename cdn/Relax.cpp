#include "Relax.h"
#include "deploy.h"
#include <assert.h>
#include <algorithm>
#include <string>

bool Relax::optimize(){
	
	unordered_set<int> including_set;
	for(const auto& cp:G.C){
		including_set.insert(cp.second.fromVertex->id);
	}
	create_pesudo_source(including_set);

	Vertex* pesudoSource = &G.V.at(-1);

	for(auto e:pesudoSource->EdgesOut){
		e->x = -e->to->d;
	}

	vector<Vertex*> deficit_set;

	for(auto& vp:G.V){
		if(vp.second.d < 0)
		    deficit_set.push_back(&vp.second);
	}
	
	vector<Vertex*> deficit_set_original = deficit_set;

	size_t size = deficit_set.size();

	bool b = true;

    for(int id = 0;;id++){

		//cout << "cost " << G.total_cost() << endl;
		//check();

		if(id%size == 0){
			if(!b)
			    break;
			b = false;
		}

		deficit_set = deficit_set_original;

		swap(deficit_set[0],deficit_set[id % size]);

		dijkstra(deficit_set.front());

		sort(deficit_set.begin() + 1,deficit_set.end(),
		[](const Vertex* lhs,const Vertex* rhs)->bool{
			return lhs->distance < rhs->distance;
		});

		for(size_t i = 1;i < size;i++){
			Vertex* dest = deficit_set[i];
			if(dest->distance == numeric_limits<int>::max())
			    break;
			else{
				bool ret = augment_flow(deficit_set[0],dest);
				if(false&&ret){
					write_result(G.to_String().c_str(), 
					(string("./tmp/") + to_string(id) + " " 
					+ to_string(i) + string(" cost ") + to_string(G.total_cost()) + ".txt").c_str());
				}
				b = b || ret;
			}
		}



		
		for(auto it = deficit_set_original.begin();it != deficit_set_original.end();){
			int source_to_node = 0;
			Edge* source_edge;

			for(auto e:(*it)->EdgesIn){
				if(e->from->id == -1){
					source_edge = e;
					break;
				}
			}

			if(source_edge->x == 0){
				it = deficit_set_original.erase(it);
			}
			else{
				++it;
			}
		}
		
	}
	        
	return true;
}

void Relax::dijkstra(Vertex* source){

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

		for(auto e:v->EdgesOut){
			if(excluding_set[e->to->id + 1] || e->to->id < 0)
			    continue;
			Vertex* u = e->to;
			if(e->bandwidth > 0 && e->bandwidth - e->x > 0 
			    && u->distance > v->distance + e->cost){
				u->distance = v->distance + e->cost;
				u->from_edge = e;
			}
		}
	}
}

bool Relax::augment_flow(Vertex* source,Vertex* dest){

	Edge* dest_edge,*source_edge;

	for(auto e:dest->EdgesIn){
		if(e->from->id == -1){
			dest_edge = e;
			break;
		}
	}

	for(auto e:source->EdgesIn){
		if(e->from->id == -1){
			source_edge = e;
			break;
		}
	}

	int min_value = numeric_limits<int>::max();
	

	Vertex* pVertex = dest;

	int cost = 0;

	while(true){
		//cout << "id: " << pVertex->id << endl;
		const Edge *const pE = pVertex->from_edge;
		cost += pE->cost;
		if(pE->id > 0)
	        min_value = min(min_value,pE->bandwidth - pE->x);
		else
			min_value = min(min_value,pE->bandwidth);
		pVertex = pVertex->from_edge->from;
		if(pVertex == source)
		    break;
	}

    if(dest->from_edge->id < 0){
		if(cost * min_value < 0){
			//可以抵消一部分流量
			//cout << "type1 : source : " << source->id << " dest: " << dest->id << endl;
			pVertex = dest;

			dest_edge->x -= min_value;
			source_edge->x += min_value;
		
			while(true){
				Edge *const r = pVertex->from_edge;
				//cout << r->from->id << "=>" << r->to->id << " min: " << min_value << endl;
				if(r->id > 0){
					r->x += min_value;
					G.E.at(-r->id).bandwidth = r->x;
				}
				else{
					G.E.at(-r->id).x -= min_value;
					r->bandwidth = G.E.at(-r->id).x;
				}
				pVertex = pVertex->from_edge->from;
				if(pVertex == source)
					break;
			}
			return true;
		}
	}
	else{
		if(dest_edge->x <= min_value && cost * min_value < G.ServerCost){
			//cout << "type2 : source : " << source->id << " dest: " << dest->id << endl;
			//流量可以替代这个服务器
			min_value = min(min_value,dest_edge->x);
			pVertex = dest;

			dest_edge->x = 0;
			source_edge->x += min_value;
		
			while(true){
				Edge *const r = pVertex->from_edge;
				//cout << r->from->id << "=>" << r->to->id << " min: " << min_value << endl;
				if(r->id > 0){
					r->x += min_value;
					G.E.at(-r->id).bandwidth = r->x;
				}
				else{
					G.E.at(-r->id).x -= min_value;
					r->bandwidth = G.E.at(-r->id).x;
				}
				pVertex = pVertex->from_edge->from;
				if(pVertex == source)
					break;
			}
			return true;
		}
	}
	
	return false;
}

vector<int> Relax::get_result(){
	vector<int> ret(G.V.size() - 1,false);
	Vertex* pesudoSource = &G.V.at(-1);
	for(auto e:pesudoSource->EdgesOut){
		if(e->x > 0)//has some flow
		    ret[e->to->id] = true;
	}
	return ret;
}