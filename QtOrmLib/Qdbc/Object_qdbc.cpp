#include "../include/Object_qdbc.h"
#include <QDebug>
#include <QTime>
#include <QSettings>
#include <QMetaType>
#include <QMetaProperty>
#include "../include/Qdbc.h"
QMap<int, QdbcTemplate*> QdbcTemplate::__instance__;
QdbcTemplate::QdbcTemplate(QObject *parent)
	: QObject(parent)
{
	Qconfig config;
	QString path;
	bool b =config.fullfilepath(path);
	if (b == false) {
		this->assert_args("can not find this config file");
	}
	int a = ++atom;
	//��ʱ��ʹ���̴߳�����Ȼ�˴�������λ�߳�
	//fullfilepath(Qconfig::filename, Qconfig::fliepath);
	mythread = new QTmeplate(this);
	QSettings *ini = new QSettings(path, QSettings::IniFormat);
	Loglevel = ini->value("TEMPLATE/Loglevel", 0).toInt();
	ini->setValue("TEMPLATE/Loglevel", QString(Qversion));
	delete ini;

	mythread->QDBC_id = "QDBC_" % QString::number(a);
	QVariantList m_data;
	m_data << path;
	mythread->do_sql(m_data, 0);
}
int QdbcTemplate::countString(QString str)
{
	int count = 0;
	for (int i =0; i < str.length();i++)
	{
		if (str[i] == "#") {
			count++;
		}

	}
	return count;
}
void QdbcTemplate::qtransactional()
{
	
	mythread->flag = 5;
	mythread->do_sql(mythread->m_data, mythread->flag);
	if (getdata_res()) {
		mythread->issqltransactionResult = true;
	}
	else
	{
		mythread->issqltransactionResult = false;
	}
	qInfo() << mythread->QDBC_id << "open the transaction!" << mythread->issqltransactionResult;
	//��Ҫ������ѯ���
}

void QdbcTemplate::qtransactional_clear()
{
	if (mythread->issqltransactionResult == false) {
		mythread->flag = 7;
		mythread->do_sql(mythread->m_data, mythread->flag);
		qDebug() << mythread->QDBC_id % "rollback the transaction!" << getdata_res();		
	}
	else {
		mythread->flag = 6;
		mythread->do_sql(mythread->m_data, mythread->flag);
		qDebug() << mythread->QDBC_id %  "commit the transaction!" << getdata_res();
	}
	
}

bool QdbcTemplate::getdata_res()
{
	bool t = true;
	if (mythread->flag == 8) {
		t = false;
	}
	return t;
}
QdbcTemplate::QdbcTemplate(QString flags)
{
	if (flags == "5") {
		QdbcTemplate::singleinstance()->qtransactional();
	}
	else
	{
		this->assert_args("can not use this  class!");
	}
}
void QdbcTemplate::args(QString value)
{
	//��һ���������
	Count_arg = this->countString(value);
	mythread->m_data.append(value);
	this->Out_count = this->In_count =  0;
}
void QdbcTemplate::assert_args(const QString& str)
{
	{
		QMutexLocker lock(&__mutex);
		mythread->m_data.clear();
		mythread->res_data.clear();
		mythread->do_sql(mythread->m_data,8);
	}
	qFatal(str.toUtf8());
}
QdbcTemplate & QdbcTemplate::operator <(QObject*  value)
{
	QString sql = mythread->m_data[0].toString();

	const QMetaObject *metaObject = value->metaObject();
	int count = metaObject->propertyCount();
	//����Ԥ����
	int index_str = 0;
	while (true)
	{
			int res  = sql.indexOf("#{", index_str + 2);
			if (res == index_str || res == -1) {
				break;
			}
			index_str = res;
			int lastindex = sql.indexOf('}', index_str);
			QString str = sql.mid(index_str + 2, lastindex - index_str -2);

			QVariant t1;
			bool resfunc = invokefunc(value, str.toUtf8(), t1);
			if (resfunc == false) {
				QString strs = "There is no such function : get" + str;
				assert_args(strs);
			}
			mythread->m_data.append(t1);
			sql.replace("#{"%str%"}", "?");
			
			In_count++;
	}

	//�����ַ���ƴ��
	index_str = 0;
	while (true)
	{
			int res = sql.indexOf("${", index_str + 1);
			if (res == index_str || res == -1) {
				break;
			}
			index_str = res;
			int lastindex = sql.indexOf('}', index_str);
			QString str = sql.mid(index_str + 2, lastindex - index_str - 2);

			QVariant t1;
			bool resfunc = invokefunc(value, str.toUtf8(), t1);
			if (resfunc == false) {
				QString strs = "There is no such function : get" + str;
				assert_args(strs);
			}
			mythread->m_data.append(t1);
			sql.replace("#{"%str%"}", t1.toString());
	}
	mythread->m_data[0] = sql;
	return *this;

}
QdbcTemplate & QdbcTemplate::operator <(QString value)
{
	args(value);
	QString sql = mythread->m_data[0].toString();
	int res = sql.indexOf("#{", 0);
	if (res < 0) {
		res = sql.indexOf("${", 0);
		if (res > 0) {
			int lastindex = sql.indexOf('}', res);
			QString str = sql.mid(res + 2, lastindex - res - 2);
			sql.replace("${"%str%"}", value);
		}
	}
	else
	{
		int right =  sql.indexOf("}", res);
		sql.replace(res,right - res + 1, "?");
		mythread->m_data.append(value);
		In_count++;
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator <(int value)
{
	QString value_str = QString::number(value);
	this->getselfAnalysis()->findchild(value_str,value);
	QString sql = mythread->m_data[0].toString();
	int res = sql.indexOf("#{", 0);
	if (res < 0) {
		res = sql.indexOf("${", 0);
		if (res > 0) {
			int lastindex = sql.indexOf('}', res);
			QString str = sql.mid(res + 2, lastindex - res - 2);
			sql.replace("${"%str%"}", value_str);
		}
	}
	else
	{
		int right = sql.indexOf("}", res);
		sql.replace(res, right - res + 1, "?");
		mythread->m_data.append(value);
		In_count++;
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator<(bool value)
{
	QString value_str = QString::number(value);
	QString sql = mythread->m_data[0].toString();
	int res = sql.indexOf("#{", 0);
	if (res < 0) {
		res = sql.indexOf("${", 0);
		if (res > 0) {
			int lastindex = sql.indexOf('}', res);
			QString str = sql.mid(res + 2, lastindex - res - 2);
			sql.replace("${"%str%"}", value_str);
		}
	}
	else
	{
		int right = sql.indexOf("}", res);
		sql.replace(res, right - res + 1, "?");
		mythread->m_data.append(value);
		In_count++;
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator<(QDateTime & date)
{
	QString value_str = date.toString(Qt::ISODate);
	QString sql = mythread->m_data[0].toString();
	int res = sql.indexOf("#{", 0);
	if (res < 0) {
		res = sql.indexOf("${", 0);
		if (res > 0) {
			int lastindex = sql.indexOf('}', res);
			QString str = sql.mid(res + 2, lastindex - res - 2);
			sql.replace("${"%str%"}", value_str);
		}
	}
	else
	{
		int right = sql.indexOf("}", res);
		sql.replace(res, right - res + 1, "?");
		mythread->m_data.append(date);
		In_count++;
	}
	mythread->m_data[0] = sql;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator >(QString & value)
{
	//��һ���������
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id %"[error:] ��ȡ����ʧ�� ʵ�ʲ���(<)��sql��������ƥ��:��������  ʵ�ʲ���(<)������" %QString::number(In_count - 1) % "sql��������" % QString::number(Count_arg - 2);
			assert_args(str);
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
	value = "";
	if (mythread->flag == 3) {
		if (this->Out_count < mythread->m_data.size()) {
			value = mythread->res_data[this->Out_count][1].toString();
		}
	}
	qInfo() << mythread->QDBC_id << "Qdbc perpare end>";
	this->Out_count++;
	return *this;
}
QdbcTemplate & QdbcTemplate::operator > (int & value)
{
	//��һ���������
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id %"[error:] ��ȡ����ʧ�� ʵ�ʲ���(<)��sql��������ƥ��:��������  ʵ�ʲ���(<)������" %QString::number(In_count - 1) % "sql��������" % QString::number(Count_arg - 2);
			assert_args(str);
			return *this;
		}
		qInfo() << mythread->QDBC_id  <<  "[QDBC] Qdbc perpare start<...";
		mythread->do_sql(mythread->m_data, mythread->flag);
		this->In_count = 0;

	}
	else
	{
		QString str = mythread->QDBC_id %  "ֻ����һ�����";
		assert_args(str);
	}
	if (mythread->flag == 8) {
		qWarning() << mythread->QDBC_id  % "���ݿ������˴���...";
		value = -1;
		return *this;
	}
	bool t = false;
	if (this->Out_count < mythread->res_data.size()) {
		value = mythread->res_data[this->Out_count][1].toInt();
	}
	qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
	this->Out_count++;
	return *this;
}
QdbcTemplate& QdbcTemplate::operator > (bool& value) {
	//��һ���������
	if (Out_count == 0) {
		if (In_count != Count_arg) {
			QString str = mythread->QDBC_id %  "[error:] ��ȡ����ʧ�� ʵ�ʲ���(<)��sql��������ƥ��:��������  ʵ�ʲ���(<)������" %QString::number(In_count) % "sql��������" % QString::number(Count_arg - 2);
			assert_args(str);
			return *this;
		}
		qInfo() << mythread->QDBC_id << "[QDBC] Qdbc perpare start<...";
		mythread->do_sql(mythread->m_data, mythread->flag);
		this->In_count = 0;

	}
	else
	{
		QString str = mythread->QDBC_id %  "ֻ����һ�����";
		assert_args(str);
	}
	if (mythread->flag == 8) {
		qWarning() << mythread->QDBC_id  % "���ݿ������˴���...";
		value = false;
		return *this;
	}
	bool t = false;
	if (this->Out_count < mythread->res_data.size()) {
		value = mythread->res_data[this->Out_count][1].toInt();
	}
	qInfo() << mythread->QDBC_id << " Qdbc perpare end>";
	Out_count++;
	return *this;
}
bool QdbcTemplate::getDabaseConnect()
{
	this->select("select 1");
	bool t = false;
	if (mythread->flag == 8) {
		t = false;
	}
	else
	{
		mythread->flag = 1;
		mythread->do_sql(mythread->m_data, mythread->flag);
		t = getdata_res();
	}
	clear();
	return t;
	
}

bool QdbcTemplate::invokefunc(QObject * value, QByteArray name, QVariant & t)
{
	QByteArray gets = "get" % name;
	QByteArray getss = "get" %name % "()";
	char* gets_ptr = getss.data();
	const QMetaObject* theMetaObject = value->metaObject();
	for (int i = 5; i < theMetaObject->methodCount(); i++)
	{
		QMetaMethod oneMethod = theMetaObject->method(i);
		if (strcmp(gets_ptr, oneMethod.methodSignature()) == 0) {
			/*	qDebug() << "typeName: " << oneMethod.typeName();
				qDebug() << "signature: " << oneMethod.methodSignature();
				qDebug() << "methodType: " << oneMethod.methodType();;
				qDebug() << "parameterNames: " << oneMethod.parameterNames() << "\n";*/
			gets_ptr = (char*)oneMethod.typeName();
			break;
		}

	}
	QString str = QString(gets_ptr);

	bool resfunc;
	if (str == "int") {
		int retVal;
		resfunc = QMetaObject::invokeMethod(value, gets, Q_RETURN_ARG(int, retVal));
		t.setValue(retVal);
	}
	else if (str == "bool")
	{
		bool retVal;
		resfunc = QMetaObject::invokeMethod(value, gets, Q_RETURN_ARG(bool, retVal));
		t.setValue(retVal);
	}
	else if(str == "QString")
	{
		QString retVal;
		resfunc = QMetaObject::invokeMethod(value, gets, Q_RETURN_ARG(QString, retVal));
		t.setValue(retVal);
	}
	else if (str == "QDateTime")
	{
		QDateTime retVal;
		resfunc = QMetaObject::invokeMethod(value, gets, Q_RETURN_ARG(QDateTime, retVal));
		t.setValue(retVal);
	}
	else
	{
		assert_args( "undef type!!!");
		resfunc = false;
	}
	return resfunc;
}

void QdbcTemplate::thread_dispatch_flag4()
{
	for each (QdbcTemplate* tmep in __instance__)
	{
		tmep->getselfthread()->flag = 4;
	}
}

QdbcTemplate::~QdbcTemplate()
{
	QdbcTemplate::singleinstance()->qtransactional_clear();
	
}

void Qconfig::setPath(char* path, char* name)
{
	fliepath = path;
	filename = name;
}

bool Qconfig::fullfilepath(QString& path)
{
	QString names;
	if (filename[0] == NULL) {
		names = "qjbctemplate.ini";
	}
	else
	{
		names = QString(filename);
	}
	path = QString(fliepath) % "/" %  names;
	QFileInfo t(path);
	bool b =  t.isFile();
	return b;
}
