#include"NFA.h"
#include <string>
using namespace std;


//extern ifstream infile;
//extern ofstream outfile;
typedef multimap<char, Node*>::iterator mult;

NFA::NFA(){
	Node * start;//开始节点
	vector<Node*> terminal;//终结点，terminal容器
	nodeIndex = 0;//表达下一个可用的表示状态的标志
	returnIndex = 0;//NFA结束时的状态数量
	//lineNum=0;
}

set<char>  NFA::getAlphTB(){
	return alphTB;
}
void NFA::GenerateNFA(string re){
	string str;
	str = PostfixForm(re);//得到后缀表达式
	stack<NFANodeSet> NFAlist;
	NFANodeSet NFAtemp;//临时节点
	NFANodeSet NFAtemp1;
	NFANodeSet resNFA;
	int i = 0;
	char ch = str[i];
	while (ch != '\0'){
		if (ch != '@'&&ch != '|'&&ch != '^'){//ch为字符或者.，直接建立状态及边                                             ？？？？
			Node* temp = new Node(nodeIndex++, false);//临时节点，表示状态节点，初始不是终结点     节点号
			Node* temp1 = new Node(nodeIndex++, true);//临时节
			temp->addOut(ch, temp1);
			alphTB.insert(ch);
			NFAtemp.start = temp;
			NFAtemp.terminal.clear();//删除容器中的所有元素
			(NFAtemp.terminal).push_back(temp1);//在容器的最后添加一个值为temp1的数据
			NFAlist.push(NFAtemp);
			returnIndex = temp1->getNum();//获得该终结点的节点号
			
		}
		else{
			switch (ch){//一个正则表达式的闭包的NFA
			case '^':{
				NFAtemp = NFAlist.top();
				NFAlist.pop();//去掉最近加入的NFA并保存到temp
				Node* lastNode = NFAtemp.terminal[0];//                                                          0？？？？？？？？
				lastNode->setTerminal(false);
				Node* temp = new Node(nodeIndex++, false);//临时节点，表示状态节点，初始不是终结点
				Node* temp1 = new Node(nodeIndex++, true);//临时节点，表示下一个状态节点，初始是终结点
				lastNode->addOut(EPSL, NFAtemp.start);//增加回环边，边上为16进制1                                  ？？？？？？？
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
				Node* temp = new Node(nodeIndex++, false);//临时节点，表示状态节点，初始不是终结点
				Node* temp1 = new Node(nodeIndex++, true);//临时节点，表示下一个状态节点，初始是终结点

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
			case '@':{//读到@直接对它前两项进行连接操作（因为是后缀表达式）
				NFAtemp = NFAlist.top();
				NFAlist.pop();
				NFAtemp1 = NFAlist.top();
				NFAlist.pop();
				(NFAtemp1.terminal[0])->setTerminal(false);
				Node* lastNode1 = NFAtemp1.terminal[0];
				Node* lastNode2 = NFAtemp.terminal[0];
				//lastNode2->setNum(NFAtemp.terminal[0]->getNum());
				//lastNode1->setNum(NFAtemp1.terminal[0]->getNum());

				//将第一个NFA的终点与第二个NFA的起始点合并
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
		finalNFA = NFA2;//结果最终NFA
	}

}

//检测meseulex.txt的定义、规则等部分的标识符
int NFA::CheckIdentifier(char ch,char next){
	if (ch == '%'){
		//char next = infile.get();
		switch (next){
		case '{':return DEF_BEGIN;
		case '}':return DEF_END;
		case '%':return SEGMENT_ID;
		default:
			//infile.seekg(-1, ios::cur);//在文件的当前位置停止
			//break;
			return MYERROR;
		}
	}
	return MYERROR;
}

/*//判断是否是合法字符
bool NFA::IsLetterNum(char ch)
{
	if ((ch <= '9'&&ch >= '0') || (ch <= 'z'&&ch >= 'a') || (ch <= 'Z'&&ch >= 'A'))
	{
		return true;
	}
	return false;
}
*/

//对正规表达式进行处理使其只有|、*、(、)等特殊符号，代换{}[]-等
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
		case '{':{//大括号内容都是已经定义的正规表达式
			int offset = re.find_first_of('}', i);//找到第一个右}
			for (j = i + 1; j<offset; j++)
				temp.append(1, re[j]);//从{到第一个右}的内容完全复制到temp
			iter = reMap.find(temp);
			intcount = reMap.count(temp);
			if (intcount <= 0)
				return false;
			if (iter != reMap.end())//如果找到，用（已经定义的正规表达式）代替{}
			{
				str.append(1, '(');
				str.append(iter->second);//粘贴map中一项中的第二个数据
				str.append(1, ')');
			}
			temp = "";
			i = offset;
			ch = re[++i];
			break; }
		case '-':{
			char before = re[i - 1];
			char after = re[i + 1];
			str.erase(str.length() - 1, 1);//从str.length()-1开始，删除一个字符
			if ((re[i - 2] <= '9'&&re[i - 2] >= '0') || (re[i - 2] <= 'z'&&re[i - 2] >= 'a') || (re[i - 2] <= 'Z'&&re[i - 2] >= 'A'))
				str.append(1, '|');			
				
			while (before <= after)
			{
				//形成“0|1|2|...”
				str.append(1, before);
				str.append(1, '|');
				before++;
			}
			str.append(1, after);
			i += 2;//向右移2位
			ch = re[i];
			break;
		}
		case '"':{//""内的内容直接转换为（内容）
			offset = re.find_first_of('"', i + 1);//从i+1位置开始寻找第一个”，返回其位置，即寻找与前一个“相匹配的”
			temp = re.substr(i + 1, offset - i - 1);//取两个引号中间的部分直接复制
			str.append(1, '(');
			str.append(temp);
			str.append(1, ')');//把（内容）接到str后面
			i = offset;
			ch = re[++i];
			break; }
		case '+':{
			str.append(1, re[i - 1]);   //将a+的形式转为aa*
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
//将*、)、. 、字符   后面加'@'  并以'#'结束   可能有问题！！！
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

//设置分隔符的优先级
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
//将RE转化为后缀表达式                      （1|2*）.(3|4)#转化为12*|.34|
string NFA::PostfixForm(string rel){
	//将*、)、. 、字符   后面加'@'  并以'#'结束   可能有问题！！！
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
			if (re.size() == 2)                                                        //？？？？
				str.append(1, ch);
			else{
				while (tempS.top() != '('){
					str.append(1, tempS.top());
					tempS.pop();
				}//反向pop
				tempS.pop();//pop（
			}
			ch = re[++i];
		}
		else if (ch == '@' || ch == '|' || ch == '^'){
			char c = tempS.top();
			while (Priority(c) >= Priority(ch)){//比较优先级，栈顶优先级高优先弹出
				str.append(1, c);
				tempS.pop();
				c = tempS.top();
			}
			tempS.push(ch);
			ch = re[++i];
		}//优先级低就压栈
		else{
			str.append(1, ch);
			ch = re[++i];
		}
	}
	while (tempS.top() != '#'){//将剩余栈内容弹出
		str.append(1, tempS.top());
		tempS.pop();
	}
	tempS.pop();//弹出#
	str.append(1, '\0');
	return str;
}