#include <vector>
#include <map>
using namespace std;

class Node
{
public:
	typedef pair<char, Node*> edge;
	Node();
	Node(bool);//�����Ƿ����ս�㹹��Node
	Node(int);
	Node(int, bool);
	//~Node();//ɾ���ڵ�ʱʹ������1
	bool isTerminal();//��ȡ�Ƿ����ս��
	void setTerminal(bool);//�����Ƿ����ս�㣬true������
	int getNum();//��ȡ�ýڵ���
	void addOut(char,Node*);//���ӳ��ߺͺ�̽ڵ�
	multimap<char, Node*> getOutAll();//��ȡ���еĳ��ߺͺ�̽ڵ㣬����multimap<char, Node*>
	vector<Node*> findNext(char);//��ȡָ�����ߵĺ�̽ڵ㣬���ؽڵ�ָ���vector
	int getNodeSum();//��ȡ�ܵĽڵ�����

private:
	static int sum;
	int num;//�ڵ���
	bool isTermi;//�Ƿ����ս��
	multimap<char, Node*> out;//���ڼ���
};

