//
// Created by rei on 18/04/21.
//

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

using namespace std;

#define PORT 3331
#define SIZE 1024

bool loginCheck(bool, char *, const int &, string *, string *);

#endif //TCP_CLIENT_H
