#include "SimplexTest.h"
#include "Simplex.h"

void SimplexTest(){
	/*
	min f = 4x1 + 5x2
    x1 + 4x2 >= 5       (1)
    3x1 + 2x2 >= 7      (2)
    x1 >= 0, x2 >= 0, both integer.
	*/
	Graph Dummy;//useless, my bad
	RSM_Model model(Dummy);
	model.m = 0;//addConstraint will increase it!
	model.n = 2;
	model.mn = 2;
	model.cbar = vector<double>{-4,-5};
	vector<pair<int,double>> constraint1;
	constraint1.push_back(pair<int,double>(0,-1));
	constraint1.push_back(pair<int,double>(1,-4));
	model.addConstraint(constraint1,-5);
	vector<pair<int,double>> constraint2;
	constraint2.push_back(pair<int,double>(0,-3));
	constraint2.push_back(pair<int,double>(1,-2));
	model.addConstraint(constraint2,-7);

	model.cut();
}