#ifndef MYSCROLLAREA_H
#define MYSCROLLAREA_H

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>

class MyScrollArea : public QWidget                //�½��棺�������ڽ��棬�˽���Ƕ���ڴ���2���浱��
{                                                  //���а����˹��캯�������еĻ�ͼ�¼�����
	Q_OBJECT

public:
	MyScrollArea(QWidget *parent =  Q_NULLPTR);
	~MyScrollArea();
	void paintEvent(QPaintEvent* event);
private:
};

#endif // MYSCROLLAREA_H
