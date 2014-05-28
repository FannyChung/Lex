//#pragma warning(disable:4786)
#ifndef NFA_H
#define NFA_H
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stack>
#include <vector>
#include <set>
#include "Node.h"
#define EPSL 84775
//#include <set>

//��////
#define BEGIN	301
#define DEF_BEGIN	302   //to identify %{�����岿����ʼ����
#define DEF_END		303   //to identify %}�����岿����ֹ����
#define SEGMENT_ID	304	//to identify %%�����򲿷ֱ�ʶ��
#define MYERROR		305



class NFA{
public:
	NFA();
	void GenerateNFA(string re);//����һ��������ʽ��NFANodeSet
	void JoinNFA();//�Ѽ���NFANodeSet�ϲ���һ�����NFANodeSet
	int CheckIdentifier(char ch, char next);//���meseulex.txt�Ķ��塢����Ȳ��ֵı�ʶ��%{}��
	//bool IsLetterNum(char ch);//�ж��Ƿ��ǺϷ��ַ�
	bool ChangeForm(string &re);//��������ʽ���д���ʹ��ֻ��|��*��(��)��������ţ�����{}[]-��
	//string DealWithRe(string re);//��*��)��. ���ַ�   �����'@'  ����'#'����   ���������⣡����
	int Priority(char c);//���÷ָ��������ȼ�
	string PostfixForm(string rel);//��REת��Ϊ��׺���ʽ  
	set<char> getAlphTB();
//private:
	
	typedef struct {
		Node * start;//��ʼ�ڵ�
		vector<Node*> terminal;//�ս��
	}NFANodeSet;

	map<string, string> reMap;//�洢ÿ��������ʽ��Map
	int nodeIndex;//�ڵ���
	int returnIndex;//ĳһ��NFANodeSet��terminal�ڵ�ı��
	NFANodeSet finalNFA;//���յ�NFANodeSet
	vector<NFANodeSet> NFANodeSetVector;//�洢ÿ��������ʽ��NFANodeSet
	set<char> alphTB;
	
};
#endif