#ifndef __BNB_H__
#define __BNB_H__

#include <vector>
#include <random>
#include <chrono>
#include <queue>
#include <set>

#include "optimizer.h"

using namespace std;

class branch
{
public:
    branch(int size){
		free_mask.resize(size);
		local_solution.resize(size);
		local_opt.resize(size);
		cost = numeric_limits<int>::max();
		free_num = size;
	}
    vector<int> free_mask;
	vector<int> local_solution;
	vector<float> local_opt;
	int cost;
	int free_num;
	bool operator < (const branch& other) const {
		return cost > other.cost;
	}
};

class BnB
{
public:
    BnB(Optimizer& opt,const size_t NodeNum):optimizer(opt),
	NodeNum(NodeNum),_0_1_distribution(uniform_int_distribution<int>(0,1)),
		_random_cell_distribution(uniform_int_distribution<int>(0,NodeNum - 1)),
		GlobalMin(numeric_limits<int>::max()),
		_float_distribution(uniform_real_distribution<float>(0,1))
	{}

	void init(int cost,vector<int> seed = vector<int>());

	void optimize();

	void step();

	void CostOfBranch(branch& _branch);

    Optimizer& optimizer;

    priority_queue<branch> Q;

    const size_t NodeNum;
    int GlobalMin;

    default_random_engine generator; 

	uniform_int_distribution<int> _0_1_distribution;
	uniform_int_distribution<int> _random_cell_distribution;
	uniform_real_distribution<float> _float_distribution;
};
#endif//__BNB_H__