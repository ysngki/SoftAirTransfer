#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<string.h>
#include<iostream>
#include<errno.h>
#include<unistd.h>
#include<sstream>
#include<vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "send_receive.h"


ListenHandle::ListenHandle()
{
    // create listen socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        printf("Create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // create listen address
    struct sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr("172.xx.x.xx");
    //ser_addr.sin_addr.s_addr = INADDR_ANY;
    ser_addr.sin_port = htons(8868);

    // bind listen socket to ser_address
    if (bind(listen_fd, (struct sockaddr*) & ser_addr, sizeof(ser_addr)) == -1)
    {
        printf("Bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // connect database
    database = new MyDB();
}

ListenHandle::~ListenHandle()
{
    delete database;
    close(listen_fd);
}


void ListenHandle::begin_listen()
{
    // begin to listen
    if (listen(listen_fd, 20) == -1)
    {
        printf("Listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // process request
    while (1)
    {
        int conn_fd;
        struct sockaddr_in client_addr;

        // get connection socket
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t length = sizeof(client_addr);
        std::cout << "Begin listen!!" << std::endl;

        conn_fd = accept(listen_fd, (struct sockaddr*) & client_addr, &length);

        while (conn_fd == -1)
        {
            close(conn_fd);
            continue;
        }

        std::cout << "new connection!" << std::endl;

        // receive command
        char request[100] = {0};
        int len = 0;
        int now_len = 0;
        int type = 0;
        int all_len = 0;

        if( recv(conn_fd, request, 2, 0) < 2)
        {
            std::cout << "False!!!" << std::endl;
            close(conn_fd);
        }

        all_len = static_cast<unsigned char>(request[0]);
        type = static_cast<unsigned char>(request[1]);
        request[0] = 0;
        request[1] = 0;


        while(now_len < all_len)
        {
            len = recv(conn_fd, request+now_len, all_len-now_len, 0);
            now_len += len;
        }
        

        // handle command
        switch(type)
        {
            // exec sql
            case 0:
                send(conn_fd, database->exeSQL(std::string(request)).c_str(), strlen(request), 0);
                break;
            // register
            case 1:
                handle_register(conn_fd, request);
                break;
            // login
            case 2:
                handle_login(conn_fd, request);
                break;
            default:
                std::cout << "Unkown command!!!" << std::endl;
        }

        close(conn_fd);
    }
}


void ListenHandle::handle_register(int conn_fd, char* request)
{
    std::string message;

    std::vector<std::string> input = split(std::string(request), "\n");
    if(input.size() != 2)
    {
        send(conn_fd, make_response("0") , strlen(request), 0);
    }


    std::string username = input[0];
    std::string passward = input[1];

    std::string exe_statement("select * from user where user_name = \"");
    exe_statement += username ;
    exe_statement += "\";";
    std::string exe_result = run_statement(exe_statement);

    
    // check user_name exist
    input = split(exe_result, "\n");
    if(input.size() > 1)
    {
        message.assign("false");
        my_send(conn_fd,message);
        return;
    }

    // insert (hash_pwd, id, locked, wrong_number) to id_pwd
    exe_statement = "insert into id_pwd(pwd_hash, locked, wrong_num) values (\"";
    exe_statement += passward;
    exe_statement += "\", ";
    exe_statement += " 0, 0);";
    exe_result = run_statement(exe_statement);
    exe_result = split(exe_result, "\n")[0];
    if( strcmp(exe_result.c_str(), "false") == 0)
    {
        message.assign("false");
        my_send(conn_fd,message);
        database->my_rollback();
        return;
    }

    // get id
    exe_statement = " select LAST_INSERT_ID() ;";
    exe_result = run_statement(exe_statement);
    input = split(exe_result, "\n");
    exe_result = input[1];
    std::string id = exe_result;

    // insert (username, id, mode_code, img_url) to user
    exe_statement = "insert into user(user_name, id, mode_code) values (\"";
    exe_statement += username;
    exe_statement += "\", ";
    exe_statement += id;
    exe_statement += ", 0);";
    exe_result = run_statement(exe_statement);
    exe_result = split(exe_result, "\n")[0];
    if( strcmp(exe_result.c_str(), "false") == 0)
    {
        message.assign("false");
        my_send(conn_fd,message);
        database->my_rollback();
        return;
    }

    database->my_commit();
    message.assign("true");
    my_send(conn_fd, message);
}


void ListenHandle::handle_login(int conn_fd, char* request)
{
    std::string message;

    std::vector<std::string> input = split(std::string(request), "\n");
    if(input.size() != 2)
    {
        send(conn_fd, make_response("0") , strlen(request), 0);
    }

    std::string username = input[0];
    std::string passward = input[1];

    std::string exe_statement("select * from user where user_name = \"");
    exe_statement += username ;
    exe_statement += "\";";
    std::string exe_result = run_statement(exe_statement);

    // check user_name exist
    input = split(exe_result, "\n");
    if(input.size() < 2)
    {
        message.assign("1 0");
        my_send(conn_fd,message);
        return;
    }


    // get id
    std::string id = split(input[1], " ")[1];
    // get from id_pwd
    exe_statement = "select * from id_pwd where id = ";
    exe_statement += id ;
    exe_statement += ";";
    exe_result = run_statement(exe_statement);

    // first row
    exe_result = split(exe_result, "\n")[1];
    input = split(exe_result, " ");
    // check locked
    std::string locked = input[2];
    if(locked == std::string("1"))
    {
        // locked
        message.assign("3 0");
        my_send(conn_fd,message);
        return;
    }
    // get wrong num
    int wrong_num = atoi(input[3].c_str());
    // get pwd
    std::string pwd = input[1];

    if(pwd == passward)
    {
        message.assign("0 ");
        message += id;
        // set wrong num to 0
        wrong_num = 0;
        exe_statement = "UPDATE id_pwd SET wrong_num=0 WHERE id=";
        exe_statement += id ;
        exe_statement += ";";
        std::cout << exe_statement << std::endl;
        run_statement(exe_statement);
    }
    else
    {
        ++wrong_num;
        // lock
        if(wrong_num == 5)
        {
            exe_statement = "UPDATE id_pwd SET wrong_num=5, locked = 1 WHERE id=";
            exe_statement += id ;
            exe_statement += ";";
            std::cout << exe_statement << std::endl;
            run_statement(exe_statement);
            message.assign("3 0");
        }
        else
        {
            exe_statement = "UPDATE id_pwd SET wrong_num= ";

            std::stringstream ss;  
            ss << wrong_num;                  
            std::string temp_s;
            ss >> temp_s;

            exe_statement += temp_s;
            exe_statement += " WHERE id=";
            exe_statement += id ;
            exe_statement += ";";
            std::cout << exe_statement << std::endl;
            run_statement(exe_statement);

            message.assign("2 ");
            message += temp_s;
        }
    }
    
    database->my_commit();
    my_send(conn_fd,message);
}


int ListenHandle::my_send(int conn_fd, std::string& message)
{
    char *my_message = make_response(message.c_str());
    int len = send(conn_fd, my_message, strlen(my_message), 0);
    free(my_message);
    return len;
}


char* ListenHandle::make_response(const char* respone)
{
    char* my_statement = (char*)malloc((strlen(respone) + 2) * sizeof(char));
    memset(my_statement, 0, (strlen(respone) + 2) * sizeof(char));

    //设置长度，不包括'\0'
    *my_statement = strlen(respone);
    //设置命令类型
    memcpy(my_statement + 1, respone, strlen(respone));

    return my_statement;
}


std::vector<std::string> ListenHandle::split(const std::string& s, const char* delimiter)
{
    char temp = delimiter[0];
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, temp))
    {
        tokens.push_back(token);
    }
    return tokens;
}


std::string ListenHandle::run_statement(std::string statement)
{
    return database->exeSQL(statement);
}


MyDB::MyDB()
{
    connection = mysql_init(NULL);

    if (connection == NULL)
    {
        std::cout << "Error:" << mysql_error(connection);
        exit(1);
    }

    if (mysql_real_connect(connection, "localhost", "root",
        "xxxxxxxx", "databse_name", 0, NULL, 0) == NULL) {
        printf("Error %u: %s\n", mysql_errno(connection), mysql_error(connection));
        exit(1);
    }

    mysql_autocommit(connection,0);

    std::cout << "Database connection succeed!" << std::endl;
}


MyDB::~MyDB()
{
    if (connection != NULL)
    {
        mysql_close(connection);
    }
}



std::string MyDB::exeSQL(std::string sql)
{
    std::string s_result;
	// mysql_query()执行成功返回0，失败返回非0值。与PHP中不一样
	if(mysql_query(connection, sql.c_str()))
	{
		std::cout << "Query Error:" << mysql_error(connection);
        std::cout << std::endl;
        s_result += "false\n";
		return s_result;
	}
	else
	{
        s_result += "true\n";
		result = mysql_store_result(connection); // 获取结果集
        if(result == NULL)
            return s_result;
		// mysql_field_count()返回connection查询的列数
		while ((row = mysql_fetch_row(result)))
		{
			// mysql_num_fields()返回结果集中的字段数
			for(int j=0; j < mysql_num_fields(result); ++j)
			{
                if(row[j])
                {
                    s_result += row[j];
                }
                else
                {
                    s_result += "NULL";
                }
                s_result += " ";
			}
            s_result += "\n";
			//std::cout << std::endl;
		}
		// 释放结果集的内存
		mysql_free_result(result);
        return s_result;
	}
}

void MyDB::my_commit()
{
    mysql_commit(connection);
}


void MyDB::my_rollback()
{
    mysql_rollback(connection);
}

