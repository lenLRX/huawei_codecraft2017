#include <chrono>
#include "deploy.h"
#include <stdio.h>
#include "data_structures.h"
#include "parse.h"
#include "Relax.h"

#include "SSPA2.h"

#include "Firefly.h"

#include "Timer.h"
#include "zkw.h"
#include "BnB.h"
#include "Simplex.h"
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



void deploy_server2(char * topo[MAX_EDGE_NUM], int line_num,char * filename){
	Timer::getInstance().start();
	//Timer::getInstance().set(80);
	Timer::getInstance().set(88500);
	Graph G;
	parse(topo,line_num,G);
	G.save();

    
	Relax init_optimizer(G);
	int cost = init_optimizer.optimize();
	
	
	SSPA2 optimizer(G);

	BnB solver(optimizer,G.VertexNum);
	solver.init(cost,init_optimizer.get_result());
	solver.optimize();

    /*
	FireflySolver solver(optimizer,20,0.01,1,G.VertexNum);
	solver.init(init_optimizer.get_result());
	
	for(auto& c:solver.consumer_map){
		c = true;
	}
	
	solver.optimize(100000);
	*/
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
	
	//cout << "visited size " << solver.tabu.size() << " " << optimizer.counter << endl;
	G.restore_globalmin();
	write_result(G.to_String().c_str(), filename);
	//write_result(G.to_String().c_str(),filename);
}


void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename){
	Timer::getInstance().start();
	//Timer::getInstance().set(80);
	Timer::getInstance().set(88500);
	/*
	Graph G2;
	parse2(topo,line_num,G2);


	RSM_Model RSM(G2);
	RSM.init();
	//RSM.cut();
	auto banlist = RSM.GetBanlist();
	*/
	/*
	vector<pair<int,double >> constraint;
	for(int i = 0;i < G2.VertexNum * G2.ServerLvlNum;i++){
		constraint.push_back(pair<int,double>(G2.EdgeNum + i,-1));
	}
	RSM.addConstraint(constraint,-53);
	RSM.optimize();
	*/
	

	//cout << "cost " << RSM.CalcCost() << endl;

	//write_result(RSM.to_String().c_str(), filename);

	

	Graph G;
	parse(topo,line_num,G);
	G.save();

	

	SSPA2 optimizer(G);

	FireflySolver solver(optimizer,20,0.01,1,G.VertexNum);

	Relax init_optimizer(G);
	//int cost = init_optimizer.optimize();
	//solver.init(init_optimizer.get_result());

	for(auto& c:solver.consumer_map){
		c = true;
	}

	//for(int b:banlist){
	//	solver.consumer_map[b] =false;
	//}

	
	
	solver.optimize(100000);
	
	G.restore_globalmin();
	//cout << "baned num " << banlist.size() << endl;
	write_result(G.to_String().c_str(),filename);
	//write_result(RSM.to_String().c_str(), filename);
	
}


//你要完成的功能总入口
