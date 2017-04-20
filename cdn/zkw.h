#ifndef __ZKW_H__
#define __ZKW_H__

#include "optimizer.h"

class zkw:public Optimizer{
public:
	zkw(Graph& G):Optimizer(G){
		v_accessed = raw_v_accessed + 1;
	}
	virtual int optimize();

	int aug(int no,int m);
	bool modifylabel();
	int raw_v_accessed[1201];
	int* v_accessed; 
	int n,m,pi=0,cost=0;
	int counter = 0;
};

#endif//__ZKW_H__