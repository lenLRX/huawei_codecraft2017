#include "Simplex.h"
#include <cassert>
#include <cmath>

static const double epsilon1 = 0.00001;
static const double epsilon2 = 0.00000001;

void printX(vector<int> x) {
	cout << "{";
	for (size_t j = 0; j < x.size(); j++) {
		cout << " x" << x[j];
	}
	cout << " }";
}

void printVector(vector<double> v) {
	for (size_t f = 0; f < v.size(); f++) {
		cout << v[f] << "\t";
	}
	cout << endl;
}

double determVar(int number, vector<int> xb, vector<int> xn, 
	vector<double> bbar) {	

	for (size_t i = 0; i < xn.size(); i++) {
		if (xn[i] == number)
			return 0;
	}
	for (size_t j = 0; j < xb.size(); j++) {
		if (xb[j] == number) {
			if (fabs(bbar[j]) > 0.001) {
				return bbar[j];
			}
			else {
				return 0;
			}
		}
	}

	return -1; // Will be obvious if a mistake
}

RSM_Model RSM_Model::Dual(){
	
	RSM_Model dual(G);
	/*
	dual.m = n;
	dual.n = m;
	dual.mn = m + n;

	dual.init_space();
	for(int i = 0;i < dual.b.size();i++){
		dual.b[i] = c[i];
	}
	
	for(int i = 0;i < b.size();i++){
		dual.c[i] = b[i];
	}
	//dual.c = b;

	for (size_t row = 0; row < m; ++row) {
        for (size_t col = 0; col < n; ++col) {
            dual.A[row * dual.m + col] = A[col * (m) + row];
        }
    }

	dual.init_slack();

	dual.optimize();
	*/
	return dual;
}

void RSM_Model::init_space(){
	//A = vector<vector<double>>(mn, vector<double>(m));
	A = SparseMatrix<double>(mn);

	b = vector<double>(m);
	c = vector<double>(mn,0.0);
	cbar = vector<double>(mn);
	bbar = vector<double>(m);
	y = vector<double>(m);
	xb = vector<int>(m);
	xn = vector<int>(n);
}

void RSM_Model::init(){
	init_space();
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
		A.row[in * m + vid] = -1;
	}

	for(int out:EdgesOut){
		A[out * m + vid] = 1;
	}

	for(int i = EdgeNum + vid * G.ServerLvlNum;
	    i < EdgeNum + (vid + 1) * G.ServerLvlNum;i++){
		A[i * m + vid] = -G.const_array_Server_Ability[(i - EdgeNum) % G.ServerLvlNum];
	}

	//A[(EdgeNum + G.VertexNum+G.VertexNum * G.ServerLvlNum + EdgeNum + vid) * m + vid] = 1;//slack

	b[vid] = -d;
}

void RSM_Model::init_slack(){
	for (int i = 0; i < m; i++) { // A & b
	    //if(b[i] > 0){
			A[(i + n) * m + i] = 1;
		    Ab[i * m + i] = 1;
		//}
		/*
		else{
			//reverse Sign
			for(int j = 0;j < n;j++){
				A[j * m + i] *= -1;
			}

			A[(i + n) * m + i + 1] = -1;
		    Ab[i * m + i ] = -1;
			b[i] *= -1;
		}
		*/
		
	}	
	
	
}

void RSM_Model::SetupObjectFunc(){
	int i = 0;
	for(;i < EdgeNum;i++){
		c[i] = G.mem.array_Edge_cost[i];

        //UB of Edge
		A[i * m + G.VertexNum + i] = 1;
		//A[(EdgeNum + G.VertexNum+G.VertexNum * G.ServerLvlNum + EdgeNum + G.VertexNum + i) * m + G.VertexNum + i] = -1;//slack
		//A[(i + G.VertexNum * G.ServerLvlNum + EdgeNum) * m + G.VertexNum + i] = 1;//bigM
		b[G.VertexNum + i] = G.mem.array_Edge_bandwidth[i];
	}
    
	//n coef
	for(;i < EdgeNum + G.VertexNum * G.ServerLvlNum;i++){
		c[i] = G.const_array_Vertex_Server_Cost[(i - EdgeNum) / G.ServerLvlNum]
		    + G.const_array_Server_Cost[(i - EdgeNum) % G.ServerLvlNum];
	}

	for(int i = 0;i < G.VertexNum;i++){
		for(int j = 0;j < G.ServerLvlNum;j++){
			A[(i * G.ServerLvlNum + j + EdgeNum) * m + G.VertexNum + EdgeNum + i] = 1;
		}

		//A[(EdgeNum + G.VertexNum + G.VertexNum * G.ServerLvlNum + EdgeNum + G.VertexNum + EdgeNum + i) * m + G.VertexNum + EdgeNum + i] = -1;//slack
		//A[(EdgeNum + i + G.VertexNum * G.ServerLvlNum + EdgeNum) * m + G.VertexNum + EdgeNum + i] = 1;//bigM
		//no more than 1 server in a vertex
		// -1 for change sign to >=
		b[G.VertexNum + EdgeNum + i] = 1;
	}

    //init_slack();
	
}

void RSM_Model::optimize(){
	cbar = c;
	for (size_t j = 0; j < mn; j++) { // c & x
		if (j < n) {
			xn[j] = j;
		}
		else {
			xb[j - n] = j;
		}
	}
	bbar = b;

	init_slack();
	
#ifdef DBG_PRINT
	cout << endl << "--- output ---" << endl << endl;
	cout << "m = " << m << "\tn = " << n << endl;

	cout << "c = ";
	printVector(c);

	cout << "b = ";
	printVector(b);
#endif
	
	

	// Work
	bool finished = false;
	size_t iteration = 1;
	int enter, leave, col;

	double opt_value = 0.0;

	while (!finished && iteration <= MAX_ITERATIONS	) {

#ifdef DBG_PRINT
		cout << "A =" << endl;
		for (size_t i = 0; i < m; i++) {
			for (size_t j = 0; j < mn; j++) {
				cout << A[j * m + i] << "\t";
			}
			cout << endl;
		}
		cout << endl << endl;
		cout << "N = ";				// print null vars
		printX(xn);
		cout << "\tB = ";			// print basic vars
		printX(xb);

		cout << endl << "bbar =\t";	// print bbar
		printVector(bbar);

		cout << endl << "y =\t";	// print y
		printVector(y);

		cout << "cbar\t";			// print cbar
		for (size_t f = 0; f < n; f++) {
			cout << "x" << xn[f] << " " << cbar[f] << "\t";
		}
#endif
		int pivot_row = GetSmallest(bbar);
		if (pivot_row == -1) { // if none, we're done here
			//double z = IP(y, b);
	
			cout << endl << endl << "Optimal value of ";
			cout << opt_value << " has been reached." << endl;

			for(int i = 0;i < n;i++){
				for(int j = 0;j < m;j++){
					if(xb[j] == i){
						cout << "x" << i << " = " << bbar[j] << endl;
					}
				}
			}
			
			finished = true;
			break;
		}
#ifdef DBG_PRINT	
		cout <<"pivot row is" << pivot_row << endl;
#endif
		int pivot_col = find_pivot_col(pivot_row);

		if (pivot_col == -1) {
			finished = true;
			cout << "Solution is unbounded" << endl;			
			break;
		}

		xb[pivot_row] = pivot_col;
#ifdef DBG_PRINT
		cout  <<"pivot col is" << pivot_col << endl;
#endif
		//divid row by pivot so it will be 1

		double pivot_value = A[pivot_col * m + pivot_row];
#ifdef DBG_PRINT
		cout << "pivot_value " << pivot_value << endl;
#endif
		for(int i = 0;i < mn;i++){
			A[i * m + pivot_row] /= pivot_value;
		}

		bbar[pivot_row] /= pivot_value;

		//canceling pivot col
		for(int i = 0;i < m;i++){
			if(i == pivot_row)
			    continue;
			double t = - A[pivot_col * m + i] / pivot_value;

			for(int j = 0;j < mn;j++){
				A[j * m + i] += t * A[j * m + pivot_row];
			}
			bbar[i] += t * bbar[pivot_row];
		}

		//update cbar

		double ct = -cbar[pivot_col] / pivot_value;

		for(int j = 0;j < mn;j++){
			cbar[j] += ct * A[j * m + pivot_row];
		}

		opt_value += ct * bbar[pivot_row];

		cout << iteration << endl;
		iteration++; 
	}	
}

int RSM_Model::find_pivot_col(int pivot_row){
	int pos = -1;
	double ratio;
	double smallest = 0;
	bool first = true;
	for(int i = 0;i < mn;i++){
		//ignore sign
		double a = fabs(A[i * m + pivot_row]);
		double _c = fabs(c[i]);
		if(_c > 0.0000001 && a > 0.0000001){
			ratio = a / _c;
			if(first || ratio < smallest){
				first = false;
				smallest = ratio;
				pos = i;
			}
		}

		
	}
	//cout << "smallest ratio " << smallest << endl;
	return pos;
}

int RSM_Model::GetSmallest(const vector<double>& bbar) {
	int result = -1;
	double smallest = 0;

	for (size_t i = 0; i < bbar.size(); i++) {
		if (bbar[i] < smallest) {
			smallest = bbar[i];
			result = i;
		}		
	}

	//cout << "smallest " << smallest << endl;

	return result;
}