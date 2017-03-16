#ifndef __FIREFLY_H__
#define __FIREFLY_H__

#include "LagrangianRelaxation.h"
#include <random>
#include <chrono>

class Firefly
{
public:
    void resize(int n);
	vector<bool> bits;
	vector<bool> newbits;//更新后的基因放在这里，防止其他个体读取到新的基因
	int objective = 0;
};

class FireflySolver
{
public:
    FireflySolver(const LR& lr,const int population,
	    const float Gamma,const float Alpha,const size_t NodeNum):lr(lr),
		population(population),Gamma(Gamma),Alpha(Alpha),NodeNum(NodeNum),
		_0_1_distribution(uniform_int_distribution<int>(0,1)),
		_random_cell_distribution(uniform_int_distribution<int>(0,NodeNum - 1)),
		GlobalMin(numeric_limits<int>::max()),
		_float_distribution(uniform_real_distribution<float>(0,1)){
			Fireflies.resize(population);
			for(auto& fly:Fireflies){
				fly.resize(NodeNum);
				Randomize(fly);
			}

			for(size_t i = 0;i < NodeNum;i++){
				Fireflies[0].bits[i] = 0;
			}

			for(const auto& cp:lr.OriginalGraph.C){
				Fireflies[0].bits[cp.second.fromVertex] = 1;
			}
		}
	
	void UpdateObjectiveAndBestFly();

	void CostOfFly(Firefly& fly);

	void Randomize(Firefly& fly);

	void Beta_step(Firefly& fly1,Firefly& fly2);

	void Alpha_step(Firefly& fly);

	void optimize();

	LR lr;

	const int population;
	const float Gamma;
	const float Alpha;
	const size_t NodeNum;

	int Fmin;
	int Fminpos;
	int GlobalMin;

	string result;

	vector<Firefly> Fireflies;

	default_random_engine generator;

	chrono::high_resolution_clock::time_point start_time;

	uniform_int_distribution<int> _0_1_distribution;
	uniform_int_distribution<int> _random_cell_distribution;
	uniform_real_distribution<float> _float_distribution;
};

#endif//__FIREFLY_H__