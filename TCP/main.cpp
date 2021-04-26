#include <iostream>
#include <cstdlib>
#include <thread>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <map>
using namespace std;

#define NUM_THREADS     5

class Server{
private:
    char * startTime;
    int id;
public:
    Server(int threadId, time_t start){
        id = threadId;
        startTime = ctime(&start);
    }
    ~Server()= default;
    void printHello(){
        printf("Thread-%d: hello world! %s\n", id, startTime);
    }
};



void PrintHello(int thread_id)
{
    auto * serverThread = new Server(thread_id, time(nullptr));
    serverThread->printHello();
    delete serverThread;
}

int main ()
{
    /* multi-threading part */
//    vector <thread> threads;
//    for( int i=0; i < NUM_THREADS; i++ ){
//        cout << "main() : Thread " << i << " start." << endl;
//        threads.emplace_back(PrintHello, i);
//        switch (NUM_THREADS - i){
//            case 1:
//                threads[i].join();
//                break;
//            default:
//                threads[i].detach();
//        }
//    }

    /*read file part
     * cannot synchronously read & write one file-stream object */
//    ifstream file("messagelog.txt");
//    if (!file){
//        ofstream newFile;
//        newFile.open("messagelog.txt");
//        newFile << "hello world\n";
//        newFile.close();
//    }
//    else{
//        string content;
//        while(getline(file, content)){
//            cout << content;
//        }
//    }
//    file.close();
   /* Get local IP*/
//    char hostName[128];
//    gethostname(hostName, sizeof(hostName));
//    cout << hostName <<endl;
//    struct hostent *host = gethostbyname(hostName);
//    cout << host->h_name << endl;
//    const char* first_ip = inet_ntoa(*(struct in_addr*)(host->h_addr_list[0]));
//    cout << first_ip << endl;

/*STL map */
//    map<string, int> userInfo{};
//    userInfo["Hans"] = 100;
//    userInfo["yoda"] = 96;
//    map<string, int> ::const_iterator p;
//    for(p = userInfo.cbegin(); p != userInfo.cend(); p++){
//        cout << p->first << ": " << p->second << endl;
//    }

    cout << strncmp("LogIn", "LogIn Hans Jad", 5);
    return 0;
}