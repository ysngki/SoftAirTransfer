#pragma once
#ifndef SEND_RECEIVE_H
#define SEND_RECEIVE_H
#include<mysql/mysql.h>
#include<string>
#include<vector>

class MyDB
{
public:
	MyDB();
	~MyDB();
	//bool initDB(std::string host, std::string  user, std::string  pwd, std::string  db_name);
	std::string exeSQL(std::string  sql);
	void my_commit();
	void my_rollback();
private:
	MYSQL* connection;
	MYSQL_RES* result;
	MYSQL_ROW row;
};


class ListenHandle
{
private:
	int listen_fd;
	MyDB *database;

	std::string run_statement(std::string statement);
	void handle_register(int conn_fd, char* request);
	void handle_login(int conn_fd, char* request);
	
	std::vector<std::string> split(const std::string& s, const char* delimiter);
	int my_send(int conn_fd, std::string& message);
	// add length to response
	char* make_response(const char* respone);
public:
	ListenHandle();
	~ListenHandle();
	void begin_listen();
};


#endif // !SEND_RECEIVE_H

