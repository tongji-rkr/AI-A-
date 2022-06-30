#ifndef MYSCROLLAREA_H
#define MYSCROLLAREA_H

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>

class MyScrollArea : public QWidget                //新界面：滑动窗口界面，此界面嵌套在窗口2界面当中
{                                                  //其中包含了构造函数和其中的绘图事件函数
	Q_OBJECT

public:
	MyScrollArea(QWidget *parent =  Q_NULLPTR);
	~MyScrollArea();
	void paintEvent(QPaintEvent* event);
private:
};

#endif // MYSCROLLAREA_H
