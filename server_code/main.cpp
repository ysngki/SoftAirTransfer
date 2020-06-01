#include <iostream>
#include "send_receive.h"

int main(int argc, char** argv)
{
    ListenHandle *my_listen = new ListenHandle();
    //std::string result = my_listen.run_statement(std::string("INSERT INTO user_mode_right VALUES(100,0)"));
    my_listen->begin_listen();
    
    delete my_listen;
}


