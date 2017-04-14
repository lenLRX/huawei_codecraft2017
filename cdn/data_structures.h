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
#include <cassert>

//#define LEN_DBG

const int MaxEdgeNum = 400;//21 * 2;//reverse edge
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

class GraphMemory{
public:
    void init(int VertexNum,int TotalEdgeNum,int ConsumerNum){

		VNum = VertexNum;
		ENum = TotalEdgeNum;
		CNum = ConsumerNum;

		raw_array_Vertex_d = new int[VNum];
		raw_array_Vertex_consumer_id = new int[VNum];
		raw_array_Vertex_EdgesOut = new int[ENum];
		raw_array_Vertex_EdgesIn = new int[ENum];
		raw_array_Vertex_distance = new int[VNum];
		raw_array_Vertex_from_edge = new int[VNum];

		array_Vertex_d = raw_array_Vertex_d + 1;
		array_Vertex_consumer_id = raw_array_Vertex_consumer_id + 1;
		array_Vertex_EdgesOut = raw_array_Vertex_EdgesOut;
		array_Vertex_EdgesIn = raw_array_Vertex_EdgesIn;
		array_Vertex_distance = raw_array_Vertex_distance + 1;
		array_Vertex_from_edge = raw_array_Vertex_from_edge + 1;

		array_Consumer_fromVertex = new int[CNum];
		array_Consumer_requirement = new int[CNum];
		array_Consumer_remaining_requirement = new int[CNum];

		raw_array_Edge_from = new int[ENum];
		raw_array_Edge_to = new int[ENum];
		raw_array_Edge_bandwidth = new int[ENum];
		raw_array_Edge_cost = new int[ENum];
		raw_array_Edge_x = new int[ENum];
		raw_array_Edge_visited = new int[ENum];
		raw_array_Edge_ResidualEdgeNo = new int[ENum];
		raw_array_Edge_IsReversEdge = new int[ENum];

		array_Edge_from = raw_array_Edge_from;
		array_Edge_to = raw_array_Edge_to;
		array_Edge_bandwidth = raw_array_Edge_bandwidth;
		array_Edge_cost = raw_array_Edge_cost;
		array_Edge_x = raw_array_Edge_x;
		array_Edge_visited = raw_array_Edge_visited;
		array_Edge_ResidualEdgeNo = raw_array_Edge_ResidualEdgeNo;
		array_Edge_IsReversEdge = raw_array_Edge_IsReversEdge;

		i_counter = 0;
	}

	void copy_from(GraphMemory& other){
		i_counter = other.i_counter;
		VNum = other.VNum;
	    ENum = other.ENum;
	    CNum = other.CNum;

		memcpy(raw_array_Vertex_d,other.raw_array_Vertex_d,sizeof(int) * VNum);
		memcpy(raw_array_Vertex_consumer_id,other.raw_array_Vertex_consumer_id,sizeof(int) * VNum);
		memcpy(raw_array_Vertex_EdgesOut,other.raw_array_Vertex_EdgesOut,sizeof(int) * ENum);
		memcpy(raw_array_Vertex_EdgesIn,other.raw_array_Vertex_EdgesIn,sizeof(int) * ENum);
		memcpy(raw_array_Vertex_distance,other.raw_array_Vertex_distance,sizeof(int) * VNum);
		memcpy(raw_array_Vertex_from_edge,other.raw_array_Vertex_from_edge,sizeof(int) * VNum);

		memcpy(array_Consumer_fromVertex,other.array_Consumer_fromVertex,sizeof(int) * CNum);
		memcpy(array_Consumer_requirement,other.array_Consumer_requirement,sizeof(int) * CNum);
		memcpy(array_Consumer_remaining_requirement,other.array_Consumer_remaining_requirement,sizeof(int) * CNum);

		memcpy(raw_array_Edge_from,other.raw_array_Edge_from,sizeof(int) * ENum);
		memcpy(raw_array_Edge_to,other.raw_array_Edge_to,sizeof(int) * ENum);
		memcpy(raw_array_Edge_bandwidth,other.raw_array_Edge_bandwidth,sizeof(int) * ENum);
		memcpy(raw_array_Edge_cost,other.raw_array_Edge_cost,sizeof(int) * ENum);
		memcpy(raw_array_Edge_x,other.raw_array_Edge_x,sizeof(int) * ENum);
		memcpy(raw_array_Edge_visited,other.raw_array_Edge_visited,sizeof(int) * ENum);
		memcpy(raw_array_Edge_ResidualEdgeNo,other.raw_array_Edge_ResidualEdgeNo,sizeof(int) * ENum);
		memcpy(raw_array_Edge_IsReversEdge,other.raw_array_Edge_IsReversEdge,sizeof(int) * ENum);
	}

	void release(){
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
	}

	int VNum;
	int ENum;
	int CNum;

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

	int i_counter;
};

class Graph
{
public:
    Graph():ServerLvlNum(0){
		const_array_Vertex_Server_Cost = new int[MaxVertexNum];
		const_array_Server_Ability = new int[MaxServerLvlNum];
		const_array_Server_Cost = new int[MaxServerLvlNum];
	}

	void init(GraphMemory& memory){
		for(int i = -1;i < VertexNum;i++){
			memory.array_Vertex_d[i] = 0;
			memory.array_Vertex_consumer_id[i] = -1;
		    memory.array_Vertex_distance[i] = 0;
		    memory.array_Vertex_from_edge[i] = -1;
			int size = array_Vertex2Edge_len[i];
		}

		for(int i = 0;i < memory.ENum;i++){
			memory.array_Vertex_EdgesOut[i] = -1;
		    memory.array_Vertex_EdgesIn[i] = -1;
			memory.array_Edge_from[i] = -777;
		    memory.array_Edge_to[i] = -777;
		    memory.array_Edge_bandwidth[i] = 0;
		    memory.array_Edge_cost[i] = 0;
		    memory.array_Edge_x[i] = 0;
	        memory.array_Edge_visited[i] = 0;
		    memory.array_Edge_ResidualEdgeNo[i] = -1;
		    memory.array_Edge_IsReversEdge[i] = false;
		}

		for(int i = 0;i < memory.CNum;i++){
			memory.array_Consumer_fromVertex[i] = -777;
			memory.array_Consumer_requirement[i] = -777;
		    memory.array_Consumer_remaining_requirement[i] = -777;
		}
	}

	~Graph(){
	}

	void save(){
		backup_mem.copy_from(mem);
	}

	void restore(){
		mem.copy_from(backup_mem);
	}


	void save_globalmin(){
		global_min_mem.copy_from(mem);
	}

	void restore_globalmin(){
		mem.copy_from(global_min_mem);
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
		int size = array_Vertex2Edge_len[vertexid];
		for(int i = 0;i < size;i++){
			if(mem.array_Vertex_EdgesOut[array_Vertex2Edge_offset[vertexid] + i] < 0){
				mem.array_Vertex_EdgesOut[array_Vertex2Edge_offset[vertexid] + i] = edgeid;
				return;
			}
		}
		throw "error";
	}

	void push_EdgesIn(int vertexid, int edgeid){
		int size = array_Vertex2Edge_len[vertexid];
		for(int i = 0;i < size;i++){
			if(mem.array_Vertex_EdgesIn[array_Vertex2Edge_offset[vertexid] + i] < 0){
				mem.array_Vertex_EdgesIn[array_Vertex2Edge_offset[vertexid] + i] = edgeid;
				return;
			}
		}
		throw "error";
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
		
		mem.array_Edge_from[id] = from;
		mem.array_Edge_to[id] = to;
		if(!isReverse)
		    mem.array_Edge_bandwidth[id] = bandwidth;
		mem.array_Edge_cost[id] = cost;
		mem.array_Edge_x[id] = 0;
		mem.array_Edge_visited[id] = 0;
		mem.array_Edge_ResidualEdgeNo[id] = ResidualEdgeNo;
		mem.array_Edge_IsReversEdge[id] = isReverse;
	}

	void add_Consumer(const int id,int fromVertex,const int requirement){
		mem.array_Consumer_fromVertex[id] = fromVertex;
	    mem.array_Consumer_requirement[id] = requirement;
	    mem.array_Consumer_remaining_requirement[id] = requirement;

		mem.array_Vertex_consumer_id[fromVertex] = id;
		mem.array_Vertex_d[fromVertex] = -requirement;
	}

	int GetAnID(){
		return mem.i_counter++;
	}

	void start_from_source(vector<list<int>>& result,list<int> path,int node,int flow,int ServerLvl){
		int comsumer_id = mem.array_Vertex_consumer_id[node];
		if(comsumer_id >= 0){
			result.push_back(path);
			result.back().push_back(comsumer_id);
			int remaining_requirement = mem.array_Consumer_remaining_requirement[comsumer_id];
			if(flow > remaining_requirement){
				result.back().push_back(remaining_requirement);
				flow -= remaining_requirement;
				mem.array_Consumer_remaining_requirement[comsumer_id] = 0;
			}
			else{
				result.back().push_back(flow);
				mem.array_Consumer_remaining_requirement[comsumer_id] -= flow;
				flow = 0;
			}
			result.back().push_back(ServerLvl);
		}
		
		int Esize = array_Vertex2Edge_len[node];
		int offset = array_Vertex2Edge_offset[node];
		for(int i = 0;i < Esize;i++){
			int edge = mem.array_Vertex_EdgesOut[offset + i];
			if(edge < 0)
			    break;//end of EdgesOut
#ifdef LEN_DBG
			cout << "node: " << node->id << " flow " 
			<< flow << " => e " << e->to->id << " x " << e->x << endl;
#endif
			if(flow <= 0)
		        return;
			if(mem.array_Edge_x[edge] > 0 && 
			mem.array_Edge_visited[edge] < mem.array_Edge_x[edge]){//还有空余带宽
#ifdef LEN_DBG
			    cout << "x " << edge.x << " visited " << edge.visited << endl;
#endif
			    int next_flow;
				if(flow > mem.array_Edge_x[edge] - mem.array_Edge_visited[edge])
				    next_flow = mem.array_Edge_x[edge] - mem.array_Edge_visited[edge];
				else
				    next_flow = flow;
				flow -= next_flow;//减去被分流的部
				mem.array_Edge_visited[edge] += next_flow;
				int next_node = mem.array_Edge_to[edge];
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
		for(int i = 0;i < mem.ENum;i++){
			if(!mem.raw_array_Edge_IsReversEdge[i]){// not pesudo source
			    sum += mem.raw_array_Edge_cost[i] * mem.raw_array_Edge_x[i];
			}
		}

		int Esize = array_Vertex2Edge_len[-1];
		int offset = array_Vertex2Edge_offset[-1];
		for(int i = 0;i < Esize;i++){
			int e = mem.array_Vertex_EdgesOut[offset + i];
			if(e < 0)
			    break;
			if(mem.array_Edge_x[e] > 0)
			    sum += get_ServerCost(mem.array_Edge_to[e],mem.array_Edge_x[e]);
		}
		return sum;
	}

	inline int get_ServerCost(int v,int demand){
		return const_array_Server_Cost[get_suitable_Server(demand)] 
		    + const_array_Vertex_Server_Cost[v];
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
		int Esize = array_Vertex2Edge_len[-1];
		int offset = array_Vertex2Edge_offset[-1];
		for(int i = 0;i < Esize;i++){
			int e = mem.array_Vertex_EdgesOut[offset + i];
			if(e < 0)
			    break;
			if(mem.array_Edge_x[e] > 0){//has some flow
			    real_source[mem.array_Edge_to[e]] 
				    = mem.array_Edge_x[e];
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
		return ret;
	}


	void cost_start_from_source(vector<int>& result,int path_cost,int node,int flow){
		int comsumer_id = mem.array_Vertex_consumer_id[node];
		if(comsumer_id >= 0){
			
			int remaining_requirement = mem.array_Consumer_remaining_requirement[comsumer_id];
			if(flow > remaining_requirement){
				result.push_back(path_cost * remaining_requirement);
				flow -= remaining_requirement;
				mem.array_Consumer_remaining_requirement[comsumer_id] = 0;
			}
			else{
				result.push_back(path_cost * flow);
				mem.array_Consumer_remaining_requirement[comsumer_id] -= flow;
				flow = 0;
			}
			    
		}
		
		int Esize = array_Vertex2Edge_len[-1];
		int offset = array_Vertex2Edge_offset[-1];
		for(int i = 0;i < Esize;i++){
			int edge = mem.array_Vertex_EdgesOut[offset + i];
			if(edge < 0)
			    break;//end of EdgesOut
			if(flow <= 0)
		        return;
			if(mem.array_Edge_x[edge] > 0 && 
			mem.array_Edge_visited[edge] < mem.array_Edge_x[edge]){//还有空余带宽
			    int next_flow;
				if(flow > mem.array_Edge_x[edge] - mem.array_Edge_visited[edge])
				    next_flow = mem.array_Edge_x[edge] - mem.array_Edge_visited[edge];
				else
				    next_flow = flow;
				flow -= next_flow;//减去被分流的部
				mem.array_Edge_visited[edge] += next_flow;
				int next_node = mem.array_Edge_to[edge];
				path_cost += mem.array_Edge_cost[edge];
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

	int* array_Vertex2Edge_offset;
	int* array_Vertex2Edge_len;

	int* raw_array_Vertex2Edge_offset;
	int* raw_array_Vertex2Edge_len;

	int* const_array_Vertex_Server_Cost;
	int* const_array_Server_Cost;
	int* const_array_Server_Ability;

	GraphMemory mem;
	GraphMemory backup_mem;
	GraphMemory global_min_mem;

    /*
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
	*/

private:
};
//#undef LEN_DBG
#endif//__DATA_STRUCTURE_H__