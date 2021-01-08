#pragma once

#include <QObject>
#include "IDatabase.h"

class  QdbcFactory : public QObject 
{
	Q_OBJECT
public:
	//0��ʾ��ʼ��ģʽ�̳߳� 	1��ʾ��ʼ�����߳����ݿ�
	static	IDatabase*  createDataSource(int id);
	QdbcFactory(QObject *parent);
	~QdbcFactory();
};
