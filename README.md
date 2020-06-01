用c++实现的，服务器（linux）与客户端（windows）之间TCP简易传输的代码。
* * *
确保服务器安装了mysql，server code内的代码放到一个文件夹下,然后编译。
编译指令如下，要注意的是根据mysql lib的位置修改"-L"后的这个路径。
```
g++ -L/www/server/mysql/lib -lmysqlclient -o my_server main.cpp send_receive.cpp
```
-----

#### 客户端函数的返回值：

注册:	

​			my_register(“my_username/n1234567”);

​			返回值： std::string

​			| 情况 | 返回值 |

​			| ---- | ---- |

​			| 失败 | false |

​			| 连接失败 | false! |

​			| 成功 | id (注意是string类型,比如“1”) |


