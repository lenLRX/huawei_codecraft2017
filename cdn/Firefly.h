#ifndef __FIREFLY_H__
#define __FIREFLY_H__

#include "LagrangianRelaxation.h"
#include <random>

class Firefly
{
public:
    void resize(int n);
	vector<bool> bits;
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

	vector<Firefly> Fireflies;

	default_random_engine generator;

	uniform_int_distribution<int> _0_1_distribution;
	uniform_int_distribution<int> _random_cell_distribution;
	uniform_real_distribution<float> _float_distribution;
};

#endif//__FIREFLY_H__