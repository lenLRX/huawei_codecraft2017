#include <chrono>
#include "deploy.h"
#include <stdio.h>
#include "data_structures.h"
#include "parse.h"
#include "Relax.h"
/*
#include "SSPA2.h"

#include "Firefly.h"
*/
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

    
	Relax init_optimizer(G);
	init_optimizer.optimize();
	/*
	SSPA2 optimizer(G);

	FireflySolver solver(optimizer,20,0.001,1,G.V.size());
	solver.init(init_optimizer.get_result());
	for(auto& c:solver.consumer_map){
		c = true;
	}
	solver.optimize(100);
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
	
	
	cout << "visited size " << solver.tabu.size() << " " << optimizer.counter << endl;
	write_result(solver.result.c_str(), filename);
	*/
}


//你要完成的功能总入口


#ifdef USE_LEGACY
void deprecated_deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	
	Graph G = parse(topo,line_num);
	//G.debug_print();
	LR optimizer(G);
	unordered_set<int> exclude;

	Graph minGraph;
	int minCost = numeric_limits<int>::max();

	auto start_time = chrono::high_resolution_clock::now();
	default_random_engine generator;

	vector<int> ids(optimizer.OriginalGraph.V.size());

	for(int i = 0;i < ids.size();i++){
		ids[i] = i;
	}
	optimizer.refresh();
	optimizer.dijkstraPrepare();

	//for(auto& vp:optimizer.OriginalGraph.V)
	for(int i = optimizer.OriginalGraph.C.size() - 2;i > 0 ;i--)
	{
		for(int times = 0;times < 10;){
			exclude.clear();

			
		    vector<double> weight(optimizer.OriginalGraph.V.size());
			for(auto& pv:optimizer.OriginalGraph.V){
				/*
				int sum = 0;
				for(int e_no:pv.second.EdgesOut){
					sum += optimizer.OriginalGraph.E.at(e_no).bandwidth
					/optimizer.OriginalGraph.E.at(e_no).cost;
				}
				*/

				weight[pv.first] = pv.second.weight;
				cout << pv.first << " => " << pv.second.weight << endl;
			}

			discrete_distribution<int> distribution(weight.begin(),weight.end());
			

			

			//random_shuffle(ids.begin(),ids.end());

			for(int j = 0;j < i;j++){
				//exclude.insert(ids[j]);
				int ex = distribution(generator);
				exclude.insert(ex);
			}


			auto time_now = chrono::high_resolution_clock::now();
			if(chrono::duration_cast<chrono::seconds>(time_now - start_time).count() > 80)
				break;
			//exclude.insert(vp.first);
			optimizer.refresh();
			optimizer.create_pesudo_source(exclude);
			//optimizer.rand_a_source();
			bool b = optimizer.optimize();
			if(!b)
				continue;
			//optimizer.check();
			optimizer.updatePesudoCost();
			int cost = optimizer.G.total_cost();
			if(cost < minCost){
				minCost = cost;
				minGraph = optimizer.G;
			}
			cout << optimizer.G.total_cost() << endl;

			int max_id = -1;
			int max_cost = 0;

			for(auto& pc:optimizer.pesudoCost){
				if(pc.second > 0 && pc.second > max_cost){
					max_cost = pc.second;
					max_id = pc.first;
				}
			}

			times++;

			//exclude.insert(max_id);

			//string result = optimizer.G.to_String();
			//cout << result << endl;
		}
		
	}

    /*
	for(auto& e:optimizer.G.E){
		if(e.second.x > 0)
		    cout << e.second.from << " => " << e.second.to << " x: " << e.second.x << endl;
	}
	*/
	
	// 需要输出的内容
	char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

    //testLR();


	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	//write_result(minGraph.to_String().c_str(), filename);

}

void testLR(){
	Graph G;
	G.VertexNum = 4;
	G.EdgeNum = 6;
	G.add_Edge(G.GetAnID(),1,2,2,5);
	G.add_Edge(G.GetAnID(),2,4,1,2);
	G.add_Edge(G.GetAnID(),1,3,2,1);
	G.add_Edge(G.GetAnID(),3,4,5,0);
	G.add_Edge(G.GetAnID(),2,3,3,4);
	G.add_Edge(G.GetAnID(),3,2,2,3);

    //TODO: use initializer list
    Vertex v1(1);
	v1.EdgesOut.insert(1);
	v1.EdgesIn.insert(-1);
	v1.EdgesOut.insert(3);
	v1.EdgesIn.insert(-3);
	//v1.d = 3;
	G.V[1] = v1;

	Vertex v2(2);
	v2.EdgesOut.insert(2);
	v2.EdgesIn.insert(-2);

	v2.EdgesOut.insert(5);
	v2.EdgesIn.insert(-5);

	v2.EdgesIn.insert(1);
	v2.EdgesOut.insert(-1);

	v2.EdgesIn.insert(6);
	v2.EdgesOut.insert(-6);
	//v2.d = 2;
	G.V[2] = v2;

	Vertex v3(3);
	v3.EdgesOut.insert(4);
	v3.EdgesIn.insert(-4);

	v3.EdgesOut.insert(6);
	v3.EdgesIn.insert(-6);

	v3.EdgesIn.insert(3);
	v3.EdgesOut.insert(-3);

	v3.EdgesIn.insert(5);
	v3.EdgesOut.insert(-5);

	v3.d = -1;
	G.V[3] = v3;

	Vertex v4(4);
	v4.EdgesIn.insert(2);
	v4.EdgesOut.insert(-2);

	v4.EdgesIn.insert(4);
	v4.EdgesOut.insert(-4);

	v4.d = -4;
	G.V[4] = v4;

	Vertex v5(5);
	//v5.EdgesIn.insert(7);
	//v5.EdgesOut.insert(-7);
	G.V[5] = v5;

	LR optimizer(G);
	unordered_set<int> exclude;
	exclude.insert(1);
	optimizer.refresh();
	optimizer.create_pesudo_source();
	optimizer.optimize();
	optimizer.check();
	cout << "test optimize done" << endl;

	for(int i = 1;i < 5;i++){
		cout << "G.V[" << i << "]= " << optimizer.G.V[i].d << endl;
	}

	for(auto& e:optimizer.G.E){
		cout << e.second.from << " => " << e.second.to << " x: " << e.second.x << endl;
	}
}
#endif