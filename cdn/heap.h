#ifndef __HEAP_H__
#define __HEAP_H__

#include<vector>
#include<algorithm>

#include"data_structures.h"

using namespace std;


template<class T,class cmp>
class heap{
public:
    heap(cmp cmp_fn):cmp_fn(cmp_fn){}
	inline void push(T element){
		container.push_back(element);
		push_heap(container.begin(),container.end(),cmp_fn);
	}

	inline void reheapify(){
        make_heap(container.begin(),container.end(),
		cmp_fn);
	}

	inline void pop(){
		pop_heap(container.begin(),container.end(),cmp_fn);
		container.pop_back();
	}

	inline T top(){
		return container.front();
	}

	inline bool empty(){
		return container.empty();
	}
    cmp cmp_fn;
    vector<T> container;
};

#endif//__HEAP_H__