#pragma once
// ReSharper disable All
#include<iostream>
#include<vector>
#include<cassert>
#include"framework.h"
#include<windows.h>
using namespace std;


class Menu;
class Node :public Instance
{
private:
	int lev = 0;//�ڵ�㼶�����ڵ�Ϊ0
public:
	Node* parent = nullptr; //���ڵ�ָ�룬���ڵ�Ϊnullptr
	void (*func)(Menu& munu, Canvas& canvas) = nullptr; //�ڵ㹦�ܺ�����ѭ��ִ��
	void (*onceFunc)(Menu& munu) = nullptr; //�ڵ㹦�ܸ���������ִֻ��һ��
	string tag; //�ڵ���
	vector<Node*> childs; //�ӽڵ�ָ���
	bool funcNode = false;  //�Ƿ�Ϊ��һ���ܽڵ�

	//��ǰ�ڵ�㼶
	int Level() { return lev; }

	
#pragma region �ڵ㹹��
	//����/����
	Node()
	{
		tag = "RootNode";
		lev = 0;
	}
	//�βΣ����ڵ�ָ�룬�ڵ������ڵ㹦�ܺ���(ѭ��)���Ƿ�Ϊ��һ���ܽڵ㣬��������(ִֻ��һ��)
	Node(Node* p, const char* t, void f(Menu& menu, Canvas& canvas) = nullptr, bool fnode = false, void once(Menu& menu) = nullptr)
	{
		assert(p != nullptr);
		parent = p;
		func = f;
		tag = t;
		funcNode = fnode;
		lev = p->lev + 1;
		onceFunc = once;
		parent->childs.push_back(this);
	}
	//DFS����
	~Node()
	{
		if (childs.size() == 0) return;
		for (auto i : childs) delete i;
	}
	
#pragma endregion
#pragma region �ڵ��л�
	//������ת����һ�ڵ�
	Node* Next(int id, bool* result=NULL)
	{
		assert(id > 0);  
		if (id >= 1 && id <= childs.size())
		{
			if (result != NULL)*result = true;
			return childs[id - 1];
		}
		if (result != NULL)*result = false;
		return this;
	}
	//���Է�����һ�ڵ�
	Node* Last(bool* result = NULL)
	{
		if (parent != nullptr)
		{
			if (result != NULL)*result = true;
			return parent;
		}
		if (result != NULL)*result = false;
		return this;
	}
#pragma endregion
};

//�ڵ�˵��࣬����������߼���ֻ����ά���ڵ���
class Menu :public Instance
{
	Node* root;  //���ڵ�
	
	Menu() = default;
public:
	//��ǰ�ڵ�,��Ȼ���Ա��޸��ˣ����ǲ�����....��
	Node* current;
	Canvas* canvas;//����

	//����/����
	Menu(Node* first, Canvas* cv)
	{
		root = first;  //���ڵ�
		canvas = cv;  //����
		current = first;  //��ǰ�ڵ�

	}
	~Menu()
	{
		//�ڸ��ڵ�DFS��������N����
		delete root;
	}
#pragma region �ڵ���ת����

	//�ڵ㺯��������ڵ���ת�������������ת����current���䣬ÿ����ת�����ýڵ���¸�������
	void ToRoot()
	{
		current = root;
		if (current->onceFunc != nullptr) current->onceFunc(*this);
	}
	//�ڵ㺯��������ڵ���ת�������������ת����current���䣬ÿ����ת�����ýڵ���¸�������
	void Last()
	{
		current = current->Last();
		if (current->onceFunc != nullptr) current->onceFunc(*this);
	}
	//�ڵ㺯��������ڵ���ת�������������ת����current���䣬ÿ����ת�����ýڵ���¸�������
	void Next(int idx)
	{
		current = current->Next(idx);
		if (current->onceFunc != nullptr) current->onceFunc(*this);
	}

#pragma endregion
};
//�����飬����һ���Զ�ȡ����̨����
class InputGroup
{
private:
	COORD coord;
	HANDLE handle;
	struct KeyValuePair
	{
		string key;
		bool value;
		char* format;
		void* p;
	};
	vector<KeyValuePair> pool;
	bool nowarning = false;
public:
	InputGroup(int initY, bool warnning = true)
	{
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
		coord.X = 0;
		coord.Y = initY;
		nowarning = !warnning;
	}
	void Add(const string& str, const char* formatter, void* pp)
	{

		pool.push_back({ str,true,(char*)formatter,pp });
	}
	void Add(const string& str)
	{
		pool.push_back({ str,false });
	}

	void Print()
	{
		for (auto& i : pool)
		{
			cout << i.key << endl;
		}
	}
	bool GetInput()
	{
		for (auto& i : pool)
		{
			if (i.value)
			{
				coord.X = i.key.length();
				SetConsoleCursorPosition(handle, coord);

				int r = scanf(i.format, i.p);
				if (r == 0 && !nowarning)
				{
					if (!nowarning)MessageBox(NULL, ("�����ʽ����"), ("�����������벢���³���"), MB_SYSTEMMODAL | MB_ICONSTOP);
					return false;
				}

			}
			coord.Y++;
		}
		return true;
	}
};

