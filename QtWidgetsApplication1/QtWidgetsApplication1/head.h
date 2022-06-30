#pragma once
#include<iostream>
#include<unordered_map>
#include<queue>
#include<fstream>
#include<random>
#include<algorithm>
#include<cmath>
using namespace std;

class grid//结构体存储八数码问题的状态
{
public:
	int stat[3][3], target[3][3];//目前状态及目标状态
	int H, F, G;//H 模拟距离结果还有几步，G搜索深度，F两者运算相加
	int* zero;//空格即0的位置
	grid(int s[3][3], int e[3][3], int fa_G = -1)//三参构造函数
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				stat[i][j] = s[i][j];
				target[i][j] = e[i][j];
			}
		}
		update(fa_G);
		find0();
	}
	bool isend();//判断是否到最终节点
	void update(int fa_G);//更新G、H、F
	void fH();//计算H
	void fG(int fa_G);//计算G
	void fF();//计算F
	int* findx(int x);//找到x在stat对应位置
	void find0();//找到0在stat对应位置
	friend bool operator <(const grid& a, const grid& b);//运算符重载用于优先队列
};

//上下左右
const int dx[4] = { 0,0,1,-1 };
const int dy[4] = { -1,1,0,0 };
//数组大小
const int N = 1e6 + 10;

void init();//初始化函数
bool operator <(const grid& a, const grid& b);//运算符重载用于优先队列

int value_Man(int x, int y);//曼哈顿距离估值函数
double value_Eu(int x, int y);//欧几里得距离估值函数
int value_Misplace(int x, int y);//错位数个数

int trans(int stat[3][3]);//转换函数,将3*3的二维数组转换为int
void printAns(int moves, int times);//输出路径结果到文件中
void seeAns(int stat, int times);//对fa进行处理得到Stat_num
void seeAnsBothWay(int stat, int times);//对fa1,fa2进行处理得到Stat_num
void Search(int startStat[3][3], int endStat[3][3]);//搜索函数(bfs,gbfs,A*)
void BothWaySearch(int startStat[3][3], int endStat[3][3]);//双向搜索函数(Bothwaybfs,Bothwaygbfs,BothwayA*)
int judge(int a[]);//判断函数，计算逆序对的奇偶性
//void random(int startStat[3][3], int endStat[3][3]);
void IDA_findx(int stat[3][3], int& x, int& y, int target);//IDA*的findx函数
int IDA_fH(int stat[3][3], int endStat[3][3]);//IDA*的fH函数,计算曼哈顿距离
void IDDFS(int stat[3][3], int endStat[3][3], int& dfs_end, int k, int dep);//IDA*的DFS函数
void IDA_star(int startStat[3][3], int endStat[3][3]);//IDA*函数
bool equal(int a[], int b[]);//判断两个数组是否相等
bool repeat(int a[]);//判断数组中是否重复