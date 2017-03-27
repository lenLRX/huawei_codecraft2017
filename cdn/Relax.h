#ifndef __RELAX_H__
#define __RELAX_H__

#include "optimizer.h"

class Relax:public Optimizer
{
public:
    Relax(Graph& G):Optimizer(G){}
    virtual bool optimize();
	void dijkstra(Vertex* source);
	bool augment_flow(Vertex* source,Vertex* dest);
	vector<int> get_result();
};

#endif//__RELAX_H__