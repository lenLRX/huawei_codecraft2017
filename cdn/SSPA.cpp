#include "SSPA.h"
#include <assert.h>
#include <algorithm>

bool SSPA::optimize(){
	Vertex* pesudoSource = &G.V.at(-1);
	vector<Vertex*> deficit_set;
	vector<int> minvec;

	for(auto& vp:G.V){
		if(vp.second.d < 0)
		    deficit_set.push_back(&vp.second);
	}

    while(true){
		if(deficit_set.size() == 0)
		    break;//done!

		minvec.clear();
		Vertex* dest = nullptr;
		vector<Edge*> path = dijkstra(pesudoSource,dest);
		if(path.size() == 0)
			return false;
		minvec.push_back(pesudoSource->d);
		minvec.push_back(-dest->d);

		for(auto pE:path){
			if(pE->id > 0)
			    minvec.push_back(pE->bandwidth - pE->x);
		    else
			    minvec.push_back(pE->bandwidth);
		}

		int a = *min_element(minvec.begin(),minvec.end());
		pesudoSource->d -= a;
		dest->d += a;

		for(auto r:path){
			if(r->id > 0){
				r->x += a;
			    G.E.at(-r->id).bandwidth = r->x;
				if(r->from->id == -1){
					r->cost = 0;
				}
			}
			else{
				G.E.at(-r->id).x -= a;
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

vector<Edge*> SSPA::dijkstra(Vertex* source,Vertex*& dest){
	vector<Edge*> ret;
	auto cmp = [this](int lhs,int rhs)->bool{
		return this->G.V.at(lhs).distance < this->G.V.at(rhs).distance;
	};

	vector<Vertex*> Q;
	for(auto& v:G.V){
		v.second.distance = numeric_limits<int>::max();
		Q.push_back(&v.second);
	}
	source->distance = 0;

	size_t v_size = G.V.size();

	set<Vertex*> excluding_set;
	
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
		    return vector<Edge*>();

		swap(Q[i],Q[min_element_pos]);

		excluding_set.insert(Q[i]);

		if(Q[i]->d < 0){
			dest = Q[i];
			assert(dest != nullptr);
			break;//early stop
		}

		Vertex* v = Q[i];

		int my_distance = v->distance;

		for(auto e:v->EdgesOut){
			if(excluding_set.count(e->to))
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
}