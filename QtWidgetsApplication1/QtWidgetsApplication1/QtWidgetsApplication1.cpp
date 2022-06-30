#include "QtWidgetsApplication1.h"
#include<QPushButton>
#include<QPainter>
#include<vector>
#include<string>
#include<qbrush.h>
#include<fstream>
#include<iostream>
#include<unordered_map>
#include<queue>
#include<random>
#include<algorithm>
#include<cmath>
#include"head.h"
#include<QDialog>
#include<windows.h>
#include <QInputDialog>
#include<QLineEdit>//提示消息
#include<qmessagebox.h>
#include<QCompleter>
using namespace std;



int Stat[3][3][1000] = { 0 };     //存储状态，数组的第三维表示步数
int count0 = 0;                  //计数器，用于记录当前步数
vector<QString> a{ "0", "1", "2", "3", "4", "5", "6", "7","8","9" };
int color_first[10] = { 0,0,51,178,255,255,204,255,76,224 };
int color_second[10] = { 0,255,153,102,0,255,255,0,0,224 };
int color_third[10] = { 0,128,255,255,0,51,255,127,153,224 };
int a1[9] = { 0 }, b1[9] = { 0 };  //全局变量用于接收从输入框所输入的数组

unordered_map<int, string> SavePath = {
	{1,"bfs"},{2,"gbfs"},{3,"A_star_Man"}, {4,"A_star_Eu"},{5,"A_star_Man+Gmod11"},{6,"A_star_misplace"},
	{7,"A_star_Man1+mis1"},{8,"IDA_star"},{9,"Bothway_bfs"},{10,"Bothway_gbfs"},{11,"Bothway_A_star_Man"}
};
unordered_map<int, int> fa, fa1, fa2, vis1, vis2, Stat_F;
//fa、fa1、fa2用于存每个状态的上一个状态，vis1、vis2用于在双向搜索中记录状态
//STat_F记录所有状态的F值
unordered_map<int, int[4]> son, son2;
//son、son2用于Search函数和Bothwaysearch函数中存储每个状态的下n个状态,stat_f用于存储搜索树的F值。
priority_queue<grid> q, q2;
//q、q2用于Search函数和Bothwaysearch函数中的优先队列。
int Stat_Num[N];
//记录路径
int flag = 1;
//选择搜索方式
//flag=1 广度优先搜索
//flag=2 贪婪最佳优先搜索
//flag=3 A*搜索 曼哈顿距离
//flag=4 A*搜索 欧几里得距离
//flag=5 A*搜素 曼哈顿距离+G(n)取模优化 (路径不一定为最优路径)
//flag=6 A*搜索 错位数字个数
//flag=7 A*搜索 错位数字与曼哈顿距离的线性组合
//flag=8 IDA* 曼哈顿距离
//flag=9 双向广度优先搜索
//flag=10 双向贪婪最佳优先搜索
//flag=11 双向A*搜索 曼哈顿距离
//我们在可视化界面中只选择了flag=3,5,7,11,其中双向A*搜索的效果最好。
int mod = 11;//A*算法的其中一种优化中使用的模
int ida_times, ida_moves;//IDA*算法扩展结点次数和路径长度
double man_coeff = 0.64, mis_coeff = 0.36;//错位数字与曼哈顿距离的线性组合中两者占比

void init()//初始化
{
	fa.clear();
	fa1.clear();
	fa2.clear();
	son.clear();
	son2.clear();
	vis1.clear();
	vis2.clear();
	while (!q.empty())q.pop();
	while (!q2.empty())q2.pop();
}
bool grid::isend()//判断是否到最终节点
{
	return trans(stat) == trans(target);
}
void grid::update(int fa_G)//更新G、H、F
{
	fH();
	fG(fa_G);
	fF();
}
void grid::fH()//计算H
{
	double sum = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int targetx = target[i][j];
			if (targetx == 0)continue;//如果是空格就跳过
			int* nowP = findx(targetx);//找到对应位置
			//计算value
			if (flag == 2 || flag == 3 || flag == 5 || flag == 11)
				sum += value_Man(nowP[0] - i, nowP[1] - j);
			else if (flag == 4)
				sum += value_Eu(nowP[0] - i, nowP[1] - j);
			else if (flag == 6)
				sum += value_Misplace(targetx, stat[i][j]);
			else if (flag == 7)
				sum += man_coeff * value_Man(nowP[0] - i, nowP[1] - j) + mis_coeff * value_Misplace(targetx, stat[i][j]);
		}
	}
	H = int(sum);//double转int方便比较
}
void grid::fG(int fa_G)//计算G
{
	G = fa_G + 1;
}
void grid::fF()//计算F
{
	if (flag == 1 || flag == 9)F = G;//bfs或Bothwaybfs
	else if (flag == 2 || flag == 10)F = H;//gbfs或Bothwaygbfs
	else if (flag != 5)F = G + H;//A*
	else F = G % mod + H;//A* mod11优化
}
int* grid::findx(int x)//找到x在stat中的位置
{
	int* sit = new int[2];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (stat[i][j] == x)
			{
				sit[0] = i, sit[1] = j;
				return sit;
			}
		}
	}
	return sit;
}
void grid::find0()//找到0在stat中的位置
{
	zero = findx(0);
}
bool operator <(const grid& a, const grid& b)//运算符重载用于优先队列
{
	if (a.F != b.F)
		return a.F > b.F;
	else
		return a.G > b.G;
}
int value_Man(int x, int y)//曼哈顿距离估值函数
{
	return abs(x) + abs(y);
}
double value_Eu(int x, int y)//欧几里得距离估值函数
{
	return sqrt(x * x + y * y);
}
int value_Misplace(int x, int y)//错位数个数
{
	return (x != y) ? 1 : 0;
}
int trans(int stat[3][3])//转换函数,将3*3的二维数组转换为int
{
	//例如
	//{ 6, 8, 2 }
	//{ 7, 0, 3 }
	//{ 1, 4, 5 }
	//转换为682703145。
	int e = 1e8;
	int num = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			num += stat[i][j] * e;
			e /= 10;
		}
	}
	return num;
}
void printAns(int moves, int times)//输出结果路径到文件中
{
	ofstream out;
	string path = SavePath[flag] + ".txt";
	out.open(path);
	out << moves << " " << times << endl;
	for (int k = 1; k <= moves; k++)
	{
		int num = Stat_Num[k], e = 1e8;
		for (int i = 0; i < 3; i++, out << endl)
		{
			for (int j = 0; j < 3; j++)
			{
				out << num / e << ' ';
				num = num % e;
				e /= 10;
			}
		}
		out << "-----------------------------------------------" << endl;
	}
	out.close();
}
void seeAns(int stat, int times)//对fa进行处理得到Stat_num
{
	int moves = 0, t = stat;
	//这步从结果向上回溯找到初始结点结束
	while (t != -1)
	{
		Stat_Num[++moves] = t;
		t = fa[t];
	}
	reverse(Stat_Num + 1, Stat_Num + 1 + moves);//逆转数组
	printAns(moves, times);//输出数组
}
void seeAnsBothWay(int stat, int times)//对fa1,fa2进行处理得到Stat_num
{
	int moves = 0, t = stat;
	while (t != -1)
	{
		Stat_Num[++moves] = t;
		t = fa1[t];
	}
	reverse(Stat_Num + 1, Stat_Num + 1 + moves);
	t = fa2[stat];
	while (t != -1)
	{
		Stat_Num[++moves] = t;
		t = fa2[t];
	}
	printAns(moves, times);
}
void Search(int startStat[3][3], int endStat[3][3])	//搜索函数
{
	int times = 0, x = 0, y = 0, xx = 0, yy = 0, statnum = 0, fa_statnum = trans(startStat);
	grid g(startStat, endStat);
	fa[fa_statnum] = -1;						//初始状态没有父节点
	Stat_F[fa_statnum] = g.F;
	q.push(g);										//向优先队列加入初始节点
	while (!q.empty())
	{
		grid s = q.top();
		int x = s.zero[0], y = s.zero[1];
		fa_statnum = trans(s.stat);
		q.pop();
		for (int d = 0; d < 4; d++)
		{
			xx = x + dx[d], yy = y + dy[d];
			if (xx < 0 || xx >= 3 || yy < 0 || yy >= 3)continue;//如果越界就跳过
			swap(s.stat[x][y], s.stat[xx][yy]);		//产生新的子状态
			statnum = trans(s.stat);
			if (fa[statnum] != 0)					//走过这个状态
			{
				swap(s.stat[xx][yy], s.stat[x][y]);
				continue;
			}
			grid tmp(s.stat, endStat, s.G);
			fa[statnum] = fa_statnum;
			son[fa_statnum][d] = statnum;
			Stat_F[statnum] = tmp.F;
			times++;
			if (tmp.isend())						//到达最终状态
			{
				seeAns(statnum, times);
				return;
			}
			q.push(tmp);
			swap(s.stat[xx][yy], s.stat[x][y]);		//恢复s.stat
		}
	}
}
void BothWaySearch(int startStat[3][3], int endStat[3][3])//双向搜索函数
{
	int times = 0, turn = 0, x = 0, y = 0, xx = 0, yy = 0, statnum = 0, fa_statnum = 0;
	fa1[trans(startStat)] = -1;
	fa2[trans(endStat)] = -1;
	grid gstart(startStat, endStat);
	grid gend(endStat, startStat);
	q.push(gstart);
	q2.push(gend);
	while (!q.empty() && !q2.empty())
	{
		if (q.size() < q2.size())turn = 0;				//哪个搜索树节点少就搜哪个
		else turn = 1;
		grid s = (turn == 0 ? q.top() : q2.top());
		int fa_statnum = trans(s.stat);
		if (turn == 0)
		{
			q.pop();
			if (vis2[fa_statnum])						//如果在另一个树有这个状态，说明对上了，结束
			{
				seeAnsBothWay(fa_statnum, times);
				return;
			}
		}
		else
		{
			q2.pop();
			if (vis1[fa_statnum])						//如果在另一个树有这个状态，说明对上了，结束
			{
				seeAnsBothWay(fa_statnum, times);
				return;
			}
		}
		x = s.zero[0], y = s.zero[1];
		for (int d = 0; d < 4; d++)							//这里操作同上一个函数
		{
			xx = x + dx[d], yy = y + dy[d];
			if (xx < 0 || xx >= 3 || yy < 0 || yy >= 3)continue;
			swap(s.stat[x][y], s.stat[xx][yy]);
			statnum = trans(s.stat);
			if (turn == 0)
			{
				if (fa1[statnum] != 0)
				{
					swap(s.stat[xx][yy], s.stat[x][y]);
					continue;
				}
				fa1[statnum] = fa_statnum;
				son[fa_statnum][d] = statnum;
				vis1[statnum] = 1;
			}
			else
			{
				if (fa2[statnum] != 0)
				{
					swap(s.stat[xx][yy], s.stat[x][y]);
					continue;
				}
				fa2[statnum] = fa_statnum;
				son2[fa_statnum][d] = statnum;
				vis2[statnum] = 1;
			}
			times++;
			if (turn == 0)
			{
				grid tmpG(s.stat, endStat, s.G);
				q.push(tmpG);
			}
			else
			{
				grid tmpG(s.stat, startStat, s.G);
				q2.push(tmpG);
			}
			swap(s.stat[xx][yy], s.stat[x][y]);
		}
	}
}
void IDA_findx(int stat[3][3], int& x, int& y, int target)//IDA*的findx函数
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (stat[i][j] == target)
			{
				x = i;
				y = j;
				return;
			}
		}
	}
}
int IDA_fH(int stat[3][3], int endStat[3][3])//IDA*的fH函数,计算曼哈顿距离
{
	int H = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int target = endStat[i][j];
			int x = 0, y = 0;
			IDA_findx(stat, x, y, target);
			H += value_Man(x - i, y - j);
		}
	}
	return H;
}
void IDDFS(int stat[3][3], int endStat[3][3], int& dfs_end, int depth, int G)//IDA*的DFS函数
{
	ida_times++;
	int H = IDA_fH(stat, endStat);
	if (H == 0)
	{
		Stat_Num[G] = trans(stat);
		ida_moves = G;
		dfs_end = trans(stat);
		return;
	}
	if (H + G > depth || dfs_end != 0)return;
	int x = 0, y = 0;
	IDA_findx(stat, x, y, 0);
	for (int d = 0; d < 4; d++)
	{
		if (x + dx[d] < 0 || x + dx[d] >= 3 || y + dy[d] < 0 || y + dy[d] >= 3)continue;
		swap(stat[x][y], stat[x + dx[d]][y + dy[d]]);
		IDDFS(stat, endStat, dfs_end, depth, G + 1);
		swap(stat[x + dx[d]][y + dy[d]], stat[x][y]);
		if (dfs_end != 0)Stat_Num[G] = trans(stat);
	}
}
void IDA_star(int startStat[3][3], int endStat[3][3])//IDA*函数，从深度小到大依次进行搜索，避免在一条枝条上过于搜素浪费时间。
{
	for (int depth = 1; depth <= 1000; depth++)
	{
		int dfs_end = 0;
		fa[trans(startStat)] = -1;
		IDDFS(startStat, endStat, dfs_end, depth, 1);
		if (dfs_end != 0)break;
	}
	printAns(ida_moves, ida_times);//输出结果
}
int judge(int a[])//判断函数，计算逆序对的奇偶性
{
	int num = 0;
	for (int i = 0; i < 9; i++)
	{
		for (int j = i + 1; j < 9; j++)
		{
			if (a[j] != 0 && a[i] > a[j])
				num++;
		}
	}
	return num % 2;
}
//void random(int startStat[3][3], int endStat[3][3])
//{
//	while (1)
//	{
//		int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 }, b[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
//		shuffle(begin(a), end(a), default_random_engine(random_device()()));
//		shuffle(begin(b), end(b), default_random_engine(random_device()()));
//		if (judge(a) == judge(b))
//		{
//			for (int i = 0; i < 3; i++)
//			{
//				for (int j = 0; j < 3; j++)
//				{
//					startStat[i][j] = a[i * 3 + j];
//					endStat[i][j] = b[i * 3 + j];
//				}
//			}
//			return;
//		}
//	}
//}
bool equal(int a[], int b[])//判断两个数组是否相等
{
	for (int i = 0; i < 9; i++)
	{
		if (a[i] != b[i])return false;
	}
	return true;
}
bool repeat(int a[])//判断数组中是否重复
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = i + 1; j < 9; j++)
		{
			if (a[i] == a[j])
				return true;
		}
	}
	return false;
}


//窗口一的构造函数：设置相应的按钮以及尺寸
QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	int startStat[3][3] = { 0 };
	int endStat[3][3] = { 0 };
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			startStat[i][j] = a1[i * 3 + j];
			endStat[i][j] = b1[i * 3 + j];
		}
	}

	init();
	Search(startStat, endStat);//进行搜索操作

	QPushButton* btn = new QPushButton;         //表示进行下一步演示的按钮
	QPushButton* exit = new QPushButton;        //表示退出的按钮
	QPushButton* another = new QPushButton;     //表示生成搜索树示意图的按钮
	QPushButton* auto0 = new QPushButton;       //表示自动演示的按钮
	btn->setParent(this);                       //以下四句话将所有的按钮设置成窗口一的子类
	exit->setParent(this);
	another->setParent(this);
	auto0->setParent(this);

	auto0->setText("auto");                     //以下四句话分别指定按钮上面的文字
	btn->setText("next step");
	exit->setText("exit");
	another->setText("tree");
	btn->resize(100, 50);                       //以下四句话指定按钮的尺寸
	exit->resize(100, 50);
	another->resize(100, 50);
	auto0->resize(100, 50);

	btn->move(400, 100);                        //以下四句话指定按钮的位置
	auto0->move(400, 0);
	exit->move(400, 300);
	another->move(400, 200);
	resize(600, 400);                           //调节窗口大小
	setWindowTitle("eight-number");             //设置窗口标题


	auto0->setFont(QFont("YouYuan", 10, 1));    //设置每一个按钮上面的字体 
	auto0->setStyleSheet("background-color:yellow");
	auto0->setFlat(true);

	btn->setFont(QFont("YouYuan", 10, 1));
	btn->setStyleSheet("background-color:yellow");
	btn->setFlat(true);

	exit->setFont(QFont("YouYuan", 10, 1));
	exit->setStyleSheet("background-color:yellow");
	exit->setFlat(true);

	another->setFont(QFont("YouYuan", 10, 1));
	another->setStyleSheet("background-color:yellow");
	another->setFlat(true);


	ifstream infile;
	string ReadPath = SavePath[flag] + ".txt";//读入路径
	infile.open(ReadPath);   //读入文件中的数据，表示经过的路径上的各种状态数组
	int amount = 0, times = 0;
	infile >> amount >> times;
	string s;
	for (int k = 0; k < amount; k++)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				infile >> Stat[i][j][k];
			}
		}
		getline(infile, s);
		getline(infile, s);
	}
	infile.close();

	connect(exit, &QPushButton::clicked, [=]() {       //当按下exit按钮时，count0计数器清零，页面关闭
		count0 = 0;
		close();

		});
	connect(btn, &QPushButton::clicked, [=]() {        //当按下btn按钮时，更新界面表示演示下一步操作
													   //如果已经到达终态则会有弹窗提示
		update();
		if (count0 < amount - 1)
			count0++;
		if (count0 == amount - 1) {
			QMessageBox::information(this, "info", "end");
		}
		});

	connect(auto0, &QPushButton::clicked, [=]() {     //当按下auto0按钮时，进入自动演示模式
													  //到达终态后弹窗提示
		while (1)
		{
			if (count0 == amount - 1)
			{
				QMessageBox::information(this, "info", "end");
				break;
			}
			count0++;
			repaint();
			Sleep(300);
		}


		});

	connect(another, &QPushButton::clicked, [=]() {   //当按下another按钮时，开始演示搜索树界面，其中如果新建的界面在槽函数结束之后会被系统收回，就改为将界面放入堆中
		new_win = new QtWidgetsApplication2();
		new_win->show();

		});

}


//void QtWidgetsApplication3::slotEditClicked()
//{
//	QLineEdit* button = (QLineEdit*)sender();
//	cout << button->text().toInt();
//}

QtWidgetsApplication3::QtWidgetsApplication3(QWidget* parent)         //主界面的构造
	: QWidget(parent)
{
	setWindowTitle("input states");                     //设置窗口的标题
	resize(350, 330);                                   //设置窗口的大小尺寸

	QPushButton* start = new QPushButton;
	start->setText("start");
	start->setParent(this);
	start->move(10, 110);
	QPushButton* end = new QPushButton;
	end->setText("end");
	end->setParent(this);
	end->move(200, 110);


	QIntValidator* aIntValidator = new QIntValidator; //QIntValidator可以限制QLineEdit的输入范围
	aIntValidator->setRange(0, 9);                    //以下分别为：
	QLineEdit* aEdit1 = new QLineEdit;                //生成一个文本输入框
	aEdit1->setValidator(aIntValidator);              //文本输入框只能接受0-9之间的整数
	aEdit1->setParent(this);                          //文本输入框的父类设置为该窗口
	aEdit1->move(30, 10);                             //文本输入框移动到某个指定位置
	aEdit1->resize(20, 20);                           //指定文本输入框的大小
	QLineEdit* aEdit2 = new QLineEdit;                //一共18个按钮
	aEdit2->setValidator(aIntValidator);
	aEdit2->setParent(this);
	aEdit2->move(60, 10);
	aEdit2->resize(20, 20);
	QLineEdit* aEdit3 = new QLineEdit;
	aEdit3->setValidator(aIntValidator);
	aEdit3->setParent(this);
	aEdit3->move(90, 10);
	aEdit3->resize(20, 20);
	QLineEdit* aEdit4 = new QLineEdit;
	aEdit4->setValidator(aIntValidator);
	aEdit4->setParent(this);
	aEdit4->move(30, 40);
	aEdit4->resize(20, 20);
	QLineEdit* aEdit5 = new QLineEdit;
	aEdit5->setValidator(aIntValidator);
	aEdit5->setParent(this);
	aEdit5->move(60, 40);
	aEdit5->resize(20, 20);
	QLineEdit* aEdit6 = new QLineEdit;
	aEdit6->setValidator(aIntValidator);
	aEdit6->setParent(this);
	aEdit6->move(90, 40);
	aEdit6->resize(20, 20);
	QLineEdit* aEdit7 = new QLineEdit;
	aEdit7->setValidator(aIntValidator);
	aEdit7->setParent(this);
	aEdit7->move(30, 70);
	aEdit7->resize(20, 20);
	QLineEdit* aEdit8 = new QLineEdit;
	aEdit8->setValidator(aIntValidator);
	aEdit8->setParent(this);
	aEdit8->move(60, 70);
	aEdit8->resize(20, 20);
	QLineEdit* aEdit9 = new QLineEdit;
	aEdit9->setValidator(aIntValidator);
	aEdit9->setParent(this);
	aEdit9->move(90, 70);
	aEdit9->resize(20, 20);

	int x = 200;

	QLineEdit* bEdit1 = new QLineEdit;
	bEdit1->setValidator(aIntValidator);
	bEdit1->setParent(this);
	bEdit1->move(10 + x, 10);
	bEdit1->resize(20, 20);
	QLineEdit* bEdit2 = new QLineEdit;
	bEdit2->setValidator(aIntValidator);
	bEdit2->setParent(this);
	bEdit2->move(40 + x, 10);
	bEdit2->resize(20, 20);
	QLineEdit* bEdit3 = new QLineEdit;
	bEdit3->setValidator(aIntValidator);
	bEdit3->setParent(this);
	bEdit3->move(70 + x, 10);
	bEdit3->resize(20, 20);
	QLineEdit* bEdit4 = new QLineEdit;
	bEdit4->setValidator(aIntValidator);
	bEdit4->setParent(this);
	bEdit4->move(10 + x, 40);
	bEdit4->resize(20, 20);
	QLineEdit* bEdit5 = new QLineEdit;
	bEdit5->setValidator(aIntValidator);
	bEdit5->setParent(this);
	bEdit5->move(40 + x, 40);
	bEdit5->resize(20, 20);
	QLineEdit* bEdit6 = new QLineEdit;
	bEdit6->setValidator(aIntValidator);
	bEdit6->setParent(this);
	bEdit6->move(70 + x, 40);
	bEdit6->resize(20, 20);
	QLineEdit* bEdit7 = new QLineEdit;
	bEdit7->setValidator(aIntValidator);
	bEdit7->setParent(this);
	bEdit7->move(10 + x, 70);
	bEdit7->resize(20, 20);
	QLineEdit* bEdit8 = new QLineEdit;
	bEdit8->setValidator(aIntValidator);
	bEdit8->setParent(this);
	bEdit8->move(40 + x, 70);
	bEdit8->resize(20, 20);
	QLineEdit* bEdit9 = new QLineEdit;
	bEdit9->setValidator(aIntValidator);
	bEdit9->setParent(this);
	bEdit9->move(70 + x, 70);
	bEdit9->resize(20, 20);

	QPushButton* btnx = new QPushButton;    //设置关闭按钮
	btnx->setText("exit");                  //设置按钮上面的文本
	btnx->setParent(this);                  //将按钮的父类设置为该界面
	btnx->move(10, 200);                    //指定按钮的位置
	btnx->resize(50, 50);
	connect(btnx, &QPushButton::clicked, [=]() {          //信号函数为按下按钮，槽函数接受信号之后将界面关闭
		close();
		});

	QPushButton* btn1 = new QPushButton;    //设置搜索按钮
	btn1->setText("A*_Man");                  //设置按钮上面的文本
	btn1->setParent(this);                  //将按钮的父类设置为该界面
	btn1->move(80, 180);                    //指定按钮的位置
	connect(btn1, &QPushButton::clicked, [=]() {
		//信号函数为按下按钮，槽函数接受信号之后将界面关闭a1[0] = aEdit1->text().toInt();
		a1[0] = aEdit1->text().toInt();
		a1[1] = aEdit2->text().toInt();
		a1[2] = aEdit3->text().toInt();
		a1[3] = aEdit4->text().toInt();
		a1[4] = aEdit5->text().toInt();
		a1[5] = aEdit6->text().toInt();
		a1[6] = aEdit7->text().toInt();
		a1[7] = aEdit8->text().toInt();
		a1[8] = aEdit9->text().toInt();

		b1[0] = bEdit1->text().toInt();
		b1[1] = bEdit2->text().toInt();
		b1[2] = bEdit3->text().toInt();
		b1[3] = bEdit4->text().toInt();
		b1[4] = bEdit5->text().toInt();
		b1[5] = bEdit6->text().toInt();
		b1[6] = bEdit7->text().toInt();
		b1[7] = bEdit8->text().toInt();
		b1[8] = bEdit9->text().toInt();

		if (!equal(a1, b1) && !repeat(a1) && !repeat(b1) && judge(a1) == judge(b1))                  //如果输入合规，那么进入演示界面
		{
			flag = 3;
			QtWidgetsApplication1* w = new QtWidgetsApplication1;
			w->show();
		}
		else
		{                                       //如果输入不合规，弹窗报错
			QMessageBox::critical(this, "critical", "illegal input");
		}
		});

	QPushButton* btn2 = new QPushButton;    //设置搜索按钮
	btn2->setText("A*_Man+G%11");                  //设置按钮上面的文本
	btn2->setParent(this);                  //将按钮的父类设置为该界面
	btn2->move(220, 180);                    //指定按钮的位置
	connect(btn2, &QPushButton::clicked, [=]() {          //信号函数为按下按钮，槽函数接受信号之后将界面关闭a1[0] = aEdit1->text().toInt();
		a1[0] = aEdit1->text().toInt();
		a1[1] = aEdit2->text().toInt();
		a1[2] = aEdit3->text().toInt();
		a1[3] = aEdit4->text().toInt();
		a1[4] = aEdit5->text().toInt();
		a1[5] = aEdit6->text().toInt();
		a1[6] = aEdit7->text().toInt();
		a1[7] = aEdit8->text().toInt();
		a1[8] = aEdit9->text().toInt();

		b1[0] = bEdit1->text().toInt();
		b1[1] = bEdit2->text().toInt();
		b1[2] = bEdit3->text().toInt();
		b1[3] = bEdit4->text().toInt();
		b1[4] = bEdit5->text().toInt();
		b1[5] = bEdit6->text().toInt();
		b1[6] = bEdit7->text().toInt();
		b1[7] = bEdit8->text().toInt();
		b1[8] = bEdit9->text().toInt();

		if (!equal(a1, b1) && !repeat(a1) && !repeat(b1) && judge(a1) == judge(b1))                  //如果输入合规，那么进入演示界面
		{
			flag = 5;
			QtWidgetsApplication1* w = new QtWidgetsApplication1;
			w->show();

		}
		else {                                       //如果输入不合规，弹窗报错
			QMessageBox::critical(this, "critical", "illegal input");
		}
		});

	QPushButton* btn3 = new QPushButton;    //设置关闭按钮
	btn3->setText("A*_Misplace");                  //设置按钮上面的文本
	btn3->setParent(this);                  //将按钮的父类设置为该界面
	btn3->move(80, 240);                    //指定按钮的位置
	connect(btn3, &QPushButton::clicked, [=]() {          //信号函数为按下按钮，槽函数接受信号之后将界面关闭a1[0] = aEdit1->text().toInt();
		a1[0] = aEdit1->text().toInt();
		a1[1] = aEdit2->text().toInt();
		a1[2] = aEdit3->text().toInt();
		a1[3] = aEdit4->text().toInt();
		a1[4] = aEdit5->text().toInt();
		a1[5] = aEdit6->text().toInt();
		a1[6] = aEdit7->text().toInt();
		a1[7] = aEdit8->text().toInt();
		a1[8] = aEdit9->text().toInt();

		b1[0] = bEdit1->text().toInt();
		b1[1] = bEdit2->text().toInt();
		b1[2] = bEdit3->text().toInt();
		b1[3] = bEdit4->text().toInt();
		b1[4] = bEdit5->text().toInt();
		b1[5] = bEdit6->text().toInt();
		b1[6] = bEdit7->text().toInt();
		b1[7] = bEdit8->text().toInt();
		b1[8] = bEdit9->text().toInt();

		if (!equal(a1, b1) && !repeat(a1) && !repeat(b1) && judge(a1) == judge(b1))                  //如果输入合规，那么进入演示界面
		{
			flag = 6;
			QtWidgetsApplication1* w = new QtWidgetsApplication1;
			w->show();

		}
		else {                                       //如果输入不合规，弹窗报错
			QMessageBox::critical(this, "critical", "illegal input");
		}
		});


	QPushButton* btn4 = new QPushButton;                 //提交按钮，按下之后生成演示界面
	btn4->setText("Bothway_A*");                            //设置按钮上面的文字 
	btn4->setParent(this);                               //将窗口设置为按钮的父类
	btn4->move(220, 240);                                //设置按钮的位置
	connect(btn4, &QPushButton::clicked, [=]() {           //信号函数为按下按钮，槽函数接受信号之后将所有文本输入框的值传入数组变量中
		a1[0] = aEdit1->text().toInt();
		a1[1] = aEdit2->text().toInt();
		a1[2] = aEdit3->text().toInt();
		a1[3] = aEdit4->text().toInt();
		a1[4] = aEdit5->text().toInt();
		a1[5] = aEdit6->text().toInt();
		a1[6] = aEdit7->text().toInt();
		a1[7] = aEdit8->text().toInt();
		a1[8] = aEdit9->text().toInt();

		b1[0] = bEdit1->text().toInt();
		b1[1] = bEdit2->text().toInt();
		b1[2] = bEdit3->text().toInt();
		b1[3] = bEdit4->text().toInt();
		b1[4] = bEdit5->text().toInt();
		b1[5] = bEdit6->text().toInt();
		b1[6] = bEdit7->text().toInt();
		b1[7] = bEdit8->text().toInt();
		b1[8] = bEdit9->text().toInt();

		if (!equal(a1, b1) && !repeat(a1) && !repeat(b1) && judge(a1) == judge(b1))                  //如果输入合规，那么进入演示界面
		{
			flag = 11;
			QtWidgetsApplication1* w = new QtWidgetsApplication1;
			w->show();
		}
		else {                                       //如果输入不合规，弹窗报错
			QMessageBox::critical(this, "critical", "illegal input");
		}
		});
}

//void QtWidgetsApplication3::paintEvent3(QPaintEvent* event) {        //窗口3的绘图事件
//	QPainter painter(this);
//	painter.drawLine(0, 100, 100, 100);
//	painter.drawText(QRect(10, 100,30,20),"start");
//	painter.drawText(QRect(20, 100, 10, 20), "end");
//}


QtWidgetsApplication2::QtWidgetsApplication2(QMainWindow* parent)       //窗口二：中间嵌套一个（10000*5000）的滑动窗口
	: QMainWindow(parent)
{
	resize(1200, 800);
	QScrollArea* pScroll = new QScrollArea(this);
	this->setCentralWidget(pScroll);
	MyScrollArea* pTempWnd = new MyScrollArea(pScroll);
	pScroll->setWidget(pTempWnd);
	pTempWnd->setFixedSize(10000, 5000);
	QVBoxLayout* pLay = new QVBoxLayout(pTempWnd);
	pTempWnd->setLayout(pLay);
}

void QtWidgetsApplication2::paintEvent(QPaintEvent* event)
{      //窗口二的绘图事件函数

}

void QtWidgetsApplication1::paintEvent(QPaintEvent* event) {     //窗口一的绘图事件
	QPainter painter(this);
	//QPixmap pix;

	//painter.drawPixmap(0, 0, 1200, 800, QPixmap(":/image/image.png"));
	//painter.drawLine(QPoint(1100, 0), QPoint(1100, 800));
	painter.translate(50, 50);                                  //改变绘图位置的基准点
	QBrush brush1(QColor(204, 102, 0));                           //设置刷子颜色
	painter.setBrush(brush1);
	painter.setFont(QFont("宋体", 25, 1));                      //设置字体
	painter.drawRect(0, 0, 304, 304);                           //在界面上画矩形
	painter.drawRect(350, -50, 100, 50);
	painter.drawRect(350, 50, 100, 50);
	painter.drawRect(350, 150, 100, 50);
	painter.drawRect(350, 250, 100, 50);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (a[Stat[i][j][count0]] != "0") {

				QBrush brush0(QColor(color_first[Stat[i][j][count0]], color_second[Stat[i][j][count0]], color_third[Stat[i][j][count0]]));
				painter.setBrush(brush0);
				painter.drawRect((2 + 202 * j) / 2, (2 + 202 * i) / 2, 100, 100);
				painter.drawText(QRect((82 + 202 * j) / 2, (62 + 202 * i) / 2, 50, 50), a[Stat[i][j][count0]]);
			}
		}
	}
	QBrush brush(QColor(255, 255, 255));
	painter.setBrush(brush);
	painter.translate(0, 0);
}
