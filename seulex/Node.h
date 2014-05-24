#include <vector>
#include <map>
using namespace std;

class Node
{
public:
	typedef pair<char, Node*> edge;
	Node();
	Node(bool);//根据是否是终结点构造Node
	Node(int);
	Node(int, bool);
	//~Node();//删除节点时使总数减1
	bool isTerminal();//获取是否是终结点
	void setTerminal(bool);//定义是否是终结点，true代表是
	int getNum();//获取该节点编号
	void addOut(char,Node*);//增加出边和后继节点
	multimap<char, Node*> getOutAll();//获取所有的出边和后继节点，返回multimap<char, Node*>
	vector<Node*> findNext(char);//获取指定出边的后继节点，返回节点指针的vector
	int getNodeSum();//获取总的节点数量

private:
	static int sum;
	int num;//节点编号
	bool isTermi;//是否是终结点
	multimap<char, Node*> out;//出口集合
};

