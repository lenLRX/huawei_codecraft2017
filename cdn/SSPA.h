#ifndef __SSPA_H__
#define __SSPA_H__

#include "optimizer.h"

class SSPA:public Optimizer{
public:
	SSPA(Graph& G):Optimizer(G){}
	virtual bool optimize();

	vector<Edge*> dijkstra(Vertex* source,Vertex*& dest);
};

#endif//__SSPA_H__