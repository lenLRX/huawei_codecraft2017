#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

const double zero_tolerance = 1E-6;

template<typename T = double>
class SparseMatrix{
public:
    SparseMatrix()=default;
    SparseMatrix(int row_num){
		rows.resize(row_num);
	}

	void pretty_print(int col_num){
		for (size_t i = 0; i < rows.size(); i++) {
			for (size_t j = 0; j < col_num; j++) {
				bool got = false;
				for(size_t k = 0;k < rows[i].size();k++){
					if(rows[i][k].first == j){
						got = true;
					    cout << rows[i][k].second << "\t";
					}
				}
				if(!got)
				    cout << int(0) << "\t";
			}
			cout << endl;
		}
	}

    vector<vector<pair<int,T>>> rows;
};

template<typename T = double>
class ColMajorSparseMatrix{
public:
    ColMajorSparseMatrix()=default;
    ColMajorSparseMatrix(int col_num){
		cols.resize(col_num);
		for(int i = 0;i < col_num;i++){
			//rows[i].reserve(100);
		}
	}

    vector<vector<pair<int,T>>> cols;
};


//mat.rows[dest] += coef X mat.rows[source]
template<typename T = double>
void row_operation(SparseMatrix<T>& mat,int source,int dest,double coef){
	int source_row_size = mat.rows[source].size();
	int source_row_idx = 0;
	int dest_row_size = mat.rows[dest].size();
	int dest_row_idx = 0;

	vector<pair<int,double>> new_row;

	while(source_row_idx < source_row_size 
		 && dest_row_idx < dest_row_size){
		
		if(mat.rows[source][source_row_idx].first 
			> mat.rows[dest][dest_row_idx].first){
			//do sth
			new_row.emplace_back(mat.rows[dest][dest_row_idx]);
			dest_row_idx++;
		}
		else if(mat.rows[source][source_row_idx].first 
			< mat.rows[dest][dest_row_idx].first){
			new_row.emplace_back(
				mat.rows[source][source_row_idx].first,
				coef * mat.rows[source][source_row_idx].second
			);
			source_row_idx++;
		}
		else{

			double _value_insert = 
				 mat.rows[dest][dest_row_idx].second + coef * mat.rows[source][source_row_idx].second;

			//insert if non zero
			if(fabs(_value_insert) > zero_tolerance)
			    new_row.emplace_back(
			    mat.rows[source][source_row_idx].first,
				_value_insert
			    );
			    dest_row_idx++;
				source_row_idx++;
		}
	}//while

	if(source_row_idx == source_row_size){
		//job is done for pivot dest,check current dest.
		for(;dest_row_idx < dest_row_size;dest_row_idx++){
			new_row.emplace_back(mat.rows[dest][dest_row_idx]);
			}
	}
	else{
		for(;source_row_idx < source_row_size;source_row_idx++){
			new_row.emplace_back(
			mat.rows[source][source_row_idx].first,
			coef * mat.rows[source][source_row_idx].second
			);
	    }
	}
	mat.rows[dest] = new_row;
}

template<typename T = double>
void GaussJordanInversion(SparseMatrix<T>& mat){
	int dim = mat.rows.size();
	SparseMatrix<T> Identity(dim);
	//init Identity SparseMatrix
	for(int i = 0;i < dim;i++){
		Identity.rows[i].emplace_back(i,1);
	}

    //elimating col
	for(int col = 0;col < dim;col++){
		if(mat.rows[col].size() > 0 && 
		    mat.rows[col][0].first == col){
			// non-zero element at this col
		}
		else{
			bool found = false;
			//find a valid one and swap,if not found, error!
			for(int _row = col + 1;_row < dim;_row++){
				if(mat.rows[_row].size() > 0 && 
					mat.rows[_row][0].first == col){
					// non-zero element at this col
					swap(mat.rows[col],mat.rows[_row]);
					swap(Identity.rows[col],Identity.rows[_row]);
					found = true;
					break;
				}
			}
			if(!found){
				cout << "can't invert this matrix" << endl;
				throw 0;
			}
		}

		//divide to make the first element is 1
		double first_element = mat.rows[col][0].second;
		for(auto& pa:mat.rows[col]){
			pa.second /= first_element;
		}

		for(auto& pa:Identity.rows[col]){
			pa.second /= first_element;
		}

		//elimating from top to bottom

		for(int row = 0;row < dim;row++){
			if(row == col)
			    continue;
			pair<int,double> col_at_row = pair<int,double>(-1,0.0);
			//TODO:binary search
			size_t row_size = mat.rows[row].size();
			for(size_t j = 0;j < row_size;j++){
		        const auto& pa = mat.rows[row][j];
				if(pa.first == col){
					col_at_row = pa;
					break;
				}
			}

			if(col_at_row.first == -1){
				continue;//it is already 0 just do nothing
			}

			double t = - col_at_row.second;
			if(fabs(t)>zero_tolerance){
                row_operation<double>(mat,col,row,t);
				row_operation<double>(Identity,col,row,t);
			}
		}
	}
	mat = Identity;
}

#endif