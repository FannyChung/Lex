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
#include <tuple>
#include <iterator>
#include "Node.h"
#define EPSL 84775
using namespace std;

ifstream infile("lextext.l");
ofstream outfile("lex.yy.c");
Node* startOfDFA;
vector<string> valNameTb;//.l文件自定义的变量
vector<char> alphaTB;//字符表
typedef pair<char, Node*> edge;
typedef map<char, set<Node*>> edgesByChar;
edgesByChar inedge;//入边表（反向表）
edgesByChar outedge;//出边表
vector<Node*> minDFA;//DFA'的节点集合

map<int, string> NFAandAction;//DFA和action的对应表
map<int, string> DFAandAction;//DFA和action的对应表
map<int, string> DFAoandAction;//miniDFA和action的对应表
typedef set<Node*> group;//节点的集合，组
group endStates;//终结点集合
group nendStates;//非终结点集合
typedef vector<set<Node*>> layer;//节点的集合的集合，即组的集合，从图上看可以看作树的一层


set<Node*> closure(vector<Node*> S){//计算某个状态集合的epsilon闭包虽然不能重复，但使用set不方便插入的同时遍历，所以使vector
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
		int num = var->getNum();
		cout << num << (var->isTerminal()) ? "is Terminal" : "";
		cout << endl;
		multimap<char, Node*>::const_iterator itr;
		multimap<char, Node*> out = var->getOutAll();
		itr = out.begin();
		while (itr != out.end())
		{
			cout << num << "-->" << (itr->second)->getNum() << '\t';
			itr++;
		}
		cout << endl;
	}
	cout << endl;
}
//判断两个类型是否相等
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

/*
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
}*/
//计算两个set的交集和差集
template <class T>
pair<set<T>, set<T>> jiaocha(set<T> s1, set<T> s2){
	set<T> r1, r2;
	for each (T var in s1)
	{
		if (s2.find(var) == s2.end()){
			r1.insert(var);
		}
		else
		{
			r2.insert(var);
		}
	}
	return make_pair(r1, r2);
}
//Hopcroft's algorithm最小化DFA,参考wikipedia，自动机理论、语言和计算导论 worst case:O(ns log n), where n is the number of states and s is the size of the alphabet.
void miniDFA(Node* start){
	layer P;
	P.push_back(endStates);
	P.push_back(nendStates);//P := {F, Q \ F};
	layer w;
	w.push_back(endStates);//W := {F};
	//stack<set<Node*>> pstack;
	//pstack.push(endStates);
	//pstack.push(nendStates);//P := {F, Q \ F};

	vector<int> deleteInvec;//记录P中要删除的元素的下标
	while (!w.empty())//while (W is not empty) do
	{
		group A = w.back();//  choose and remove a set A from W
		w.pop_back();
		for each (char ch in alphaTB)// for each c in ∑ do
		{
			set<Node*> X;//let X be the set of states for which a transition on c leads to a state in A  X--ch--->A
			set<Node*> tmp = outedge.find(ch)->second;//状态nd在ch字符对应的出边表里
			for each (Node* nd in tmp)
			{
				vector<Node*>::iterator iter;//且状态nd的后继节点在A中，则把状态nd加入X
				vector<Node*> tmp1 = nd->findNext(ch);
				for (iter = tmp1.begin(); iter != tmp1.end(); ++iter)
				{
					//cout << (*iter).first << "  " << (*iter).second->getNum() << endl;
					if (A.find(*iter) != A.end()){
						X.insert(nd);
					}
				}
			}
			if (X.empty())
				continue;
			int i = 0;
			while (i < P.size()) //for each set Y in P for which X ∩ Y is nonempty and Y \ X is nonempty do
			{
				if (find(deleteInvec.begin(), deleteInvec.end(), i) != deleteInvec.end()){
					i++;
					continue;
				}
				set<Node*> Y = P[i];
				//set<Node*> jiaoset = Jiao(X, Y);
				//set<Node*> subset = sub(Y, X);
				pair<set<Node*>, set<Node*>> res = jiaocha(Y, X);
				set<Node*> jiaoset = res.first;
				set<Node*> subset = res.second;

				if (jiaoset.empty() || subset.empty()){
					i++;
					continue;
				}

				//  replace Y in P by the two sets X ∩ Y and Y \ X
				deleteInvec.push_back(i);
				P.push_back(jiaoset);
				P.push_back(subset);
				layer::iterator itrL = find(w.begin(), w.end(), Y);
				if (itrL != w.end()){                                  // if Y is in W
					layer::iterator rmvw = remove(w.begin(), w.end(), Y);//      replace Y in W by the same two sets
					w.erase(rmvw, w.end());
					w.push_back(jiaoset);
					w.push_back(subset);
				}
				else                                    //else add  the small one of X ∩ Y and Y \ X
				{
					if (jiaoset.size() <= subset.size())
						w.push_back(jiaoset);
					else
						w.push_back(subset);
				}
				i++;
			}
		}
	}
	//考虑到erase效率低，新建一个P2存放P中不应该被删除的元素
	layer P2;
	for (int i = 0; i < P.size(); i++)
	{
		vector<int>::iterator itr = find(deleteInvec.begin(), deleteInvec.end(), i);
		if (itr == deleteInvec.end())
			P2.push_back(P[i]);
	}
	//找到等价类中存在起始点的集合
	/*for each (set<Node*> var in P2)
	{
	for each(Node* _node in var){
	if ()
	}
	}*/
	cout << "division done!!!!!!!!\n size =" << P2.size() << "\nbegin new DFA-----------------" << endl;
	//划分子集完成，建立新的DFA节点和连接关系
	int i = 0;
	typedef tuple<int, char, int> move;
	set<move> movevec;//点-边-点的三元组的集合
	for each (set<Node*> setOfNodes in P2)//重新建立minDFA的节点,P2中每个集合对应一个节点
	{
		Node* minNode = new Node(i);
		pair<int, string> actionmap = make_pair(i, "");//新建一个对应的int和string的pair，如果集合里有终态，则把对应的action合并,并加入映射表DFAoandAction中
		for each (Node* nodeRpr in setOfNodes)
		{
			if (nodeRpr->isTerminal()){//如果节点集合里有一个节点是终态，则这个新的minDFA的节点是终态
				minNode->setTerminal(true);
				actionmap.second += DFAandAction.find(nodeRpr->getNum())->second;
				actionmap.second += "\n";
			}
			for each (char c in alphaTB)
			{
				Node* nextNd = nodeRpr->findNext(c)[0];//找出节点对应字符的后继，dfa节点一个字符只对应一个后继
				//遍历P里的节点状态，找到出边的节点所在的集合的编号
				for (int j = 0; j < P2.size(); j++)
				{
					if (P2[j].find(nextNd) != P2[j].end()){
						movevec.insert(make_tuple(i, c, j));//增加映射
						break;
					}
				}
			}
			if (minNode->isTerminal())
				DFAoandAction.insert(actionmap);
		}
		minDFA.push_back(minNode);
		i++;
	}

	i = 0;
	for each (move _move in movevec)
	{
		minDFA[get<0>(_move)]->addOut(get<1>(_move), minDFA[get<2>(_move)]);
		cout << "add edge " << get<0>(_move) << '\t' << get<1>(_move) << '\t' << get<2>(_move) << endl;
	}
	displayNodeVector(minDFA);
	//删除dead state 和not reachable state
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
	pair<int, string> ationmap = make_pair(dfaNo, "");
	for each (Node* nd in vec1)   //根据该状态集合里是否有终态确定新状态是否是终态
	{
		if (nd->isTerminal()){
			isTermin = true;
			ationmap.second += NFAandAction.find(nd->getNum())->second;
			ationmap.second += "\n";
		}
	}
	if (isTermin)
		DFAandAction.insert(ationmap);
	Node* node1 = new Node(dfaNo++, isTermin);//建立 新的DFA节点
	if (isTermin)
		endStates.insert(node1);
	else
		nendStates.insert(node1);

	dfaNodes.push_back(node1);//加入dfaNodes的集合
	startOfDFA = node1;
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
				outedge.find(c)->second.insert(dfaNodes[j - 1]);//增加出边表
			}
			else//如果不存在，建立新的节点，加边
			{
				p++;
				states.push_back(e);
				bool isTermin = false;
				pair<int, string> ationmap = make_pair(dfaNo, "");
				for each (Node* nd in e)   //根据该状态集合里是否有终态确定新状态是否是终态
				{
					bool ndisT = nd->isTerminal();
					if (ndisT){
						isTermin = true;
						ationmap.second += NFAandAction.find(nd->getNum())->second;
						ationmap.second += "\n";
					}
				}
				if (isTermin)
					DFAandAction.insert(ationmap);
				Node* newNode = new Node(dfaNo++, isTermin);
				if (isTermin)            //根据是否终结点加入终结点集合或非终结点集合
					endStates.insert(newNode);
				else
					nendStates.insert(newNode);

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
//向输出文件写入内容
void writeToc(){
	outfile << "#include<iostream>\nusing namespace std;\n"
		"const int MYERROR=1000000;"
		"ifstream infile(\"test.cpp\");\n"
		"ofstream outfile(\"testout.txt\");\n"
		"void yylex(){\n"
		"string yytext;\n"
		"while(true){\n"
		"\tinfile>>yytext;\n"
		"\t\tint state=0;\n"
		"\t\tint i=0;\n"
		"\t\tchar ch=yytext[i];\n"
		"\t\twhile(i<=yytext.length()){\n"
		"\t\t\tswitch(state){\n";
	for each (Node* nd in minDFA)
	{
		outfile << "\t\t\tcase " << nd->getNum() << ":\n";
		if (nd->isTerminal()){//如果是终态,执行action
			//根据节点编号找到对应的string
			outfile << "\t\t\t\t\tif(i==yytext.length()){\n";
			map<int, string>::iterator itr = DFAoandAction.find(nd->getNum());
			if (itr != DFAoandAction.end())
				outfile << "\t\t\t\t\t" << itr->second << "\n"
				"\t\t\t\t\tbreak;\n"
				"\t\t\t\t}";

			cout << itr->first << '\t' << itr->second << endl;
		}

		typedef multimap<char, Node*> chedge;
		chedge outs = nd->getOutAll();
		chedge::iterator itouts = outs.begin();
		bool ifelse = true;
		while (itouts != outs.end())//对于该节点的每个出边，都有一个if else
		{
			outfile << "\t\t\t\t";
			if (ifelse){
				outfile << "if";
				ifelse = false;
			}
			else{
				outfile << "else if";
			}
			outfile << "(ch=='" << itouts->first << "'){\n"
				"\t\t\t\t\tstate=" << itouts->second->getNum() << ";\n"
				"\t\t\t\t\tbreak;\n";
			itouts++;
		}
		if (outs.size() != 0){//最后的情况------------------------------------------------
			outfile << "\t\t\t\telse{\n"
				"\t\t\t\t\treturn \"MYERROR\";\n"
				"\t\t\t\t\tbreak;\n"
				"\t\t\t\t}\n";
		}
	}
	outfile << "\t\t\t\tdefault:\n"  //default case
		"\t\t\t\t\treturn \"MYERROR\";\n"
		"\t\t\t}\n"                 //end switch
		"\t\t\ti++;\n"			//next char
		"\t\t\tch=yytext[i];\n"
		"\t\t}\n"                    //end while for each string
		"\t}\n"                      //end while 
		"}\n";                 //end function
}
int main(){
	//string inf = "lextext.l";
	//string outf = "lex.yy.c";
	//infile.open("lextext.l", std::ifstream::in, 1);//只读
	//outfile.open("lex.yy.c", std::ofstream::out);
	Node* n1 = new Node(1);
	Node* n2 = new Node(2);
	Node* n3 = new Node(3);
	Node* n4 = new Node(4);
	Node* n5 = new Node(5);
	Node* n6 = new Node(6);
	Node* n7 = new Node(7);
	Node* n8 = new Node(8);
	n8->setTerminal(true);

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
	NFAandAction.insert(make_pair(8,"doing"));
	cout << "\nbegin generate DFA--------------------" << endl;
	generateDFA(n1);
	miniDFA(startOfDFA);
	writeToc();
	delete n1; delete n2; delete n3; delete n4; delete n5; delete n6; delete n7; delete n8;
	infile.close();
	outfile.close();
	return 0;
}
