#ifndef __SPFA_H__
#define __SPFA_H__
#include "optimizer.h"
class SPFA:public Optimizer
{
	SPFA(Graph& G):Optimizer(G){}
	virtual bool optimize();

	vector<Edge*> spfa(Vertex* source,Vertex*& dest);
};
#endif//__SPFA_H__