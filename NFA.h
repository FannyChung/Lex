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

//宏////
#define BEGIN	301
#define DEF_BEGIN	302   //to identify %{，定义部分起始符号
#define DEF_END		303   //to identify %}，定义部分终止符号
#define SEGMENT_ID	304	//to identify %%，规则部分标识符
#define MYERROR		305



class NFA{
public:
	NFA();
	void GenerateNFA(string re);//生成一个正规表达式的NFANodeSet
	void JoinNFA();//把几个NFANodeSet合并成一个大的NFANodeSet
	int CheckIdentifier(char ch, char next);//检测meseulex.txt的定义、规则等部分的标识符%{}等
	//bool IsLetterNum(char ch);//判断是否是合法字符
	bool ChangeForm(string &re);//对正规表达式进行处理使其只有|、*、(、)等特殊符号，代换{}[]-等
	//string DealWithRe(string re);//将*、)、. 、字符   后面加'@'  并以'#'结束   可能有问题！！！
	int Priority(char c);//设置分隔符的优先级
	string PostfixForm(string rel);//将RE转化为后缀表达式  
	set<char> getAlphTB();
//private:
	
	typedef struct {
		Node * start;//开始节点
		vector<Node*> terminal;//终结点
	}NFANodeSet;

	map<string, string> reMap;//存储每个正规表达式的Map
	int nodeIndex;//节点编号
	int returnIndex;//某一个NFANodeSet的terminal节点的编号
	NFANodeSet finalNFA;//最终的NFANodeSet
	vector<NFANodeSet> NFANodeSetVector;//存储每个正规表达式的NFANodeSet
	set<char> alphTB;
	
};
#endif