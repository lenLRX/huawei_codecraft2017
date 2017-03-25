#include "SPFA.h"
#include <queue>
#include <assert.h>
#include <algorithm>

bool SPFA::optimize(){
	Vertex* pesudoSource = &G.V.at(-1);
	vector<Vertex*> deficit_set;

	for(auto& vp:G.V){
		if(vp.second.d < 0)
		    deficit_set.push_back(&vp.second);
	}

    while(true){
		if(deficit_set.size() == 0)
		    break;//done!
		
		int min_value = numeric_limits<int>::max();

		Vertex* dest = nullptr;
		vector<Edge*> path = spfa(pesudoSource,dest);
		if(path.size() == 0)
			return false;
		
		min_value = min(min_value,pesudoSource->d);
		min_value = min(min_value,-dest->d);

		for(auto pE:path){
			if(pE->id > 0)
			    min_value = min(min_value,pE->bandwidth - pE->x);
		    else
			    min_value = min(min_value,pE->bandwidth);
		}

		pesudoSource->d -= min_value;
		dest->d += min_value;

		for(auto r:path){
			if(r->id > 0){
				r->x += min_value;
			    G.E.at(-r->id).bandwidth = r->x;
			}
			else{
				G.E.at(-r->id).x -= min_value;
				r->bandwidth = G.E.at(-r->id).x;
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

vector<Edge*> SPFA::spfa(Vertex* source,Vertex*& dest){
	vector<Edge*> ret;

	queue<Vertex*> Q;
	for(auto& v:G.V){
		v.second.distance = numeric_limits<int>::max();
	}
	source->distance = 0;

	size_t v_size = G.V.size();

	Q.push(source);

	//set<Vertex*> excluding_set;
	vector<int> excluding_set(G.V.size(),false);
	vector<int> count(G.V.size(),false);
	/*
	while(true){
		if(Q.empty())
		    break;
		Vertex* now = Q.front();
		Q.pop();

		excluding_set[now->id + 1] = true;



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

	if(dest == nullptr){
		return vector<Edge*>();
	}
	

	Vertex* pVertex = dest;

	//cout << pVertex->distance << endl;

	unordered_set<Vertex*> visited_set;

    //back trace
	while(true){
		
		if(visited_set.count(pVertex)){
			ret.clear();
			break;
		}
		    
		visited_set.insert(pVertex);

		ret.push_back(pVertex->from_edge);
		pVertex = pVertex->from_edge->from;
		if(pVertex == source)
		    break;
	}

	reverse(ret.begin(),ret.end());

	return ret;
	*/
}