#pragma once

#include <QObject>
#include <QList>
#include "QTemplateThread.h"
#include <QThread>
#include <QCoreApplication>
#include <QAtomicInt>
#include <QMutex>
#include <QStringBuilder>
#include <QMetaMethod>
#include <QFileInfo>
#include "analysis_container.h"
#include "qtormlib_global.h"
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
class QTORMLIB_EXPORT Object_utils
{
public:
	Object_utils() {

	}
	~Object_utils() {

	}
	template <typename T>
	static QString toString(T* src) {
		QString str = "[ ";
		const QMetaObject *metaObject = src->metaObject();
		int count = metaObject->propertyCount();
		for (int i = 1; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			QString strs = QString(qmeta.name());
			str.append(strs % ":" %  src->property(qmeta.name()).toString() % " ");
		}
		str.append("]");
		return str;
	}
	template <typename T>
	static QString toString(QList<T*>& value) {
		QString str = "[ ";
		for (int i = 0; i < value.size(); i++) {
			const QMetaObject *metaObject = value[i]->metaObject();
			int count = metaObject->propertyCount();
			for (int j = 1; j < count; j++) {
				QMetaProperty qmeta = metaObject->property(j);
				QString strs = QString(qmeta.name());
				str.append(strs % ":" %  value[i]->property(qmeta.name()).toString() % " ");
			}
			str.append(" && ");
		}
		str.append("]");
		return str;
	}
	template <typename T>
	static void copy(T* src,T* dec) {
		const QMetaObject *metaObject = src->metaObject();
		int count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			src->setProperty(qmeta.name(),dec->property(qmeta.name()));
		}
	}
	template <typename T>
	static void clear(T* & data) {
		const QMetaObject *metaObject = data->metaObject();
		int count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			data->setProperty(qmeta.name(), NULL);
		}
	}

	template <typename T>
	static void clear(T& data) {
		const QMetaObject *metaObject = data.metaObject();
		int count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			data.setProperty(qmeta.name(), NULL);
		}
	}
	template <typename T>
	static bool isNULL(T& data) {
		return !isClear(data);
	}

	template <typename T>
	static bool isClear(T& data) {
		const QMetaObject *metaObject = data.metaObject();
		int count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			QMetaProperty qmeta = metaObject->property(i);
			QVariant t = data.property(qmeta.name());
			if (QMetaType::QDateTime == t.type()) {
				QDateTime time = t.toDateTime();
				bool s = time.isNull();
				if (s == true)  return false;
			}
			else
			{
				if (t != NULL) {
					return false;
				}
			}
		}
		return true;
	}
private:

};
class QTORMLIB_EXPORT Qconfig {
public:
	void setPath(char* path, char* name);
	bool fullfilepath(QString& path);
};
/*��ǰ��һ��������Ϣ*/
static int atom;
static QMutex __mutex;
static char* fliepath;
static char* filename;

class QTORMLIB_EXPORT QdbcTemplate : public QObject
{
	Q_OBJECT
private:
	int thread_id;

	bool isMyclass = true; //�Ƿ�Ϊ�Զ����࣬�����Զ����࣬��Ϊtrue ��Ҫ�̳�Object_jdbc��Ĭ��Ϊfalse��

	int Out_count_row = 0;	//��
	int	Out_count = 0;		//��

	int In_count = 0;		//�ж���������б�
	int Count_arg = 0; //�ж���ǣ�
	
	//int change_in; //1,2  �ж� (Qselect < t > t) ��ʽ
	int countString(QString str);
	//********************
	int Loglevel = 0;
	//int timeout = 4;
	QTmeplate* mythread;
	Analysis_container* analysis = NULL;
	
	

	void clear() {
		this->In_count =  0;
		mythread->res_data.clear();
		mythread->res_data.clear();
	}
public:
	QdbcTemplate& select(const QString& sql) {
		ponit_clear();
		mythread->flag = 1;
		mythread->m_data.clear();
		args(sql);
		return *this;
	}
	QdbcTemplate& update(const QString& sql) {
		ponit_clear();
		mythread->flag = 2;
		mythread->m_data.clear();
		args(sql);
		return *this;
	}
	void  wait_initdatabase() {

	}
	QTmeplate* getselfthread() {
		return mythread;
	}
	Analysis_container* getselfAnalysis() {
		if (analysis == NULL) {
			analysis = new Analysis_container;
		}
		return analysis;
	}
	void deleteselfAnalysis() {
		if (analysis != NULL) {
			delete analysis;
			analysis = NULL;
		}
	}
	//ֻ�ܼ�������Ƿ���������
	bool getDabaseConnect();


	static	QdbcTemplate*  singleinstance() {
		int threadid =  quintptr(QThread::currentThreadId());
		QMutexLocker lock(&__mutex);
		if (!__instance__.contains(threadid)) {
			QObject * o = NULL;
			QdbcTemplate* t = new QdbcTemplate(o);
			t->thread_id = threadid;
			__instance__[threadid] = t;
			//��ª��δ��ʼ�����߳�
		}
		return __instance__[threadid];
	}
	static	QMap<int, QdbcTemplate*>  Allinstance() {
		return __instance__;
	}

public:
	void qtransactional();
	void qtransactional_clear();
	bool getdata_res();		
	QdbcTemplate(QObject *parent);

	QdbcTemplate(QString falgs);
	QdbcTemplate& arg(int a) {
		return *this;
	}
	//mysql�鿴��ǰ����  show FULL PROCESSLIST
	void args(QString value = "");
	void assert_args(const QString &str);
	//��������**********************************************
	QdbcTemplate& operator < (QObject* value);
	QdbcTemplate& operator < (QDateTime& date);
	QdbcTemplate& operator < (QString value);
	QdbcTemplate& operator < (int value);
	QdbcTemplate& operator < (bool value);
	//�������**********************************************
	QdbcTemplate& operator > (int& value);
	QdbcTemplate& operator > (bool& value);
	QdbcTemplate& operator > (QString& value);

	template <typename T>
	QdbcTemplate& operator > (T* &value);

	template <typename T>
	QdbcTemplate& operator > (T &value);

	template <typename T>
	QdbcTemplate& operator > (QList<T*>& value);

	bool invokefunc(QObject*  value,QByteArray name,QVariant& t);
	//�̷ַ߳�
	void thread_dispatch_flag4();

	QMap<int,void*> adress;

	/****c code****/
	

	template <typename T>
	T* New_adress() {
		T* t1 = new T;
		adress[(int)t1] = t1;
		return t1;
	}
	template <typename T>
	void delete_address(T* t1) {
		delete t1;
		this->adress.remove((int)t1);
	}
	void ponit_clear() {
		QMap<int, void*>::const_iterator i;
		for (i = adress.constBegin(); i != adress.constEnd(); ++i) {
			delete i.value();
		}
		adress.clear();
		deleteselfAnalysis();
	}
	void QdbcTemplateClear();
	~QdbcTemplate();
private:
	static QMap<int,QdbcTemplate*> __instance__;
};
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator>(T & value)
{
	Object_utils::clear(value);
	//��һ���������
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id % "[error:] ��ȡ����ʧ�� ʵ�ʲ���(<<)��sql��������ƥ��:��������ʵ�ʲ���������" %QString::number(In_count) % "sql��������" % QString::number(Count_arg - 1);
			qFatal(str.toUtf8());
			return *this;
		}

		qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
		mythread->do_sql(mythread->m_data, mythread->flag);
		this->In_count = 0;

	}
	else
	{
		QString str = mythread->QDBC_id %  "ֻ����һ�����";
		assert_args(str);
	}
	if (mythread->flag == 8) {
		qWarning() << mythread->QDBC_id % "���ݿ������˴���...";
		return *this;
	}
	while (this->Out_count < mythread->res_data.size()) {
		QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
		value.setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

		this->Out_count_row += 2;
		if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
			this->Out_count_row = 0;
			this->Out_count++;
			break;
		}
	}
	qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
	this->Out_count++;
	return *this;
}
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator>(T *& value)
{
	//��һ���������
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id % "[error:] ��ȡ����ʧ�� ʵ�ʲ���(<<)��sql��������ƥ��:��������ʵ�ʲ���������" %QString::number(In_count) % "sql��������" % QString::number(Count_arg - 1);
			qFatal(str.toUtf8());
			value = NULL;
			return *this;
		}

		qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
		mythread->do_sql(mythread->m_data, mythread->flag);
		this->In_count = 0;

	}
	else
	{
		QString str = mythread->QDBC_id %  "ֻ����һ�����";
		assert_args(str);
	}
	if (mythread->flag == 8) {
		value = NULL;
		qWarning() << mythread->QDBC_id % "���ݿ������˴���...";
		return *this;
	}
	int size = mythread->res_data.size();
	if (size == 0) {
		value = NULL;
		return *this;
	}
	value = New_adress<T>();
	Object_utils::clear(value);
	while (this->Out_count < mythread->res_data.size()) {
		QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
		value->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

		this->Out_count_row += 2;
		if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
			this->Out_count_row = 0;
			this->Out_count++;
			break;
		}
	}
	qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
	this->Out_count++;
	return *this;
}
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator >(QList<T*>& value)
{
	//��һ���������
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id  % "[error:] ��ȡ����ʧ�� ʵ�ʲ���(<)��sql��������ƥ��:��������  ʵ�ʲ���(<)������" %QString::number(In_count - 1) % "sql��������" % QString::number(Count_arg - 2);
			assert_args(str);
			value.clear();
			return *this;
		}

		qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
		mythread->do_sql(mythread->m_data, mythread->flag);
		this->In_count = 0;

	}
	else
	{
		QString str = mythread->QDBC_id %  "ֻ����һ�����";
		assert_args(str);
	}
	if (mythread->flag == 8) {
		qWarning() << mythread->QDBC_id %  "���ݿ������˴���...";
		return *this;
	}
	T* t1 = New_adress<T>();
	Object_utils::clear(t1);
	while (this->Out_count < mythread->res_data.size()) {
		QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
		t1->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

		this->Out_count_row += 2;
		if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
			this->Out_count_row = 0;
			this->Out_count++;
			value.append(t1);
			t1 = New_adress<T>();
			Object_utils::clear(t1);
		}
		if (this->Out_count == mythread->res_data.size()) {
			delete_address(t1);
			qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
			this->Out_count++;
		}
	}
	return *this;
	// TODO: �ڴ˴����� return ���
}
