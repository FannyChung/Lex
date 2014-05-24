#include"Node.h"
#include<vector>
#include <utility>
#include<iostream>
#include <iterator>
#include <map>

using namespace std;

int Node::sum = 0;

Node::Node()
{
	isTermi = false;
}

Node::Node(int num)
{
	this->num = num;
	isTermi = false;
}

Node::Node(int num, bool isTer)
{
	this->num = num;
	isTermi = isTer;
}


Node::Node(bool isTer){
	isTermi = isTer;
}



void Node::setTerminal(bool isTer){
	this->isTermi = isTer;
}

bool Node::isTerminal(){
	return isTermi;
}

int Node::getNum(){
	return num;
}

void Node::addOut(char on, Node* end){
	out.insert(edge(on, end));
//	_ASSERTE(_CrtCheckMemory);
}

multimap<char, Node*> Node::getOutAll(){
	return out;
}

typedef multimap<char, Node*>::size_type sz_type;

vector<Node*> Node::findNext(char key){
	vector<Node*> s;
	/*
	multimap<char, Node*>::iterator  position,end ;
	position=out.lower_bound(key);
	end = out.upper_bound(key);
	while (position != end)
	{
		s.push_back(position->second);
		++position;
	}*/
	sz_type cnt = out.count(key);
	multimap<char, Node*>::const_iterator itr;
	itr= out.find(key);

	for (sz_type i = 0; i !=cnt; itr++,i++)
	{
		s.push_back(itr->second);
	}
	return s;
}

int Node::getNodeSum(){
	return sum;
}