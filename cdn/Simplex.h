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

	void BnB();


	void init_space();
	void init_slack();

	void mainLoop();

	bool optimize();


    void AddVertexBalance(const vector<int>& EdgesIn,
	    const vector<int>& EdgesOut,int vid,int d);

    void SetupObjectFunc();

	int CalcCost();

	void addConstraint(vector<pair<int,float_type>> line,float_type rhs);

	int GetSmallest(const vector<float_type>& bbar);

    //offset,col
	pair<int,int> find_pivot_col(int pivot_row);
	
	void eliminateError();

	float_type CalcOptval();

	string to_String();

	void printSolution();

	set<int> GetBanlist();

	vector<float_type> x;

    SparseMatrix<float_type> A_origin;
	SparseMatrix<float_type> A_origin_col_major;
	SparseMatrix<float_type> A;

	vector<float_type> c_origin;

	vector<float_type> cbar;				// c

    vector<float_type> b_origin;

	vector<float_type> bbar;				// b

	vector<int> xb;
	vector<int> xn;						// x
	float_type t;									// for getting bbar
	set<int> banlist;

	float_type opt_value;

	int EdgeNum;
	int n;//num vars
	int m;//num constraints
	int mn;
	Graph& G;
};

#endif//__SIMPLEX_H__