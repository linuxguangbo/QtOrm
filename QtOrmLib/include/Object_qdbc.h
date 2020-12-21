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
#include <QMultiHash>
#include "analysis_container.h"
#include "qtormlib_global.h"
#include <QStringList>
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
struct  ElemsXmlObj
{
	QObject* value; //存值
	QStringList parents; //指向父辈们的链表
};
struct  ElemsXml
{
	QString property_name;
	int level;  //属于的层级
	int flag;  //输入1:list 还是 2:object
	bool isId; //是否为id
	QString id_value; //存id
	QObject* value; //存值
	QString parent; //指向父辈们的链表
};

/*当前的一下配置信息*/
static int atom;
static QMutex __mutex;
static char* fliepath;
static char* filename;

class QTORMLIB_EXPORT QdbcTemplate : public QObject
{
	Q_OBJECT
private:
	int thread_id;

	bool isMyclass = true; //是否为自定义类，对于自定义类，置为true 需要继承Object_jdbc。默认为false。

	int Out_count_row = 0;	//行
	int	Out_count = 0;		//列

	int In_count = 0;		//有多个个参数列表
	int Count_arg = 0; //有多个是？
	
	//int change_in; //1,2  判断 (Qselect < t > t) 格式
	int countString(QString str);
	QHash<QString,ElemsXml> tablenames;
	QHash<QString, ElemsXmlObj*> tablenameobjs;
	//QHash<QString, ElemsXmlObj*> tablename_objs;
	void analysis_macro (char* str);
	bool isUseUnion = false;
	//对于关联查询，不支持select *
	void querytable(QString& str);
	//********************
	int Loglevel = 0;
	//int timeout = 4;
	QTmeplate* mythread;
	Analysis_container* analysis = NULL;
	
	bool isautomemory;

	QString Va_data = ""; 

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
	QdbcTemplate& Validation_collection(char* sql);
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
	//只能检查数据是否连接正常
	bool getDabaseConnect();


	static	QdbcTemplate*  singleinstance() {
		int threadid =  quintptr(QThread::currentThreadId());
		if (!__instance__.contains(threadid)) {
			QMutexLocker lock(&__mutex);
			QObject * o = NULL;
			QdbcTemplate* t = new QdbcTemplate(o);
			t->thread_id = threadid;
			__instance__[threadid] = t;
			//检陋，未初始化的线程
		}
		return __instance__[threadid];
	}
	static	QHash<int, QdbcTemplate*>  Allinstance() {
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
	//mysql查看当前连接  show FULL PROCESSLIST
	void args(QString value = "");
	void assert_args(const QString &str);
	//参数输入**********************************************
	QdbcTemplate& operator < (QObject* value);
	QdbcTemplate& operator < (QObject& value);
	QdbcTemplate& operator < (QDateTime& date);
	QdbcTemplate& operator < (const QString& value);
	QdbcTemplate& operator < (int value);
	QdbcTemplate& operator < (bool value);
	//参数输出**********************************************
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
	//retrun 0: 对象  return 1：QList<对象> 并分配内存
	//name 为 key	keyname 为定义的class name 
	int invokefunc(QObject* value, QByteArray& name, QByteArray& keyname);
	QObject* get_invokefunc(QObject * value, QByteArray& keyname, int index);
	void set_invokefunc(QObject * value, QByteArray & keyname);
	//线程分发
	void thread_dispatch_flag4();

	QHash<int,QObject*> adress;

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
	void hashclear() {
		if (isautomemory == true) {
			QHash<int, QObject*>::const_iterator i;
			for (i = adress.constBegin(); i != adress.constEnd(); ++i) {
				QObject* t = i.value();
				delete t;
			}
		}
		adress.clear();
	}
	void ponit_clear() {
		hashclear();
		deleteselfAnalysis();
	}
	void QdbcTemplateClear();
	~QdbcTemplate();
private:
	static QHash<int,QdbcTemplate*> __instance__;
};
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator>(T & value)
{
	Object_utils::clear(value);
	//第一个输出参数
	if (isUseUnion == true) {
		if (Out_count == 0) {
			if (In_count != Count_arg) {
				QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
				qFatal(str.toUtf8());
				return *this;
			}

			qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
			mythread->do_sql(mythread->flag);
			this->In_count = 0;
		}
		else
		{
			QString str = mythread->QDBC_id %  "只能有一个输出";
			assert_args(str);
		}
		if (mythread->flag == 8) {
			qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
			return *this;
		}
		QString main_value = ""; //先找id
		while (this->Out_count < mythread->res_data.size()) {
			QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();

			//bool b = value.setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

			if (!tablenames.contains(key)) {
				assert_args("Duplicate fields ");
			}
			ElemsXml el = tablenames[key];
			QObject* t1 = &value;
			if (el.level == 0) {
				value.setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
			}
			else
			{
				ElemsXmlObj* objx = tablenameobjs[el.parent];
				for (int i = 1; i <  objx->parents.size(); i++) {
					QString partentanme = objx->parents[i];
					ElemsXmlObj* els = tablenameobjs[partentanme];
					if (els->value == NULL) {
						this->set_invokefunc(t1, partentanme.toUtf8());
						adress[(int)t1] = t1;
					}
					else
					{
						QObject * obj = this->get_invokefunc(t1, partentanme.toUtf8(), -1);
						t1 = obj;
					}
				}
				objx->value->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
			}

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
	else
	{
		if (Out_count == 0) {
			if (In_count != Count_arg) {
				QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
				qFatal(str.toUtf8());
				return *this;
			}

			qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
			mythread->do_sql(mythread->flag);
			this->In_count = 0;

		}
		else
		{
			QString str = mythread->QDBC_id %  "只能有一个输出";
			assert_args(str);
		}
		if (mythread->flag == 8) {
			qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
			return *this;
		}
		while (this->Out_count < mythread->res_data.size()) {
			QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
			bool s =value.setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
			if (s == false) {
				assert_args("can not set Property!");
			}

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
}
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator>(T *& value)
{
	if (isUseUnion == true) {
		//if (value == NULL) {
		//	//第一个输出参数
		//	if (Out_count == 0) {
		//		if (In_count != Count_arg) {
		//			QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
		//			qFatal(str.toUtf8());
		//			value = NULL;
		//			return *this;
		//		}

		//		qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
		//		mythread->do_sql(mythread->flag);
		//		this->In_count = 0;

		//	}
		//	else
		//	{
		//		QString str = mythread->QDBC_id %  "只能有一个输出";
		//		assert_args(str);
		//	}
		//	if (mythread->flag == 8) {
		//		value = NULL;
		//		qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
		//		return *this;
		//	}
		//	int size = mythread->res_data.size();
		//	if (size == 0) {
		//		value = NULL;
		//		return *this;
		//	}
		//	value = New_adress<T>();
		//	Object_utils::clear(value);
		//	//fake -> class Name
		//	QMap<QString, QString> mpclass;
		//	while (this->Out_count < mythread->res_data.size()) {
		//		QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
		//		bool b = value->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

		//		if (b == false) {
		//			QString tbname = tablenames[key];
		//			if (tablenames.contains(tbname)) {
		//				tbname = tablenames[tbname];
		//			}
		//			if (mpclass.contains(tbname)) {
		//				QObject* obj = this->invokefunc(value, mpclass[tbname].toLatin1(),-1);
		//				obj->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
		//			}
		//			else
		//			{
		//				QByteArray keyname;
		//				if (this->invokefunc(value, tbname.toLatin1(), keyname) == 1) {
		//					mpclass[tbname] = keyname;
		//					QObject* obj = this->invokefunc(value, keyname,-1);
		//					adress[(int)obj] = obj;
		//					obj->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
		//				}
		//			}
		//		}

		//		this->Out_count_row += 2;
		//		if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
		//			this->Out_count_row = 0;
		//			this->Out_count++;
		//			break;
		//		}
		//	}
		//	qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
		//	this->Out_count++;
		//	return *this;
		//}
		//else
		//{
		//	//第一个输出参数
		//	if (Out_count == 0) {
		//		if (In_count != Count_arg) {
		//			QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
		//			qFatal(str.toUtf8());
		//			value = NULL;
		//			return *this;
		//		}

		//		qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
		//		mythread->do_sql(mythread->flag);
		//		this->In_count = 0;

		//	}
		//	else
		//	{
		//		QString str = mythread->QDBC_id %  "只能有一个输出";
		//		assert_args(str);
		//	}
		//	if (mythread->flag == 8) {
		//		value = NULL;
		//		qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
		//		return *this;
		//	}
		//	int size = mythread->res_data.size();
		//	if (size == 0) {
		//		value = NULL;
		//		return *this;
		//	}
		//	//value = New_adress<T>();
		//	Object_utils::clear(value);
		//	QMap<QString, QString> mpclass;
		//	while (this->Out_count < mythread->res_data.size()) {
		//		QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
		//		bool b = value->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

		//		if (b == false) {
		//			QString tbname = tablenames[key];
		//			if (tablenames.contains(tbname)) {
		//				tbname = tablenames[tbname];
		//			}
		//			if (mpclass.contains(tbname)) {
		//				QObject* obj = this->invokefunc(value, mpclass[tbname].toLatin1(),-1);
		//				obj->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
		//			}
		//			else
		//			{
		//				QByteArray keyname;
		//				if (this->invokefunc(value, tbname.toLatin1(), keyname) == 1) {
		//					mpclass[tbname] = keyname;
		//					QObject* obj = this->invokefunc(value, keyname, -1);
		//					adress[(int)obj] = obj;
		//					obj->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
		//				}
		//			}
		//		}

		//		this->Out_count_row += 2;
		//		if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
		//			this->Out_count_row = 0;
		//			this->Out_count++;
		//			break;
		//		}
		//	}
		//	qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
		//	this->Out_count++;
		//	return *this;
		//}
	}
	else {
		if (value == NULL) {
			//第一个输出参数
			if (Out_count == 0) {
				if (In_count != Count_arg) {
					QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
					qFatal(str.toUtf8());
					value = NULL;
					return *this;
				}

				qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
				mythread->do_sql(mythread->flag);
				this->In_count = 0;

			}
			else
			{
				QString str = mythread->QDBC_id %  "只能有一个输出";
				assert_args(str);
			}
			if (mythread->flag == 8) {
				value = NULL;
				qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
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
		else
		{
			//第一个输出参数
			if (Out_count == 0) {
				if (In_count != Count_arg) {
					QString str = mythread->QDBC_id % "[error:] 读取参数失败 实际参数(<<)与sql语句参数不匹配:参数过多实际参数个数：" %QString::number(In_count) % "sql语句参数：" % QString::number(Count_arg - 1);
					qFatal(str.toUtf8());
					value = NULL;
					return *this;
				}

				qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
				mythread->do_sql(mythread->flag);
				this->In_count = 0;

			}
			else
			{
				QString str = mythread->QDBC_id %  "只能有一个输出";
				assert_args(str);
			}
			if (mythread->flag == 8) {
				value = NULL;
				qWarning() << mythread->QDBC_id % "数据库遇到了错误...";
				return *this;
			}
			int size = mythread->res_data.size();
			if (size == 0) {
				value = NULL;
				return *this;
			}
			//value = New_adress<T>();
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
	}
}
template<typename T>
inline QdbcTemplate & QdbcTemplate::operator >(QList<T*>& value)
{
	if (isUseUnion == true) {
		//第一个输出参数
		///*if (Out_count == 0) {
		//	if (In_count != Count_arg) {
		//		QString str = mythread->QDBC_id  % "[error:] 读取参数失败 实际参数(<)与sql语句参数不匹配:参数过多  实际参数(<)个数：" %QString::number(In_count - 1) % "sql语句参数：" % QString::number(Count_arg - 2);
		//		assert_args(str);
		//		value.clear();
		//		return *this;
		//	}

		//	qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
		//	mythread->do_sql(mythread->flag);
		//	this->In_count = 0;

		//}
		//else
		//{
		//	QString str = mythread->QDBC_id %  "只能有一个输出";
		//	assert_args(str);
		//}
		//if (mythread->flag == 8) {
		//	qWarning() << mythread->QDBC_id %  "数据库遇到了错误...";
		//	return *this;
		//}
		//T* t1 = New_adress<T>();
		//Object_utils::clear(t1);
		//QMap<QString, QString> mpclass;
		//while (this->Out_count < mythread->res_data.size()) {
		//	QByteArray key = mythread->res_data[this->Out_count][this->Out_count_row].toByteArray();
		//	bool b = t1->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);

		//	if (b == false) {
		//		QString tbname = tablenames[key];
		//		if (tablenames.contains(tbname)) {
		//			tbname = tablenames[tbname];
		//		}
		//		if (mpclass.contains(tbname)) {
		//			QObject* obj = this->invokefunc(t1, mpclass[tbname].toLatin1(),-1);
		//			obj->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
		//		}
		//		else
		//		{
		//			QByteArray keyname;
		//			if (this->invokefunc(t1, tbname.toLatin1(), keyname) == 1) {
		//				mpclass[tbname] = keyname;
		//				QObject* obj = this->invokefunc(t1, keyname, -1);
		//				adress[(int)obj] = obj;
		//				obj->setProperty(key, mythread->res_data[this->Out_count][this->Out_count_row + 1]);
		//			}
		//		}
		//	}

		//	this->Out_count_row += 2;
		//	if (this->Out_count_row >= mythread->res_data[Out_count].size()) {
		//		this->Out_count_row = 0;
		//		this->Out_count++;
		//		value.append(t1);
		//		QStringList ls = mpclass.keys();
		//		t1 = New_adress<T>();
		//		for (int i = 0; i < ls.length();i++)
		//		{
		//			this->invokefunc(t1, ls[i].toLatin1(), mpclass[ls[i]].toLatin1());
		//		}
		//		Object_utils::clear(t1);
		//	}
		//	if (this->Out_count == mythread->res_data.size()) {
		//		delete_address(t1);
		//		qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
		//		this->Out_count++;
		//	}
		//}
		//return *this;*/
		// TODO: 在此处插入 return 语句
	}
	else
	{
		//第一个输出参数
		if (Out_count == 0) {
			if (In_count != Count_arg) {
				QString str = mythread->QDBC_id  % "[error:] 读取参数失败 实际参数(<)与sql语句参数不匹配:参数过多  实际参数(<)个数：" %QString::number(In_count - 1) % "sql语句参数：" % QString::number(Count_arg - 2);
				assert_args(str);
				value.clear();
				return *this;
			}

			qInfo() << mythread->QDBC_id << " Qdbc perpare start<...";
			mythread->do_sql(mythread->flag);
			this->In_count = 0;

		}
		else
		{
			QString str = mythread->QDBC_id %  "只能有一个输出";
			assert_args(str);
		}
		if (mythread->flag == 8) {
			qWarning() << mythread->QDBC_id %  "数据库遇到了错误...";
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
		// TODO: 在此处插入 return 语句
	}

}
