#ifndef __NETWORKSIMPLEX_H__
#define __NETWORKSIMPLEX_H__
#include "optimizer.h"

class NetworkSimplex:public Optimizer
{
public:
    NetworkSimplex(Graph& G):Optimizer(G){}
	virtual bool optimize();
};

#endif//__NETWORKSIMPLEX_H__