#include "next.h"
#include<iostream>
#include<fstream>
#include "head.h"
#include<unordered_map>
#include<queue>
#include<sstream>
#include<QPainter>
#include<QPushButton>
#include<stack>

using namespace std;

const int height0 = 200;                                //每层高度
extern unordered_map<int, int[4]>son, son1;
extern unordered_map<int, int> Stat_F;
extern int Stat_Num[N];
extern vector<QString> a;
extern int flag;


MyScrollArea::MyScrollArea(QWidget* parent)
	: QWidget(parent) {}

MyScrollArea::~MyScrollArea() {}

void MyScrollArea::paintEvent(QPaintEvent* event) {    //滑动窗口的绘图事件  
	QPainter painter(this);                            //将父类设置为MyScrollArea窗口
	queue<int> child;
	child.push(Stat_Num[1]);
	int father = 0;
	int elem_number = 1;
	int x = 20;
	int y = 20 - height0;
	int ccount = 0;
	while (1) {                                       //将父结点输出到屏幕上
		y += height0;                                 //再将子节点入队
		ccount++;                                     //这样表示搜索树的一层（层序遍历）
		int count_num = 0;
		int number_ = 0;
		while (count_num < elem_number) {
			//cout << count_num << endl;
			father = child.front();
			child.pop();
			if (Stat_Num[ccount] == father) {
				QPen pen(QColor(255, 0, 0));
				painter.setPen(pen);
			}
			else {
				QPen pen(QColor(0, 0, 0));
				painter.setPen(pen);
			}
			int father0 = father;
			int e = 1e8;
			for (int i = 0; i < 3; i++)               //将整数分解为数组的形式
			{
				for (int j = 0; j < 3; j++)
				{
					painter.drawRect(x + count_num * 100 + j * 30, y + i * 30, 30, 30);
					if (father0 / e != 0)
						painter.drawText(QRect(x + count_num * 100 + j * 30 + 10, y + i * 30 + 10, 30, 30), a[father0 / e]);
					father0 = father0 % e;
					e /= 10;
				}
			}
			stringstream ss; ss << Stat_F[father];
			string sout = "F = " + ss.str();
			painter.drawText(QRect(x + count_num * 100 + 50, y + 90 + 10, 100, 30), sout.c_str());
			for (int i = 0; i < 4; i++)
			{                                            //分别输出到屏幕上    
				if (son[father][i] != 0)
				{
					painter.drawLine(x + count_num * 100 + 45, y + 90, x + number_ * 100 + 45, y + height0);
					child.push(son[father][i]);
					number_++;
				}
			}
			count_num++;
		}
		elem_number = number_;
		if (number_ == 0)                             //当队列中所有父节点都没有子节点时，循环结束
			break;
	}
}


