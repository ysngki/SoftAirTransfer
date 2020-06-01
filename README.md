用c++实现的，服务器（linux）与客户端（windows）之间TCP简易传输的代码
* * *
确保服务器安装了mysql，server code内的代码放到一个文件夹下,然后编译，指令如下:
```
g++ -L/www/server/mysql/lib -lmysqlclient -o my_server main.cpp send_receive.cpp
```

