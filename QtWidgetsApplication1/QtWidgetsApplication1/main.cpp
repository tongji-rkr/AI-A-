#include "QtWidgetsApplication1.h"
#include <QtWidgets/QApplication>
#include<iostream>
#include<unordered_map>
#include<queue>
#include<fstream>
#include<random>
#include<algorithm>
#include<QLineEdit>//提示消息
#include<QCompleter>
using namespace std;

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QtWidgetsApplication3 w0;  //创建主窗口界面
	w0.show(); //显示界面
	return a.exec();
}