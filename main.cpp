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
vector<string> valNameTb;//.l�ļ��Զ���ı���
typedef pair<char, Node*> edge;
typedef map<char, set<Node*>> edgesByChar;
//edgesByChar inedge;//��߱������
edgesByChar outedge;//���߱�
vector<Node*> minDFA;//DFA'�Ľڵ㼯��
map<int, string> NFAandAction;//NFA��̬��ACTION�Ķ�Ӧ��ϵ��
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
	//���ǵ�eraseЧ�ʵͣ��½�һ��P2���P�в�Ӧ�ñ�ɾ����Ԫ��(���˵�һ���ڵ�endstates)
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
	//�����Ӽ���ɣ������µ�DFA�ڵ�����ӹ�ϵ
	int i = 0;
	typedef tuple<int, char, int> move;
	set<move> movevec;//��-��-�����Ԫ��ļ���
	for each (set<Node*> setOfNodes in P2)//���½���minDFA�Ľڵ�,P2��ÿ�����϶�Ӧһ���ڵ�
	{
		Node* minNode = new Node();
		pair<int, string> actionmap = make_pair(0, "");//�½�һ����Ӧ��int��string��pair���������������̬����Ѷ�Ӧ��action�ϲ�,������ӳ���DFAoandAction��
		bool firstFlag = true;//ѡȡ��һ���ڵ���Ϊ�µĽڵ�ı��
		for each (Node* nodeRpr in setOfNodes)
		{
			if (firstFlag){
				minNode->setNum(nodeRpr->getNum());
				firstFlag = false;
			}
			if (nodeRpr->isTerminal()){//����ڵ㼯������һ���ڵ�����̬��������µ�minDFA�Ľڵ�����̬
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
				Node* nextNd = nextByc[0];//�ҳ��ڵ��Ӧ�ַ��ĺ�̣�dfa�ڵ�һ���ַ�ֻ��Ӧһ�����
				//����P��Ľڵ�״̬���ҵ����ߵĽڵ����ڵļ��ϵı��
				for (int j = 0; j < P2.size(); j++)
				{
					if (P2[j].find(nextNd) != P2[j].end()){
						movevec.insert(make_tuple(i, c, j));//����ӳ��
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
	//ɾ��dead state ��not reachable state
}

void initInEdge(){
	for each (char c in alphaTB)
	{
		set<Node*> vc;
		outedge.insert(make_pair(c, vc));
		//inedge.insert(make_pair(c, vc));//---------------------------------����
	}
}
//������㣬����DFA�Ľڵ㼯�Ϻ����ӱ�
void generateDFA(Node* start){
	int dfaNo = 0;
	vector<Node*> dfaNodes;//dfa���µĽڵ㼯��

	layer states;//dfa��״̬���ϣ�ÿ��״̬���ж���ڵ�
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
	Node* node1 = new Node(dfaNo++, isTermin);//���� �µ�DFA�ڵ�
	if (isTermin){
		DFAandAction.insert(ationmap);
		endStates.insert(node1);
	}
	else
		nendStates.insert(node1);

	dfaNodes.push_back(node1);//����dfaNodes�ļ���
	startOfDFA = node1;
	initInEdge();
	int p = 1;//��ǰ�µ�DFA��Ľڵ�����
	int j = 0;
	while (j <= p)
	{
		set<Node*> temp1 = states[j];//ȡ������
		for each (char c in alphaTB)
		{
			set<Node*> e = DFAedge(temp1, c);//��ü��ϵıհ�
			if (e.empty())
				continue;

			bool flag = false;//�ж�states���Ƿ���� ����ıհ�
			int i = 1;
			for (; i <= p; i++)
			{
				if (states[i] == e){
					flag = true;
					break;
				}
			}
			if (flag){//����Ѿ����ڣ���ֱ�Ӽӱ�
				//trans[j][c] = i;
				dfaNodes[j - 1]->addOut(c, dfaNodes[i - 1]);//��������

				//inedge.find(c)->second.insert(dfaNodes[i - 1]);//������߱�
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

				//inedge.find(c)->second.insert(newNode);//������߱�
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
		if (nd->isTerminal()){//�������̬,ִ��action
			//���ݽڵ����ҵ���Ӧ��string
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

	//����
	NFA nfa;
	int lineNum = 0;

	string infilename = "seulex.txt";
	string outfilename = "lex.yy.c";

	infile.open(infilename.c_str(), ios::in);
	outfile.open(outfilename.c_str(), ios::out);

	cout << "Start Analysing" << endl;
	if (infile.good() == false){					//good() ��ʾ�ļ�������û�ж�д����Ҳ���ǻ��ģ�Ҳû�н���.good() �� istream �ķ�����Ҳ�� ifstream �ķ���
		cout << "Open file fail!" << endl;
		return 0;
	}
	//�����
	char ch = infile.get();	//����������ȡһ���ַ���(������ת��intֵ�ͷ�)
	char next = infile.get();
	int state = nfa.CheckIdentifier(ch, next);
	if (state != DEF_BEGIN){
		cout << "File formation error,please CheckIdentifierIdentifier!" << endl;
		return 0;
	}
	while (!infile.eof() && state != DEF_END){		//�ļ�δ��ȡ�꣬��δ����DEF_END
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

	//������ʽ
	infile.get();//����
	state = BEGIN;
	pair<string, string> strpair;				//id�ؼ��ֺ�ת�����re������ʽ
	while (!infile.eof() && state != SEGMENT_ID){//�ڹ���κͶ����֮���RE���ʽ
		ch = infile.get();
		next = infile.get();
		infile.seekg(-1, ios::cur);
		if (ch == '%'){
			next = infile.get();
			state = nfa.CheckIdentifier(ch, next);
			if (state == MYERROR)				//����Ϊ���Ӧ����if(state��= SEGMENT_ID)
			{
				cout << "There is an error in line " << lineNum << endl;
				return 0;
			}
			continue;
		}
		else
			infile.seekg(-1, ios::cur);
		string id, re;//id�ؼ��� re������ʽ
		infile >> id >> re;
		strpair.first = id;
		nfa.ChangeForm(re);
		strpair.second = re;
		nfa.reMap.insert(strpair);
		lineNum++;
		infile.get();
	}
	cout << "The Regular Express segment finished!" << endl;

	//�����
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
			infile.seekg(-1, ios::cur);				//�ú������ļ�ָ��ӵ�ǰλ�û���һλ
		string re, action;
		string rowstr;
		getline(infile, rowstr);						//֮��Ͷ�λ����һ����
		lineNum++;
		//��ȡ����
		string s = "\t";
		int offset = rowstr.find_first_of(s);
		re = rowstr.substr(0, offset);//Tab֮ǰ���ַ���
		if (!nfa.ChangeForm(re)){
			cout << "Regular Express Error!" << endl;
			return 0;
		}
		while (rowstr[offset] == '\t' || rowstr[offset] == ' ')
			offset++;	//�ƶ����ո����Ϊֹ
		string tempstring = rowstr.substr(offset, rowstr.size() - offset + 1);//���θ���
		int len = tempstring.length();
		action = tempstring.substr(1, len - 2);
		nfa.GenerateNFA(re);//������ӦNFA
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



