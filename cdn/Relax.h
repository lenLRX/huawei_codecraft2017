#ifndef __RELAX_H__
#define __RELAX_H__

#include "optimizer.h"

class Relax:public Optimizer
{
public:
    Relax(Graph& G):Optimizer(G){}
    virtual bool optimize();
	void save_topo();
	void dijkstra(int source);
	bool augment_flow(int source,int dest);
	void reverse_dijkstra(int source);
	bool reverse_augment_flow(int source,vector<int> deficit_set);
	vector<int> get_result();

	vector<int> postOptimize();

	int cost_sum;
};

#endif//__RELAX_H__