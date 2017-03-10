#ifndef __LAGRANGIAN_RELAXATION_H__
#define __LAGRANGIAN_RELAXATION_H__

#include <unordered_map>
#include <unordered_set>
using namespace std;

#include "data_structures.h"

class LagrangianRelaxation{
public:
    LagrangianRelaxation(Graph G):OriginalGraph(G){
	}
    Graph G;
	Graph OriginalGraph;

	bool optimize();

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

	int e(unordered_set<Vertex*>& S);
	unordered_set<Edge*> get_rij(unordered_set<Vertex*>& S); 
	int r_pi(unordered_set<Edge*>& R_ij);
	int C_ij_pi(int i,int j,Edge* edge);
	vector<Edge*> dijkstra(int source,int dest);

	void refresh(){
		G = OriginalGraph;
	}

	void create_pesudo_source(unordered_set<int> ExcludingVertex = unordered_set<int>());
	void rand_a_source();
	void check();
};

typedef LagrangianRelaxation LR;

#endif//__LAGRANGIAN_RELAXATION_H__