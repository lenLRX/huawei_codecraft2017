#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include "optimizer.h"
#include "Matrix.h"
#include <map>


class RSM_Model{
public:
    RSM_Model(Graph& G):G(G),opt_value(0.0){
		EdgeNum = G.EdgeNum;
		n = G.VertexNum * G.ServerLvlNum + EdgeNum;
		m = G.VertexNum * 2 + EdgeNum;

		mn = m + n;
	}

	RSM_Model Dual();

	void init();

	void SetBanlist(set<int> blist);

	void cut();


	void init_space();
	void init_slack();

	void mainLoop();

	void optimize();


    void AddVertexBalance(const vector<int>& EdgesIn,
	    const vector<int>& EdgesOut,int vid,int d);

    void SetupObjectFunc();

	int CalcCost();

	void addConstraint(vector<pair<int,double>> line,double rhs);

	int GetSmallest(const vector<double>& bbar);

    //offset,col
	pair<int,int> find_pivot_col(int pivot_row);
	
	void eliminateError();

	double CalcOptval();

	string to_String();

	void printSolution();

	set<int> GetBanlist();

	vector<double> x;

    SparseMatrix<double> A_origin;
	SparseMatrix<double> A_origin_col_major;
	SparseMatrix<double> A;

	vector<double> c_origin;

	vector<double> cbar;				// c

    vector<double> b_origin;

	vector<double> bbar;				// b

	vector<int> xb;
	vector<int> xn;						// x
	double t;									// for getting bbar
	set<int> banlist;

	double opt_value;

	int EdgeNum;
	int n;//num vars
	int m;//num constraints
	int mn;
	Graph& G;
};

#endif//__SIMPLEX_H__