#include "stdafx.h"
#include "DBOperation.h"


DBOperation::DBOperation()
{
}


DBOperation::~DBOperation()
{
}

int DBOperation::setLaserCamerasResult()
{
	return 0;
}

int DBOperation::setVideoCamerasResult()
{
	try {
		sql::Driver *driver;
		sql::PreparedStatement *pstmt;
		/* Create a connection */
		driver = get_driver_instance();
		/* Connect to the MySQL test database */
		std::auto_ptr< sql::Connection > con(driver->connect("tcp://127.0.0.1:3306", "root", "123456"));
		con->setSchema("laser_datas");
		// std::auto_ptr< sql::PreparedStatement >  pstmt;
		pstmt = con->prepareStatement("INSERT INTO laser_datas(id) VALUES (?)");
		pstmt->setInt(1, 0);
		pstmt->executeUpdate();
		delete pstmt;
		/*
		TRUNCATE `hg_detection_db`.`camerasinfo`
		TRUNCATE `hg_detection_db`.`laserinfo`
		ANALYZE TABLE `camerasinfo`, `laserinfo`, `steelpipeinfo`
		CHECK TABLE `camerasinfo`, `laserinfo`, `steelpipeinfo`
		CHECKSUM TABLE `camerasinfo`, `laserinfo`, `steelpipeinfo`
		OPTIMIZE TABLE `camerasinfo`, `laserinfo`, `steelpipeinfo`
		REPAIR TABLE `camerasinfo`, `laserinfo`, `steelpipeinfo`
		pstmt.reset(con->prepareStatement("CALL add_country(?,?,?)"));
		pstmt->setString(1, code_vector[i]);
		pstmt->setString(2, name_vector[i]);
		pstmt->setString(3, cont_vector[i]);
		pstmt->execute();
		*/
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line  » " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	cout << endl;
	return EXIT_SUCCESS;
}

int DBOperation::getVideoCamerasResult()
{
	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "123456");
		/* Connect to the MySQL test database */
		con->setSchema("laser_datas");
		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
		while (res->next()) {
			cout << "\t... MySQL replies: ";
			/* Access column data by alias or column name */
			cout << res->getString("_message") << endl;
			cout << "\t... MySQL says it again: ";
			/* Access column data by numeric offset, 1 is the first column */
			cout << res->getString(1) << endl;
		}
		delete res;
		delete stmt;
		delete con;
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line  » "<< __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	cout << endl;
	return EXIT_SUCCESS;
}

int DBOperation::getLaserCamerasResult()
{
	return EXIT_SUCCESS;
}
