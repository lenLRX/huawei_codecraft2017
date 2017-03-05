#include "LagrangianRelaxation.h"

#include <list>
#include <vector>
#include <algorithm>

void LagrangianRelaxation::optimize(){
	unordered_set<Vertex*> excess_set;
	unordered_set<Vertex*> deficit_set;
	unordered_set<Vertex*> S;
	unordered_set<Edge*> R_ij;
	vector<int> minvec;
	list<Edge*> pred;
	int a;
	int e_S;
	int r_pi_S;

    //step4
	int e_s;
	int e_j;
	do{
		step1:
		    excess_set.clear();
			deficit_set.clear();
			S.clear();
		    for(auto& vp:G.V){
				if(vp.second.d > 0)
				    excess_set.insert(&vp.second);
				if(vp.second.d < 0)
				    deficit_set.insert(&vp.second);
			}
			if(excess_set.empty() && deficit_set.empty())
			    break;
			else{
				S.insert(*excess_set.begin());
				goto step2;
			}
		step2:
            e_S = e(S);
			R_ij = get_rij(S);
			r_pi_S = r_pi(R_ij);
			if(e_S > r_pi_S)
			    goto step3;
			else
			{
				for(auto r:R_ij){
					if(C_ij_pi(r->from,r->to,r) == 0){
						if(G.V.at(r->to).d < 0)
						    goto step4;
						else{
							pred.push_back(r);
							S.insert(&G.V.at(r->to));
							goto step2;
						}
					}
			    }
			}
		step3:
		    minvec.clear();
		    R_ij = get_rij(S);
			for(auto r:R_ij){
				if(C_ij_pi(r->from,r->to,r) == 0){
					//首先沿(S,S')中所有满足Cij_pi=0的弧(i,j)增广流量使之饱和
					G.V.at(r->from).d += (r->bandwidth - r->x);
					G.V.at(r->to).d += (r->bandwidth - r->x);
					r->x = r->bandwidth;
				}
				if(r->bandwidth - r->x > 0){
					minvec.push_back(C_ij_pi(r->from,r->to,r));
				}
			}
			//然后将S中的每个节点上的势增加a=min{Cij_pi|(i,j)属于(S,S'),rij > 0}转STEP1

			a = *min_element(minvec.begin(),minvec.end());
			for(auto s:S){
				s->pi += a;
			}
			goto step1;
		step4:
		    minvec.clear();
		    //STEP4:根据pred中记录的节点，确定子树中从s到j的一条增广路P。
		    e_s = G.V.at(pred.front()->from).d;
			e_j = G.V.at(pred.back()->to).d;
			minvec.push_back(e_s);
			minvec.push_back(e_j);
			for(auto r:pred){
				minvec.push_back(r->bandwidth - r->x);
			}

			a = *min_element(minvec.begin(),minvec.end());
			//沿P增广流量delta=min{e(s),-e(j),min{rij|(i,j)属于P}}。转STEP1
			G.V.at(pred.front()->from).d+=a;
			for(auto r:pred){
				r->x += a;
				G.V.at(r->to).d += a;
			}
			pred.clear();
			goto step1;
	}while(0);
}

int LagrangianRelaxation::e(unordered_set<Vertex*>& S){
	int sum = 0;
	for(auto s:S){
		sum += s->d;
	}
	return sum;
}

unordered_set<Edge*> LagrangianRelaxation::get_rij(unordered_set<Vertex*>& S){
	unordered_set<int> S_int_set;//S的id的编号的集合
	unordered_set<Edge*> R_ij;
	for(auto s:S){
		S_int_set.insert(s->id);
	}

	for(auto s:S){
		for(auto edge:s->EdgesOut){
			Edge* tmp = &G.E.at(edge);
			if(S_int_set.count(tmp->to) == 0)
			    R_ij.insert(tmp);
		}
	}
	return R_ij;
}

int LagrangianRelaxation::r_pi(unordered_set<Edge*>& R_ij){
	int sum = 0;

	for(auto r:R_ij){
		if(C_ij_pi(r->from,r->to,r) == 0){
			sum += (r->bandwidth - r->x);
		}
	}

	return sum;
}

int LagrangianRelaxation::C_ij_pi(int i,int j,Edge* edge){
	int pi_i = G.V.at(i).pi;
	int pi_j = G.V.at(j).pi;
	int c_ij = edge->bandwidth;
	return c_ij - pi_i + pi_j;
}