#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include "optimizer.h"
#include <map>

template<typename T = double>
class SparseMatrix{
public:
    SparseMatrix()=default;
    SparseMatrix(int row_num){
		rows.resize(row_num);
	}

    vector<map<int,T>> rows;
};

class RSM_Model{
public:
    RSM_Model(Graph& G):G(G){
		EdgeNum = G.EdgeNum;
		n = G.VertexNum * G.ServerLvlNum + EdgeNum;
		m = G.VertexNum * 2 + EdgeNum;

		mn = m + n;
	}

	RSM_Model Dual();

	void init();
	void init_space();
	void init_slack();

	void optimize();


    void AddVertexBalance(const vector<int>& EdgesIn,
	    const vector<int>& EdgesOut,int vid,int d);


    void SetupObjectFunc();


	int GetSmallest(const vector<double>& bbar);
	int find_pivot_col(int pivot_row);

	vector<double> x;
    vector<double> c;


    //vector<vector<double>> A;
	SparseMatrix<double> A;
	vector<double> b;

	vector<double> cbar;				// c
	vector<double> abarj;	// A
	//vector<vector<double>> Ab;							// B
	vector<double> Ab;
	//vector<vector<double>> e;								// E
	vector<double> e;
	//vector<vector<double>> etaFile;//Eta File
	vector<double> bbar;				// b
	vector<double> y;										// y
	vector<int> xb;
	vector<int> xn;						// x
	double t;									// for getting bbar

    int MAX_ITERATIONS = 1000;
	int EdgeNum;
	int n;//num vars
	int m;//num constraints
	int mn;
	int BigM = 10000000;
	Graph& G;
};

#endif//__SIMPLEX_H__