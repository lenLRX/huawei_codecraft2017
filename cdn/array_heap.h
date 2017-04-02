#ifndef __ARRAY_HEAP_H__
#define __ARRAY_HEAP_H__

class array_heap{
public:
    array_heap(int capacity):capacity(capacity){
		array = new int[capacity];
		int size = capacity;
	}

	~array_heap(){
		delete[] array;
	}

    int size;
    int capacity;
    int* array;
};

#endif//__ARRAY_HEAP_H__