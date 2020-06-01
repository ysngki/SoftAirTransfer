#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <iostream>
#include<vector>
#include<sstream>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll


class ClientSendReceive
{
private:
    SOCKET sock;

    // add length and type to command
    char* make_command(const char* command_arg, char command_type)
    {
        char* my_statement = (char*)malloc((strlen(command_arg) + 3) * sizeof(char));
        memset(my_statement, 0, (strlen(command_arg) + 3) * sizeof(char));

        //设置长度，不包括'\0'
        *my_statement = strlen(command_arg);
        //设置命令类型
        *(my_statement + 1) = command_type;
        memcpy(my_statement + 2, command_arg, strlen(command_arg));

        return my_statement;
    }

    std::vector<std::string> split(const std::string& s, const char* delimiter)
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
public:
    ClientSendReceive()
    {
        //初始化DLL
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        //创建套接字
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        //向服务器发起请求
        sockaddr_in sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_addr.s_addr = inet_addr("140.143.211.31");
        sockAddr.sin_port = htons(8868);
        if (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) < 0) {
            printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
            exit(1);
        }
    } 

    ~ClientSendReceive()
    {
        //关闭套接字
        closesocket(sock);
        //终止使用 DLL
        WSACleanup();
    }

    std::string run_sql(const char* command_arg)
    {
        char* my_command = make_command(command_arg, 0);
        
        //发送数据,没有发‘\0'
        int temp_len = send(sock, my_command, strlen(my_command+2) + 2, 0);

        //接收服务器传回的数据
        std::string result;
        char szBuffer[MAXBYTE] = { 0 };
        do {
            memset(szBuffer, 0, MAXBYTE);
            temp_len = recv(sock, szBuffer, MAXBYTE, NULL);
            result += szBuffer;
        } while (temp_len < 0);

        free(my_command);
        return result;
    }

    // my_register("yang/n1234567");
    bool my_register(const char* command_arg)
    {
        char* my_command = make_command(command_arg, 1);

        //发送数据,没有发‘\0'
        int temp_len = send(sock, my_command, strlen(my_command + 2) + 2, 0);
        free(my_command);
        //接收服务器传回的数据
        char szBuffer[MAXBYTE] = { 0 };

        int all_len = 0;
        if (recv(sock, szBuffer, 1, 0) < 1)
        {
            std::cout << "False!!!" << std::endl;
            return false;
        }
        all_len = static_cast<unsigned char>(szBuffer[0]);
        szBuffer[0] = 0;

        int now_len = 0;

        do {
            temp_len = recv(sock, szBuffer+now_len, MAXBYTE, NULL);
            now_len += temp_len;
        } while (now_len < all_len);

        return strcmp(szBuffer,"false")==0?false:true;
    }

    bool upload_file(const char* file_path, const char* vir_upper_dic)
    {
        return true;
    }

    std::vector<int> my_login(const char* command_arg)
    {
        char* my_command = make_command(command_arg, 2);

        //发送数据,没有发‘\0'
        int temp_len = send(sock, my_command, strlen(my_command + 2) + 2, 0);
        free(my_command);
        //接收服务器传回的数据
        char szBuffer[MAXBYTE] = { 0 };

        int all_len = 0;
        if (recv(sock, szBuffer, 1, 0) < 1)
        {
            std::vector<int> result({ 4,0 });
            return result;
        }
        all_len = static_cast<unsigned char>(szBuffer[0]);
        szBuffer[0] = 0;

        int now_len = 0;

        do {
            temp_len = recv(sock, szBuffer + now_len, MAXBYTE, NULL);
            now_len += temp_len;
        } while (now_len < all_len);

        std::vector<std::string> temp_s = split(std::string(szBuffer), " ");
        std::vector<int> result;
        result.push_back(std::stoi(temp_s[0]));
        result.push_back(std::stoi(temp_s[1]));
        return result;   
    }
};



int main(void)
{
    ClientSendReceive* my_test = new ClientSendReceive();

    /*std::vector<int> result = my_test->my_login("ya\n1234567");
    std::cout << result[1] << std::endl;*/

    //bool result = my_test->my_register("zhangdsdsaffdfd\n1234567");
    //std::cout << result << std::endl;

   //std::string result =  my_test->run_sql("select * from id_pwd");
   //std::cout << result << std::endl;

    //bool result = my_test->upload_file("/test.txt", "/");
    //std::cout << result << std::endl;
    delete my_test;
}
