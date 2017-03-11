#include "LagrangianRelaxation.h"

#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <limits>
#include <cassert>
#include <queue>

//#define LEN_DBG

bool LagrangianRelaxation::optimize(){
	unordered_set<Vertex*> excess_set;
	unordered_set<Vertex*> deficit_set;
	unordered_set<Vertex*> S;
	unordered_set<Edge*> R_ij;
	vector<int> minvec;
	vector<int> pred;
	int a;
	int e_S;
	int r_pi_S;

    //step4
	int e_s;
	int e_j;
	do{
		step1:
#ifdef LEN_DBG
		    cout << "step1" << endl;
		    cout << "source " << G.V[-1].d << endl;
#endif
		    pred.clear();
		    excess_set.clear();
			deficit_set.clear();
			S.clear();
		    for(auto& vp:G.V){
				if(vp.second.d > 0)
				    excess_set.insert(&vp.second);
				if(vp.second.d < 0)
				    deficit_set.insert(&vp.second);
			}
#ifdef LEN_DBG
			cout << excess_set.size() << endl;
#endif
			if(excess_set.empty() && deficit_set.empty())
			    return true;
			else{
				S.insert(*excess_set.begin());
				pred.push_back((*excess_set.begin())->id);
				goto step2;
			}
		step2:
#ifdef LEN_DBG
		    cout << "step2" << endl;
#endif
            e_S = e(S);
#ifdef LEN_DBG
			//debug
			cout << "S { ";
			for(auto s:S){
				cout << s->id << ",";
			}
			cout << "}" << endl;
			//debug
#endif

			R_ij = get_rij(S);
			r_pi_S = r_pi(R_ij);
			if(false && e_S > r_pi_S)//WTF WTF????
			    goto step3;
			else
			{
				for(auto r:R_ij){
					if(C_ij_pi(r->from,r->to,r) == 0){
#ifdef LEN_DBG
						cout << "r->from " << r->from << " r->to " << r->to << endl;
#endif
						if(G.V.at(r->to).d < 0){
							//error we should record vertex,not edge!!
							//pred.push_back(r);
							pred.push_back(r->to);
							goto step4;
						}
						else{
							//pred.push_back(r);
							pred.push_back(r->to);
							S.insert(&G.V.at(r->to));
							goto step2;
						}
					}
			    }
			}
			goto step3;
			return false;
			//cout << "error" << endl;
		step3:
#ifdef LEN_DBG
		    cout << "step3" << endl;
#endif
		    minvec.clear();
		    R_ij = get_rij(S);
			for(auto r:R_ij){
				/*
				if(C_ij_pi(r->from,r->to,r) == 0 && r->id > 0){
#ifdef LEN_DBG
					cout << "r->from " << r->from << " r->to " << r->to << endl;
#endif
                    if(r->id > 0){
						if(r->from == -1){
							G.V.at(r->from).d -= 1;
							G.V.at(r->to).d += 1;
							r->x += 1;
							G.E.at(-r->id).bandwidth = r->x;
						}
						else{
							//首先沿(S,S')中所有满足Cij_pi=0的弧(i,j)增广流量使之饱和
							G.V.at(r->from).d -= (r->bandwidth - r->x);
							G.V.at(r->to).d += (r->bandwidth - r->x);
							r->x = r->bandwidth;
							G.E.at(-r->id).bandwidth = r->bandwidth;
						}
					}
					else{
						//清空残留网络对应的这条边
						Edge& originalEdge = G.E.at(-r->id);
						G.V.at(r->from).d -= originalEdge.x;
						G.V.at(r->to).d += originalEdge.x;
						originalEdge.x = 0;
						r->bandwidth = 0;
					}
					
				}
				*/

				if(r->bandwidth - r->x > 0){
					minvec.push_back(C_ij_pi(r->from,r->to,r));
				}
				
			}
			//然后将S中的每个节点上的势增加a=min{Cij_pi|(i,j)属于(S,S'),rij > 0}转STEP1
			//cout << "len(minvec) " << minvec.size() << endl;
#ifdef LEN_DBG
			cout << "G.V[-1] " << G.V[-1].d << endl;
#endif
			a = *min_element(minvec.begin(),minvec.end());
			for(auto s:S){
				s->pi += a;
			}
			goto step1;
		step4:
#ifdef LEN_DBG
		    cout << "step4" << endl;
#endif
		    minvec.clear();
		    //STEP4:根据pred中记录的节点，确定子树中从s到j的一条增广路P。
			if(pred.size() > 0){
				e_s = G.V.at(pred.front()).d;
				e_j = G.V.at(pred.back()).d;
				minvec.push_back(e_s);
				minvec.push_back(-e_j);

				vector<Edge*> path = dijkstra(pred.front(),pred.back());

				if(path.size() == 0)
				    return false;

				for(auto pE:path){
					if(pE->id > 0)
					    minvec.push_back(pE->bandwidth - pE->x);
				    else
					    minvec.push_back(pE->bandwidth);
				}

				/*
				vector<pair<Edge*,Edge*>> path;
				for(int i = 0;i < pred.size() -1;i++){
					auto& from = G.V.at(pred[i]);
					//取两节点间所有边的流量之和的(既然来回费用相等)
					int sum = 0;
					pair<Edge*,Edge*> p(nullptr,nullptr);//正向弧和反向弧
					for(int e:from.EdgesOut){
						Edge* pe = &G.E.at(e);
						if(pe->to == pred[i+1]){
							if(pe->id > 0){
								p.first = pe;
								sum += pe->bandwidth - pe->x;
							}
							else{
								p.second = pe;
								sum += pe->bandwidth;
							}
							    
						}
					}
					if(p.first == nullptr && p.second == nullptr)
					    assert(false);
					minvec.push_back(sum);
					path.push_back(p);
				}
				*/
				/*
				for(auto r:pred){
					minvec.push_back(r->bandwidth - r->x);
				}
				*/

				a = *min_element(minvec.begin(),minvec.end());
				//沿P增广流量delta=min{e(s),-e(j),min{rij|(i,j)属于P}}。转STEP1
				//源减少d，终点增加d
				G.V.at(pred.front()).d -= a;
				G.V.at(pred.back()).d += a;
				//拆分流量a
				for(auto r:path){
					/*
					int local_a = a;
					if(p.second != nullptr){//如果有反向弧,优先填满反向弧
						if(p.second->bandwidth >= local_a){//如果反向弧的残留容量大于等于a则优先填满反向弧
						    p.second->bandwidth -= local_a;
							G.E.at(-p.second->id).x -= local_a;
						}
						else{//流量足够大，直接把反向弧清空
						    local_a -= p.second->bandwidth;
							p.second->bandwidth = 0;
							G.E.at(-p.second->id).x = 0;
						}
					}
					if(p.first == nullptr){
						assert(local_a == 0);
						continue;
					}
					//将剩下流量压入正向边
					p.first->x += local_a;
					assert(p.first->x <= p.first->bandwidth);
					//处理该正向边的反向边
					G.E.at(-p.first->id).bandwidth = p.first->x;
					*/
					
					//legacy codes
					if(r->id > 0){
						r->x += a;
					    G.E.at(-r->id).bandwidth = r->x;
					}
					else{
						G.E.at(-r->id).x -= a;
						r->bandwidth = G.E.at(-r->id).x;
					}
					
				}
			}
			else{
				return false;
				//cout << "empty" << endl;
			}
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

#ifdef LEN_DBG
	cout << "R_ij :{" << endl;
#endif
	for(auto s:S){
		for(auto edge:s->EdgesOut){
			Edge* tmp = &G.E.at(edge);
			if(S_int_set.count(tmp->to) == 0){
				R_ij.insert(tmp);
#ifdef LEN_DBG
				cout << "id " << tmp->id << " from " << tmp->from 
				<< " to " << tmp->to << " x " << tmp->x << endl;
#endif
			}
			    
		}
	}
#ifdef LEN_DBG
	cout << "}" << endl;
#endif
	return R_ij;
}

int LagrangianRelaxation::r_pi(unordered_set<Edge*>& R_ij){
	int sum = 0;

	for(auto r:R_ij){
		if(C_ij_pi(r->from,r->to,r) == 0){
			if(r->from == - 1){
				continue;
			}
			else
			    sum += (r->bandwidth - r->x);
		}
	}

	return sum;
}

int LagrangianRelaxation::C_ij_pi(int i,int j,Edge* edge){
	int pi_i = G.V.at(i).pi;
	int pi_j = G.V.at(j).pi;
	int c_ij = edge->cost;

    /*
	if(edge->from == -1){
		if(edge->id > 0)
		    c_ij = G.ServerCost / (edge->x + 1);
		else
		    c_ij = G.ServerCost / edge->bandwidth;
	}
	*/

	//if(edge->id > 0)
	    return c_ij - pi_i + pi_j;
	//else
	//    return pi_j - pi_i - c_ij;
}

void LagrangianRelaxation::create_pesudo_source(unordered_set<int> ExcludingVertex){
	int pesudo_source_id = -1;
	Vertex pesudo_source(pesudo_source_id);

	int sum = 0;

	for(auto& pv:G.V){
		sum += pv.second.d;
		if(ExcludingVertex.count(pv.first))
		    continue;
		Edge e;
		e.id = G.GetAnID();
		e.from = pesudo_source_id;
		e.to = pv.second.id;
		e.bandwidth = numeric_limits<int>::max();
		e.cost = pesudoCost.at(pv.first);
		e.x = 0;

		//cout << pv.first << " : " << e.cost << endl;

		Edge ResidualEdge;
		ResidualEdge.id = -e.id;
		ResidualEdge.from = e.to;
		ResidualEdge.to = e.from;
		ResidualEdge.bandwidth = e.bandwidth;
		ResidualEdge.cost = -e.cost;
		ResidualEdge.x = 0;
		e.ResidualEdgeNo = -e.id;

		G.E[e.id] = e;
		G.E[-e.id] = ResidualEdge;
		pv.second.EdgesIn.insert(e.id);
		pv.second.EdgesOut.insert(-e.id);
		pesudo_source.EdgesOut.insert(e.id);
		pesudo_source.EdgesIn.insert(-e.id);

		//cout << "e.id " << e.id << endl;

		
	}

	pesudo_source.d = -sum;

	//cout << "pesudo_source.d " << pesudo_source.d << endl;

	G.V[pesudo_source_id] = pesudo_source;
}

void LagrangianRelaxation::rand_a_source(){
	int sum = 0;
	for(auto& pv:G.V){
		sum += pv.second.d;
	}

	for(auto& pv:G.V){
		if(pv.second.consumer_id >= 0)
		    continue;
		int out_sum = 0;
		for(int no:pv.second.EdgesOut){
			out_sum += G.E[no].bandwidth;
		}

        if(-sum >= out_sum)
		    pv.second.d = out_sum;
		else
		    pv.second.d = -sum;
		
		sum += pv.second.d;

	}

	//G.V.begin()->second.d = -sum;
}

void LagrangianRelaxation::check(){
	for(auto& vp:G.V){
		int sum = 0;
		for(auto edge_no:vp.second.EdgesIn){
			if(G.E[edge_no].id > 0)
			    sum += G.E[edge_no].x;
		}

		for(auto edge_no:vp.second.EdgesOut){
			if(G.E[edge_no].id > 0)
			    sum -= G.E[edge_no].x;
		}

		if(sum != 0){
			cout << "ID : " << vp.first << " Vertex sum " << sum << endl;
		}

		if(vp.second.d !=0 )
		    cout << "d: " << vp.second.d << endl;
	}
}

vector<Edge*> LagrangianRelaxation::dijkstra(int source,int dest){
	vector<Edge*> ret;
	auto cmp = [this](int lhs,int rhs)->bool{
		return this->G.V.at(lhs).distance < this->G.V.at(rhs).distance;
	};

	vector<int> Q;
	for(auto& v:G.V){
		v.second.distance = numeric_limits<int>::max();
		Q.push_back(v.first);
	}
	G.V.at(source).distance = 0;

	size_t v_size = G.V.size();

	set<int> excluding_set;
	
	for(size_t i = 0;i < v_size - 1;i++){
		int min_element_pos = -1;
		int min_value = numeric_limits<int>::max();
		for(size_t j = i;j < v_size;j++){
			if(G.V.at(Q[j]).distance < min_value){
				min_value = G.V.at(Q[j]).distance;
				min_element_pos = j;
			}
		}

		if(min_element_pos < 0)
		    return vector<Edge*>();

		swap(Q[i],Q[min_element_pos]);

		excluding_set.insert(Q[i]);

		if(Q[i] == dest){
			break;//early stop
		}

		Vertex& v = G.V.at(Q[i]);

		int my_distance = v.distance;

		for(auto E_id:v.EdgesOut){
			Edge& e = G.E.at(E_id);
			if(excluding_set.count(e.to))
			    continue;
			Vertex& u = G.V.at(e.to);
			if(e.bandwidth > 0 && e.bandwidth - e.x > 0 
			    && u.distance > v.distance + e.cost){
				u.distance = v.distance + e.cost;
				u.from_edge = &e;
			}
		}
	}
	

	Vertex* pVertex = &G.V.at(dest);

	//cout << pVertex->distance << endl;

	unordered_set<int> visited_set;

    //back trace
	while(true){
		
		if(visited_set.count(pVertex->id)){
			ret.clear();
			break;
		}
		    
		visited_set.insert(pVertex->id);

		ret.push_back(pVertex->from_edge);
		pVertex = &G.V.at(pVertex->from_edge->from);
		if(pVertex->id == source)
		    break;
	}

	reverse(ret.begin(),ret.end());

	return ret;
}