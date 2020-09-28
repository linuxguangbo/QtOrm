/****************************************************************************
**
** Copyright (C) 2020  The Qdbc Authors. All rights reserved.
** Contact: 1104559085@qq.com
**
** This file is part of the Qdbc module of the Qdbc  Toolkit.
** Supported protocols��Apache License 2.0
** Software protocol view license file
** Please refer to the readme file for instructions
****************************************************************************/
#pragma once
#include "Object_qdbc.h"
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
#define Q_ATTR(T,member) \
	Q_PROPERTY(T member READ get##member WRITE set##member) \
public: \
	Q_INVOKABLE T get##member() { return member;} \
	Q_INVOKABLE void set##member(T t) {member = t;} \
	Q_INVOKABLE T member; 

#define Q_ATTR_ALIAS(T,member,alias) \
	Q_PROPERTY(T alias READ get##member WRITE set##member) \
public:	\
	Q_INVOKABLE T get##member() { return member;} \
	Q_INVOKABLE void set##member(T t) {member = t;} \
	Q_INVOKABLE T member; 


#define  Qendl " "


#define  Qif(condition,va) \
		Qendl%QdbcTemplate::singleinstance()->getselfAnalysis()->getif(condition,1,va)%Qendl	

#define Qelif(condition,va) \
		Qendl%QdbcTemplate::singleinstance()->getselfAnalysis()->getif(condition,2,va)%Qendl	

#define Qelse(va) \
		Qendl%QdbcTemplate::singleinstance()->getselfAnalysis()->getif(false,3,va)%Qendl	


#define  Qswitch(const1) \
		Qendl%QdbcTemplate::singleinstance()->getselfAnalysis()->getswitch(const1,"",10)%Qendl
#define Qcase(const1,value1)  \
		Qendl%QdbcTemplate::singleinstance()->getselfAnalysis()->getswitch(const1,value1,11)%Qendl
#define Qdefault(value1)  \
		Qendl%QdbcTemplate::singleinstance()->getselfAnalysis()->getswitch("",value1,12)%Qendl


#define  Qforeach(begin,separator,end,collection) \
		Qendl%QdbcTemplate::singleinstance()->getselfAnalysis()->getforeach(begin,separator,end,collection)%Qendl

#define  IntList QList<int>



#define	 Qselect(x) QdbcTemplate::singleinstance()->select(x)
#define  Qupdate(x) Qdelete(x)
#define  Qinsert(x) Qdelete(x)
#define	 Qdelete(x) QdbcTemplate::singleinstance()->update(x) 
#define  Qclear()  QdbcTemplate::singleinstance()->ponit_clear() 
#define Qtransactional() QdbcTemplate t("5")

#define QgetDabaseConnect() QdbcTemplate::singleinstance()->getDabaseConnect();

#define  QDBC_Id QdbcTemplate::singleinstance()->getselfthread()->get_QDBC_id()

#define QBye()	QdbcTemplate::singleinstance()->QdbcTemplateClear()

#define QMajor_version "20.09"
#define Qminor_version "017"
#define Qstable   "s"
#define Qtest     "c"

#define  Qversion QMajor_version "." Qminor_version  Qtest

#define  QMysql "QMYSQL"
#define  QOracle "QOCI"
#define  QSqlite3 "QSQLITE"  
#define  QSqlite2 "QSQLITE2"

//��ǰ��֧�ֵ����ݿ�
#define  Qcurrentdatebase (QMysql)

#define QconfigPath(path,name)  do \
	{	\
	Qconfig t;  \
	t.setPath(path,name); \
}while (0) 
#define  Hint 0
#if Hiint
	// T* Ϊָ�����ͣ���Ҫ�̳���Qobject����ʹ��Q_ATTR����Q_ATTR_ALIAS�궨������ԣ� ���ظ�ʽ���ַ���
	Object_utils::toString(T* src);
	//QList<T*>&  ΪQList<T*> QList�����TΪָ�����ͣ���Ҫ�̳���Qobject����ʹ��Q_ATTR����Q_ATTR_ALIAS�궨������ԣ� ���ظ�ʽ���ַ���
	Object_utils::toString(QList<T*>& value)
	//T* src Ϊָ�����ͣ� T* dec Ϊָ������ ��Ҫ�̳���Qobject����ʹ��Q_ATTR����Q_ATTR_ALIAS�궨������ԣ��˷�������ֵ����
	Object_utils::copy(T* src, T* dec)
	//T* data Ҫ�̳���Qobject����ʹ��Q_ATTR����Q_ATTR_ALIAS�궨������ԣ��˷��������������
	Object_utils::clear(T* & data)
	//T&  data Ҫ�̳���Qobject����ʹ��Q_ATTR����Q_ATTR_ALIAS�궨������ԣ��˷��������������
	Object_utils::clear(T& data)
	//T&  data Ҫ�̳���Qobject����ʹ��Q_ATTR����Q_ATTR_ALIAS�궨������ԣ��˷��������ж������Ƿ�Ϊ��
	Object_utils::isNULL(T& data)
	//T&  data Ҫ�̳���Qobject����ʹ��Q_ATTR����Q_ATTR_ALIAS�궨������ԣ��˷��������ж������Ƿ����
	Object_utils::isClear(T& data)
#endif