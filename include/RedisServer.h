#include "../include/RedisDatabase.h"
#include "./RedisCmd.h"
#include <string>
#include <sys/epoll.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <cstring>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>

using namespace std;

#define MAX_EVENTS 1024
#define BF_SIZE 1024

class RedisServer
{
    public:
        RedisServer(int port);
        void Start();
        void ShutDown();

    private:
        int port; 
        int EpollFd;
        int ServerSocket, ClientSocket;
        bool running;
        // making fd's non blocking for epoll
        int EnableNonBlocking(int); 

        // setup signal handling for graceful shutdown (ctrl + c)
        void SetupSignalHandler();
};
