//
// Created by rei on 18/04/20.
//


#include "server.h"

#include <utility>

UserInfo::UserInfo(string userName, string userPassword, int attemptTimes, time_t startTime,
        bool failedLog, bool connectedFlag) {
    this->userName = move(userName);
    this->password = move(userPassword);
    this->attempts = attemptTimes;
    this->loginTime = startTime;
    this->failedLog = failedLog;
    this->connectFlag = connectedFlag;
}

void listeningSocket(int id, time_t startTime, mutex * clock){
    string IP = "127.0.0.1";
    int port = 3331;
    auto socket = new ListeningServer(id, startTime, IP, port, clock);
    socket->run();
    delete socket;
}

ListeningServer::ListeningServer(int id, time_t startTime, const string & IP, const int & port, mutex * clock) {
    __threadId = id;
    __startTime = ctime(&startTime);
    __serverIP = IP;
    __serverPort = port;
    TCPClock = clock;
    _readMessageLog();
    _readCredentials();
}

void ListeningServer::run() {
    int server_fd, new_socket;
    struct sockaddr_in listenSocket {};
    int addrlen = sizeof(struct sockaddr_in);
    bzero(&listenSocket, addrlen);
    string hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        return;
    }
    listenSocket.sin_family = AF_INET;
    listenSocket.sin_addr.s_addr = INADDR_ANY;
    listenSocket.sin_port = htons(__serverPort);
    // bind listen socket for to enable listening
    if (::bind(server_fd, (struct sockaddr *) &listenSocket,
             addrlen) < 0)
    {
        perror("bind failed");
        return;
    }
    if (listen(server_fd, MAXCLIENT) < 0)
    {
        perror("Initiate listening failed");
        return;
    }
    vector <struct sockaddr_in> clients;
    //vector<thread> serverSockets;
    while(isServerOpen){
        clients.emplace_back();
        if ((new_socket = accept(server_fd, (struct sockaddr *) &clients[-1],
                                 (socklen_t*)&addrlen))<0)
        {
            perror("accept failed");
            return;
        }
        thread t = thread(userSocket, ++__threadId, new_socket, &listenSocket, &clients[-1], TCPClock);
        t.detach();
    }
    close(server_fd);
    serverIsClose = true;
    cout << "Server shut down" << endl;
}

void ListeningServer:: _readMessageLog() {
    ifstream file("messagelog.txt");
    if (!file){
        ofstream newFile;
        newFile.open("messagelog.txt");
        newFile.close();
    }
    else{
        string content;
        while(getline(file, content))
            MessageNumber ++;
    }
    file.close();
}

void ListeningServer::_readCredentials() {
    string content;
    int pos;
    int count = 0;

    ifstream userAuthentication("credentials.txt");
    if (!userAuthentication){
        perror("No such a file credentials.txt");
        return;
    }

    //userLog.open(" cse_usrlog.txt", ios::out | ios::trunc );
    while(getline(userAuthentication, content)){
        pos = content.find(' ');
        assert(pos > 0);
        string userName = content.substr(0, pos);
        string userPassword = content.substr(pos + 1);
        // #1 ; Hans; Jedis*12
        userInfo.emplace_back(userName, userPassword, 0, time(nullptr), false, false);
        // map[Hans] = 1
        authorisedClients[userName] = count++;
    }
    userAuthentication.close();
//    vector<UserInfo>::iterator it;
//    for(it = userInfo.begin(); it != userInfo.end(); it ++){
//        cout << it->userName << " " << it->password << endl;
//    }
}

UserServer::UserServer(int id, int socketid, struct sockaddr_in * server, struct sockaddr_in * client, mutex * clock) {
    __threadId = id;
    __clientID = socketid;
    time_t start = time(nullptr);
    __startTime = reinterpret_cast<string *> (ctime(&start));
    __server = server;
    __client = client;
    TCPClock = clock;
}

void UserServer::run(){
    int userID = -1;
    while(!__isLogIn && __isOpen){
        userID = _authentication();
    }
    while(__isOpen){
        if(recv(__clientID , __buffer, SIZE, 0) == 0){
            break;
        }
        if (strncmp("OUT", __buffer, 3) == 0){
            _out(userID);
        }
        else if(strncmp("MSG", __buffer, 3) == 0){
            _msg(userID, __clientID);
        }
        else if(strncmp("RDM", __buffer, 3) == 0){
            _rdm(userID, __clientID);
        }
        else if(strncmp("EDT", __buffer, 3) == 0){
        }
        else if(strncmp("DLT", __buffer, 3) == 0){
        }
    }
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &__client->sin_addr, str, sizeof(str));
    time_t closeTime = time(nullptr);
    printf("EVENT: %s (%s: %hu) log out at : %s\n", userInfo[userID].userName.c_str(), str,
           ntohs(__client->sin_port), ctime(&closeTime));
    close(__clientID);
}

int UserServer::_authentication() {
    try {
        recv(__clientID , __buffer, SIZE, 0);
    }
    catch (exception * e){
        __isOpen = false;
        return -1;
    }

    while (strncmp("LogIn", __buffer, 5) != 0){
        send(__clientID, "LogIn", 5, 0);
        recv(__clientID , __buffer, SIZE, 0);
    }
    string userCommand = string(__buffer);
    int pos = userCommand.find(' ');
    userCommand.assign(userCommand, pos+1); // remove command header

    pos = userCommand.find(' ');
    string userName = userCommand.substr(0, pos);
    string password = userCommand.substr(pos);
    map<string,int>::iterator it;
    TCPClock->lock();
    it = authorisedClients.find(userName);
    // no such a username
    if(it == authorisedClients.end()){
        send(__clientID, "Invalid", 7, 0);
        TCPClock->unlock();
        return -1;
    }
    int user_id = it->second;
    // invalid password
    if(password != userInfo[user_id].password){
        if(userInfo[user_id].failedLog && time(nullptr) - userInfo[user_id].loginTime < 10){
            send(__clientID, "Protect", 7, 0);
            TCPClock->unlock();
            return -1;
        }
        if(userInfo[user_id].failedLog && time(nullptr) - userInfo[user_id].loginTime >= 10){
            userInfo[user_id].failedLog = false;
            send(__clientID, "Invalid", 7, 0);
            TCPClock->unlock();
            return -1;
        }
        if(userInfo[user_id].attempts > MAXFAILEDLOGIN){
            userInfo[user_id].attempts = 0;
            userInfo[user_id].failedLog = true;
            userInfo[user_id].loginTime = time(nullptr);
            send(__clientID, "Invalid", 7, 0);
            TCPClock->unlock();
            return -1;
        }
        userInfo[user_id].attempts++;
        userInfo[user_id].loginTime = time(nullptr);
        send(__clientID, "Invalid", 7, 0);
        TCPClock->unlock();
        return -1;
    }
    //valid password
    else{
        if(userInfo[user_id].failedLog && time(nullptr) - userInfo[user_id].loginTime < 10){
            send(__clientID, "Protect", 7, 0);
            TCPClock->unlock();
            return -1;
        }
        userInfo[user_id].attempts = 0;
        userInfo[user_id].failedLog = false;
        userInfo[user_id].loginTime = time(nullptr);
        userInfo[user_id].connectFlag = true;
    }
    TCPClock->unlock();
    send(__clientID, "Welcome", 7, 0);
    __isLogIn = true;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &__client->sin_addr, str, sizeof(str));
    printf("EVENT: %s (%s: %hu) log in at : %s\n", userName.c_str(), str,
            ntohs(__client->sin_port), ctime(&userInfo[user_id].loginTime));
    return user_id;
}

void UserServer::_out(const int & user_id) {
    __isOpen = false;
    TCPClock->lock();
    userInfo[user_id].connectFlag = false;
    TCPClock->unlock();
}

void UserServer::_msg(const int & user_id, const int & client_id) {
    string message = string(__buffer);
    message.assign(message, message.find(' '));
    TCPClock->lock();
    ofstream messageLog("messagelog.txt", ios::app);
    time_t current = time(nullptr);
    messageLog << "#" << MessageNumber <<"; " << userInfo[user_id].userName + ";" + message + "; " << ctime(&current);
    messageLog.close();
    TCPClock->unlock();

    string feedback = userInfo[user_id].userName + " posted MSG #" + to_string(MessageNumber) + " at ";
    feedback += ctime(&current);
    send(client_id, feedback.c_str(), feedback.length(), 0);
    cout << feedback;
    MessageNumber ++;

}

void UserServer::_rdm(const int & user_id, const int & client_id) {
    string message = string(__buffer);
    int message_id = stoi(message.assign(message, message.find(' ') + 1));
    cout << userInfo[user_id].userName + " issued RDM command: " << endl;
    TCPClock->lock();
    ifstream messageLog("messagelog.txt", ios::in);
    TCPClock->unlock();
    string content;
    int count = 1;
    while(getline(messageLog, content)){
        if(count >= message_id){
            send(client_id, content.c_str(), content.length(), 0);
            cout << content << endl;
            usleep(100);
        }
        count ++;
    }
    messageLog.close();
    send(client_id, "End", 3, 0);
}

void userSocket(int id, int socketid, struct sockaddr_in * server, struct sockaddr_in * client, mutex * clock){
    auto userServer = new UserServer(id, socketid, server, client, clock);
    userServer->run();
    delete userServer;
}

int main(int argc, char const *argv[])
{
    mutex systemClock;
    thread listeningThread(listeningSocket, 1, time(nullptr), &systemClock);
    listeningThread.detach(); // running as joinable thread
    while(true){
        cout << "To stop services, input\033[1;32m close\033[0m" <<endl;
        string userCommand;
        cin >> userCommand;
        if(userCommand == "close") {
            isServerOpen = false;
            sleep(1);  // 5,000 us == 5 ms  linux version
            break;
        }
    }
    return 0;
}
