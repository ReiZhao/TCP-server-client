# TCP-server-client
Sample TCP server &amp; TCP client in C++

## TCP server (C++ 11)
Implmented based on socket, tested on Linux/Mac OS 
### Arguments:
*  Running on local host
*  using port 3331 (default)

### Functions:
*  User authentication
*  Receieve user message and save in messagelog.txt
*  send user messages from messagelog.txt
*  Edit / delete message in messagelog.txt (not finished)
*  Recording active users

## TCP client (C++ 11)
Implmented based on socket, tested on Linux/Mac OS
### Arguments:
*  Running on local host
*  using random port (default)

### Functions:
*  User authentication
*  Send & Recv message from TCP server

##  DEMO
### Server
  Running server.cpp (multi-thread version)
  
### Clent
  Running clent.cpp, user authentication info in credentials.txt
  
### Server Commands:
  close:  shut down main thread, stop program.
  
### Client Commands:
1.  MSG xxxx:   Post a message
2.  RDM 2:      Read all message start at the second message in recording
3.  OUT:        Stop connection
