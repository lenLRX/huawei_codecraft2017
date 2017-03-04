#ifndef __DATA_STRUCTURE_H__
#define __DATA_STRUCTURE_H__

#include <unordered_map>
#include <unordered_set>

#include <iostream>

using namespace std;

class Graph;

class Vertex
{
public:
    int id;
	unordered_set<int> EdgesOut;
	unordered_set<int> EdgesIn;

    Vertex(int id):id(id){}
	Vertex(){}
};

class Consumer
{
public:
    int id;
	int fromVertex;
	int requirement;

    Consumer(const int id,const int fromVertex,const int requirement)
:id(id),fromVertex(fromVertex),requirement(requirement)
	{}

	Consumer(){}
};

class Edge
{
public:
    int id;
    int from;
	int to;
	int bandwidth;
	int current_bandwidth;
	int cost;

    Edge(int id,
        int from,
	    int to,
	    int bandwidth,
	    int cost):
		id(id),from(from),to(to),bandwidth(bandwidth),cost(cost)
		{}
	Edge(){}
};

class Graph
{
public:
    Graph() = default;
    void add_Edge(int id,
        int from,
	    int to,
	    int bandwidth,
	    int cost){
		E[id] = Edge(id,from,to,bandwidth,cost);
	}

	void add_Vertex(int id){
		V[id] = Vertex(id);
	}

	void add_Consumer(const int id,const int fromVertex,const int requirement){
		C[id] = Consumer(id,fromVertex,requirement);
	}

	void VertexCompletement(){
		for(auto& p:E){
			auto& v_from = GetVertex(p.second.from);
			v_from.EdgesOut.insert(p.first);
			auto& v_to = GetVertex(p.second.to);
			v_to.EdgesIn.insert(p.first);
		}

		for(auto& cp:C){
			GetVertex(cp.second.fromVertex);
		}
	}

	Vertex& GetVertex(int id){//add if not exist
	    if(V.find(id) == V.end())
		    add_Vertex(id);
		return V[id];
	}

	void debug_print(){
		cout << V.size() << " " << E.size() / 2 << " " << C.size() << endl;
	}

    unordered_map<int,Edge> E;
	unordered_map<int,Vertex> V;
	unordered_map<int,Consumer> C;
	int EdgeNum;
	int VertexNum;
	int ConsumerNum;
	int ServerCost;
};

#endif//__DATA_STRUCTURE_H__