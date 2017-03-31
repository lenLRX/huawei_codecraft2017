#ifndef __ZKW_H__
#define __ZKW_H__

#include "optimizer.h"

class zkw:public Optimizer{
public:
	zkw(Graph& G):Optimizer(G){}
	virtual bool optimize();

	int aug();
	bool modlabel();
};

#endif//__ZKW_H__