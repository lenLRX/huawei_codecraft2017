#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include "optimizer.h"

class SparseMatrix{
public:

};

class RSM_Model{
public:
    RSM_Model(Graph& G):G(G){
		EdgeNum = G.EdgeNum * 2;
		n = G.VertexNum * G.ServerLvlNum + EdgeNum;
		m = G.VertexNum * 2 + EdgeNum;

		mn = m + n;
		//A = vector<vector<double>>(mn, vector<double>(m));
		A = vector<double>(mn*m);

		b = vector<double>(m);
		c = vector<double>(mn,0.0);
		cbar = vector<double>(n);
		abarj = vector<double>(m);
		//Ab = vector<vector<double>>(m, vector<double>(m));
		Ab = vector<double>(m * m);
		//e = vector<vector<double>>(m, vector<double>(m));
		e = vector<double>(m * m);
		//etaFile = vector<vector<double>>(MAX_ITERATIONS, vector<double>(m + 1));
		etaFile = vector<double>(MAX_ITERATIONS * (m + 1));
		bbar = vector<double>(m);
		y = vector<double>(m);
		xb = vector<int>(m);
		xn = vector<int>(n);
	}

	void init();

	void optimize();


    void AddVertexBalance(const vector<int>& EdgesIn,
	    const vector<int>& EdgesOut,int vid,int d);


    void SetupObjectFunc();

	double IP(const vector<double>& a,const vector<double>& b);

	int GetHighest(const vector<double>& cbar);

	int ratioLeaving(const vector<double>& b,
        const vector<double>& aj, const vector<int>& xb);

	vector<double> x;
    vector<double> c;


    //vector<vector<double>> A;
	vector<double> A;
	vector<double> b;

	vector<double> cbar;				// c
	vector<double> abarj;	// A
	//vector<vector<double>> Ab;							// B
	vector<double> Ab;
	//vector<vector<double>> e;								// E
	vector<double> e;
	//vector<vector<double>> etaFile;//Eta File
	vector<double> etaFile;
	vector<double> bbar;				// b
	vector<double> y;										// y
	vector<int> xb;
	vector<int> xn;						// x
	double t;									// for getting bbar

    int MAX_ITERATIONS = 100;
	int EdgeNum;
	int n;//num vars
	int m;//num constraints
	int mn;
	int BigM;
	Graph& G;
};

#endif//__SIMPLEX_H__