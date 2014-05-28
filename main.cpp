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
#include "NFA.h"
//#include "dfa.h"
using namespace std;

#define EPSL 84775

ifstream infile;
ofstream outfile;

set<char> alphaTB;
Node* startOfDFA;
vector<string> valNameTb;//.l文件自定义的变量
typedef pair<char, Node*> edge;
typedef map<char, set<Node*>> edgesByChar;
//edgesByChar inedge;//入边表（反向表）
edgesByChar outedge;//出边表
vector<Node*> minDFA;//DFA'的节点集合
map<int, string> NFAandAction;//NFA终态到ACTION的对应关系表
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
		cout << num << "   isterminal??  " << (var->isTerminal());
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
	//考虑到erase效率低，新建一个P2存放P中不应该被删除的元素(除了第一个节点endstates)
	layer P2;
	for (int i = 1; i < P.size(); i++)
	{
		vector<int>::iterator itr = find(deleteInvec.begin(), deleteInvec.end(), i);
		if (itr == deleteInvec.end())
			P2.push_back(P[i]);
	}
	for each (Node* var in endStates)
	{
		set<Node*> svar;
		svar.insert(var);
		P2.push_back(svar);
	}
	cout << "division done!!!!!!!!\nminDFA size =" << P2.size() << "\nbegin new DFA-----------------" << endl;
	//划分子集完成，建立新的DFA节点和连接关系
	int i = 0;
	typedef tuple<int, char, int> move;
	set<move> movevec;//点-边-点的三元组的集合
	for each (set<Node*> setOfNodes in P2)//重新建立minDFA的节点,P2中每个集合对应一个节点
	{
		Node* minNode = new Node();
		pair<int, string> actionmap = make_pair(0, "");//新建一个对应的int和string的pair，如果集合里有终态，则把对应的action合并,并加入映射表DFAoandAction中
		bool firstFlag = true;//选取第一个节点作为新的节点的编号
		for each (Node* nodeRpr in setOfNodes)
		{
			if (firstFlag){
				minNode->setNum(nodeRpr->getNum());
				firstFlag = false;
			}
			if (nodeRpr->isTerminal()){//如果节点集合里有一个节点是终态，则这个新的minDFA的节点是终态
				minNode->setTerminal(true);
				actionmap.first = minNode->getNum();
				actionmap.second += DFAandAction.find(nodeRpr->getNum())->second;
			}
			if (nodeRpr->getOutAll().empty())
				continue;
			for each (char c in alphaTB)
			{
				vector<Node*> nextByc = nodeRpr->findNext(c);
				if (nextByc.empty())
					continue;
				Node* nextNd = nextByc[0];//找出节点对应字符的后继，dfa节点一个字符只对应一个后继
				//遍历P里的节点状态，找到出边的节点所在的集合的编号
				for (int j = 0; j < P2.size(); j++)
				{
					if (P2[j].find(nextNd) != P2[j].end()){
						movevec.insert(make_tuple(i, c, j));//增加映射
						break;
					}
				}
			}
		}
		if (minNode->isTerminal())
			DFAoandAction.insert(actionmap);
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

void initInEdge(){
	for each (char c in alphaTB)
	{
		set<Node*> vc;
		outedge.insert(make_pair(c, vc));
		//inedge.insert(make_pair(c, vc));//---------------------------------出错
	}
}
//接受起点，构造DFA的节点集合和连接表
void generateDFA(Node* start){
	int dfaNo = 0;
	vector<Node*> dfaNodes;//dfa的新的节点集合

	layer states;//dfa的状态集合，每个状态里有多个节点
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
	Node* node1 = new Node(dfaNo++, isTermin);//建立 新的DFA节点
	if (isTermin){
		DFAandAction.insert(ationmap);
		endStates.insert(node1);
	}
	else
		nendStates.insert(node1);

	dfaNodes.push_back(node1);//加入dfaNodes的集合
	startOfDFA = node1;
	initInEdge();
	int p = 1;//当前新的DFA里的节点数量
	int j = 0;
	while (j <= p)
	{
		set<Node*> temp1 = states[j];//取出集合
		for each (char c in alphaTB)
		{
			set<Node*> e = DFAedge(temp1, c);//求该集合的闭包
			if (e.empty())
				continue;

			bool flag = false;//判断states里是否存在 求出的闭包
			int i = 1;
			for (; i <= p; i++)
			{
				if (states[i] == e){
					flag = true;
					break;
				}
			}
			if (flag){//如果已经存在，则直接加边
				//trans[j][c] = i;
				dfaNodes[j - 1]->addOut(c, dfaNodes[i - 1]);//增加连接

				//inedge.find(c)->second.insert(dfaNodes[i - 1]);//增加入边表
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

				//inedge.find(c)->second.insert(newNode);//增加入边表
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
	outfile << "#include<iostream>\n"
		"using namespace std;\n"
		"const int MYERROR=1000000;\n"
		"ifstream infile(\"test.cpp\");\n"
		"ofstream outfile(\"testout.txt\");\n"
		"void yylex(){\n"
		"string yytext;\n"
		"while(!infile.eof()){\n"
		"\tinfile>>yytext;\n"
		"\toutfile<<yytext<<'\t'<<analysis(yytext);\n"
		"}\n\n"            //end yylex
		"string analysis(string yytext){\n"
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
			outfile << "\t\t\t\tif(i==yytext.length()){\n";
			map<int, string>::iterator itr = DFAoandAction.find(nd->getNum());
			if (itr != DFAoandAction.end()){
				outfile << "\t\t\t\t\t" << itr->second << "\n"
					"\t\t\t\t\tbreak;\n";
				cout <<nd->getNum()<<'\t'<< itr->second << endl;
			}
			outfile << "\t\t\t\t}\n";
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

	//变量
	NFA nfa;
	int lineNum = 0;

	string infilename = "seulex.txt";
	string outfilename = "lex.yy.c";

	infile.open(infilename.c_str(), ios::in);
	outfile.open(outfilename.c_str(), ios::out);

	cout << "Start Analysing" << endl;
	if (infile.good() == false){					//good() 表示文件正常，没有读写错误，也不是坏的，也没有结束.good() 是 istream 的方法，也是 ifstream 的方法
		cout << "Open file fail!" << endl;
		return 0;
	}
	//定义段
	char ch = infile.get();	//从输入流读取一个字符，(并将它转成int值送返)
	char next = infile.get();
	int state = nfa.CheckIdentifier(ch, next);
	if (state != DEF_BEGIN){
		cout << "File formation error,please CheckIdentifierIdentifier!" << endl;
		return 0;
	}
	while (!infile.eof() && state != DEF_END){		//文件未读取完，或未到达DEF_END
		ch = infile.get();
		if (ch == '\t' || ch == ' ')
			continue;
		if (ch == '%'){
			next = infile.get();
			state = nfa.CheckIdentifier(ch, next);
			continue;
		}
		if (ch == '\n')
			lineNum++;
		outfile.put(ch);
	}
	cout << "The definition segment finished!" << endl;

	//正规表达式
	infile.get();//换行
	state = BEGIN;
	pair<string, string> strpair;				//id关键字和转换后的re正规表达式
	while (!infile.eof() && state != SEGMENT_ID){//在规则段和定义段之间的RE表达式
		ch = infile.get();
		next = infile.get();
		infile.seekg(-1, ios::cur);
		if (ch == '%'){
			next = infile.get();
			state = nfa.CheckIdentifier(ch, next);
			if (state == MYERROR)				//我认为这儿应该是if(state！= SEGMENT_ID)
			{
				cout << "There is an error in line " << lineNum << endl;
				return 0;
			}
			continue;
		}
		else
			infile.seekg(-1, ios::cur);
		string id, re;//id关键字 re正规表达式
		infile >> id >> re;
		strpair.first = id;
		nfa.ChangeForm(re);
		strpair.second = re;
		nfa.reMap.insert(strpair);
		lineNum++;
		infile.get();
	}
	cout << "The Regular Express segment finished!" << endl;

	//规则段
	infile.get();
	state = BEGIN;
	while (!infile.eof() && state != SEGMENT_ID){
		ch = infile.get();
		if (ch == '%'){
			next = infile.get();
			state = nfa.CheckIdentifier(ch, next);
			if (state == MYERROR){
				cout << "There is an error in line " << lineNum << endl;
				return 0;
			}
			continue;
		}
		else
			infile.seekg(-1, ios::cur);				//该函数将文件指针从当前位置回退一位
		string re, action;
		string rowstr;
		getline(infile, rowstr);						//之后就定位到下一行了
		lineNum++;
		//读取规则
		string s = "\t";
		int offset = rowstr.find_first_of(s);
		re = rowstr.substr(0, offset);//Tab之前的字符段
		if (!nfa.ChangeForm(re)){
			cout << "Regular Express Error!" << endl;
			return 0;
		}
		while (rowstr[offset] == '\t' || rowstr[offset] == ' ')
			offset++;	//移动到空格结束为止
		string tempstring = rowstr.substr(offset, rowstr.size() - offset + 1);//后半段复制
		int len = tempstring.length();
		action = tempstring.substr(1, len - 2);
		nfa.GenerateNFA(re);//建立对应NFA
		cout << "Regular Express:	" << re << endl;
		cout << "the index is:	" << nfa.returnIndex << endl;
		NFAandAction.insert(make_pair(nfa.returnIndex, action));
	}
	cout << "The rules segment finished!" << endl;
	cout << endl;
	nfa.JoinNFA();

	alphaTB = nfa.getAlphTB();

	cout << "\nbegin to generate DFA--------------------" << endl;
	generateDFA(nfa.finalNFA.start);
	cout << "\nbegin to minimaze DFA_____________________________________\n" << endl;
	miniDFA(startOfDFA);
	writeToc();
	infile.close();
	outfile.close();
	return 0;
	//outfile << 111;

	/*map<int, string>::iterator itr = NFAandAction.begin();
	while (itr != NFAandAction.end())
	{
	cout << itr->first << '\t' << itr->second << endl;
	itr++;
	}
	*/
}



