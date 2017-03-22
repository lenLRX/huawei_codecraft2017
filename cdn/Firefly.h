#ifndef __FIREFLY_H__
#define __FIREFLY_H__

#include "LagrangianRelaxation.h"
#include <random>
#include <chrono>
#include <queue>
#include <set>

class Firefly
{
public:
    void resize(int n);
	vector<int> bits;
	vector<int> newbits;//更新后的基因放在这里，防止其他个体读取到新的基因
	int objective = 0;

	int count(){
		size_t bits_size = newbits.size();
		int c = 0;
		for(size_t i = 0;i < bits_size;i++){
			if(newbits[i])
			    c++;
		}
		return c;
	}
};

class MyDistance {
public:
	int site;
	int d_ij;
	MyDistance(int m_site, int m_dij) {
		site = m_site;
		d_ij = m_dij;
	}
	bool operator < (const MyDistance & a) const {
		return d_ij < a.d_ij;
	}
};

class FireflySolver
{
public:
    FireflySolver(LR& lr,const int population,
	    const float Gamma,const float Alpha,const size_t NodeNum):lr(lr),
		population(population),Gamma(Gamma),Alpha(Alpha),NodeNum(NodeNum),
		_0_1_distribution(uniform_int_distribution<int>(0,1)),
		_random_cell_distribution(uniform_int_distribution<int>(0,NodeNum - 1)),
		GlobalMin(numeric_limits<int>::max()),
		_float_distribution(uniform_real_distribution<float>(0,1)){
			consumer_map = vector<int>(NodeNum,false);
			for(auto cp:lr.G.C){
				consumer_map[cp.second.fromVertex->id] = true;
				cout << cp.second.fromVertex->id << endl;
			}
			Fireflies.resize(population);
			for(auto& fly:Fireflies){
				fly.resize(NodeNum);
				Randomize(fly);
			}

			for(size_t i = 0;i < NodeNum;i++){
				Fireflies[0].bits[i] = 0;
			}

			for(const auto& cp:lr.G.C){
				Fireflies[0].bits[cp.second.fromVertex->id] = 1;
			}
		}
	
	void UpdateObjectiveAndBestFly();

	void CostOfFly(Firefly& fly);

	void Randomize(Firefly& fly);

	void Beta_step(Firefly& fly1,Firefly& fly2);

	void Alpha_step(Firefly& fly);
	
	void Alpha2_step(Firefly& fly_dull,Firefly& fly);
	
	int Get_Closer(vector<Firefly> Fireflies,int site,int k);

	void optimize();

	LR& lr;
	
	const int population;
	const float Gamma;
	const float Alpha;
	const size_t NodeNum;

	int minServerNum;

	int Fmin;
	int Fminpos;
	int GlobalMin;

	string result;

	vector<Firefly> Fireflies;

	set<vector<int>> tabu;

	vector<int> consumer_map;

	default_random_engine generator;

	chrono::high_resolution_clock::time_point start_time;

	uniform_int_distribution<int> _0_1_distribution;
	uniform_int_distribution<int> _random_cell_distribution;
	uniform_real_distribution<float> _float_distribution;
};

#endif//__FIREFLY_H__