#ifndef __PARSE_H__
#define __PARSE_H__

#include <sstream>
#include <string>

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

	parse_first_line(G,graph[0]);
	i = 2;

	for(;;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		if(s.length() < 3)
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
		if(s.length() < 2)
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
		if(s.length() < 4)
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