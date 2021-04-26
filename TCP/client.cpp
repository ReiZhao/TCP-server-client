//
// Created by rei on 18/04/20.
//
#include <vector>
#include "client.h"

bool loginCheck(bool isLogIn, char * buffer, const int &sock, string * userName, string * password){
    cout << "UserName: ";
    cin >> *userName;
    cout << "Password: ";
    cin >> *password;
    string loginMessage = "LogIn ";
    loginMessage += *userName + " " + *password;
    send(sock, loginMessage.c_str(), loginMessage.length(), 0);
    if(recv(sock, buffer, SIZE, 0) < 0){
        return false;
    }
    if(strncmp("Invalid", buffer, 7) == 0){
        cout << "Invalid Password. Please try again" << endl;
        return false;
    }
    if(strncmp("Protect", buffer, 7) == 0){
        cout << "Your account is blocked due to multiple login failures. Please try again later" << endl;
        return false;
    }
    if(strncmp("Welcome", buffer, 7) == 0) {
        cout << "Welcome to TOOM!" << endl;
        return true;
    }
    return false;
}

int main(int argc, char const *argv[])
{
    bool isLogIn = false;
    string UserName;
    string Password;
    bool isOpen = true;
    char buffer[SIZE];
    int sock = 0;
    struct sockaddr_in serv_addr {};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    // do authentication check

    while(!isLogIn){
        isLogIn = loginCheck(isLogIn, buffer, sock, &UserName, &Password);
    }
    char userCommand[300];
    cin.getline(userCommand, 300);
    while (isOpen){
        cout << "Enter one of the following commands (MSG, DLT, EDT, RDM, OUT): ";
        cin.getline(userCommand, 300);
        if (strncmp(userCommand, "OUT", 3) == 0){
            send(sock, "OUT", 3, 0);
            usleep(5000);
            isOpen = false;
            cout << "Goodbye " << UserName << endl;
        }
        else if (strncmp(userCommand, "MSG", 3) == 0){
            send(sock, userCommand, 300, 0);
            recv(sock, buffer, SIZE, 0);
            cout << buffer << endl;
        }
        else if (strncmp(userCommand, "RDM", 3) == 0){
            send(sock, userCommand, 300, 0);
            memset(buffer, 0, SIZE);
            recv(sock, buffer, SIZE, 0);
            while(strncmp(buffer, "End", 3) != 0){
                cout << buffer << endl;
                memset(buffer, 0, SIZE);
                recv(sock, buffer, SIZE, 0);
            }
        }
        usleep(100);
    }
    close(sock);
    return 0;
}
