#ifndef __LAGRANGIAN_RELAXATION_H__
#define __LAGRANGIAN_RELAXATION_H__

#include <unordered_map>
#include <unordered_set>
#include <set>
using namespace std;

#include "data_structures.h"

class LagrangianRelaxation{
public:
    LagrangianRelaxation(Graph& G):G(G){
		for(auto& vp:G.V){
			pesudoCost[vp.first] = 0;
		}
	}
    Graph& G;
	//Graph OriginalGraph;
	unordered_map<int,int> pesudoCost;

	void updatePesudoCost(){
		/*
		for(auto& pc:pesudoCost){
			pesudoCost[pc.first] = 0;
		}
		*/
		for(auto pesudo_source_out:G.V.at(-1).EdgesOut){
			if(pesudo_source_out->x > 0){//has some flow
			    pesudoCost[pesudo_source_out->to->id] = G.ServerCost / pesudo_source_out->x;
			}
		}
	}

	bool optimize(bool patrial = false);

	/*
	STEP1:如果网络中不含有任何盈余节点和亏空节点，则已经得到最优解；否则在残量网络中选择一个盈余节点s，令S={s}，继续下一步
	*/
	//void step1();

    /*
	STEP2:如果e(S)>r(pi,S),则转STEP3；否则在(S,S')中选取一条满足Cij_pi=0的弧(i,j)，若e(j)<0，则转STEP4；否则令pred(j) = i,S = S U {j},继续STEP2.
	*/
	//void step2();

	/*
	STEP3:首先沿(S,S')中所有满足Cij_pi=0的弧(i,j)增广流量使之饱和，然后将S中的每个节点上的势增加a=min{Cij_pi|(i,j)属于(S,S'),rij > 0}转STEP1
	*/
	//void step3();

    /*
	STEP4:根据pred中记录的节点，确定子树中从s到j的一条增广路P。沿P增广流量delta=min{e(s),-e(j),min{rij|(i,j)属于P}}。转STEP1
	*/
	//void step4();

	int e(set<Vertex*>& S);
	vector<Edge*> get_rij(set<Vertex*>& S); 
	int r_pi(vector<Edge*>& R_ij);
	int C_ij_pi(Vertex* i,Vertex* j,Edge* edge);
	vector<Edge*> dijkstra(Vertex* source,Vertex* dest);

	void dijkstraPrepare();
	void _dijkstraPrepare(int source);

	void refresh(){
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

	void create_pesudo_source(unordered_set<int> ExcludingVertex = unordered_set<int>());
	void rand_a_source();
	void check();
};

typedef LagrangianRelaxation LR;

#endif//__LAGRANGIAN_RELAXATION_H__