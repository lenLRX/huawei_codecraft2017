#ifndef __ARRAY_HEAP_H__
#define __ARRAY_HEAP_H__

#include "data_structures.h"

class array_heap{
public:
    array_heap(int capacity,Graph& G):G(G),capacity(capacity){
		array = new int[capacity];
		raw_arr_idx = new int[capacity];
		arr_idx = raw_arr_idx + 1;
		size = capacity;
		for(int i = 0;i < capacity;i++){
			array[i] = i - 1;
			arr_idx[i - 1] = i;
		}
	}

	int extract_min() {
		int ret = array[0];
		if(size > 1){
			array[0] = array[size - 1];
			arr_idx[array[0]] = -100;
			arr_idx[array[size - 1]] = 0;
			size--;
			heapify(0);
		}
		else{
			size--;
		}
		
		return ret;
	}


	void decrease_key(int key) {
		int i = arr_idx[key];
		while (i > 0) {
			if (G.mem.array_Vertex_distance[array[parent(i)]] 
			    > G.mem.array_Vertex_distance[array[i]]) {
				swap_with_parent(i);
				i = parent(i);
			} else {
				break;
			}
		}
	}

	inline int left(int i) const {
		return 2 * i + 1;
	}

	inline int right(int i) const {
		return 2 * i + 2;
	}

	inline int parent(int i) const {
		return (i - 1) / 2;
	}

	void heapify(int i) {
		int n = size;

		while (i < n) {
			int l = left(i);
			int r = right(i);
			int m = i;

			if (l < n && 
			    G.mem.array_Vertex_distance[array[l]] 
				< G.mem.array_Vertex_distance[array[m]]) {
				m = l;
			}

			if (r < n && G.mem.array_Vertex_distance[array[r]]
			 < G.mem.array_Vertex_distance[array[m]]) {
				m = r;
			}

			if (m != i) {
				swap_with_parent(m);
				i = m;
			} else {
				break;
			}
		}
	}

	void swap_with_parent(int i) {
		int pi = parent(i);

		int tmp = array[i];

		array[i] = array[pi];

		array[pi] = tmp;

		arr_idx[array[i]] = i;
		arr_idx[array[pi]] = pi;
	}

	inline int getmin(){
		return array[0];
	}

	~array_heap(){
		delete[] array;
		delete[] raw_arr_idx;
	}

    Graph& G;
    int size;
    int capacity;
	int* raw_arr_idx;
	int* arr_idx;
    int* array;
};

#endif//__ARRAY_HEAP_H__