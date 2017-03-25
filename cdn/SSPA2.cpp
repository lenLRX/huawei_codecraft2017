#include "SSPA2.h"
#include <assert.h>
#include <algorithm>

bool SSPA2::optimize(){
	Vertex* pesudoSource = &G.V.at(-1);
	vector<Vertex*> deficit_set;

	for(auto& vp:G.V){
		if(vp.second.d < 0)
		    deficit_set.push_back(&vp.second);
	}

    while(true){
		if(deficit_set.size() == 0)
		    break;//done!
		
		

		dijkstra(pesudoSource);

		sort(deficit_set.begin(),deficit_set.end(),
		[](const Vertex* lhs,const Vertex* rhs)->bool{
			return lhs->distance < rhs->distance;
		});

		if(deficit_set[0]->distance == numeric_limits<int>::max())
		    return false;//no solution

		for(Vertex* dest:deficit_set){
			if(dest->distance == numeric_limits<int>::max())
			    break;
			else{
				augment_flow(dest);
			}
		}
        
		for(auto it = deficit_set.begin();it != deficit_set.end();){
			if((*it)->d == 0){
				it = deficit_set.erase(it);
			}
			else{
				++it;
			}
		}
	}
	        
	return true;
}

void SSPA2::dijkstra(Vertex* source){

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
		    return ;

		swap(Q[i],Q[min_element_pos]);

		excluding_set[Q[i]->id + 1] = true;

		Vertex* v = Q[i];

		int my_distance = v->distance;

		for(auto e:v->EdgesOut){
			if(excluding_set[e->to->id + 1])
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

void SSPA2::augment_flow(Vertex* dest){
	Vertex* pesudoSource = &G.V.at(-1);
	int min_value = numeric_limits<int>::max();
	min_value = min(min_value,pesudoSource->d);
	min_value = min(min_value,-dest->d);

	Vertex* pVertex = dest;

	while(true){
		const Edge *const pE = pVertex->from_edge;
		if(pE->id > 0)
	        min_value = min(min_value,pE->bandwidth - pE->x);
		else
			min_value = min(min_value,pE->bandwidth);
		pVertex = pVertex->from_edge->from;
		if(pVertex == pesudoSource)
		    break;
	}

	pesudoSource->d -= min_value;
	dest->d += min_value;

	pVertex = dest;
	
	while(true){
		Edge *const r = pVertex->from_edge;
		if(r->id > 0){
			r->x += min_value;
			G.E.at(-r->id).bandwidth = r->x;
		}
		else{
			G.E.at(-r->id).x -= min_value;
			r->bandwidth = G.E.at(-r->id).x;
		}
		pVertex = pVertex->from_edge->from;
		if(pVertex == pesudoSource)
		    break;
	}
}