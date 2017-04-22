#include "Simplex.h"
#include <cassert>
#include <cmath>
#define DBG_PRINT
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
	A = SparseMatrix<double>(m);

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
		A.rows[vid].push_back(pair<int,double>(in,-1));
	}

	for(int out:EdgesOut){
		A.rows[vid].push_back(pair<int,double>(out,1));
	}

	for(int i = EdgeNum + vid * G.ServerLvlNum;
	    i < EdgeNum + (vid + 1) * G.ServerLvlNum;i++){
		A.rows[vid].push_back(pair<int,double>(i,-G.const_array_Server_Ability[(i - EdgeNum) % G.ServerLvlNum]));
	}

	//A[(EdgeNum + G.VertexNum+G.VertexNum * G.ServerLvlNum + EdgeNum + vid) * m + vid] = 1;//slack

	b[vid] = -d;
}

void RSM_Model::init_slack(){
	for (int i = 0; i < m; i++) { // A & b
	    A.rows[i].push_back(pair<int,double>(i + n,1));
	}	
	
	
}

void RSM_Model::SetupObjectFunc(){
	int i = 0;
	for(;i < EdgeNum;i++){
		c[i] = G.mem.array_Edge_cost[i];

        //UB of Edge
		A.rows[G.VertexNum + i].push_back(pair<int,double>(i,1));
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
			A.rows[G.VertexNum + EdgeNum + i].push_back(pair<int,double>(i * G.ServerLvlNum + j + EdgeNum,1));
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
	for(int i = 0;i < n;i++){
		//c[i] *= -1;
	}
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

	while (!finished) {

#ifdef DBG_PRINT
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
		cout << endl << "pivot row is " << pivot_row << endl;
#endif
		pair<int,int> offset_col = find_pivot_col(pivot_row);

		int pivot_offset = offset_col.first;
		int pivot_col = offset_col.second;

		if (pivot_col == -1) {
			finished = true;
			cout << "Solution is unbounded" << endl;			
			break;
		}

		xb[pivot_row] = pivot_col;
#ifdef DBG_PRINT
		cout  <<"pivot col is " << pivot_col << endl;
#endif
		//divid row by pivot so it will be 1

		double pivot_value = A.rows[pivot_row][pivot_offset].second;
#ifdef DBG_PRINT
		cout << "pivot_value " << pivot_value << endl;
#endif
        
		auto A_buffer = SparseMatrix<double>(m);
		for(pair<int,double> pa:A.rows[pivot_row]){
			pa.second /= pivot_value;
			A_buffer.rows[pivot_row].push_back(pa);
		}


		

        //new pivot value is 1!!!!!!!!
		//canceling pivot col
		for(int i = 0;i < m;i++){
			if(i == pivot_row)
			    continue;
			
			pair<int,double> pivot_col_at_any_row = pair<int,double>(-1,0.0);
			size_t row_size = A.rows[i].size();
			for(size_t j = 0;j < row_size;j++){
		        const auto& pa = A.rows[i][j];
				if(pa.first == pivot_col){
					pivot_col_at_any_row = pa;
					break;
				}
			}


			if(pivot_col_at_any_row.first == -1)
			    continue;//it is already 0
			
			double t = - pivot_col_at_any_row.second / pivot_value;
			if(fabs(t)>epsilon1){
#ifdef DBG_PRINT
                cout << "canceling col: " << i << " : " << t << endl;
#endif
                int pivot_row_size = A.rows[pivot_row].size();
				int pivot_row_idx = 1;
				int current_row_size = A.rows[i].size();
				int current_row_idx = 1;

				while(pivot_row_idx < pivot_row_size 
				    && current_row_idx < current_row_size){
					
					if(A.rows[pivot_row][pivot_row_idx].first 
					    > A.rows[i][current_row_idx].first){
						//do sth
						A_buffer.rows[i].push_back(A.rows[i][current_row_idx]);
						current_row_idx++;
					}
					else if(A.rows[pivot_row][pivot_row_idx].first 
					    < A.rows[i][current_row_idx].first){
						A_buffer.rows[i].push_back(
							pair<int,double>(A.rows[pivot_row][pivot_row_idx].first,
							t * A.rows[pivot_row][pivot_row_idx].second)
						);
						pivot_row_idx++;
					}
					else{
						double _value_insert = 
						    A.rows[i][current_row_idx].second + t * A.rows[pivot_row][pivot_row_idx].second;
						//insert if non zero
						if(fabs(_value_insert) > epsilon1)
							A_buffer.rows[i].push_back(
								pair<int,double>(A.rows[pivot_row][pivot_row_idx].first,
								_value_insert)
							);
						current_row_idx++;
						pivot_row_idx++;
					}
				}

				if(pivot_row_idx == pivot_row_size){
					//job is done for pivot row,check current row.
					for(;current_row_idx < current_row_size;current_row_idx++){
						A_buffer.rows[i].push_back(A.rows[i][current_row_idx]);
					}
				}
				else{
					for(;pivot_row_idx < pivot_row_size;pivot_row_idx++){
						A_buffer.rows[i].push_back(
							pair<int,double>(A.rows[pivot_row][pivot_row_idx].first,
							t * A.rows[pivot_row][pivot_row_idx].second)
						);
					}
				}
				/*
                for(auto pivot_iter = A.rows[pivot_row].begin();
				    pivot_iter != A.rows[pivot_row].end();pivot_iter++){
					A.rows[i][pivot_iter->first] += t * pivot_iter->second;
					//zero now
					if(fabs(A.rows[i][pivot_iter->first]) < epsilon1)
					    A.rows[i].erase(pivot_iter->first);
				}
				*/
				/*
				for(auto iter = A.rows[i].begin();iter != A.rows[i].end();){
					auto pivot_iter = A.rows[pivot_row].find(iter->first);
					if(pivot_iter != A.rows[pivot_row].end()){
						iter->second += t * pivot_iter->second;
						if(fabs(iter->second) < epsilon2){
							//almost 0,remove it
							iter = A.rows[i].erase(iter);
						}
						else{
							iter++;
						}
					}
					else{
						iter++;
					}
				}
				*/
			    bbar[i] += t * bbar[pivot_row];
			}
			
		}

		bbar[pivot_row] /= pivot_value;

		//update cbar

		double ct = -cbar[pivot_col] / pivot_value;

		for(auto& pa:A.rows[pivot_row]){
			cbar[pa.first] += ct * pa.second;
		}

		opt_value += ct * bbar[pivot_row];
        //swap buffer!
		A = A_buffer;

		cout << iteration << endl;
		iteration++; 
	}	
}
//offset,col
pair<int,int> RSM_Model::find_pivot_col(int pivot_row){
	pair<int,int> pos = pair<int,int>(-1,-1);
	double ratio;
	double smallest = 0;
	bool first = true;
	size_t row_size = A.rows[pivot_row].size();
	for(size_t i = 0;i < row_size;i++){
		const auto& pa = A.rows[pivot_row][i];
		//ignore sign
		double a = pa.second;
		if(a < 0){
			a = fabs(a);
		}
		else
		    continue;
		double _c = fabs(cbar[pa.first]);
		if(_c > 0.0000001 && a > 0.0000001){
			ratio =_c / a;
			if(first || ratio < smallest){
				first = false;
				smallest = ratio;
				pos.first = i;
				pos.second = pa.first;
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