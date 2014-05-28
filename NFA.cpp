#include"NFA.h"
#include <string>
using namespace std;


//extern ifstream infile;
//extern ofstream outfile;
typedef multimap<char, Node*>::iterator mult;

NFA::NFA(){
	Node * start;//��ʼ�ڵ�
	vector<Node*> terminal;//�ս�㣬terminal����
	nodeIndex = 0;//�����һ�����õı�ʾ״̬�ı�־
	returnIndex = 0;//NFA����ʱ��״̬����
	//lineNum=0;
}

set<char>  NFA::getAlphTB(){
	return alphTB;
}
void NFA::GenerateNFA(string re){
	string str;
	str = PostfixForm(re);//�õ���׺���ʽ
	stack<NFANodeSet> NFAlist;
	NFANodeSet NFAtemp;//��ʱ�ڵ�
	NFANodeSet NFAtemp1;
	NFANodeSet resNFA;
	int i = 0;
	char ch = str[i];
	while (ch != '\0'){
		if (ch != '@'&&ch != '|'&&ch != '^'){//chΪ�ַ�����.��ֱ�ӽ���״̬����                                             ��������
			Node* temp = new Node(nodeIndex++, false);//��ʱ�ڵ㣬��ʾ״̬�ڵ㣬��ʼ�����ս��     �ڵ��
			Node* temp1 = new Node(nodeIndex++, true);//��ʱ��
			temp->addOut(ch, temp1);
			alphTB.insert(ch);
			NFAtemp.start = temp;
			NFAtemp.terminal.clear();//ɾ�������е�����Ԫ��
			(NFAtemp.terminal).push_back(temp1);//��������������һ��ֵΪtemp1������
			NFAlist.push(NFAtemp);
			returnIndex = temp1->getNum();//��ø��ս��Ľڵ��
			
		}
		else{
			switch (ch){//һ��������ʽ�ıհ���NFA
			case '^':{
				NFAtemp = NFAlist.top();
				NFAlist.pop();//ȥ����������NFA�����浽temp
				Node* lastNode = NFAtemp.terminal[0];//                                                          0����������������
				lastNode->setTerminal(false);
				Node* temp = new Node(nodeIndex++, false);//��ʱ�ڵ㣬��ʾ״̬�ڵ㣬��ʼ�����ս��
				Node* temp1 = new Node(nodeIndex++, true);//��ʱ�ڵ㣬��ʾ��һ��״̬�ڵ㣬��ʼ���ս��
				lastNode->addOut(EPSL, NFAtemp.start);//���ӻػ��ߣ�����Ϊ16����1                                  ��������������
				lastNode->addOut(EPSL, temp1);
				temp->addOut(EPSL, NFAtemp.start);
				temp->addOut(EPSL, temp1);

				resNFA.start = temp;
				resNFA.terminal.clear();
				(resNFA.terminal).push_back(temp1);
				NFAlist.push(resNFA);
				returnIndex = temp1->getNum();
				break;
			}
			case '|':{ NFAtemp = NFAlist.top();
				NFAlist.pop();
				NFAtemp1 = NFAlist.top();
				NFAlist.pop();
				Node* temp = new Node(nodeIndex++, false);//��ʱ�ڵ㣬��ʾ״̬�ڵ㣬��ʼ�����ս��
				Node* temp1 = new Node(nodeIndex++, true);//��ʱ�ڵ㣬��ʾ��һ��״̬�ڵ㣬��ʼ���ս��

				temp->addOut(EPSL, NFAtemp.start);
				temp->addOut(EPSL, NFAtemp1.start);

				Node* lastNode1 = NFAtemp.terminal[0];
				Node* lastNode2 = NFAtemp1.terminal[0];
				lastNode1->setTerminal(false);
				lastNode2->setTerminal(false);
				//lastNode2->setNum(NFAtemp1.terminal[0]->getNum());//                                     ??????????
				//lastNode1->setNum(NFAtemp.terminal[0]->getNum());//                                      ??????????
				lastNode1->addOut(EPSL, temp1);
				lastNode2->addOut(EPSL, temp1);
				resNFA.start = temp;
				resNFA.terminal.clear();
				resNFA.terminal.push_back(temp1);
				NFAlist.push(resNFA);
				returnIndex = temp1->getNum();
				break;
			}
			case '@':{//����@ֱ�Ӷ���ǰ����������Ӳ�������Ϊ�Ǻ�׺���ʽ��
				NFAtemp = NFAlist.top();
				NFAlist.pop();
				NFAtemp1 = NFAlist.top();
				NFAlist.pop();
				(NFAtemp1.terminal[0])->setTerminal(false);
				Node* lastNode1 = NFAtemp1.terminal[0];
				Node* lastNode2 = NFAtemp.terminal[0];
				//lastNode2->setNum(NFAtemp.terminal[0]->getNum());
				//lastNode1->setNum(NFAtemp1.terminal[0]->getNum());

				//����һ��NFA���յ���ڶ���NFA����ʼ��ϲ�
				multimap<char, Node*> a = NFAtemp.start->getOutAll();
				for (mult it = a.begin(); it != a.end(); it++)
					lastNode1->addOut((*it).first, (*it).second);
				resNFA.start = NFAtemp1.start;
				resNFA.terminal.clear();
				resNFA.terminal.push_back(lastNode2);
				NFAlist.push(resNFA);

				returnIndex = lastNode2->getNum();
				break;
			}
			default:
				cout << "Create NFA error for string " << re << " !" << endl;
				break;
			}
		}
		ch = str[++i];
		NFAtemp.start = NULL;
		NFAtemp.terminal.clear();
		NFAtemp1.start = NULL;
		NFAtemp1.terminal.clear();
		resNFA.start = NULL;
		resNFA.terminal.clear();
	}
	NFANodeSetVector.push_back(NFAlist.top());
	NFAlist.pop();

}

void NFA::JoinNFA(){
	Node* temp = new Node(nodeIndex++, false);
	NFANodeSet NFA1;
	NFANodeSet NFA2;
	if (NFANodeSetVector.size() == 0)
	{
		cout << "the table is null" << endl;
	}
	if (NFANodeSetVector.size() == 1)
	{
		finalNFA = NFANodeSetVector[0];
	}
	else
	{
		for (int i = 0; i<NFANodeSetVector.size(); i++)
		{
			NFA1 = NFANodeSetVector[i];
			temp->addOut(EPSL, NFA1.start);
			NFA2.terminal.push_back(NFA1.terminal[0]);
		}
		NFA2.start = temp;
		finalNFA = NFA2;//�������NFA
	}

}

//���meseulex.txt�Ķ��塢����Ȳ��ֵı�ʶ��
int NFA::CheckIdentifier(char ch,char next){
	if (ch == '%'){
		//char next = infile.get();
		switch (next){
		case '{':return DEF_BEGIN;
		case '}':return DEF_END;
		case '%':return SEGMENT_ID;
		default:
			//infile.seekg(-1, ios::cur);//���ļ��ĵ�ǰλ��ֹͣ
			//break;
			return MYERROR;
		}
	}
	return MYERROR;
}

/*//�ж��Ƿ��ǺϷ��ַ�
bool NFA::IsLetterNum(char ch)
{
	if ((ch <= '9'&&ch >= '0') || (ch <= 'z'&&ch >= 'a') || (ch <= 'Z'&&ch >= 'A'))
	{
		return true;
	}
	return false;
}
*/

//��������ʽ���д���ʹ��ֻ��|��*��(��)��������ţ�����{}[]-��
bool NFA::ChangeForm(string &re){
	int i = 0;
	string str = "";
	string temp = "";
	char ch = re[i];
	int offset;
	int intcount;
	int j;
	map<string, string>::iterator iter;
	while (ch != '\0'){
		switch (ch){
		case '[':{
			str.append(1, '(');
			ch = re[++i];
			break; }
		case ']':{
			str.append(1, ')');
			ch = re[++i];
			break; }
		case '{':{//���������ݶ����Ѿ������������ʽ
			int offset = re.find_first_of('}', i);//�ҵ���һ����}
			for (j = i + 1; j<offset; j++)
				temp.append(1, re[j]);//��{����һ����}��������ȫ���Ƶ�temp
			iter = reMap.find(temp);
			intcount = reMap.count(temp);
			if (intcount <= 0)
				return false;
			if (iter != reMap.end())//����ҵ����ã��Ѿ������������ʽ������{}
			{
				str.append(1, '(');
				str.append(iter->second);//ճ��map��һ���еĵڶ�������
				str.append(1, ')');
			}
			temp = "";
			i = offset;
			ch = re[++i];
			break; }
		case '-':{
			char before = re[i - 1];
			char after = re[i + 1];
			str.erase(str.length() - 1, 1);//��str.length()-1��ʼ��ɾ��һ���ַ�
			if ((re[i - 2] <= '9'&&re[i - 2] >= '0') || (re[i - 2] <= 'z'&&re[i - 2] >= 'a') || (re[i - 2] <= 'Z'&&re[i - 2] >= 'A'))
				str.append(1, '|');			
				
			while (before <= after)
			{
				//�γɡ�0|1|2|...��
				str.append(1, before);
				str.append(1, '|');
				before++;
			}
			str.append(1, after);
			i += 2;//������2λ
			ch = re[i];
			break;
		}
		case '"':{//""�ڵ�����ֱ��ת��Ϊ�����ݣ�
			offset = re.find_first_of('"', i + 1);//��i+1λ�ÿ�ʼѰ�ҵ�һ������������λ�ã���Ѱ����ǰһ������ƥ��ġ�
			temp = re.substr(i + 1, offset - i - 1);//ȡ���������м�Ĳ���ֱ�Ӹ���
			str.append(1, '(');
			str.append(temp);
			str.append(1, ')');//�ѣ����ݣ��ӵ�str����
			i = offset;
			ch = re[++i];
			break; }
		case '+':{
			str.append(1, re[i - 1]);   //��a+����ʽתΪaa*
			str.append(1, '^');
			ch = re[++i];
			break; }
		default:{
			str.append(1, ch);
			ch = re[++i];
			break; }
		}
	}
	re = str;
	return true;
}

/*
//��*��)��. ���ַ�   �����'@'  ����'#'����   ���������⣡����
string NFA::DealWithRe(string re)
{
	string str = "";
	for (int i = 0; i<re.length(); i++)
	{
		char ch = re[i];
		str.append(1, ch);
		if (ch != '|' && ch != '(')
		{
			char temp = re[i + 1];
			if (temp != '\0' && temp != '^' && temp != '|' && temp != ')')
				str.append(1, '@');
		}
	}
	str.append(1, '#');
	return str;
}
*/

//���÷ָ��������ȼ�
int NFA::Priority(char c)
{
	if (c == '#')
	{
		return 0;
	}
	else if (c == '|')
	{
		return 1;
	}
	else if (c == '.')
		return 2;
	else if (c == '@')
	{
		return 3;
	}
	else if (c == '^')
	{
		return 4;
	}
	else{
		return 0;
	}
}
//��REת��Ϊ��׺���ʽ                      ��1|2*��.(3|4)#ת��Ϊ12*|.34|
string NFA::PostfixForm(string rel){
	//��*��)��. ���ַ�   �����'@'  ����'#'����   ���������⣡����
	//string re = DealWithRe(rel);
	string str1 = "";
	for (int i = 0; i<rel.length(); i++)
	{
		char ch = rel[i];
		str1.append(1, ch);
		if (ch != '|' && ch != '(')
		{
			char temp = rel[i + 1];
			if (temp != '\0' && temp != '^' && temp != '|' && temp != ')')
				str1.append(1, '@');
		}
	}
	str1.append(1, '#');
	string re=str1;


	string str;
	int i = 0;
	int j = 0;
	stack<char> tempS;
	tempS.push('#');
	char ch = re[i];
	while (ch != '#')
	{
		if (ch == ' ')
			ch = re[++i];
		else if (ch == '('){
			tempS.push(ch);
			ch = re[++i];
		}
		else if (ch == ')'){
			if (re.size() == 2)                                                        //��������
				str.append(1, ch);
			else{
				while (tempS.top() != '('){
					str.append(1, tempS.top());
					tempS.pop();
				}//����pop
				tempS.pop();//pop��
			}
			ch = re[++i];
		}
		else if (ch == '@' || ch == '|' || ch == '^'){
			char c = tempS.top();
			while (Priority(c) >= Priority(ch)){//�Ƚ����ȼ���ջ�����ȼ������ȵ���
				str.append(1, c);
				tempS.pop();
				c = tempS.top();
			}
			tempS.push(ch);
			ch = re[++i];
		}//���ȼ��;�ѹջ
		else{
			str.append(1, ch);
			ch = re[++i];
		}
	}
	while (tempS.top() != '#'){//��ʣ��ջ���ݵ���
		str.append(1, tempS.top());
		tempS.pop();
	}
	tempS.pop();//����#
	str.append(1, '\0');
	return str;
}