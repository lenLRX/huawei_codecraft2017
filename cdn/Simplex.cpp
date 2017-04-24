#include "Simplex.h"
#include <cassert>
#include <algorithm>
#include <cmath>
//#define DBG_PRINT
//#define DBG_PRINT__cut

bool IntTest(double num){
	return fabs(nearbyint(num) - num) < zero_tolerance;
}


RSM_Model RSM_Model::Dual(){
	
	RSM_Model dual(G);

	return dual;
}

void RSM_Model::init_space(){
	A = SparseMatrix<double>(m);
	cbar = vector<double>(mn);
	bbar = vector<double>(m);
	y = vector<double>(m);
	xb = vector<int>(m);
	xn = vector<int>(n);
}

void RSM_Model::SetBanlist(set<int> blist){
	banlist = blist;
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
			    EdgesOut.emplace_back(e);
		}

		for(int i = 0;i < Esize;i++){
			int e = G.mem.array_Vertex_EdgesIn[offset + i];
			if(e < 0)
			    break;
			else
			    EdgesIn.emplace_back(e);
		}
		AddVertexBalance(EdgesIn,EdgesOut,v,-G.mem.array_Vertex_d[v]);
	}
	SetupObjectFunc();
}

void RSM_Model::AddVertexBalance(const vector<int>& EdgesIn,
    const vector<int>& EdgesOut,int vid,int d){

	for(int in:EdgesIn){
		A.rows[vid].emplace_back(in,-1);
	}

	for(int out:EdgesOut){
		A.rows[vid].emplace_back(out,1);
	}

	for(int i = EdgeNum + vid * G.ServerLvlNum;
	    i < EdgeNum + (vid + 1) * G.ServerLvlNum;i++){
		A.rows[vid].emplace_back(i,-G.const_array_Server_Ability[(i - EdgeNum) % G.ServerLvlNum]);
	}

    //we must ensure the order
    sort(A.rows[vid].begin(),A.rows[vid].end(),
	[](const pair<int,double>& lhs, const pair<int,double>& rhs)->bool{
		return lhs.first < rhs.first;
	});

	//A[(EdgeNum + G.VertexNum+G.VertexNum * G.ServerLvlNum + EdgeNum + vid) * m + vid] = 1;//slack

	bbar[vid] = -d;
}

void RSM_Model::init_slack(){
	for (int i = 0; i < m; i++) { // A & b
	    A.rows[i].emplace_back(i + n,1);
	}	
	
	
}

void RSM_Model::SetupObjectFunc(){
	int i = 0;
	for(;i < EdgeNum;i++){
		cbar[i] = G.mem.array_Edge_cost[i];

        //UB of Edge
		A.rows[G.VertexNum + i].emplace_back(i,1);
		//A[(EdgeNum + G.VertexNum+G.VertexNum * G.ServerLvlNum + EdgeNum + G.VertexNum + i) * m + G.VertexNum + i] = -1;//slack
		//A[(i + G.VertexNum * G.ServerLvlNum + EdgeNum) * m + G.VertexNum + i] = 1;//bigM
		bbar[G.VertexNum + i] = G.mem.array_Edge_bandwidth[i];
	}
    
	//n coef
	for(;i < EdgeNum + G.VertexNum * G.ServerLvlNum;i++){
		cbar[i] = G.const_array_Vertex_Server_Cost[(i - EdgeNum) / G.ServerLvlNum]
		    + G.const_array_Server_Cost[(i - EdgeNum) % G.ServerLvlNum];
	}

	for(int i = 0;i < G.VertexNum;i++){
		for(int j = 0;j < G.ServerLvlNum;j++){
			A.rows[G.VertexNum + EdgeNum + i].emplace_back(i * G.ServerLvlNum + j + EdgeNum,1);
		}

		//A[(EdgeNum + G.VertexNum + G.VertexNum * G.ServerLvlNum + EdgeNum + G.VertexNum + EdgeNum + i) * m + G.VertexNum + EdgeNum + i] = -1;//slack
		//A[(EdgeNum + i + G.VertexNum * G.ServerLvlNum + EdgeNum) * m + G.VertexNum + EdgeNum + i] = 1;//bigM
		//no more than 1 server in a vertex
		// -1 for change sign to >=
		bbar[G.VertexNum + EdgeNum + i] = 1;
	}

    //init_slack();
	for(int i = 0;i < n;i++){
		cbar[i] *= -1;
	}
}

void RSM_Model::cut(){
	bool done = false;
	for (size_t j = 0; j < mn; j++) { // c & x
		if (j < n) {
			xn[j] = j;
		}
		else {
			xb[j - n] = j;
		}
	}

	//init_slack();
	while(!done){
		optimize();
		break;
		done = true;
		//for(int i = 0;i < n;i++){
			for(int j = 0;j < m;j++){
			    if(xb[j] < n){
				//if(true){
					//basic var
					if(!IntTest(bbar[j])){
						//still not integer add constraint for jth row
						done = false;
						vector<pair<int,double>> constraint;
						bool all_integer = true;
						size_t row_size = A.rows[j].size();
						for(size_t offset = 0;offset < row_size;offset++){
							if(!IntTest(A.rows[j][offset].second)){
								all_integer = false;
								break;
							}
						}
						if(all_integer){
							bbar[j] = floor(bbar[j]);
							//throw "shit";
						}
						else{
							for(size_t offset = 0;offset < row_size;offset++){
								double value = floor(A.rows[j][offset].second) - A.rows[j][offset].second;
								if(fabs(value) > zero_tolerance)
									constraint.emplace_back(A.rows[j][offset].first,-fabs(value));
							}
							addConstraint(constraint,floor(bbar[j]) - bbar[j]);
						}
						
						break;
					}
				}
			}
		//}
	}
}

void RSM_Model::printSolution(){
            
	for(int i = 0;i < n;i++){
		for(int j = 0;j < m;j++){
			if(xb[j] == i){
				cout << "x" << i << " = " << bbar[j] << endl;
			}
		}
	}
}


void RSM_Model::mainLoop(){
#if 0
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
#endif
    vector<pair<vector<pair<int,double>>,double>> constraint_list;
	for(int opt_time = 0;opt_time < 120;opt_time++){
		init();
		optimize();
		int max = 0.0;
		int max_vid;
		for(int i = 0;i < n;i++){
			for(int j = 0;j < m;j++){
				if(xb[j] == i){
					//cout << "x" << i << " = " << bbar[j] << endl;
					if(i < EdgeNum){
						continue;
					}
					else{
						if(bbar[j] > max){
							max = bbar[j];
							max_vid = (i - EdgeNum)/G.ServerLvlNum;
						}
					}
				}
			}
		}

		vector<pair<int,double>> constraint;
		for(int i = 0;i< G.ServerLvlNum;i++)
		    constraint.emplace_back(G.EdgeNum + max_vid * G.ServerLvlNum+i,-1);
		//addConstraint(constraint,-1);
	}
}

void RSM_Model::optimize(){
	
#ifdef DBG_PRINT
	cout << endl << "--- output ---" << endl << endl;
	cout << "m = " << m << "\tn = " << n << endl;

	cout << "c = ";
	printVector(cbar);

	cout << "b = ";
	printVector(bbar);
#endif

#ifdef DBG_PRINT__cut
        cout << "A =" << endl;
		for (size_t i = 0; i < m; i++) {
			for (size_t j = 0; j < mn; j++) {
				bool got = false;
				for(size_t k = 0;k < A.rows[i].size();k++){
					if(A.rows[i][k].first == j){
						got = true;
					    cout << A.rows[i][k].second << "\t";
					}
				}
				if(!got)
				    cout << int(0) << "\t";
			}
			cout << endl;
		}
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
		cout << endl;
#endif
	

	// Work
	bool finished = false;
	size_t iteration = 1;
	int enter, leave, col;

	while (!finished) {

#ifdef DBG_PRINT
        cout << "A =" << endl;
		for (size_t i = 0; i < m; i++) {
			for (size_t j = 0; j < mn; j++) {
				bool got = false;
				for(size_t k = 0;k < A.rows[i].size();k++){
					if(A.rows[i][k].first == j){
						got = true;
					    cout << A.rows[i][k].second << "\t";
					}
				}
				if(!got)
				    cout << int(0) << "\t";
			}
			cout << endl;
		}
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
		    cout << endl << endl << "Optimal value of ";
	        cout << opt_value << " has been reached." << endl;
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
			cout << "Solution is unbounded row size: " << A.rows[pivot_row].size() << endl;
			throw 0 ;			
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
        
		//auto A_buffer = SparseMatrix<double>(m);
		


		

        //new pivot value is 1!!!!!!!!
		//canceling pivot col
		for(int i = 0;i < m;i++){
			if(i == pivot_row)
			    continue;
			
			pair<int,double> pivot_col_at_any_row = pair<int,double>(-1,0.0);
			//TODO binary search
			size_t row_size = A.rows[i].size();
			for(size_t j = 0;j < row_size;j++){
		        const auto& pa = A.rows[i][j];
				if(pa.first == pivot_col){
					pivot_col_at_any_row = pa;
					break;
				}
			}


			if(pivot_col_at_any_row.first == -1){
				//A_buffer.rows[i] = A.rows[i];
				continue;//it is already 0 just do nothing
			}
			    
			
			double t = - pivot_col_at_any_row.second / pivot_value;
			if(fabs(t)>zero_tolerance){
                row_operation<double>(A,pivot_row,i,t);
			    bbar[i] += t * bbar[pivot_row];
			}
			
		}

		

		//update cbar

		double ct = -cbar[pivot_col] / pivot_value;

		for(auto& pa:A.rows[pivot_row]){
			cbar[pa.first] += ct * pa.second;
		}

		//it is usless and wrong! but doesn't matters
		opt_value += ct * bbar[pivot_row];
        //swap buffer!
		//A = A_buffer;

		bbar[pivot_row] /= pivot_value;
		for(pair<int,double>& pa:A.rows[pivot_row]){
			pa.second /= pivot_value;
			//A_buffer.rows[pivot_row].emplace_back(pa);
		}

		//cout << iteration << endl;
		iteration++; 
	}
	cout << "iteration:" << iteration << endl;	
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
		if(a > zero_tolerance){
			ratio =_c / a;
			if(first || ratio < smallest){
				first = false;
				smallest = ratio;
				pos.first = i;
				pos.second = pa.first;
			}
			else if(ratio < smallest + zero_tolerance){
				if(abs(A.rows[pivot_row][pos.first].second) < a){
					smallest = ratio;
					pos.first = i;
					pos.second = pa.first;
				}
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

void RSM_Model::addConstraint(vector<pair<int,double>> line,double rhs){
	m++;
	mn++;
	A.rows.emplace_back(line);
	A.rows[m - 1].emplace_back(mn - 1,1);

	xb.resize(m);
	xn.resize(n);

	xb.back() = m - 1;
	cbar.emplace_back(0);
	bbar.emplace_back(rhs);
}

set<int> RSM_Model::GetBanlist(){
	set<int> ret;
	for (size_t j = 0; j < mn; j++) { // c & x
		if (j < n) {
			xn[j] = j;
		}
		else {
			xb[j - n] = j;
		}
	}

	init_slack();
	optimize();
	for(int i = 0;i < n;i++){
		for(int j = 0;j < m;j++){
			if(xb[j] == i){
				//cout << "x" << i << " = " << bbar[j] << endl;
				if(i < EdgeNum){
					continue;
				}
				else{
					if(bbar[j] < zero_tolerance)
					    ret.insert((i - EdgeNum) / G.ServerLvlNum);
				}
			}
		}
	}
	return ret;
}

string RSM_Model::to_String(){
	map<int,int> real_source;
	map<int,int> serverlvl;
	for(int i = 0;i < n;i++){
		for(int j = 0;j < m;j++){
			if(xb[j] == i){
				//cout << "x" << i << " = " << bbar[j] << endl;
				if(i < EdgeNum){
					G.mem.array_Edge_x[i] = nearbyint(bbar[j]);
				}
				else{
					real_source[(i - EdgeNum) / G.ServerLvlNum] 
				        += nearbyint(bbar[j] * G.const_array_Server_Ability[(i - EdgeNum) % G.ServerLvlNum]);
					serverlvl[(i - EdgeNum) / G.ServerLvlNum] = (i - EdgeNum) % G.ServerLvlNum;
				}
			}
		}
	}

	    vector<list<int>> result;
		

		for(auto p : real_source){
			list<int> line;
			line.emplace_back(p.first);
			G.start_from_source(result,line,p.first,p.second,serverlvl[p.first]);
		}
		string newline = "\n";
		string ret;
		ret += to_string(int(result.size())) + newline;
		ret += newline;
		size_t line_num = result.size();
		for(size_t i = 0;i < line_num;i++){
			auto& line_list = result[i];
			bool first = true;
			for(int num:line_list){
				if(first){
					ret += to_string(num);
					first = false;
				}
				else{
					ret += " ";
					ret += to_string(num);
				}
			}
			if(i != line_num - 1)
			    ret += newline;
		}
		return ret;
}

int RSM_Model::CalcCost(){
	int cost = 0;
	map<int,int> cost_map;
	for(int i = 0;i < n;i++){
		for(int j = 0;j < m;j++){
			if(xb[j] == i){
				//cout << "x" << i << " = " << bbar[j] << endl;
				if(i < EdgeNum){
					cost += G.mem.array_Edge_cost[i] * nearbyint(bbar[j]);
				}
				else{
					int flow = nearbyint(bbar[j] * G.const_array_Server_Ability[(i - EdgeNum) % G.ServerLvlNum]);
					if(flow > 0){
						//cost += G.const_array_Server_Cost[(i - EdgeNum) % G.ServerLvlNum];
						    //+ G.const_array_Vertex_Server_Cost[(i - EdgeNum) / G.ServerLvlNum];
						cost_map[(i - EdgeNum) / G.ServerLvlNum] = (i - EdgeNum) % G.ServerLvlNum;
					}
					
				}
			}
		}
	}
	for(auto pc:cost_map){
		cost += G.const_array_Server_Cost[pc.second] 
		    + G.const_array_Vertex_Server_Cost[pc.first];
	}
	return cost;
}