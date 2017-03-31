//https://raw.githubusercontent.com/nkkosuri/Dijkstra-using-Fibonnaci-Heaps/master/dijikstra.cpp
//modified by myself

#ifndef __FIBONACCIHEAP_H__
#define __FIBONACCIHEAP_H__

#include<vector>
#include<limits>
#include"data_structures.h"

using namespace std;

/** node structure for a node in fibonacci heap */
struct fibonacci_node {
	int degree;                 //Number of children for this node.
	fibonacci_node *parent;     //Parent pointer.
	fibonacci_node *child;      //Pointer to the first child of a node.
	fibonacci_node *left;       //Pointer to the left sibling.
	fibonacci_node *right;      //Pointer to the right sibling.
	bool mark;                  //Whether the node is marked. Used for cascading cut operation.
	bool is_infinity;
	int key;                    // This corresponds to the distance from the source node.
	Vertex* node_index;             //Pointer to the node. Simplifying, we use an int-index to represent each node.
};

/** This is the fibonacci heap data structure which has min pointer
    of type fibonacci node and total nodes in the heap*/
struct fibonacci_heap {
	fibonacci_node *min_node;
	int num_nodes;
	fibonacci_heap()
	{
	    num_nodes = 0;
	    min_node = nullptr;
	}
};



/** This function inserts a new node into the fibonacci heap. Depending on the key mentioned,
    heap min pointer is changed appropriately and degree incremented by one.
*/
void fib_heap_insert(fibonacci_heap *fib_heap_obj, fibonacci_node *new_node,
		int key) {

	fibonacci_node *min_node = fib_heap_obj->min_node;
	new_node->key = key;
	new_node->degree = 0;
	new_node->parent = nullptr;
	new_node->child = nullptr;
	new_node->left = new_node;
	new_node->right = new_node;
	new_node->mark = false;
	new_node->is_infinity = false;
	//Concatenating the root list containing new_node with root list of the heap. We place the new node next to the min node in case it is not nullptr.
	if (min_node != nullptr) {
		fibonacci_node* min_left_temp = min_node->left;
		min_node->left = new_node;
		new_node->right = min_node;
		new_node->left = min_left_temp;
		min_left_temp->right = new_node;
	}

	//Updating the min pointer.
	if (min_node == nullptr || min_node->key > new_node->key) {
		fib_heap_obj->min_node = new_node;
	}

	fib_heap_obj->num_nodes = fib_heap_obj->num_nodes + 1;
}

/** This function deals with adding truncated nodes to the root list.
    mark value of the node is set to FALSE as it is equivalent to the node being insert first time. */
void fib_heap_existing_to_root(fibonacci_heap *fib_heap_obj,
		fibonacci_node *new_node) {

	fibonacci_node *min_node = fib_heap_obj->min_node;
	new_node->parent = nullptr; //Updating the parent pointer to nullptr.
	new_node->mark = false;   //Setting the mark value of the node to false.

	/*Concatenating the root list containing new_node with root list of the heap.
	We place the new node to the left of the min node in case it is not nullptr.*/
	if (min_node != nullptr) {
		fibonacci_node* min_left_temp = min_node->left;
		min_node->left = new_node;
		new_node->right = min_node;
		new_node->left = min_left_temp;
		min_left_temp->right = new_node;

		if (min_node->key > new_node->key) {
			fib_heap_obj->min_node = new_node;
		}
	} else { //Case when there are no nodes in the root list already existing.
		fib_heap_obj->min_node = new_node;
		new_node->right = new_node;
		new_node->left = new_node;
	}
}

/** This function creates an empty heap and initializes its elements
    It is used only when we are creating a new f-heap */
fibonacci_heap *fib_heap_make() {

	fibonacci_heap *new_fib_heap = new fibonacci_heap;
	new_fib_heap->num_nodes = 0;
	new_fib_heap->min_node = nullptr;
	return new_fib_heap;

}

/** This function combines two fibonacci heaps. This function isn't required for Dijkstra implementation.*/
fibonacci_heap *fib_heap_union(fibonacci_heap *fib_heap_first,
		fibonacci_heap *fib_heap_second) {

	fibonacci_heap *appended_fib_heap = fib_heap_make();
	fibonacci_node *first_heap_min_node = fib_heap_first->min_node;
	fibonacci_node *second_heap_min_node = fib_heap_second->min_node;

	if (fib_heap_second->min_node == nullptr) {
		appended_fib_heap->min_node = first_heap_min_node;
	} else if (fib_heap_first->min_node == nullptr) {
		appended_fib_heap->min_node = second_heap_min_node;
	} else { //When both are not nullptr
		//Concatenating the root list of H2 with the root list of H
		fibonacci_node *first_heap_min_node_left_temp =
				first_heap_min_node->left;
		fibonacci_node *second_heap_min_node_left_temp =
				second_heap_min_node->left;
		first_heap_min_node->left = second_heap_min_node_left_temp;
		second_heap_min_node->left = first_heap_min_node_left_temp;
		first_heap_min_node_left_temp->right = second_heap_min_node;
		second_heap_min_node_left_temp->right = first_heap_min_node;

		if (second_heap_min_node->key < first_heap_min_node->key) {
			appended_fib_heap->min_node = second_heap_min_node;
		} else {
			appended_fib_heap->min_node = first_heap_min_node;
		}
	}

	appended_fib_heap->num_nodes = fib_heap_first->num_nodes
			+ fib_heap_second->num_nodes;
	return appended_fib_heap;
}

/** This function adds the new_child_node to the parent_node child list. Degree of the parent in incremented
    by one as well to reflect to the node*/
void fib_heap_add_child(fibonacci_node *parent_node,
		fibonacci_node *new_child_node) {

	if (parent_node->degree == 0) {
		parent_node->child = new_child_node;
		new_child_node->right = new_child_node;
		new_child_node->left = new_child_node;
		new_child_node->parent = parent_node;
	} else {
		fibonacci_node* first_child = parent_node->child;
		fibonacci_node* first_child_left_temp = first_child->left;
		first_child->left = new_child_node;
		new_child_node->right = first_child;
		new_child_node->left = first_child_left_temp;
		first_child_left_temp->right = new_child_node;
	}
	new_child_node->parent = parent_node;
	parent_node->degree = parent_node->degree + 1;
}

/** This function is used to truncate a child node from a sibling list. It could be a root list as well. */
void fib_heap_remove_node_from_root(fibonacci_node *node) {

    //if chld has siblings,then remove it from sibling list by traversing
	if (node->right != node) {
		node->right->left = node->left;
		node->left->right = node->right;
	}

    /*if the node being removed is the child node of parent, we have to set
	 some other sibling of child as child for parent.*/
	if (node->parent != nullptr) {
		int parent_degree = node->parent->degree;
		if (parent_degree == 1) {
            // if its the only child then parent has no more children. so set to nullptr.
			node->parent->child = nullptr;
		} else {
			node->parent->child = node->right;
		}
		//Parent's degree is updated from truncating the child node.
		node->parent->degree = node->parent->degree - 1;
	}
}

/** This function links two nodes as part of the consolidation operation.
    First the larger node is sliced from its sibling list and is then
    added as a child to the smaller node
    */
void fib_heap_link(fibonacci_heap *heap_inst, fibonacci_node *high_node,
		fibonacci_node *low_node) {

	fib_heap_remove_node_from_root(high_node);
	fib_heap_add_child(low_node, high_node);
	//mark value of the larger node is marked false.
	high_node->mark = false;

}

/** This method is the crucial method for fibonacci heaps where all the actual time
    is spent in pairwise merging of all the trees, to ensure at the end of the operation
    there are no two trees with the same degree.
    we scan the root list with the help of min pointer in the heap and combine trees with
    same degree into a single tree. We do this with the help of a auxillary table to see if
    there is a tree with degree already existing in our heap.
*/
void fib_heap_consolidate(fibonacci_heap *heap_inst) {

	int node_degree;
	int count = 0, root_count = 0; //root_count is used to count the number of nodes in the root list.

	if (heap_inst->num_nodes > 1) { //When the number of nodes is less then 1, consolidate makes no sense.
		int degree_table_size = heap_inst->num_nodes;
		vector<fibonacci_node*> degree_table; //This is the table via which the degrees are compared for consolidation.
		fibonacci_node *current_node = heap_inst->min_node, *start_node =
				heap_inst->min_node;
		fibonacci_node *existing_node_degree_array, *current_consolidating_node;

		fibonacci_node *temp_node = heap_inst->min_node, *iterating_node =
				heap_inst->min_node;
        //Calculating the number of nodes in the root list.
		do {
			root_count++;
			iterating_node = iterating_node->right;
			cout << temp_node->node_index->id << " " << iterating_node->node_index->id << endl;
		} while (iterating_node != temp_node);

		while (count < root_count) {
			current_consolidating_node = current_node;
			current_node = current_node->right; // This actually means node for next iternation.
			node_degree = current_consolidating_node->degree;
			while (true) { //We traverse the table each time until all same degree nodes are merged.
				while (node_degree >= degree_table.size()) {
					degree_table.push_back(nullptr); //This is a way by which we avoid segmentation fault for accessing a index in the degree table.
				}
				if (degree_table[node_degree] == nullptr) { //Adding the current node to the degree table since empty.
					degree_table[node_degree] = current_consolidating_node;
					break;
				} else {//When there is already a node existing with the same degree
					existing_node_degree_array = degree_table[node_degree];

					if (current_consolidating_node->key
							> existing_node_degree_array->key) {

						//swapping the nodes.
						fibonacci_node * temp_node = current_consolidating_node;
						current_consolidating_node = existing_node_degree_array;
						existing_node_degree_array = temp_node;
					}
					if (existing_node_degree_array
							== current_consolidating_node)break;
                    //Linking the larger of the nodes to the smaller one.
					fib_heap_link(heap_inst, existing_node_degree_array,
							current_consolidating_node);
                    //Making the current degree index as nullptr as it is incremented now.
					degree_table[node_degree] = nullptr;
					node_degree++;
				}
			}
			count++;
		}

        //Adding the different degree nodes back to the root list.
		heap_inst->min_node = nullptr;
		for (int i = 0; i < degree_table.size(); i++) {
			if (degree_table[i] != nullptr) {
				fib_heap_existing_to_root(heap_inst, degree_table[i]);
			}
		}
	}
}

/** This function extracts the minimum value from the heap based on the min_node pointer that
    each heap structure maintains. In order to verify the correctness of the fibonacci implementation
    with the array one, I am writing out each extracted min node to a file for making the comparision
    easier.
*/
fibonacci_node *fib_heap_extract_min(fibonacci_heap *heap_inst) {

	fibonacci_node *min_node = heap_inst->min_node;

	if (min_node != nullptr) {
		//Add each child in the extracted node to the root list.
		int degree = min_node->degree;
		fibonacci_node *current_child = min_node->child;
		fibonacci_node *removed_child;
		int count = 0; //Count for the children being added to the root list.

        //Iterate till all the children nodes are added to the root list.
		while (count < degree) {
			removed_child = current_child;
			current_child = current_child->right;
			fib_heap_existing_to_root(heap_inst, removed_child);
			count++;
		}

        //Removing the extracted node from the root list.
		fib_heap_remove_node_from_root(min_node);
		//Decrementing the number of nodes as a node has been removed.
		heap_inst->num_nodes = heap_inst->num_nodes - 1;
		if (heap_inst->num_nodes == 0) { //only one node at the root level.
			heap_inst->min_node = nullptr;
		} else { //More than one node at the root level.
			heap_inst->min_node = min_node->right; //It may not be the real min node.
			//Removing from the root list.
			fibonacci_node *min_node_left_temp = min_node->left;
			heap_inst->min_node->left = min_node_left_temp;
			min_node_left_temp->right = heap_inst->min_node;
			//Once
			fib_heap_consolidate(heap_inst);
		}
	}
	return min_node;
}

/** This method removes the child node from its parent in the heap.*/
void fib_heap_cut(fibonacci_heap *heap_inst, fibonacci_node *node,
		fibonacci_node *node_parent) {

	fib_heap_remove_node_from_root(node);
	fib_heap_existing_to_root(heap_inst, node);

}

/** This recursive function removes the nodes from heap if the child mark
    values are true. It goes from child to parent until it sees a parent
    whose child mark value is false. It sets the child mark of last parent
    as true since it just lost a child.*/
void fib_heap_cascading_cut(fibonacci_heap *heap_inst, fibonacci_node *node) {

	fibonacci_node *parent_node = node->parent;
	if (parent_node != nullptr) {
        // case when parent is having child mark false
		if (node->mark == false) {
			node->mark = true;
		} else {
		    // remove this node and recurse up the path until mark is false
			fib_heap_cut(heap_inst, node, parent_node);
			fib_heap_cascading_cut(heap_inst, parent_node);
		}
	}

}

/** This method sets the key field of node to amount specified as argument.
    Based on the new value it may be removed from the parent and called for
    cut and cascade methods.*/
void fib_heap_decrease_key(fibonacci_heap *heap_inst, fibonacci_node *node_inst,
		int new_key) {

	int old_key = node_inst->key;

	if (new_key > old_key) {
		return;
	}

	node_inst->key = new_key;
	if (node_inst->parent != nullptr) {
	    /*  new weight is less than parent weight, so it must be cut
            from the parent*/
		if (node_inst->key < node_inst->parent->key) {
			fibonacci_node *parent_node = node_inst->parent;
			fib_heap_cut(heap_inst, node_inst, node_inst->parent);
			fib_heap_cascading_cut(heap_inst, parent_node);
		}
	}

    //Updating the heap's min pointer appropriately.
	if (node_inst->key < heap_inst->min_node->key) {
		heap_inst->min_node = node_inst;
	}

}

/** This function can be used to delete a key from the heap. This is
    achieved by decreasing the key value to the minimum value available
    in the data type being used. This function is not in use for our current
    implementation.
    */
void fib_heap_delete(fibonacci_heap *heap_inst, fibonacci_node *node) {
	fib_heap_decrease_key(heap_inst, node, numeric_limits<int>::min());
	fib_heap_extract_min(heap_inst);
}


Vertex* get_min_distant_unmarked_node_fib_heap(Graph& graph_obj,
		fibonacci_heap *heap, vector<fibonacci_node*>& node_array) {

	int min_distance = numeric_limits<int>::max();
	//Extracting the min distant node.
	fibonacci_node *min_node = fib_heap_extract_min(heap);

	Vertex* v = nullptr;

	if (min_node != nullptr) {
		v = min_node->node_index;
		//Updating the distances for the adjacent vertices of min distant unmarked node.
		for(auto e:v->EdgesOut){
			//if(excluding_set[e->to->id + 1])
			//    continue;
			Vertex* u = e->to;
			if(e->bandwidth > 0 && e->bandwidth - e->x > 0 
			    && u->distance > v->distance + e->cost){
				u->distance = v->distance + e->cost;
				u->from_edge = e;
				fib_heap_decrease_key(heap, node_array[u->id + 1],
							u->distance);
			}
		}
	}
	return v;
}

#endif//__FIBONACCIHEAP_H__