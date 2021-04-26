//
// Created by rei on 18/04/21.
//

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

#define PORT 8080
#define SIZE 1024
#define MAXCLIENT 5
#define MAXFAILEDLOGIN 5

int MessageNumber = 1;
bool isServerOpen = true;
bool isClientLogIn = false;
bool serverIsClose = false;

map<string, int> authorisedClients;

class UserInfo{
public:
    string userName;
    string password;
    int attempts;
    time_t loginTime;
    bool failedLog;
    bool connectFlag;
    UserInfo(string, string, int, time_t, bool, bool);
    ~UserInfo() = default;
};

vector<UserInfo> userInfo;

class ListeningServer{
private:
    string __serverIP;
    int __serverPort;
    char * __startTime;
    int __threadId;
public:
    mutex * TCPClock;
    ListeningServer(int, time_t, const string&, const int&, mutex *);
    ~ListeningServer() = default;
    void run();
protected:
    void static _readCredentials();
    void static _readMessageLog();
};

class UserServer{
private:
    struct sockaddr_in * __server;
    struct sockaddr_in * __client;
    string * __startTime;
    int __threadId;
    int __clientID;
    char __buffer [SIZE] = {0};
    bool __isLogIn = false;
    bool __isOpen = true;
public:
    mutex * TCPClock;
    UserServer(int, int, struct sockaddr_in *, struct sockaddr_in *, mutex*);
    ~UserServer() = default;
    void run();
protected:
    int _authentication();
    void _out(const int &);
    void _msg(const int &, const int &);
    void _dlt();
    void _edt();
    void _rdm(const int &, const int &);
};
void listeningSocket(int, time_t, mutex *);
void userSocket(int, int, struct sockaddr_in *, struct sockaddr_in *, mutex *);
#endif //TCP_SERVER_H
