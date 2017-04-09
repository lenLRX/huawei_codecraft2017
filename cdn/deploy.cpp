#include <chrono>
#include "deploy.h"
#include <stdio.h>
#include "data_structures.h"
#include "parse.h"
#include "Relax.h"

#include "SSPA2.h"

#include "Firefly.h"

#include "Timer.h"
#include <random>
#include <algorithm>

void testLR();

/*
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename){
	Graph G = parse(topo,line_num);
	//LR optimizer(G);
	Relax optimizer(G);

	optimizer.optimize();

    cout << "cost " << optimizer.G.total_cost() << endl;

	write_result(optimizer.G.to_String().c_str(), filename);
}
*/



void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename){
	Timer::getInstance().start();
	//Timer::getInstance().set(80);
	Timer::getInstance().set(88500);
	Graph G;
	parse(topo,line_num,G);
	G.save();

    
	//Relax init_optimizer(G);
	//init_optimizer.optimize();
	
	
	SSPA2 optimizer(G);

	FireflySolver solver(optimizer,20,0.001,1,G.VertexNum);
	//solver.init(init_optimizer.get_result());
	for(auto& c:solver.consumer_map){
		c = true;
	}
	solver.optimize(100000);
	
	/*
	auto test = init_optimizer.postOptimize();
	for(auto id:test){
		cout << id << endl;
		solver.consumer_map[id] = true;
	}
	*/
	/*
	for(auto& c:solver.consumer_map){
		c = true;
	}
	solver.optimize(100000);
	*/
	
	cout << "visited size " << solver.tabu.size() << " " << optimizer.counter << endl;
	G.restore_globalmin();
	write_result(G.to_String().c_str(), filename);
	//write_result(G.to_String().c_str(),filename);
}


//你要完成的功能总入口
