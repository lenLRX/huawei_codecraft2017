#ifndef __DATA_STRUCTURE_H__
#define __DATA_STRUCTURE_H__

#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <cstdlib>
#include <cstring>

//#define LEN_DBG

const int MaxEdgeNum = 1000 * 2 + 1;//21 * 2;//reverse edge
const int MaxVertexNum = 1000;//+1 for pesudo vertex
const int MaxConsumerNum = 1000;
const int MaxServerLvlNum = 10;

using namespace std;

class Graph;
class Edge;//forward decl

class Vertex
{
public:
    
    int id;
	int pi;//
	int d;//current resource
	int consumer_id;
	vector<Edge*> EdgesOut;
	vector<Edge*> EdgesIn;
	int distance;
	int weight;
	Edge* from_edge;

    Vertex(int id):id(id),
	pi(0),d(0),consumer_id(-1),distance(-1),weight(0),from_edge(nullptr){}
	Vertex():id(0),
	pi(0),d(0),consumer_id(-1),distance(-1),weight(0),from_edge(nullptr){}
};



class Consumer
{
public:
    int id;
	Vertex* fromVertex;
	int requirement;
	int remaining_requirement;

    Consumer(const int id,Vertex* fromVertex,const int requirement)
:id(id),fromVertex(fromVertex),requirement(requirement),
remaining_requirement(requirement)
	{}

	Consumer(){}
};


class Edge
{
public:
    int id;
    Vertex* from;
	Vertex* to;
	int bandwidth;
	int cost;
	int x;//流量
	int visited;
	int ResidualEdgeNo;

    Edge(int id,
        Vertex* from,
	    Vertex* to,
	    int bandwidth,
	    int cost,
		int ResidualEdgeNo = 0):
		id(id),from(from),to(to),bandwidth(bandwidth),cost(cost),
		x(0),visited(0),ResidualEdgeNo(ResidualEdgeNo)
		{}
	Edge(){}
};



class Graph
{
public:
    Graph():i_counter(0){
		const_array_Vertex_Server_Cost = new int[MaxVertexNum];
		const_array_Server_Ability = new int[MaxServerLvlNum];
		const_array_Server_Cost = new int[MaxServerLvlNum];

		raw_array_Vertex_d = new int[MaxVertexNum + 1];
		raw_array_Vertex_consumer_id = new int[MaxVertexNum + 1];
		raw_array_Vertex_EdgesOut = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_array_Vertex_EdgesIn = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_array_Vertex_distance = new int[MaxVertexNum + 1];
		raw_array_Vertex_from_edge = new int[MaxVertexNum + 1];


		array_Vertex_d = raw_array_Vertex_d + 1;
		array_Vertex_consumer_id = raw_array_Vertex_consumer_id + 1;
		array_Vertex_EdgesOut = raw_array_Vertex_EdgesOut + MaxEdgeNum;
		array_Vertex_EdgesIn = raw_array_Vertex_EdgesIn + MaxEdgeNum;
		array_Vertex_distance = raw_array_Vertex_distance + 1;
		array_Vertex_from_edge = raw_array_Vertex_from_edge + 1;

		array_Consumer_fromVertex = new int[MaxConsumerNum];
	    array_Consumer_requirement = new int[MaxConsumerNum];
		array_Consumer_remaining_requirement = new int[MaxConsumerNum];

		raw_array_Edge_from = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_array_Edge_to = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_array_Edge_bandwidth = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_array_Edge_cost = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_array_Edge_x = new int[(MaxVertexNum + 1) * MaxEdgeNum];
	    raw_array_Edge_visited = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_array_Edge_ResidualEdgeNo = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_array_Edge_IsReversEdge = new int[(MaxVertexNum + 1) * MaxEdgeNum];

		array_Edge_from = raw_array_Edge_from + MaxEdgeNum;
		array_Edge_to = raw_array_Edge_to + MaxEdgeNum;
		array_Edge_bandwidth = raw_array_Edge_bandwidth + MaxEdgeNum;
		array_Edge_cost = raw_array_Edge_cost + MaxEdgeNum;
		array_Edge_x = raw_array_Edge_x + MaxEdgeNum;
	    array_Edge_visited = raw_array_Edge_visited + MaxEdgeNum;
		array_Edge_ResidualEdgeNo = raw_array_Edge_ResidualEdgeNo + MaxEdgeNum;
		array_Edge_IsReversEdge = raw_array_Edge_IsReversEdge + MaxEdgeNum;


		backup_raw_array_Vertex_d = new int[MaxVertexNum + 1];
		backup_raw_array_Vertex_consumer_id = new int[MaxVertexNum + 1];
		backup_raw_array_Vertex_EdgesOut = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		backup_raw_array_Vertex_EdgesIn = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		backup_raw_array_Vertex_distance = new int[MaxVertexNum + 1];
		backup_raw_array_Vertex_from_edge = new int[MaxVertexNum + 1];


		backup_array_Consumer_fromVertex = new int[MaxConsumerNum];
		backup_array_Consumer_requirement = new int[MaxConsumerNum];
		backup_array_Consumer_remaining_requirement = new int[MaxConsumerNum];


		backup_raw_array_Edge_from = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		backup_raw_array_Edge_to = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		backup_raw_array_Edge_bandwidth = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		backup_raw_array_Edge_cost = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		backup_raw_array_Edge_x = new int[(MaxVertexNum + 1) * MaxEdgeNum];
	    backup_raw_array_Edge_visited = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		backup_raw_array_Edge_ResidualEdgeNo = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		backup_raw_array_Edge_IsReversEdge = new int[(MaxVertexNum + 1) * MaxEdgeNum];

		backup_array_Edge_from = raw_array_Edge_from + MaxEdgeNum;
		backup_array_Edge_to = raw_array_Edge_to + MaxEdgeNum;
		backup_array_Edge_bandwidth = raw_array_Edge_bandwidth + MaxEdgeNum;
		backup_array_Edge_cost = raw_array_Edge_cost + MaxEdgeNum;
		backup_array_Edge_x = raw_array_Edge_x + MaxEdgeNum;
	    backup_array_Edge_visited = raw_array_Edge_visited + MaxEdgeNum;
		backup_array_Edge_ResidualEdgeNo = raw_array_Edge_ResidualEdgeNo + MaxEdgeNum;
		backup_array_Edge_IsReversEdge = raw_array_Edge_IsReversEdge + MaxEdgeNum;

		raw_globalmin_array_Vertex_d = new int[MaxVertexNum + 1];
		raw_globalmin_array_Vertex_consumer_id = new int[MaxVertexNum + 1];
		raw_globalmin_array_Vertex_EdgesOut = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_globalmin_array_Vertex_EdgesIn = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_globalmin_array_Vertex_distance = new int[MaxVertexNum + 1];
		raw_globalmin_array_Vertex_from_edge = new int[MaxVertexNum + 1];


		globalmin_array_Vertex_d = raw_globalmin_array_Vertex_d + 1;
		globalmin_array_Vertex_consumer_id = raw_globalmin_array_Vertex_consumer_id + 1;
		globalmin_array_Vertex_EdgesOut = raw_globalmin_array_Vertex_EdgesOut + MaxEdgeNum;
		globalmin_array_Vertex_EdgesIn = raw_globalmin_array_Vertex_EdgesIn + MaxEdgeNum;
		globalmin_array_Vertex_distance = raw_globalmin_array_Vertex_distance + 1;
		globalmin_array_Vertex_from_edge = raw_globalmin_array_Vertex_from_edge + 1;

		globalmin_array_Consumer_fromVertex = new int[MaxConsumerNum];
	    globalmin_array_Consumer_requirement = new int[MaxConsumerNum];
		globalmin_array_Consumer_remaining_requirement = new int[MaxConsumerNum];

		raw_globalmin_array_Edge_from = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_globalmin_array_Edge_to = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_globalmin_array_Edge_bandwidth = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_globalmin_array_Edge_cost = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_globalmin_array_Edge_x = new int[(MaxVertexNum + 1) * MaxEdgeNum];
	    raw_globalmin_array_Edge_visited = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_globalmin_array_Edge_ResidualEdgeNo = new int[(MaxVertexNum + 1) * MaxEdgeNum];
		raw_globalmin_array_Edge_IsReversEdge = new int[(MaxVertexNum + 1) * MaxEdgeNum];

		globalmin_array_Edge_from = raw_globalmin_array_Edge_from + MaxEdgeNum;
		globalmin_array_Edge_to = raw_globalmin_array_Edge_to + MaxEdgeNum;
		globalmin_array_Edge_bandwidth = raw_globalmin_array_Edge_bandwidth + MaxEdgeNum;
		globalmin_array_Edge_cost = raw_globalmin_array_Edge_cost + MaxEdgeNum;
		globalmin_array_Edge_x = raw_globalmin_array_Edge_x + MaxEdgeNum;
	    globalmin_array_Edge_visited = raw_globalmin_array_Edge_visited + MaxEdgeNum;
		globalmin_array_Edge_ResidualEdgeNo = raw_globalmin_array_Edge_ResidualEdgeNo + MaxEdgeNum;
		globalmin_array_Edge_IsReversEdge = raw_globalmin_array_Edge_IsReversEdge + MaxEdgeNum;


		init();
	}

	void init(){
		for(int i = -1;i < MaxVertexNum;i++){
			array_Vertex_d[i] = 0;
			array_Vertex_consumer_id[i] = -1;
		    array_Vertex_distance[i] = 0;
		    array_Vertex_from_edge[i] = -1;
			for(int j = 0;j < MaxEdgeNum;j++){
				array_Vertex_EdgesOut[i * MaxEdgeNum + j] = -1;
		        array_Vertex_EdgesIn[i * MaxEdgeNum + j] = -1;
				array_Edge_from[i * MaxEdgeNum + j] = -777;
		        array_Edge_to[i * MaxEdgeNum + j] = -777;
		        array_Edge_bandwidth[i * MaxEdgeNum + j] = 0;
		        array_Edge_cost[i * MaxEdgeNum + j] = 0;
		        array_Edge_x[i * MaxEdgeNum + j] = 0;
	            array_Edge_visited[i * MaxEdgeNum + j] = 0;
		        array_Edge_ResidualEdgeNo[i * MaxEdgeNum + j] = -1;
		        array_Edge_IsReversEdge[i * MaxEdgeNum + j] = false;
			}
		}

		for(int i = 0;i < MaxConsumerNum;i++){
			array_Consumer_fromVertex[i] = -777;
			array_Consumer_requirement[i] = -777;
		    array_Consumer_remaining_requirement[i] = -777;
		}
	}

	~Graph(){
		delete[] const_array_Vertex_Server_Cost;
		delete[] const_array_Server_Ability;
		delete[] const_array_Server_Cost;

		delete[] raw_array_Vertex_d;
		delete[] raw_array_Vertex_consumer_id;
		delete[] raw_array_Vertex_EdgesOut;
		delete[] raw_array_Vertex_EdgesIn;
		delete[] raw_array_Vertex_distance;
		delete[] raw_array_Vertex_from_edge;

		delete[] array_Consumer_fromVertex;
		delete[] array_Consumer_requirement;
		delete[] array_Consumer_remaining_requirement;


		delete[] raw_array_Edge_from;
		delete[] raw_array_Edge_to;
		delete[] raw_array_Edge_bandwidth;
		delete[] raw_array_Edge_cost;
		delete[] raw_array_Edge_x;
		delete[] raw_array_Edge_visited;
		delete[] raw_array_Edge_ResidualEdgeNo;
		delete[] raw_array_Edge_IsReversEdge;

		delete[] raw_globalmin_array_Vertex_d;
		delete[] raw_globalmin_array_Vertex_consumer_id;
		delete[] raw_globalmin_array_Vertex_EdgesOut;
		delete[] raw_globalmin_array_Vertex_EdgesIn;
		delete[] raw_globalmin_array_Vertex_distance;
		delete[] raw_globalmin_array_Vertex_from_edge;

		delete[] globalmin_array_Consumer_fromVertex;
		delete[] globalmin_array_Consumer_requirement;
		delete[] globalmin_array_Consumer_remaining_requirement;


		delete[] raw_globalmin_array_Edge_from;
		delete[] raw_globalmin_array_Edge_to;
		delete[] raw_globalmin_array_Edge_bandwidth;
		delete[] raw_globalmin_array_Edge_cost;
		delete[] raw_globalmin_array_Edge_x;
		delete[] raw_globalmin_array_Edge_visited;
		delete[] raw_globalmin_array_Edge_ResidualEdgeNo;
		delete[] raw_globalmin_array_Edge_IsReversEdge;

		delete[] backup_raw_array_Vertex_d;
		delete[] backup_raw_array_Vertex_consumer_id;
		delete[] backup_raw_array_Vertex_EdgesOut;
		delete[] backup_raw_array_Vertex_EdgesIn;
		delete[] backup_raw_array_Vertex_distance;
		delete[] backup_raw_array_Vertex_from_edge;

		delete[] backup_array_Consumer_fromVertex;
		delete[] backup_array_Consumer_requirement;
		delete[] backup_array_Consumer_remaining_requirement;


		delete[] backup_raw_array_Edge_from;
		delete[] backup_raw_array_Edge_to;
		delete[] backup_raw_array_Edge_bandwidth;
		delete[] backup_raw_array_Edge_cost;
		delete[] backup_raw_array_Edge_x;
		delete[] backup_raw_array_Edge_visited;
		delete[] backup_raw_array_Edge_ResidualEdgeNo;
		delete[] backup_raw_array_Edge_IsReversEdge;
	}

	void save(){
		memcpy(backup_raw_array_Vertex_d,raw_array_Vertex_d,sizeof(int) * (MaxVertexNum + 1));
		memcpy(backup_raw_array_Vertex_consumer_id,raw_array_Vertex_consumer_id,sizeof(int) * (MaxVertexNum + 1));
		memcpy(backup_raw_array_Vertex_EdgesOut,raw_array_Vertex_EdgesOut,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Vertex_EdgesIn,raw_array_Vertex_EdgesIn,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Vertex_distance,raw_array_Vertex_distance,sizeof(int) * (MaxVertexNum + 1));
		memcpy(backup_raw_array_Vertex_from_edge,raw_array_Vertex_from_edge,sizeof(int) * (MaxVertexNum + 1));
		memcpy(backup_array_Consumer_fromVertex,array_Consumer_fromVertex,sizeof(int) * MaxConsumerNum);
		memcpy(backup_array_Consumer_requirement,array_Consumer_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(backup_array_Consumer_remaining_requirement,array_Consumer_remaining_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(backup_raw_array_Edge_from,raw_array_Edge_from,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Edge_to,raw_array_Edge_to,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Edge_bandwidth,raw_array_Edge_bandwidth,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Edge_cost,raw_array_Edge_cost,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Edge_x,raw_array_Edge_x,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Edge_visited,raw_array_Edge_visited,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Edge_ResidualEdgeNo,raw_array_Edge_ResidualEdgeNo,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(backup_raw_array_Edge_IsReversEdge,raw_array_Edge_IsReversEdge,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		back_i_counter = i_counter;
	}

	void restore(){
		memcpy(raw_array_Vertex_d,backup_raw_array_Vertex_d,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_array_Vertex_consumer_id,backup_raw_array_Vertex_consumer_id,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_array_Vertex_EdgesOut,backup_raw_array_Vertex_EdgesOut,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Vertex_EdgesIn,backup_raw_array_Vertex_EdgesIn,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Vertex_distance,backup_raw_array_Vertex_distance,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_array_Vertex_from_edge,backup_raw_array_Vertex_from_edge,sizeof(int) * (MaxVertexNum + 1));
		memcpy(array_Consumer_fromVertex,backup_array_Consumer_fromVertex,sizeof(int) * MaxConsumerNum);
		memcpy(array_Consumer_requirement,backup_array_Consumer_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(array_Consumer_remaining_requirement,backup_array_Consumer_remaining_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(raw_array_Edge_from,backup_raw_array_Edge_from,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_to,backup_raw_array_Edge_to,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_bandwidth,backup_raw_array_Edge_bandwidth,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_cost,backup_raw_array_Edge_cost,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_x,backup_raw_array_Edge_x,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_visited,backup_raw_array_Edge_visited,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_ResidualEdgeNo,backup_raw_array_Edge_ResidualEdgeNo,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_IsReversEdge,backup_raw_array_Edge_IsReversEdge,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		i_counter = back_i_counter;
	}


	void save_globalmin(){
		memcpy(raw_globalmin_array_Vertex_d,raw_array_Vertex_d,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_globalmin_array_Vertex_consumer_id,raw_array_Vertex_consumer_id,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_globalmin_array_Vertex_EdgesOut,raw_array_Vertex_EdgesOut,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Vertex_EdgesIn,raw_array_Vertex_EdgesIn,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Vertex_distance,raw_array_Vertex_distance,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_globalmin_array_Vertex_from_edge,raw_array_Vertex_from_edge,sizeof(int) * (MaxVertexNum + 1));
		memcpy(globalmin_array_Consumer_fromVertex,array_Consumer_fromVertex,sizeof(int) * MaxConsumerNum);
		memcpy(globalmin_array_Consumer_requirement,array_Consumer_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(globalmin_array_Consumer_remaining_requirement,array_Consumer_remaining_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(raw_globalmin_array_Edge_from,raw_array_Edge_from,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Edge_to,raw_array_Edge_to,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Edge_bandwidth,raw_array_Edge_bandwidth,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Edge_cost,raw_array_Edge_cost,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Edge_x,raw_array_Edge_x,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Edge_visited,raw_array_Edge_visited,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Edge_ResidualEdgeNo,raw_array_Edge_ResidualEdgeNo,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_globalmin_array_Edge_IsReversEdge,raw_array_Edge_IsReversEdge,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
	}

	void restore_globalmin(){
		memcpy(raw_array_Vertex_d,raw_globalmin_array_Vertex_d,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_array_Vertex_consumer_id,raw_globalmin_array_Vertex_consumer_id,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_array_Vertex_EdgesOut,raw_globalmin_array_Vertex_EdgesOut,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Vertex_EdgesIn,raw_globalmin_array_Vertex_EdgesIn,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Vertex_distance,raw_globalmin_array_Vertex_distance,sizeof(int) * (MaxVertexNum + 1));
		memcpy(raw_array_Vertex_from_edge,raw_globalmin_array_Vertex_from_edge,sizeof(int) * (MaxVertexNum + 1));
		memcpy(array_Consumer_fromVertex,globalmin_array_Consumer_fromVertex,sizeof(int) * MaxConsumerNum);
		memcpy(array_Consumer_requirement,globalmin_array_Consumer_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(array_Consumer_remaining_requirement,globalmin_array_Consumer_remaining_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(raw_array_Edge_from,raw_globalmin_array_Edge_from,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_to,raw_globalmin_array_Edge_to,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_bandwidth,raw_globalmin_array_Edge_bandwidth,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_cost,raw_globalmin_array_Edge_cost,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_x,raw_globalmin_array_Edge_x,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_visited,raw_globalmin_array_Edge_visited,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_ResidualEdgeNo,raw_globalmin_array_Edge_ResidualEdgeNo,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
		memcpy(raw_array_Edge_IsReversEdge,raw_globalmin_array_Edge_IsReversEdge,sizeof(int) * (MaxVertexNum + 1) * MaxEdgeNum);
	}

    void add_Edge(int from,
	    int to,
	    int bandwidth,
	    int cost){
		int id1 = GetAnID();
		int id2 = GetAnID();
		
		set_Edge(id1,from,to,bandwidth,cost,id2,false);
		set_Edge(id2,to,from,bandwidth,-cost,id1,true);
	}

	void push_EdgesOut(int vertexid, int edgeid){
		for(int i = 0;i < MaxEdgeNum;i++){
			if(array_Vertex_EdgesOut[vertexid * MaxEdgeNum + i] < 0){
				array_Vertex_EdgesOut[vertexid * MaxEdgeNum + i] = edgeid;
				break;
			}
		}
	}

	void push_EdgesIn(int vertexid, int edgeid){
		for(int i = 0;i < MaxEdgeNum;i++){
			if(array_Vertex_EdgesIn[vertexid * MaxEdgeNum + i] < 0){
				array_Vertex_EdgesIn[vertexid * MaxEdgeNum + i] = edgeid;
				break;
			}
		}
	}

	void set_Edge(int id,
        int from,
	    int to,
	    int bandwidth,
	    int cost,
		int ResidualEdgeNo,
		bool isReverse){
		
		push_EdgesOut(from,id);
		push_EdgesIn(to,id);	
		
		array_Edge_from[id] = from;
		array_Edge_to[id] = to;
		if(!isReverse)
		    array_Edge_bandwidth[id] = bandwidth;
		array_Edge_cost[id] = cost;
		array_Edge_x[id] = 0;
		array_Edge_visited[id] = 0;
		array_Edge_ResidualEdgeNo[id] = ResidualEdgeNo;
		array_Edge_IsReversEdge[id] = isReverse;
	}

	void add_Consumer(const int id,int fromVertex,const int requirement){
		array_Consumer_fromVertex[id] = fromVertex;
	    array_Consumer_requirement[id] = requirement;
	    array_Consumer_remaining_requirement[id] = requirement;

		array_Vertex_consumer_id[fromVertex] = id;
		array_Vertex_d[fromVertex] = -requirement;
	}

	int GetAnID(){
		return i_counter++;
	}

	void start_from_source(vector<list<int>>& result,list<int> path,int node,int flow,int ServerLvl){
		int comsumer_id = array_Vertex_consumer_id[node];
		if(comsumer_id >= 0){
			result.push_back(path);
			result.back().push_back(comsumer_id);
			int remaining_requirement = array_Consumer_remaining_requirement[comsumer_id];
			if(flow > remaining_requirement){
				result.back().push_back(remaining_requirement);
				flow -= remaining_requirement;
				array_Consumer_remaining_requirement[comsumer_id] = 0;
			}
			else{
				result.back().push_back(flow);
				array_Consumer_remaining_requirement[comsumer_id] -= flow;
				flow = 0;
			}
			result.back().push_back(ServerLvl);
		}
		
		//for(auto e:node->EdgesOut){
		for(int i = 0;i < MaxEdgeNum;i++){
			int edge = array_Vertex_EdgesOut[node * MaxEdgeNum + i];
			if(edge < 0)
			    break;//end of EdgesOut
#ifdef LEN_DBG
			cout << "node: " << node->id << " flow " 
			<< flow << " => e " << e->to->id << " x " << e->x << endl;
#endif
			if(flow <= 0)
		        return;
			if(array_Edge_x[edge] > 0 && 
			array_Edge_visited[edge] < array_Edge_x[edge]){//还有空余带宽
#ifdef LEN_DBG
			    cout << "x " << edge.x << " visited " << edge.visited << endl;
#endif
			    int next_flow;
				if(flow > array_Edge_x[edge] - array_Edge_visited[edge])
				    next_flow = array_Edge_x[edge] - array_Edge_visited[edge];
				else
				    next_flow = flow;
				flow -= next_flow;//减去被分流的部
				array_Edge_visited[edge] += next_flow;
				int next_node = array_Edge_to[edge];
				auto _path_copy = path;
				_path_copy.push_back(next_node);
				
#ifdef LEN_DBG
				cout << " next node: " << next_node.id << " next_flow " << next_flow << endl;
#endif
				start_from_source(result,_path_copy,next_node,next_flow,ServerLvl);
			}
		}
	}


	int total_cost(){
		int sum = 0;
		int total_edge_num = (MaxVertexNum + 1) * MaxEdgeNum;
		for(int i = 0;i < total_edge_num;i++){
			if(!raw_array_Edge_IsReversEdge[i]){// not pesudo source
			    sum += raw_array_Edge_cost[i] * raw_array_Edge_x[i];
			}
		}

		//for(auto out_from:V[-1].EdgesOut){
		for(int i = 0;i < MaxEdgeNum;i++){
			int e = array_Vertex_EdgesOut[-1 * MaxEdgeNum + i];
			if(array_Edge_x[e] > 0)
			    sum += get_ServerCost(array_Edge_to[e],array_Edge_x[e]);
		}
		return sum;
	}

	inline int get_ServerCost(int v,int demand){
		return const_array_Server_Cost[get_suitable_Server(demand)] 
		    + const_array_Vertex_Server_Cost[array_Edge_to[v]];
	}

	inline int get_suitable_Server(int demand){
		for(int i = 0;i < ServerLvlNum;i++){
			if(const_array_Server_Ability[i] >= demand){
				return i;
			}
		}
		throw "error";
		return -1;
	}

	string to_String(){
		vector<list<int>> result;
		map<int,int> real_source;
		//for(auto pesudo_source_out:V.at(-1).EdgesOut){
		for(int i = 0;i < MaxEdgeNum;i++){
			if(array_Edge_x[array_Vertex_EdgesOut[-1 * MaxEdgeNum + i]] > 0){//has some flow
			    real_source[array_Edge_to[array_Vertex_EdgesOut[-1 * MaxEdgeNum + i]]] 
				    = array_Edge_x[array_Vertex_EdgesOut[-1 * MaxEdgeNum + i]];
#ifdef LEN_DBG
				cout << "pesudo_source_out => " << pesudo_source_out->to->id << "  " << pesudo_source_out->x << endl;
#endif
			}
		}

		for(auto p : real_source){
			list<int> line;
			line.push_back(p.first);
			start_from_source(result,line,p.first,p.second,get_suitable_Server(p.second));
		}
		string newline = "\n";
		string ret;
		ret += to_string(int(result.size())) + newline;
		ret += newline;
		size_t line_num = result.size();
		for(size_t i = 0;i < line_num;i++){
			auto& line_list = result[i];
			bool first = true;
			for(int num:line_list){
				if(first){
					ret += to_string(num);
					first = false;
				}
				else{
					ret += " ";
					ret += to_string(num);
				}
			}
			if(i != line_num - 1)
			    ret += newline;
		}

		memcpy(array_Consumer_fromVertex,backup_array_Consumer_fromVertex,sizeof(int) * MaxConsumerNum);
		memcpy(array_Consumer_requirement,backup_array_Consumer_requirement,sizeof(int) * MaxConsumerNum);
		memcpy(array_Consumer_remaining_requirement,backup_array_Consumer_remaining_requirement,sizeof(int) * MaxConsumerNum);

		return ret;
	}


	void cost_start_from_source(vector<int>& result,int path_cost,int node,int flow){
		int comsumer_id = array_Vertex_consumer_id[node];
		if(comsumer_id >= 0){
			
			int remaining_requirement = array_Consumer_remaining_requirement[comsumer_id];
			if(flow > remaining_requirement){
				result.push_back(path_cost * remaining_requirement);
				flow -= remaining_requirement;
				array_Consumer_remaining_requirement[comsumer_id] = 0;
			}
			else{
				result.push_back(path_cost * flow);
				array_Consumer_remaining_requirement[comsumer_id] -= flow;
				flow = 0;
			}
			    
		}
		
		//for(auto e:node->EdgesOut){
		for(int i = 0;i < MaxEdgeNum;i++){
			int edge = array_Vertex_EdgesOut[node * MaxEdgeNum + i];
			if(edge < 0)
			    break;//end of EdgesOut
			if(flow <= 0)
		        return;
			if(array_Edge_x[edge] > 0 && 
			array_Edge_visited[edge] < array_Edge_x[edge]){//还有空余带宽
			    int next_flow;
				if(flow > array_Edge_x[edge] - array_Edge_visited[edge])
				    next_flow = array_Edge_x[edge] - array_Edge_visited[edge];
				else
				    next_flow = flow;
				flow -= next_flow;//减去被分流的部
				array_Edge_visited[edge] += next_flow;
				int next_node = array_Edge_to[edge];
				path_cost += array_Edge_cost[edge];
				cost_start_from_source(result,path_cost,next_node,next_flow);
			}
		}
	}
/*
	int total_cost(){
		int cost = 0;
		vector<int> result;
		map<int,int> real_source;
		//for(auto pesudo_source_out:V.at(-1).EdgesOut){
		for(int i = 0;i < MaxEdgeNum;i++){
			if(array_Edge_x[array_Vertex_EdgesOut[-1 * MaxEdgeNum + i]] > 0){//has some flow
			    real_source[array_Edge_to[array_Vertex_EdgesOut[-1 * MaxEdgeNum + i]]] 
				    = array_Edge_x[array_Vertex_EdgesOut[-1 * MaxEdgeNum + i]];
			}
		}

		for(auto p : real_source){
			cost += ServerCost;
			cost_start_from_source(result,0,p.first,p.second);
		}

		for(int path_cost:result){
			cost += path_cost;
		}

		memcpy(array_Consumer_fromVertex,backup_array_Consumer_fromVertex,sizeof(int) * (MaxConsumerNum + 1));
		memcpy(array_Consumer_requirement,backup_array_Consumer_requirement,sizeof(int) * (MaxConsumerNum + 1));
		memcpy(array_Consumer_remaining_requirement,backup_array_Consumer_remaining_requirement,sizeof(int) * (MaxConsumerNum + 1));
		return cost;
	}
*/
	int EdgeNum;
	int VertexNum;
	int ConsumerNum;
	int ServerLvlNum;

	int* const_array_Vertex_Server_Cost;
	int* const_array_Server_Cost;
	int* const_array_Server_Ability;


    int* array_Vertex_d;
	int* array_Vertex_consumer_id;
	int* array_Vertex_EdgesOut;
	int* array_Vertex_EdgesIn;
	int* array_Vertex_distance;
	int* array_Vertex_from_edge;
	

	int* raw_array_Vertex_d;
	int* raw_array_Vertex_consumer_id;
	int* raw_array_Vertex_EdgesOut;
	int* raw_array_Vertex_EdgesIn;
	int* raw_array_Vertex_distance;
	int* raw_array_Vertex_from_edge;

	int* array_Consumer_fromVertex;
	int* array_Consumer_requirement;
	int* array_Consumer_remaining_requirement;


	int* array_Edge_from;
	int* array_Edge_to;
	int* array_Edge_bandwidth;
	int* array_Edge_cost;
	int* array_Edge_x;
	int* array_Edge_visited;
	int* array_Edge_ResidualEdgeNo;
	int* array_Edge_IsReversEdge;

	int* raw_array_Edge_from;
	int* raw_array_Edge_to;
	int* raw_array_Edge_bandwidth;
	int* raw_array_Edge_cost;
	int* raw_array_Edge_x;
	int* raw_array_Edge_visited;
	int* raw_array_Edge_ResidualEdgeNo;
	int* raw_array_Edge_IsReversEdge;

	int* backup_raw_array_Vertex_d;
	int* backup_raw_array_Vertex_consumer_id;
	int* backup_raw_array_Vertex_EdgesOut;
	int* backup_raw_array_Vertex_EdgesIn;
	int* backup_raw_array_Vertex_distance;
	int* backup_raw_array_Vertex_from_edge;

	int* backup_array_Consumer_fromVertex;
	int* backup_array_Consumer_requirement;
	int* backup_array_Consumer_remaining_requirement;


	int* backup_array_Edge_from;
	int* backup_array_Edge_to;
	int* backup_array_Edge_bandwidth;
	int* backup_array_Edge_cost;
	int* backup_array_Edge_x;
	int* backup_array_Edge_visited;
	int* backup_array_Edge_ResidualEdgeNo;
	int* backup_array_Edge_IsReversEdge;

	int* backup_raw_array_Edge_from;
	int* backup_raw_array_Edge_to;
	int* backup_raw_array_Edge_bandwidth;
	int* backup_raw_array_Edge_cost;
	int* backup_raw_array_Edge_x;
	int* backup_raw_array_Edge_visited;
	int* backup_raw_array_Edge_ResidualEdgeNo;
	int* backup_raw_array_Edge_IsReversEdge;

	int* globalmin_array_Vertex_d;
	int* globalmin_array_Vertex_consumer_id;
	int* globalmin_array_Vertex_EdgesOut;
	int* globalmin_array_Vertex_EdgesIn;
	int* globalmin_array_Vertex_distance;
	int* globalmin_array_Vertex_from_edge;

	int* raw_globalmin_array_Vertex_d;
	int* raw_globalmin_array_Vertex_consumer_id;
	int* raw_globalmin_array_Vertex_EdgesOut;
	int* raw_globalmin_array_Vertex_EdgesIn;
	int* raw_globalmin_array_Vertex_distance;
	int* raw_globalmin_array_Vertex_from_edge;

	int* globalmin_array_Consumer_fromVertex;
	int* globalmin_array_Consumer_requirement;
	int* globalmin_array_Consumer_remaining_requirement;


	int* globalmin_array_Edge_from;
	int* globalmin_array_Edge_to;
	int* globalmin_array_Edge_bandwidth;
	int* globalmin_array_Edge_cost;
	int* globalmin_array_Edge_x;
	int* globalmin_array_Edge_visited;
	int* globalmin_array_Edge_ResidualEdgeNo;
	int* globalmin_array_Edge_IsReversEdge;

	int* raw_globalmin_array_Edge_from;
	int* raw_globalmin_array_Edge_to;
	int* raw_globalmin_array_Edge_bandwidth;
	int* raw_globalmin_array_Edge_cost;
	int* raw_globalmin_array_Edge_x;
	int* raw_globalmin_array_Edge_visited;
	int* raw_globalmin_array_Edge_ResidualEdgeNo;
	int* raw_globalmin_array_Edge_IsReversEdge;

private:
    int i_counter;
	int back_i_counter;
};
//#undef LEN_DBG
#endif//__DATA_STRUCTURE_H__