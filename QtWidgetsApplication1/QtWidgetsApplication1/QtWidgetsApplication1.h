#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtWidgetsApplication1.h"
#include "next.h"
#include<fstream>


class QtWidgetsApplication2 : public QMainWindow                 //定义窗口类2：包含自身构造函数和一个绘图事件函数
{                                                                //函数实现见QtWidgetsApplication1.cpp
	Q_OBJECT                                                     //此窗口显示搜索树

public:
	QtWidgetsApplication2(QMainWindow* parent = Q_NULLPTR);

	void paintEvent(QPaintEvent* event);

private:
	Ui::QtWidgetsApplication1Class ui;                           //设置ui

};


class QtWidgetsApplication3 : public QWidget                    //定义窗口类3：包含自身构造函数
{                                                               //此窗口显示主界面
	Q_OBJECT

public:
	QtWidgetsApplication3(QWidget* parent = Q_NULLPTR);
	//void paintEvent3(QPaintEvent* event);
	//void slotEditClicked();
private:
	Ui::QtWidgetsApplication1Class ui;

};


class QtWidgetsApplication1 : public QWidget                   //定义窗口类1：包含自身构造函数和一个绘图事件函数
{                                                              //此窗口显示八数码演示界面
	Q_OBJECT

public:
	QtWidgetsApplication1(QWidget* parent = Q_NULLPTR);

	void paintEvent(QPaintEvent* event);

private:
	Ui::QtWidgetsApplication1Class ui;
	//创建下一步对象
	//next0* nt;
	QtWidgetsApplication2* new_win;                           //新界面为其成员
};

