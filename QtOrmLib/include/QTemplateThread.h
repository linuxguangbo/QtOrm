#pragma once


#include  <QVariantList>
#include <QSqlDatabase>
#include "IDatabase.h"
#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include "qtormlib_global.h"
struct db_struct
{
	QString ip;
	QString dbName;
	int port;
	QString userName;
	QString passWord;
	QString dbdriver;
	QString timeout;
	QString querystring;  //pool
};
class QTORMLIB_EXPORT QTmeplate : public QObject
{
	Q_OBJECT

public:
	QTmeplate(QObject *parent);
	QWaitCondition pauseCond;
	QMutex sync;
	bool is_pause;

	static bool is_init_done;

	//����
	void resume()
	{
		sync.lock();
		is_pause = false;
		sync.unlock();
		pauseCond.wakeAll();

	}
	//��ͣ
	void pause()
	{
		sync.lock();
		is_pause = true;
		sync.unlock();
	}
	QString QDBC_id;	//������ʾsql������id

	QString get_QDBC_id() {
		return QDBC_id;
	}
	QVariantList m_data;
	int   flag = 0; 		//0��ʾ��ʼ�� 1��ʾ��ѯ 2��ʾ���£�����ɾ���ģ�3,��ʾ��� 4����ʾ���ݿ��ӳ� 
							//5����ʾ���� 6,�����ύ 7������ع� 8,ִ��sql��䱨��
						//-1 ���������Ҫ��Ҫ��Ϊ (Qselect >> t << t) ��ʽ...

	//res
	bool issqltransactionResult = false;	//ִ������sql�ķ���ֵ
	QMap<int, QVariantList> res_data;
	bool sqlres;


	
	
	~QTmeplate();
	//QSqlDatabase* database;
	QSqlDatabase* databasetrans = NULL;//���Ӿ��

	static int qdbc_mod;  //-1 ��ʾδ�������߳�  0��ʾ�����߳����ݿ����� 1��ʾ�������ݿ����ӳ�
	static int loglevel;	//0 Ĭ�ϲ���info��־
private:
	//��ȡ����ģʽ
	//����flagΪ4
	bool set_database_isopen(QSqlDatabase* database);
	QSqlDatabase* get_connect();
	void close_connect(QSqlDatabase* db,const QString& errcoe);
public:
	bool read_date(db_struct & db, const QString & path);
	//void run();
	void do_result(QMap<int, QVariantList>  &m_data, bool sqlres, int flag);

	void do_result(bool sqlres);
	void detection_drive(QSqlDatabase* database);
	void do_sql(int falg);
};
#include <QSettings>
class mysettings : public QSettings
{
public:
	mysettings(const QString & fileName, Format format);

	QString  getchildrensfromgrop(const QString group);
	~mysettings()
	{
	}

private:

};
