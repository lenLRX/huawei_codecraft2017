#include "Simplex.h"
#include <cassert>
#include <algorithm>
#include <cmath>
//#define DBG_PRINT
//#define DBG_PRINT__cut
//#define RESET_ERROR

bool IntTest(float_type num){
	return fabs(nearbyint(num) - num) < zero_tolerance;
}

void printX(vector<int> x) {
	cout << "{";
	for (size_t j = 0; j < x.size(); j++) {
		cout << " x" << x[j];
	}
	cout << " }";
}

void printVector(vector<float_type> v) {
	for (size_t f = 0; f < v.size(); f++) {
		cout << v[f] << "\t";
	}
	cout << endl;
}

float_type determVar(int number, vector<int> xb, vector<int> xn, 
	vector<float_type> bbar) {	

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

	return dual;
}

void RSM_Model::init_space(){
	A = SparseMatrix<float_type>(m);
	A_origin = SparseMatrix<float_type>(m);
	A_origin_col_major = SparseMatrix<float_type>(mn);
	cbar = vector<float_type>(mn);
	c_origin = vector<float_type>(mn);
	bbar = vector<float_type>(m);
	b_origin = vector<float_type>(m);
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
	A_origin = A;
	//init col major

	for(int i = 0;i < m;i++){
		size_t rowsize = A_origin.rows[i].size();
		for(size_t j = 0;j < rowsize;j++){
			A_origin_col_major.rows[A_origin.rows[i][j].first].
			    emplace_back(i,A_origin.rows[i][j].second);
				            //row,value
		}
	}
	b_origin = bbar;
	c_origin = cbar;
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
	[](const pair<int,float_type>& lhs, const pair<int,float_type>& rhs)->bool{
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

    init_slack();
	for(int i = 0;i < n;i++){
		cbar[i] *= -1;
	}
}

void RSM_Model::BnB(){
	bool done = false;
	for (size_t j = 0; j < mn; j++) { // c & x
		if (j < n) {
			xn[j] = j;
		}
		else {
			xb[j - n] = j;
		}
	}


	SparseMatrix<float_type> A_prev,A_prev2;

	vector<float_type> cbar_prev,cbar_prev2;				// c

	vector<float_type> bbar_prev,bbar_prev2;				// b

	vector<int> xb_prev,xb_prev2;

	float_type prev_opt_value,prev_opt_value2;
	int prev_m,prev_mn;
	int prev_m2,prev_mn2;
	

	optimize();
    A_prev = A;
	cbar_prev = cbar;
	bbar_prev = bbar;
	xb_prev = xb;
	prev_opt_value = opt_value;
	prev_m = m;
	prev_mn = mn;

    //printSolution();
	while(!done){
		
		done = true;
		float_type min_f = 1.00;
		int max_pos = -1;

		for(int j = 0;j < m;j++){
			if(xb[j] < n){
			//if(true){
			//basic var
			    if(!IntTest(bbar[j])){
				    //still not integer add constraint for jth row
				    done = false;
					if(fabs(floor(bbar[j]) + 0.5 - bbar[j]) < min_f){
						max_pos = j;
						min_f = fabs(floor(bbar[j]) + 0.5 - bbar[j]);
					}
				}
			}
		}

		if(done)
		    break;

		cout << "bbar[" << xb[max_pos] << "] " << bbar[max_pos] << endl;

		{
			if(fabs(floor(bbar[max_pos])) < zero_tolerance){
				opt_value = 1000000;
			}
			else{
				//floor banch		
				vector<pair<int,float_type>> constraint;
				constraint.emplace_back(xb[max_pos],1);

				addConstraint(constraint,floor(bbar[max_pos]));
				if(!optimize())
					opt_value = 1000000;
			    cout << floor(bbar[max_pos]) << endl;
				//printSolution();
			}
			

			A_prev2 = A;
			cbar_prev2 = cbar;
			bbar_prev2 = bbar;
			xb_prev2 = xb;
			prev_opt_value2 = opt_value;
			prev_m2 = m;
	        prev_mn2 = mn;
		}//floor banch

		

		{
			A = A_prev;
			cbar = cbar_prev;
			bbar = bbar_prev;
			xb = xb_prev;
			opt_value = prev_opt_value;
			m = prev_m;
	        mn = prev_mn;

			if(fabs(ceil(bbar[max_pos])) < zero_tolerance){
				opt_value = 1000000;
			}
			else{
				//ceil banch		
				vector<pair<int,float_type>> constraint;
				constraint.emplace_back(xb[max_pos],-1);

				addConstraint(constraint,-ceil(bbar[max_pos]));
				if(!optimize())
					opt_value = 1000000;
				cout << ceil(bbar[max_pos]) << endl;
				//printSolution();
			}
			

			if(opt_value < prev_opt_value2){
				//ceil better than floor
				A_prev = A;
				cbar_prev = cbar;
				bbar_prev = bbar;
				xb_prev = xb;
				prev_opt_value = opt_value;
				prev_m = m;
	            prev_mn = mn;
			}
			else{
				//floor is better
				A = A_prev2;
				cbar = cbar_prev2;
				bbar = bbar_prev2;
				xb = xb_prev2;
				opt_value = prev_opt_value2;
				m = prev_m2;
	            mn = prev_mn2;

				A_prev = A;
				cbar_prev = cbar;
				bbar_prev = bbar;
				xb_prev = xb;
				prev_opt_value = opt_value;
				prev_m = m;
	            prev_mn = mn;
			}

			
		}

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
		//printSolution();
		//break;
		done = true;
		float_type max_f = 0;
		int max_pos = -1;

		//for(int i = 0;i < n;i++){
			for(int j = 0;j < m;j++){
			    if(xb[j] < n){
				//if(true){
					//basic var
					if(!IntTest(bbar[j])){
						//still not integer add constraint for jth row
						if(fabs(floor(bbar[j]) - bbar[j]) > max_f){
							max_pos = j;
							max_f = fabs(floor(bbar[j]) - bbar[j]);
						}
						done = false;
						
					}
				}
			}
		//}

		if(done)
		    break;

		vector<pair<int,float_type>> constraint;
		bool all_integer = true;
		size_t row_size = A.rows[max_pos].size();
		for(size_t offset = 0;offset < row_size;offset++){
			if(!IntTest(A.rows[max_pos][offset].second)){
				all_integer = false;
					break;
			}
		}
		cout << "bbar[max_pos] " << bbar[max_pos] << endl;
		if(all_integer){
			bbar[max_pos] = floor(bbar[max_pos]);
			//throw "shit";
		}
		else{
			for(size_t offset = 0;offset < row_size;offset++){
				float_type value = floor(A.rows[max_pos][offset].second) - A.rows[max_pos][offset].second;
				if(fabs(value) > zero_tolerance){
					constraint.emplace_back(A.rows[max_pos][offset].first,-fabs(value));
					cout << -fabs(value) << " ";
				}
									
			}
			cout << endl;
			addConstraint(constraint,-fabs(floor(bbar[max_pos]) - bbar[max_pos]));
		}
	
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
    vector<pair<vector<pair<int,float_type>>,float_type>> constraint_list;
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

		vector<pair<int,float_type>> constraint;
		for(int i = 0;i< G.ServerLvlNum;i++)
		    constraint.emplace_back(G.EdgeNum + max_vid * G.ServerLvlNum+i,-1);
		//addConstraint(constraint,-1);
	}
}

bool RSM_Model::optimize(){
	
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
			return true;
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
			return false;			
			break;
		}

		xb[pivot_row] = pivot_col;
#ifdef DBG_PRINT
		cout  <<"pivot col is " << pivot_col << endl;
#endif
		//divid row by pivot so it will be 1

		float_type pivot_value = A.rows[pivot_row][pivot_offset].second;
#ifdef DBG_PRINT
		cout << "pivot_value " << pivot_value << endl;
#endif
        
		//auto A_buffer = SparseMatrix<float_type>(m);
		


		

        //new pivot value is 1!!!!!!!!
		//canceling pivot col
		for(int i = 0;i < m;i++){
			if(i == pivot_row)
			    continue;
			
			pair<int,float_type> pivot_col_at_any_row = pair<int,float_type>(-1,0.0);
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
			    
			
			float_type t = - pivot_col_at_any_row.second / pivot_value;
			if(fabs(t)>zero_tolerance){
                row_operation<float_type>(A,pivot_row,i,t);
			    bbar[i] += t * bbar[pivot_row];
			}
			
		}

		

		//update cbar

		float_type ct = -cbar[pivot_col] / pivot_value;

		for(auto& pa:A.rows[pivot_row]){
			cbar[pa.first] += ct * pa.second;
		}

		//it is usless and wrong! but doesn't matters
		opt_value += ct * bbar[pivot_row];
        //swap buffer!
		//A = A_buffer;

		bbar[pivot_row] /= pivot_value;
		for(pair<int,float_type>& pa:A.rows[pivot_row]){
			pa.second /= pivot_value;
			//A_buffer.rows[pivot_row].emplace_back(pa);
		}

        //if(iteration % 1000 == 0)
		//eliminateError();

		//cout << iteration << endl;
		iteration++;
	}
	cout << "iteration:" << iteration << endl;	
}
//offset,col
pair<int,int> RSM_Model::find_pivot_col(int pivot_row){
	pair<int,int> pos = pair<int,int>(-1,-1);
	float_type ratio;
	float_type smallest = 0;
	bool first = true;
	size_t row_size = A.rows[pivot_row].size();
	for(size_t i = 0;i < row_size;i++){
		const auto& pa = A.rows[pivot_row][i];
		//ignore sign
		float_type a = pa.second;
		if(a < 0){
			a = fabs(a);
		}
		else
		    continue;
		float_type _c = fabs(cbar[pa.first]);
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

int RSM_Model::GetSmallest(const vector<float_type>& bbar) {
	int result = -1;
	float_type smallest = -zero_tolerance;

	for (size_t i = 0; i < bbar.size(); i++) {
		if (bbar[i] < smallest) {
			smallest = bbar[i];
			result = i;
		}		
	}

	//cout << "smallest " << smallest << endl;

	return result;
}

void RSM_Model::addConstraint(vector<pair<int,float_type>> line,float_type rhs){
	m++;
	mn++;
	A.rows.emplace_back(line);
	

	A.rows[m - 1].emplace_back(mn - 1,1);
	
    if(rhs > 0){
		for(auto& pa: A.rows.back())
		    pa.second *= -1;
	}

	xb.resize(m);
	xn.resize(n);

	xb.back() = m - 1;
	cbar.emplace_back(0);
	
	if(rhs < 0)
	    bbar.emplace_back(rhs);
	else
	    bbar.emplace_back(-rhs);
	
#ifdef RESET_ERROR
    A_origin.rows[m - 1].emplace_back(mn - 1,1);
	A_origin.rows.emplace_back(line);
	vector<pair<int,float_type>> tmp_line;
	tmp_line.emplace_back(m - 1,1);
	A_origin_col_major.rows.emplace_back(
		tmp_line);
	
	for(auto pa:line){
		A_origin_col_major.rows[pa.first].
		    emplace_back(m - 1,pa.second);
	}
	c_origin.emplace_back(0);
	b_origin.emplace_back(rhs);
#endif
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

	optimize();
	for(int i = 0;i < n;i++){
		for(int j = 0;j < m;j++){
			if(xb[j] == i){
				//cout << "x" << i << " = " << bbar[j] << endl;
				if(i < EdgeNum){
					continue;
				}
				else{
					if(bbar[j] < 0.1)
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

void RSM_Model::eliminateError(){
	SparseMatrix<float_type> B_inv(m);

	//pre build cprimeB
	vector<float_type> cprimeB(m);
	/*
	cout << "-------A_origin-------" << endl;
	A_origin.pretty_print(mn);
	cout << "-------A_origin-------" << endl;
    cout << "-------A_origin_col_major-------" << endl;
	A_origin_col_major.pretty_print(mn);
	cout << "-------A_origin_col_major-------" << endl;
	*/

    //reconstructed B
	//TODO:TRANSPOSE B_inv
	for(int i = 0;i < m;i++){
		const auto& _col = A_origin_col_major.rows[xb[i]];
		int row_size = _col.size();
		for(int j = 0;j < row_size;j++){
			B_inv.rows[_col[j].first].emplace_back(i,_col[j].second);
		}
		cprimeB[i] = c_origin[xb[i]];
	}
	//invert B
	GaussJordanInversion(B_inv);

	//recalculate cbar (reduced cost)
	for(int i = 0;i < mn;i++){
		//B_inv x Aj
		vector<float_type> v_tmp(m);
		MatMulVector(B_inv,A_origin_col_major.rows[i],v_tmp);
		cbar[i] = c_origin[i] - InnerProduct(cprimeB,v_tmp);
	}

	//recalculate bbar
	//bbar = B-1 x b_origin
	MatMulVector(B_inv,b_origin,bbar);

    //A = B_inv x A_origin_col_major
	MatMulMat(B_inv,A_origin_col_major,A);
}

float_type RSM_Model::CalcOptval(){
	opt_value = 0.0;
	for(int i = 0;i < m;i++){
		if(xb[i] < n)
		    opt_value += c_origin[xb[i]] * bbar[xb[i]];
	}
	return opt_value;
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