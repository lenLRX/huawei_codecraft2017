#ifndef __PARSE_H__
#define __PARSE_H__

#include <sstream>
#include <string>
#include <cassert>

#include "data_structures.h"
#include "lib_io.h"

using namespace std;

int GetNextInt(stringstream& ss){
	string seg;
	ss >> seg;
	return stoi(seg);
}

void parse_first_line(Graph& G,char* line){
	stringstream ss(line);
	G.VertexNum = GetNextInt(ss);
	//G.V.reserve(G.VertexNum);
	G.EdgeNum = GetNextInt(ss) * 2;//bidirection!
	//G.E.reserve(G.EdgeNum);
	G.ConsumerNum = GetNextInt(ss);
	//G.C.reserve(G.ConsumerNum);
}

void parse(char* graph[MAX_EDGE_NUM], int line_num,Graph& G){
	int i = 0;
	//1st pass
	parse_first_line(G,graph[0]);

	G.raw_array_Vertex2Edge_len = new int[G.VertexNum + 1];
	G.raw_array_Vertex2Edge_offset = new int[G.VertexNum + 1];

	G.array_Vertex2Edge_len = G.raw_array_Vertex2Edge_len + 1;
	G.array_Vertex2Edge_offset = G.raw_array_Vertex2Edge_offset + 1;

	i = 2;

	for(;;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		if(line.length() < 3)
		    break;
		ss >> s;
		int lvl_id = stoi(s);
		G.ServerLvlNum++;
		ss >> s;
		G.const_array_Server_Ability[lvl_id] = stoi(s);
		ss >> s;
		G.const_array_Server_Cost[lvl_id] = stoi(s);
	}

	i++;

	for(;;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		if(line.length() < 3)
		    break;
	}

	i++;

	vector<int> EdgeCounter(G.VertexNum,0);

	for(;;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		if(line.length() < 4)
		    break;
		ss >> s;
		int from = stoi(s);
		ss >> s;
		int to = stoi(s);
		ss >> s;
		int bandwidth = stoi(s);
		ss >> s;
		int cost = stoi(s);
		EdgeCounter[from]++;
		EdgeCounter[to]++;
	}

	G.array_Vertex2Edge_len[-1] = G.VertexNum * G.ServerLvlNum * 2;
	G.array_Vertex2Edge_offset[-1] = 0;

	for(int i = 0;i < G.VertexNum;i++){
		G.array_Vertex2Edge_len[i] = (EdgeCounter[i] + G.ServerLvlNum) * 2;
		G.array_Vertex2Edge_offset[i] = 
		    G.array_Vertex2Edge_offset[i - 1] + G.array_Vertex2Edge_len[i - 1];
	}



    //2nd pass
	i = 0;

	parse_first_line(G,graph[0]);
	i = 2;

	int Edge_slot_num = G.EdgeNum * 2 + G.VertexNum * 2 * G.ServerLvlNum * 2;

	G.mem.init(G.VertexNum + 1,
	    Edge_slot_num,
		G.ConsumerNum);
	
	cout << Edge_slot_num
	<< " <=> " << G.array_Vertex2Edge_offset[G.VertexNum - 1] + G.array_Vertex2Edge_len[G.VertexNum - 1]<< endl;

	G.init(G.mem);
	
	G.backup_mem.init(G.VertexNum + 1,
	    Edge_slot_num,
		G.ConsumerNum);
	
	G.global_min_mem.init(G.VertexNum + 1,
	    Edge_slot_num,
		G.ConsumerNum);

	for(;;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		if(line.length() < 3)
		    break;
	}

	i++;

	for(;;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		if(line.length() < 3)
		    break;
		ss >> s;
		int id = stoi(s);
		ss >> s;
		G.const_array_Vertex_Server_Cost[id] = stoi(s);
	}

	i++;

	for(;;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		if(line.length() < 4)
		    break;
		ss >> s;
		int from = stoi(s);
		ss >> s;
		int to = stoi(s);
		ss >> s;
		int bandwidth = stoi(s);
		ss >> s;
		int cost = stoi(s);

		G.add_Edge(from,to,bandwidth,cost);
		G.add_Edge(to,from,bandwidth,cost);
	}

	i++;

	for(;i < line_num;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		ss >> s;
		int ConsumerID = stoi(s);
		ss >> s;
		int fromID = stoi(s);
		ss >> s;
		int requirement = stoi(s);

		G.add_Consumer(ConsumerID,fromID,requirement);
	}
}

#endif//__PARSE_H__