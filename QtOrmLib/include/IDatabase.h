#pragma once

#include <QObject>
#include <QSqlDatabase>
class  IDatabase : public QObject
{
	Q_OBJECT

public:
	IDatabase(QObject *parent);

	~IDatabase();

public:
	//���������̣߳�ֻ�������̳߳�ʼ���ҳ�ʼ��һ��
	virtual bool LoadDatabase(const QString& db, const QString& host, const QString& dbname, int port,
		const QString& username, const QString& password,const QString& options,const QString& parms) = 0;
	virtual QSqlDatabase* openConnection() = 0;
	//�ر�����������Ϊ
	virtual bool closeConnection(QSqlDatabase* db) = 0;
	//ɾ������������Ϊ
	virtual bool removeConnection() = 0;
};
