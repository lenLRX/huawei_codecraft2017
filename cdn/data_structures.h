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

//#define LEN_DBG

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
    Graph():i_counter(1){}

    void add_Edge(int id,
        int from,
	    int to,
	    int bandwidth,
	    int cost){
		auto& from_vertex = GetVertex(from);
		auto& to_vertex = GetVertex(to);
		E[id] = Edge(id,&from_vertex,&to_vertex,bandwidth,cost,-id);
		E[-id] = Edge(-id,&to_vertex,&from_vertex,0,-cost);
	}

	void add_Vertex(int id){
		V[id] = Vertex(id);
	}

	void add_Consumer(const int id,Vertex* fromVertex,const int requirement){
		C[id] = Consumer(id,fromVertex,requirement);
	}

	void VertexCompletement(){
		for(auto& p:E){
			auto& v_from = GetVertex(p.second.from->id);
			v_from.EdgesOut.push_back(&p.second);
			auto& v_to = GetVertex(p.second.to->id);
			v_to.EdgesIn.push_back(&p.second);
		}

		for(auto& cp:C){
			GetVertex(cp.second.fromVertex->id).d = - cp.second.requirement;
			GetVertex(cp.second.fromVertex->id).consumer_id = cp.second.id;
		}
	}

	Vertex& GetVertex(int id){//add if not exist
	    if(V.find(id) == V.end())
		    add_Vertex(id);
		return V[id];
	}

	int GetAnID(){
		return i_counter++;
	}

	void debug_print(){
		cout << V.size() << " " << E.size() / 2 << " " << C.size() << endl;
	}

	void start_from_source(vector<list<int>>& result,list<int> path,Vertex* node,int flow){

		if(node->consumer_id >= 0){
			result.push_back(path);
			result.back().push_back(node->consumer_id);
			Consumer& c = C.at(node->consumer_id);
			if(flow > c.remaining_requirement){
				result.back().push_back(c.remaining_requirement);
				flow -= c.remaining_requirement;
				c.remaining_requirement = 0;
			}
			else{
				result.back().push_back(flow);
				c.remaining_requirement -= flow;
				flow = 0;
			}
			    
		}
		
		for(auto e:node->EdgesOut){
#ifdef LEN_DBG
			cout << "node: " << node->id << " flow " << flow << endl;
#endif
			if(flow <= 0)
		        return;
			auto& edge = *e;
			if(edge.x > 0 && edge.visited < edge.x){//还有空余带宽
#ifdef LEN_DBG
			    cout << "x " << edge.x << " visited " << edge.visited << endl;
#endif
			    int next_flow;
				if(flow > edge.x - edge.visited)
				    next_flow = edge.x - edge.visited;
				else
				    next_flow = flow;
				flow -= next_flow;//减去被分流的部
				edge.visited += next_flow;
				auto& next_node = V.at(edge.to->id);
				auto _path_copy = path;
				_path_copy.push_back(next_node.id);
				
#ifdef LEN_DBG
				cout << " next node: " << next_node.id << " next_flow " << next_flow << endl;
#endif
				start_from_source(result,_path_copy,&next_node,next_flow);
			}
		}
	}

	int total_cost(){
		int sum = 0;
		for(auto& e:E){
			if(e.second.from >= 0 && e.second.id > 0){// not pesudo source
			    sum += e.second.cost * e.second.x;
			}
		}

		for(auto out_from:V[-1].EdgesOut){
			if(out_from->x > 0)
			    sum += ServerCost;
		}
		return sum;
	}

	string to_String(){
		vector<list<int>> result;
		map<int,int> real_source;
		for(auto pesudo_source_out:V.at(-1).EdgesOut){
			if(pesudo_source_out->x > 0){//has some flow
			    real_source[pesudo_source_out->to->id] = pesudo_source_out->x;
#ifdef LEN_DBG
				cout << "pesudo_source_out => " << pesudo_source_out->to->id << "  " << pesudo_source_out->x << endl;
#endif
			}
		}

		for(auto p : real_source){
			list<int> line;
			line.push_back(p.first);
			Vertex* tmp = &V.at(p.first);
			start_from_source(result,line,tmp,p.second);
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

    map<int,Edge> E;
	map<int,Vertex> V;
	map<int,Consumer> C;
	int EdgeNum;
	int VertexNum;
	int ConsumerNum;
	int ServerCost;
private:
    int i_counter;
};
#endif//__DATA_STRUCTURE_H__