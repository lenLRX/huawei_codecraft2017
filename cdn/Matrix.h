#include <vector>
#include <list>

using namespace std;

template<typename T = double>
class SparseMatrix{
public:
    SparseMatrix()=default;
    SparseMatrix(int row_num){
		rows.resize(row_num);
	}

    vector<list<pair<int,T>>> rows;
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

template<typename T = double>
void GaussJordanInversion(SparseMatrix<T>& mat){
	int dim = mat.rows.size();
	SparseMatrix<T> Identity;
	
}