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
vector<string> valNameTb;//.l�Զ���ı���
//int(*trans)[256] = new int[][256];
vector<char> alphaTB;//�ַ���
typedef pair<char, Node*> edge;
typedef map<char, set<Node*>> edgesByChar;
edgesByChar inedge;//��߱������
edgesByChar outedge;//���߱�


typedef set<Node*> group;
group endStates;//�ս�㼯��
group nendStates;//���ս�㼯��
typedef vector<set<Node*>> layer;

//����ĳ��״̬���ϵ�e�հ�
set<Node*> closure(vector<Node*> S){//��Ȼ�����ظ�����ʹ��set����������ͬʱ����������ʹvector
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
		cout << var->getNum() << '\t';
	}
	cout << endl;
}
//�ж��� �Ƿ����
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
	for each (Node* nd in vec1)   //���ݸ�״̬�������Ƿ�����̬ȷ����״̬�Ƿ�����̬
	{
		if (nd->isTerminal()){
			isTermin = true;
			break;
		}
	}
	Node* node1 = new Node(dfaNo++, isTermin);//���� �µ�DFA�ڵ�
	if (isTermin)
		endStates.insert(node1);
	else
		nendStates.insert(node1);

	dfaNodes.push_back(node1);//����dfaNodes�ļ���

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
				outedge.find(c)->second.insert(dfaNodes[i - 1]);//���ӳ��߱�
			}
			else//��������ڣ������µĽڵ㣬�ӱ�
			{
				p++;
				states.push_back(e);
				bool isTermin = false;
				for each (Node* nd in e)   //���ݸ�״̬�������Ƿ�����̬ȷ����״̬�Ƿ�����̬
				{
					if (nd->isTerminal()){
						isTermin = true;
						break;
					}
				}
				Node* newNode = new Node(dfaNo++, isTermin);
				if (isTermin)            //�����Ƿ��ս������ս�㼯�ϻ���ս�㼯��
					endStates.insert(node1);
				else
					nendStates.insert(node1);

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

/*
void generateLayer(layer ly){
for each (group g in ly)
{
for each (char ch in alphaTB)
{

}
}
}*/
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
}
//Hopcroft's algorithm��С��DFA,�ο�wikipedia���Զ������ۡ����Ժͼ��㵼��
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
		for each (char ch in alphaTB)// for each c in �� do
		{
			set<Node*> X = Jiao(outedge.find(ch)->second, A);//  let X be the set of states for which a transition on c leads to a state in A  C---->A  by c
			for each (set<Node*> Y in P)  //for each set Y in P for which X �� Y is nonempty and Y \ X is nonempty do
			{
				set<Node*> jiaoset = Jiao(X, Y);
				set<Node*> subset = sub(X, Y);

				if (jiaoset.empty() || subset.empty())
					continue;

				remove(P.begin(), P.end(), Y);//  replace Y in P by the two sets X �� Y and Y \ X
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
