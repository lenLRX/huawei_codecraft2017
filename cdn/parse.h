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
	G.V.reserve(G.VertexNum);
	G.EdgeNum = GetNextInt(ss) * 2;//bidirection!
	G.E.reserve(G.EdgeNum);
	G.ConsumerNum = GetNextInt(ss);
	G.C.reserve(G.ConsumerNum);
}

Graph parse(char* graph[MAX_EDGE_NUM], int line_num){
	Graph G;
	int EdgeCounter = 0;
	int i = 0;

	parse_first_line(G,graph[0]);
	string tmp(graph[2]);
	stringstream ss(tmp);
	G.ServerCost = GetNextInt(ss);
	int halfp = G.EdgeNum / 2 + 4;
	cout << halfp << endl;
	for(i = 4;i < halfp;i++){
		string line(graph[i]);
		stringstream ss(line);
		string s;
		ss >> s;
		int from = stoi(s);
		ss >> s;
		int to = stoi(s);
		ss >> s;
		int bandwidth = stoi(s);
		ss >> s;
		int cost = stoi(s);

		G.add_Edge(EdgeCounter,from,to,bandwidth,cost);
		EdgeCounter++;
		G.add_Edge(EdgeCounter,to,from,bandwidth,cost);
		EdgeCounter++;
	}

	for(i = halfp + 1;i < line_num;i++){
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

	G.VertexCompletement();

	return G;
}

#endif//__PARSE_H__