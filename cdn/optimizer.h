#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__

#include <unordered_map>
#include <unordered_set>
#include <set>
using namespace std;

#include "data_structures.h"

class Optimizer{
public:
    Optimizer(Graph& G):G(G){}
    Graph& G;
    virtual int optimize() = 0;

	void create_pesudo_source(unordered_set<int> 
	    includingVertex = unordered_set<int>());
	
	void legacy_create_pesudo_source(unordered_set<int> 
	    includingVertex = unordered_set<int>());

	void check();
};

#endif//__OPTIMIZER_H__