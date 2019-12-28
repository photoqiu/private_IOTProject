#include <stdlib.h>
#include <iostream>
/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "jdbc/mysql_connection.h"
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#pragma once


using namespace std;

#define DBHost "tcp://127.0.0.1:3306";
#define DBUser "root";
#define DBPwd "123";
#define DBName  "laser_datas";

class DBOperation
{
public:
	DBOperation();
	~DBOperation();
	int getVideoCamerasResult();
	int getLaserCamerasResult();
	int setVideoCamerasResult();
	int setLaserCamerasResult();
private:
	
};

