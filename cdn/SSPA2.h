#ifndef __SSPA2_H__
#define __SSPA2_H__

#include "optimizer.h"

class SSPA2:public Optimizer{
public:
	SSPA2(Graph& G):Optimizer(G){}
	virtual int optimize();

	void dijkstra(int source);
	int augment_flow(int dest);
	int counter = 0;
};

#endif//__SSPA2_H__