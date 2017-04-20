#include "Simplex.h"
#include <cassert>
#include <cmath>

void RSM_Model::init(){
	for(int v = 0;v < G.VertexNum;v++){
		vector<int> EdgesIn,EdgesOut;
		int Esize = G.array_Vertex2Edge_len[v];
		int offset = G.array_Vertex2Edge_offset[v];
		for(int i = 0;i < Esize;i++){
			int e = G.mem.array_Vertex_EdgesOut[offset + i];
			if(e < 0)
			    break;
			else
			    EdgesOut.push_back(e);
		}

		for(int i = 0;i < Esize;i++){
			int e = G.mem.array_Vertex_EdgesIn[offset + i];
			if(e < 0)
			    break;
			else
			    EdgesIn.push_back(e);
		}
		AddVertexBalance(EdgesIn,EdgesOut,v,-G.mem.array_Vertex_d[v]);
	}
	SetupObjectFunc();
}

void RSM_Model::AddVertexBalance(const vector<int>& EdgesIn,
    const vector<int>& EdgesOut,int vid,int d){

	for(int in:EdgesIn){
		A[vid][in] = -1;
	}

	for(int out:EdgesOut){
		A[vid][out] = 1;
	}

	for(int i = EdgeNum + vid * G.ServerLvlNum;
	    i < EdgeNum + (vid + 1) * G.ServerLvlNum;i++){
		A[vid][i] = -1;
	}

	b[vid] = -d;
}

void RSM_Model::SetupObjectFunc(){
	int i = 0;
	for(;i < EdgeNum;i++){
		cbar[i] = c[i] = G.mem.array_Edge_cost[i];
		xn[i] = i + 1;

        //UB of Edge
		A[i][G.VertexNum + i] = 1;
		b[G.VertexNum + i] = G.mem.array_Edge_bandwidth[i];
	}

	for(;i < EdgeNum + G.VertexNum;i++){
		cbar[i] = c[i] = G.const_array_Vertex_Server_Cost[i - EdgeNum]
		    + G.const_array_Server_Cost[G.ServerLvlNum - 1];
		xn[i] = i + 1;
	}

	for(int i = 0;i < G.VertexNum;i++){
		for(int j = 0;j < G.ServerLvlNum;j++){
			A[j][G.VertexNum + EdgeNum + i] = 1;
		}
		//no more than 1 server in a vertex
		b[G.VertexNum + EdgeNum + i] = 1;
	}

	for(int i = n;i < mn;i++){
		xb[i - n] = i + 1;
	}

	for (int i = 0; i < m; i++) { // A & b
	    if(b[i] > 0){
			A[i + n][i] = 1;
		    Ab[i][i] = 1;
		}
		else{
			//reverse Sign
			for(int j = 0;j < n;j++){
				A[j][i] *= -1;
			}

			A[i + n][i] = -1;
		    Ab[i][i] = -1;
			b[i] *= -1;
		}
		
	}	
	bbar = b;


}

void RSM_Model::optimize(){
	// Work
	bool finished = false;
	size_t iteration = 1;
	int enter, leave, col;

	while (!finished && iteration <= MAX_ITERATIONS	) {		

		enter = GetHighest(cbar);
		if (enter == -1) { // if none, we're done here
			double z = IP(y, b);
			
			cout << endl << endl << "Optimal value of ";
			cout << z << " has been reached." << endl;

            /*
			cout << "Original:";
			for (size_t i = 1; i <= n; i++) {
				cout << "\tx" << i << "=";
				cout << determVar(i, xb, xn, bbar);
			}

			cout << endl << "Slack:\t";
			for (size_t i = n+1; i <= mn; i++) {
				cout << "\tx" << i << "=";
				cout << determVar(i, xb, xn, bbar);
			}
			cout << endl;
			*/
			finished = true;
			break;
		}		
		cout << endl <<"Entering variable is x" << xn[enter] << endl;

		// update d (abarj)
		abarj = A[xn[enter] - 1];
		double vp;
		for (int g = 0; g < (int)iteration-1; g++) {
			col = (int)etaFile[g][m];
			vp = abarj[col] / etaFile[g][col];
			abarj[col] = vp;			

			for (size_t i = 0; i < m; i++) {
				if (i != col) {
					abarj[i] -= etaFile[g][i] * abarj[col];
				}
			}
		}
		/*
		cout << "abarj =\t";
		printVector(abarj);
		*/

		// Determine leaving variable (where 7/10 people mess up)
		leave = ratioLeaving(bbar, abarj, xb);
		if (leave == -1) {
			finished = true;
			cout << "Solution is unbounded" << endl;			
			break;
		}
		t = bbar[leave] / abarj[leave];

		cout << "Leaving variable is x" << xb[leave] << endl;

		// Print new E col
		//cout << "E" << iteration << " = column " << leave+1 << ":\t";
		//printVector(abarj);

		// Update eta file (first input is E1 not E0)
		for (size_t i = 0; i < abarj.size(); i++) {
			etaFile[iteration - 1][i] = abarj[i];
		}
		etaFile[iteration - 1][m] = leave;
			
		swap(xb[leave], xn[enter]); // Swap entering and leaving

		// Update y
		for (size_t i = 0; i < m; i++) { 
			y[i] = c[xb[i] - 1];
		}		
		
		for (int g = iteration - 1; g >= 0; g--) {	
			col = (int)etaFile[g][m];			
			for (size_t i = 0; i < m; i++) {
				if (i != col) {
					y[col] -= etaFile[g][i] * y[i];
				}
			}

			y[col] /= etaFile[g][col];			
		}

		// Update bbar		
		for (size_t i = 0; i < m; i++) {			
			bbar[i] -= t * abarj[i];
			if (fabs(bbar[i]) < 0.001) {
				bbar[i] = 0;
			}
			if (bbar[i] < 0) { // Sign constraints
				finished = true;
				cout << "The linear program is infeasible:" << endl;
				break;
			}
		}
		bbar[leave] = t;			

		// Update cbar		
		for (size_t j = 0; j < n; j++) {
			cbar[j] = c[xn[j] - 1] - IP(y, A[xn[j] - 1]);			
		}		

		// Increase iteration for y and d updates and max iterations
		iteration++; 
	}	
}

//innerProduct
double RSM_Model::IP(const vector<double>& a,const vector<double>& b) {
	double result = 0;	
	for (size_t h = 0; h < b.size(); h++) {
		result += a[h] * b[h];
	}	

	return result;
}

int RSM_Model::GetHighest(const vector<double>& cbar) {
	int result = -1;
	double highest = 0;

	for (size_t i = 0; i < cbar.size(); i++) {
		if (cbar[i] > highest) {
			highest = cbar[i];
			result = i;
		}		
	}

	return result;
}

int RSM_Model::ratioLeaving(const vector<double>& b,
    const vector<double>& aj, const vector<int>& xb){
	int result = -1;
	double lowest = 0;
	double ratio;
	bool first = true;
	
	//cout << "ratio";
	for (size_t j = 0; j < b.size(); j++) {
		if (aj[j] > 0) {
			ratio = b[j] / aj[j];
			//cout << "\tx" << xb[j] << " " << ratio;
			
			if (ratio < lowest || first) {				
				lowest = ratio;
				result = j;
				first = false;
			}
		}
	}
	//cout << endl;

	return result;
}