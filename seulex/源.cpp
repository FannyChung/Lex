#include <iostream>
#include <stack>
#include <string>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <set>
#include "Node.h"
#define EPSL 84775
using namespace std;

ifstream infile;
ofstream outfile;
vector<string> valNameTb;//.l自定义的变量
//int(*trans)[256] = new int[][256];
vector<char> alphaTB;//字符表
typedef pair<char, Node*> edge;
typedef map<char, set<Node*>> edgesByChar;
edgesByChar inedge;//入边表（反向表）
edgesByChar outedge;//出边表


typedef set<Node*> group;
group endStates;//终结点集合
group nendStates;//非终结点集合
typedef vector<set<Node*>> layer;

//计算某个状态集合的e闭包
set<Node*> closure(vector<Node*> S){//虽然不能重复，但使用set不方便插入的同时遍历，所以使vector
	vector<Node*> T = S;
	vector<Node*> T1;
	do{
		T1 = T;
		for (int i = 0; i < T1.size(); i++)
		{
			Node* s = T1[i];
			vector<Node*> eout = s->findNext(EPSL);
			for (int j = 0; j < eout.size(); j++)
			{
				Node* next = eout[j];
				if (find(T1.begin(), T1.end(), next) == T1.end())//T1中不存在 
					T1.push_back(next);
			}
			T = T1;
		}
	} while (T1 != T);
	set<Node*> myS(T.begin(), T.end());
	return myS;
}

//求从状态集合出发经过某个字符到下一个的状态的集合
set<Node*> DFAedge(set<Node*> d, char c){
	set<Node*> d1;
	for each(Node* node in d)//对d的每一个节点
	{
		vector<Node*> eout = node->findNext(c);//求出每个节点的出边集合
		d1.insert(eout.begin(), eout.end());//将eout里的每一个点不重复的插入d1
	}
	vector<Node*> d1Vec(d1.begin(), d1.end());
	return closure(d1Vec);
}
//显示一个vector里的所有Node编号
void displayNodeVector(vector<Node*> d){
	cout << "in vector Node:\n";
	for each (Node* var in d)
	{
		cout << var->getNum() << '\t';
	}
	cout << endl;
}
//判断两 是否相等
template <class T>
bool comp(T v1, T v2)
{
	set<T> aSet;
	aSet.insert(v1);
	aSet.insert(v2);
	if (aSet.size() == 1)
		return true;
	else
		return false;
}

//接受起点，构造DFA的节点集合和连接表
void generateDFA(Node* start){
	int dfaNo = 0;
	vector<Node*> dfaNodes;//dfa的新的节点集合

	vector<set<Node*> > states;//dfa的状态集合，每个状态里有多个节点
	vector<Node*> s1;
	s1.push_back(start);//NFA起点转成一个vector
	set<Node*> emp;//先加入一个空集到states
	states.push_back(emp);
	set<Node*> vec1 = closure(s1);//求以起点开始的闭包，加入states
	states.push_back(vec1);
	bool isTermin = false;
	for each (Node* nd in vec1)   //根据该状态集合里是否有终态确定新状态是否是终态
	{
		if (nd->isTerminal()){
			isTermin = true;
			break;
		}
	}
	Node* node1 = new Node(dfaNo++, isTermin);//建立 新的DFA节点
	if (isTermin)
		endStates.insert(node1);
	else
		nendStates.insert(node1);

	dfaNodes.push_back(node1);//加入dfaNodes的集合

	int p = 1;//当前新的DFA里的节点数量
	int j = 0;
	while (j <= p)
	{
		for each (char c in alphaTB)
		{
			set<Node*> temp1 = states[j];//取出集合
			set<Node*> e = DFAedge(temp1, c);//求该集合的闭包
			set<Node*> vc;
			inedge.insert(make_pair(c, vc));
			outedge.insert(make_pair(c, vc));

			if (e.empty())
				continue;
			bool flag = false;//判断states里是否存在 求出的闭包
			int i = 1;
			for (; i <= p; i++)
			{
				if (comp(states[i], e)){
					flag = true;
					break;
				}
			}
			if (flag){//如果已经存在，则直接加边
				//trans[j][c] = i;
				dfaNodes[j - 1]->addOut(c, dfaNodes[i - 1]);//增加连接

				inedge.find(c)->second.insert(dfaNodes[i - 1]);//增加入边表
				outedge.find(c)->second.insert(dfaNodes[i - 1]);//增加出边表
			}
			else//如果不存在，建立新的节点，加边
			{
				p++;
				states.push_back(e);
				bool isTermin = false;
				for each (Node* nd in e)   //根据该状态集合里是否有终态确定新状态是否是终态
				{
					if (nd->isTerminal()){
						isTermin = true;
						break;
					}
				}
				Node* newNode = new Node(dfaNo++, isTermin);
				if (isTermin)            //根据是否终结点加入终结点集合或非终结点集合
					endStates.insert(node1);
				else
					nendStates.insert(node1);

				dfaNodes.push_back(newNode);
				dfaNodes[j - 1]->addOut(c, newNode);

				inedge.find(c)->second.insert(newNode);//增加入边表
				outedge.find(c)->second.insert(dfaNodes[j - 1]);//增加出边表
			}
		}
		cout << j << "次循环" << endl;
		for each (Node* var in states[j])
		{
			cout << var->getNum() << '\t';
		}
		cout << endl;
		j++;
	}
	//删除原来的NFA所有节点

	displayNodeVector(dfaNodes);
	/*
	for each (Node* dfaNd in dfaNodes)
	{
	delete dfaNd;
	}*/
}

/*
void generateLayer(layer ly){
for each (group g in ly)
{
for each (char ch in alphaTB)
{

}
}
}*/
//计算两个set的交集
template <class T>
set<T> Jiao(set<T> s1, set<T> s2){
	set<T> result;
	for each (T var in s1)
	{
		if (s2.find(var) != s2.end())
			result.insert(var);
	}
	return result;
}
//计算两个set的差集
template <class T>
set<T> sub(set<T> s1, set<T> s2){
	set<T> result;
	for each (T var in s1)
	{
		if (s2.find(var) == s2.end())
			result.insert(var);
	}
	return result;
}
//Hopcroft's algorithm最小化DFA,参考wikipedia，自动机理论、语言和计算导论
void miniDFA(){
	layer P;
	P.push_back(endStates);
	P.push_back(nendStates);//P := {F, Q \ F};
	layer w;
	w.push_back(endStates);//W := {F};
	while (!w.empty())//while (W is not empty) do
	{
		group A = w.back();//  choose and remove a set A from W
		w.pop_back();
		for each (char ch in alphaTB)// for each c in ∑ do
		{
			set<Node*> X = Jiao(outedge.find(ch)->second, A);//  let X be the set of states for which a transition on c leads to a state in A  C---->A  by c
			for each (set<Node*> Y in P)  //for each set Y in P for which X ∩ Y is nonempty and Y \ X is nonempty do
			{
				set<Node*> jiaoset = Jiao(X, Y);
				set<Node*> subset = sub(X, Y);

				if (jiaoset.empty() || subset.empty())
					continue;

				remove(P.begin(), P.end(), Y);//  replace Y in P by the two sets X ∩ Y and Y \ X
				P.push_back(jiaoset);
				P.push_back(subset);
				layer::iterator itrL = find(w.begin(), w.end(), Y);
				if (itrL != w.end()){          // if Y is in W
					remove(P.begin(), P.end(), w);//      replace Y in W by the same two sets
					w.push_back(jiaoset);
					w.push_back(subset);
				}
				else                                    //else
				{
					if (jiaoset.size() <= subset.size())
						w.push_back(jiaoset);
					else
						w.push_back(subset);
				}
			}
		}
	}

}
int main(){
	Node* n1 = new Node(1);
	Node* n2 = new Node(2);
	Node* n3 = new Node(3);
	Node* n4 = new Node(4);
	Node* n5 = new Node(5);
	Node* n6 = new Node(6);
	Node* n7 = new Node(7);
	Node* n8 = new Node(8);
	n8->setTerminal(false);

	n1->addOut(EPSL, n2);

	n2->addOut('a', n2);
	n2->addOut('b', n2);
	n2->addOut(EPSL, n3);

	n3->addOut('a', n4);
	n3->addOut('b', n5);

	n4->addOut('a', n6);

	n5->addOut('b', n6);

	n6->addOut(EPSL, n7);

	n7->addOut('a', n7);
	n7->addOut('b', n7);
	n7->addOut(EPSL, n8);

	alphaTB.push_back('a');
	alphaTB.push_back('b');

	vector<Node*> I;
	I.push_back(n1);

	generateDFA(n1);
	delete n1; delete n2; delete n3; delete n4; delete n5; delete n6; delete n7; delete n8;
	//delete[]trans;
	return 0;
}
