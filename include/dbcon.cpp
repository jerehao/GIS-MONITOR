﻿#include "dbcon.h"
#include <QString>
#include <QStringList>
#include <QtSql>
#include <QDebug>

//#define DB_HOST "localhost"
//#define DB_NAME "gis-database"
//#define DB_USER "root"
//#define DB_PASSWD ""
QString dbcon::dbHost="localhost";
QString dbcon::dbName="gis-database";
QString dbcon::dbUserName="root";
QString dbcon::dbPasswd="";
QString dbcon::prefix="h_";
//#define DB_HOST "rm-bp1bv2p9m6x3n8fc1.mysql.rds.aliyuncs.com"
//#define DB_NAME "rho93hp8qb"
//#define DB_USER "rho93hp8qb"
//#define DB_PASSWD "HFkeding1994"

dbcon::dbcon():isopen(false),group_id(0)
{
    QSettings configIniRead("monitor.ini", QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    dbcon::dbHost = (configIniRead.value("/DateBase/DB_HOST","localhost").toString()=="000")?"rm-bp1bv2p9m6x3n8fc1.mysql.rds.aliyuncs.com":configIniRead.value("/DateBase/DB_HOST","localhost").toString();
    dbcon::dbName = (configIniRead.value("/DateBase/DB_NAME","gis-database").toString()=="000")?"rho93hp8qb":configIniRead.value("/DateBase/DB_NAME","gis-database").toString();
    dbcon::dbUserName = (configIniRead.value("/DateBase/DB_USER","root").toString()=="000")?"rho93hp8qb":configIniRead.value("/DateBase/DB_USER","root").toString();
    dbcon::dbPasswd = (configIniRead.value("/DateBase/DB_PASSWD","").toString()=="000")?"HFkeding1994":configIniRead.value("/DateBase/DB_PASSWD","").toString();
    dbcon::prefix = configIniRead.value("/DateBase/PREFIX","h_").toString();

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(dbcon::dbHost);
    db.setDatabaseName(dbcon::dbName);
    db.setUserName(dbcon::dbUserName);
    db.setPassword(dbcon::dbPasswd);
    db.setPort(3306);
    open();
}
dbcon::~dbcon()
{
    close();
}
bool dbcon::open()
{
    isopen=db.open();
    return isopen;
}
void dbcon::reopen()
{
    QSqlQuery query;
    if (!query.exec("SELECT 1+1"))
        isopen=db.open();
}
void dbcon::close()
{
    isopen=false;
    db.close();
}
bool dbcon::get_phonenumbers(QStringList &phoneNumberList)
{
    QSqlQuery query;
    int user_gid=0;
    QString str;
    if(group_id!=0){
        queries="SELECT user_gid FROM "+dbcon::prefix+"groups WHERE group_id="+QString::number(group_id);
        query.exec(queries);
        if (query.next()) {
            user_gid=query.value(0).toInt();
            qDebug()<<query.value(0).toString();
        }
        queries.clear();
        query.clear();
    }

    str=(user_gid==0)?QString("user_gid=0"):QString("(user_gid=0 OR user_gid=%1)").arg(QString::number(user_gid));
    phoneNumberList.clear();

    queries="SELECT user_phone FROM "+dbcon::prefix+"users WHERE is_send=1 AND "+str;
    query.exec(queries);
    while (query.next()) {
        phoneNumberList.append(query.value(0).toString());
    }
	return !phoneNumberList.isEmpty();
}

unsigned int dbcon::get_dev(QString &devNum)
{
    QSqlQuery query;
    queries="SELECT dev_id,group_id FROM "+dbcon::prefix+"devs WHERE dev_number='"+devNum+"'";
    query.exec(queries);
    unsigned int devId=0;
    group_id=0;
    if (query.next()){
        devId=query.value(0).toInt();
        group_id=query.value(1).toInt();
    }
    return devId;
//        groupId=query.value(1).toInt();
//        lineId=query.value(2).toInt();
//        devPhase=query.value(3).toString();
//   }

//    if(groupId!=0){
//        queries.clear();
//        query.clear();
//        queries="SELECT group_loc,group_name FROM groups WHERE group_id="+groupId;
//        query.exec(queries);
//        if (query.next()){
//            groupLocName.clear();
//            groupLocName=query.value(0).toString()+"-"+query.value(1).toString();
//        }
//    }
//    if(lineId!=0){
//        queries.clear();
//        query.clear();
//        queries="SELECT line_name FROM groups WHERE line_id="+lineId;
//        query.exec(queries);
//        if (query.next()){
//            lineName.clear();
//            lineName=query.value(0).toString();
//        }
//    }
}

void dbcon::insert_alarm(AlarmInfo &alarm,int flag)
{
    QSqlQuery query;
    QString QStraction;
    QStraction=(flag==1)?"alarms":"histories";
    QStraction=dbcon::prefix+QStraction;
    queries=QString("INSERT INTO %1(action_time,action_num,i_num,tem,hum,dev_id) VALUES('%2',%3,%4,%5,%6,%7)").arg(QStraction,alarm.time,QString::number(alarm.actionCount),QString::number(alarm.iNum),alarm.tem,alarm.hum,QString::number(alarm.devId));
    query.exec(queries);
}
