#include "optimizer.h"

void Optimizer::create_pesudo_source(unordered_set<int> ExcludingVertex){
	int pesudo_source_id = -1;
	Vertex pesudo_source(pesudo_source_id);

	G.V[pesudo_source_id] = pesudo_source;

	auto p_pesudo_source = &G.V[pesudo_source_id];

	int sum = 0;

	for(auto& pv:G.V){
		if(pv.first == -1)
		    continue;
		sum += pv.second.d;
		//cout << pv.first << " : " << pv.second.d << endl;
		if(ExcludingVertex.count(pv.first) == 0)
		    continue;
		//cout << pv.first << endl;
		Edge e;
		e.id = G.GetAnID();
		e.from = p_pesudo_source;
		e.to = &pv.second;
		e.bandwidth = numeric_limits<int>::max();
		e.cost = 0;
		//e.cost = 0;
		//e.cost = G.ServerCost;
		e.x = 0;

		//cout << pv.first << " : " << e.cost << endl;

		Edge ResidualEdge;
		ResidualEdge.id = -e.id;
		ResidualEdge.from = e.to;
		ResidualEdge.to = e.from;
		ResidualEdge.bandwidth = e.bandwidth;
		ResidualEdge.cost = -e.cost;
		ResidualEdge.x = 0;
		e.ResidualEdgeNo = -e.id;

		G.E[e.id] = e;
		auto p_e = &G.E[e.id];
		G.E[-e.id] = ResidualEdge;
		auto p_ResidualEdge = &G.E[-e.id];
		pv.second.EdgesIn.push_back(p_e);
		pv.second.EdgesOut.push_back(p_ResidualEdge);
		p_pesudo_source->EdgesOut.push_back(p_e);
		p_pesudo_source->EdgesIn.push_back(p_ResidualEdge);

		//cout << "e.id " << e.id << endl;

		
	}

	p_pesudo_source->d = -sum;

	//cout << "pesudo_source.d " << pesudo_source.d << endl;

	
}

void Optimizer::refresh(){
		//G = OriginalGraph;
		vector<int> EdgesToErase;

		for(auto& ep:G.E){
			if(ep.second.from->id == -1){
				EdgesToErase.push_back(ep.first);
			}
			else{
				ep.second.x = 0;
				ep.second.visited = 0;
				if(ep.second.id < 0){
					ep.second.bandwidth = 0;
				}
			}
		}

		for(auto& vp:G.V){
			for(auto it = vp.second.EdgesIn.begin(); it != vp.second.EdgesIn.end();){
				if((*it)->from->id == -1){
					it = vp.second.EdgesIn.erase(it);
				}
				else
				    it++;
			}

			for(auto it = vp.second.EdgesOut.begin(); it != vp.second.EdgesOut.end();){
				if((*it)->to->id == -1){
					it = vp.second.EdgesOut.erase(it);
				}
				else
				    it++;
			}
			vp.second.pi = 0;
			vp.second.d = 0;
			if(vp.second.consumer_id >= 0){
				vp.second.d = - G.C.at(vp.second.consumer_id).requirement;
			}
		}

		G.V.erase(-1);

		for(int k:EdgesToErase){
			G.E.erase(k);
		}

		for(auto& cp:G.C){
			cp.second.remaining_requirement = cp.second.requirement;
		}
	}

void Optimizer::check(){
	for(auto& vp:G.V){
		int sum = 0;
		for(auto edge_no:vp.second.EdgesIn){
			if(edge_no->id > 0)
				sum += edge_no->x;
		}

		for(auto edge_no:vp.second.EdgesOut){
			if(edge_no->id > 0)
				sum -= edge_no->x;
		}

		if(sum != 0){
			cout << "ID : " << vp.first << " Vertex sum " << sum << endl;
		}

		if(vp.second.d !=0 )
			cout << "d: " << vp.second.d << endl;
		
		if(vp.second.d + sum != 0){
			cout << "ID : " << vp.first << "unbalanced" << endl;
		}
	}
}