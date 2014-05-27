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
vector<string> valNameTb;//.l�ļ��Զ���ı���
vector<char> alphaTB;//�ַ���
typedef pair<char, Node*> edge;
typedef map<char, set<Node*>> edgesByChar;
edgesByChar inedge;//��߱������
edgesByChar outedge;//���߱�
vector<Node*> minDFA;//DFA'�Ľڵ㼯��

map<int, string> NFAandAction;//DFA��action�Ķ�Ӧ��
map<int, string> DFAandAction;//DFA��action�Ķ�Ӧ��
map<int, string> DFAoandAction;//miniDFA��action�Ķ�Ӧ��
typedef set<Node*> group;//�ڵ�ļ��ϣ���
group endStates;//�ս�㼯��
group nendStates;//���ս�㼯��
typedef vector<set<Node*>> layer;//�ڵ�ļ��ϵļ��ϣ�����ļ��ϣ���ͼ�Ͽ����Կ�������һ��


set<Node*> closure(vector<Node*> S){//����ĳ��״̬���ϵ�epsilon�հ���Ȼ�����ظ�����ʹ��set����������ͬʱ����������ʹvector
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
				if (find(T1.begin(), T1.end(), next) == T1.end())//T1�в����� 
					T1.push_back(next);
			}
			T = T1;
		}
	} while (T1 != T);
	set<Node*> myS(T.begin(), T.end());
	return myS;
}

//���״̬���ϳ�������ĳ���ַ�����һ����״̬�ļ���
set<Node*> DFAedge(set<Node*> d, char c){
	set<Node*> d1;
	for each(Node* node in d)//��d��ÿһ���ڵ�
	{
		vector<Node*> eout = node->findNext(c);//���ÿ���ڵ�ĳ��߼���
		d1.insert(eout.begin(), eout.end());//��eout���ÿһ���㲻�ظ��Ĳ���d1
	}
	vector<Node*> d1Vec(d1.begin(), d1.end());
	return closure(d1Vec);
}

//��ʾһ��vector�������Node���
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
//�ж����������Ƿ����
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
//��������set�Ľ���
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
//��������set�Ĳ
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
//��������set�Ľ����Ͳ
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
//Hopcroft's algorithm��С��DFA,�ο�wikipedia���Զ������ۡ����Ժͼ��㵼�� worst case:O(ns log n), where n is the number of states and s is the size of the alphabet.
void miniDFA(Node* start){
	layer P;
	P.push_back(endStates);
	P.push_back(nendStates);//P := {F, Q \ F};
	layer w;
	w.push_back(endStates);//W := {F};
	//stack<set<Node*>> pstack;
	//pstack.push(endStates);
	//pstack.push(nendStates);//P := {F, Q \ F};

	vector<int> deleteInvec;//��¼P��Ҫɾ����Ԫ�ص��±�
	while (!w.empty())//while (W is not empty) do
	{
		group A = w.back();//  choose and remove a set A from W
		w.pop_back();
		for each (char ch in alphaTB)// for each c in �� do
		{
			set<Node*> X;//let X be the set of states for which a transition on c leads to a state in A  X--ch--->A
			set<Node*> tmp = outedge.find(ch)->second;//״̬nd��ch�ַ���Ӧ�ĳ��߱���
			for each (Node* nd in tmp)
			{
				vector<Node*>::iterator iter;//��״̬nd�ĺ�̽ڵ���A�У����״̬nd����X
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
			while (i < P.size()) //for each set Y in P for which X �� Y is nonempty and Y \ X is nonempty do
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

				//  replace Y in P by the two sets X �� Y and Y \ X
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
				else                                    //else add  the small one of X �� Y and Y \ X
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
	//���ǵ�eraseЧ�ʵͣ��½�һ��P2���P�в�Ӧ�ñ�ɾ����Ԫ��
	layer P2;
	for (int i = 0; i < P.size(); i++)
	{
		vector<int>::iterator itr = find(deleteInvec.begin(), deleteInvec.end(), i);
		if (itr == deleteInvec.end())
			P2.push_back(P[i]);
	}
	//�ҵ��ȼ����д�����ʼ��ļ���
	/*for each (set<Node*> var in P2)
	{
	for each(Node* _node in var){
	if ()
	}
	}*/
	cout << "division done!!!!!!!!\n size =" << P2.size() << "\nbegin new DFA-----------------" << endl;
	//�����Ӽ���ɣ������µ�DFA�ڵ�����ӹ�ϵ
	int i = 0;
	typedef tuple<int, char, int> move;
	set<move> movevec;//��-��-�����Ԫ��ļ���
	for each (set<Node*> setOfNodes in P2)//���½���minDFA�Ľڵ�,P2��ÿ�����϶�Ӧһ���ڵ�
	{
		Node* minNode = new Node(i);
		pair<int, string> actionmap = make_pair(i, "");//�½�һ����Ӧ��int��string��pair���������������̬����Ѷ�Ӧ��action�ϲ�,������ӳ���DFAoandAction��
		for each (Node* nodeRpr in setOfNodes)
		{
			if (nodeRpr->isTerminal()){//����ڵ㼯������һ���ڵ�����̬��������µ�minDFA�Ľڵ�����̬
				minNode->setTerminal(true);
				actionmap.second += DFAandAction.find(nodeRpr->getNum())->second;
				actionmap.second += "\n";
			}
			for each (char c in alphaTB)
			{
				Node* nextNd = nodeRpr->findNext(c)[0];//�ҳ��ڵ��Ӧ�ַ��ĺ�̣�dfa�ڵ�һ���ַ�ֻ��Ӧһ�����
				//����P��Ľڵ�״̬���ҵ����ߵĽڵ����ڵļ��ϵı��
				for (int j = 0; j < P2.size(); j++)
				{
					if (P2[j].find(nextNd) != P2[j].end()){
						movevec.insert(make_tuple(i, c, j));//����ӳ��
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
	//ɾ��dead state ��not reachable state
}


//������㣬����DFA�Ľڵ㼯�Ϻ����ӱ�
void generateDFA(Node* start){
	int dfaNo = 0;
	vector<Node*> dfaNodes;//dfa���µĽڵ㼯��

	vector<set<Node*> > states;//dfa��״̬���ϣ�ÿ��״̬���ж���ڵ�
	vector<Node*> s1;
	s1.push_back(start);//NFA���ת��һ��vector
	set<Node*> emp;//�ȼ���һ���ռ���states
	states.push_back(emp);
	set<Node*> vec1 = closure(s1);//������㿪ʼ�ıհ�������states
	states.push_back(vec1);
	bool isTermin = false;
	pair<int, string> ationmap = make_pair(dfaNo, "");
	for each (Node* nd in vec1)   //���ݸ�״̬�������Ƿ�����̬ȷ����״̬�Ƿ�����̬
	{
		if (nd->isTerminal()){
			isTermin = true;
			ationmap.second += NFAandAction.find(nd->getNum())->second;
			ationmap.second += "\n";
		}
	}
	if (isTermin)
		DFAandAction.insert(ationmap);
	Node* node1 = new Node(dfaNo++, isTermin);//���� �µ�DFA�ڵ�
	if (isTermin)
		endStates.insert(node1);
	else
		nendStates.insert(node1);

	dfaNodes.push_back(node1);//����dfaNodes�ļ���
	startOfDFA = node1;
	int p = 1;//��ǰ�µ�DFA��Ľڵ�����
	int j = 0;
	while (j <= p)
	{
		for each (char c in alphaTB)
		{
			set<Node*> temp1 = states[j];//ȡ������
			set<Node*> e = DFAedge(temp1, c);//��ü��ϵıհ�
			set<Node*> vc;
			inedge.insert(make_pair(c, vc));
			outedge.insert(make_pair(c, vc));

			if (e.empty())
				continue;
			bool flag = false;//�ж�states���Ƿ���� ����ıհ�
			int i = 1;
			for (; i <= p; i++)
			{
				if (comp(states[i], e)){
					flag = true;
					break;
				}
			}
			if (flag){//����Ѿ����ڣ���ֱ�Ӽӱ�
				//trans[j][c] = i;
				dfaNodes[j - 1]->addOut(c, dfaNodes[i - 1]);//��������

				inedge.find(c)->second.insert(dfaNodes[i - 1]);//������߱�
				outedge.find(c)->second.insert(dfaNodes[j - 1]);//���ӳ��߱�
			}
			else//��������ڣ������µĽڵ㣬�ӱ�
			{
				p++;
				states.push_back(e);
				bool isTermin = false;
				pair<int, string> ationmap = make_pair(dfaNo, "");
				for each (Node* nd in e)   //���ݸ�״̬�������Ƿ�����̬ȷ����״̬�Ƿ�����̬
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
				if (isTermin)            //�����Ƿ��ս������ս�㼯�ϻ���ս�㼯��
					endStates.insert(newNode);
				else
					nendStates.insert(newNode);

				dfaNodes.push_back(newNode);
				dfaNodes[j - 1]->addOut(c, newNode);

				inedge.find(c)->second.insert(newNode);//������߱�
				outedge.find(c)->second.insert(dfaNodes[j - 1]);//���ӳ��߱�
			}
		}
		cout << j << "��ѭ��" << endl;
		for each (Node* var in states[j])
		{
			cout << var->getNum() << '\t';
		}
		cout << endl;
		j++;
	}
	//ɾ��ԭ����NFA���нڵ�

	displayNodeVector(dfaNodes);
	/*
	for each (Node* dfaNd in dfaNodes)
	{
	delete dfaNd;
	}*/
}
//������ļ�д������
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
		if (nd->isTerminal()){//�������̬,ִ��action
			//���ݽڵ����ҵ���Ӧ��string
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
		while (itouts != outs.end())//���ڸýڵ��ÿ�����ߣ�����һ��if else
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
		if (outs.size() != 0){//�������------------------------------------------------
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
	//infile.open("lextext.l", std::ifstream::in, 1);//ֻ��
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
