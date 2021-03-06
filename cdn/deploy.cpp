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
#include "SimplexTest.h"
#include <random>
#include <algorithm>

void testLR();

void MatrixInversionTest(){
	SparseMatrix<float_type> Mat;
	Mat.rows = {
		{{0,3},{2,2}},
		{{0,2},{2,-2}},
		{{1,1},{2,1}}
	};
	cout << "origin" << endl;
	Mat.pretty_print(3);
	GaussJordanInversion(Mat);
	cout << "after inversion" << endl;
	Mat.pretty_print(3);
}

void deploy_server66(char * topo[MAX_EDGE_NUM], int line_num,char * filename){
	SimplexTest();
	//MatrixInversionTest();
}

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
	
	Graph G2;
	parse2(topo,line_num,G2);


	RSM_Model RSM(G2);
	RSM.init();
	//RSM.cut();
	RSM.BnB();
	//RSM.printSolution();

	

	write_result(RSM.to_String().c_str(), filename);
	
}

void deploy_server6666(char * topo[MAX_EDGE_NUM], int line_num,char * filename){
	Timer::getInstance().start();
	//Timer::getInstance().set(80);
	Timer::getInstance().set(88500);
	
	Graph G2;
	parse2(topo,line_num,G2);


	RSM_Model RSM(G2);
	RSM.init();
	//RSM.cut();
	//RSM.BnB();
	//RSM.printSolution();

	set<int> banlist = RSM.GetBanlist();
	RSM.A.rows.clear();

	
	Graph G;
	parse(topo,line_num,G);
	G.save();

	SSPA2 optimizer(G);

	FireflySolver solver(optimizer,20,0.01,1,G.VertexNum);
	
	for(auto& c:solver.consumer_map){
		c = true;
	}

	for(int ban:banlist){
		solver.consumer_map[ban] = false;
	}
    solver.optimize(10000000);

	cout << banlist.size() << endl;

	G.restore_globalmin();
	write_result(G.to_String().c_str(), filename);
	
}


//你要完成的功能总入口
