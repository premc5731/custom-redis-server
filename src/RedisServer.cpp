#include "../include/RedisServer.h"

static RedisServer *Server = NULL;

// Make socket non-blocking
int RedisServer :: EnableNonBlocking(int fd) 
{
    int flags = 0, iRet = 0;
    flags = fcntl(fd, F_GETFL, 0);
    
    if (flags == -1) 
    {
        cerr << "ERROR: Unable to get flags of client\n";
        return -1;
    }

    iRet = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (iRet == -1) 
    {
        cerr << "ERROR: Unable to set make client nonblocking\n";
        return -1;
    }

    return iRet;
}

RedisServer :: RedisServer(int port)
{
    this->port = port;
    this->ServerSocket = -1;
    this->running = true;
    Server = this;
    SetupSignalHandler();
}

void SignalHandler(int iSigNum)
{
    if(Server)
    {
        cout << "\nCaught signal : " << iSigNum << "\nRedis Server is Terminating...\n";
        Server->ShutDown();
        exit(iSigNum);
    }
}

void RedisServer :: SetupSignalHandler()
{
    signal(SIGINT, SignalHandler);
}

void RedisServer :: ShutDown()
{
    running = false;

    if (ServerSocket != -1)
    {
        // Before ShutDown, save the data in to database
        if (RedisDatabase::getInstance().Save("Redisdb.bin"))
        {
            cout << "Data Saved to Redisdb.bin\n";
        }
        else
        {
            cerr << "Error in saving database\n";
        }
        close(ServerSocket);
    }

    cout << "Server Terminated\n";
}

void RedisServer :: Start()
{
    int reuse = 1, iRet = 0, Nfds = 0, i = 0;
    RedisCmd RCmd;
    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClientAddr;
    struct epoll_event Event, ClientEvents[MAX_EVENTS];
    socklen_t AddrLen = sizeof(ClientAddr);

    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ServerSocket < 0)
    {
        cerr << "ERROR: creating server socket\n";
        return;
    }

    // making server socket non blocking
    EnableNonBlocking(ServerSocket);

    // setting up port to reuse immediately after ShutDown
    setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // configure server ip address and port no
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(this->port);
    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    // bind server ip address and port no
    iRet = bind(ServerSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr));
    if(iRet == -1)
    {
        cerr << "ERROR: Binding Server Socket\n";
        return;
    }

    iRet = listen(ServerSocket, 11);

    if(iRet < 0)
    {
        cerr << "ERROR: Listening on Server Socket\n";
        return;
    }

    EpollFd = epoll_create1(0);
    if(EpollFd < 0)
    {
        cerr << "ERROR: Unable to create epoll instance\n";
        return;
    }

    // configure server socket for epoll list
    Event.events = EPOLLIN;   // socket is listener
    Event.data.fd = ServerSocket;

    epoll_ctl(EpollFd, EPOLL_CTL_ADD, ServerSocket, &Event);

    cout << "Redis Server Listening on port " << port << "\n";



    while(running)
    {
        Nfds = epoll_wait(EpollFd, ClientEvents, MAX_EVENTS, -1);

        for(i = 0; i < Nfds; i++)
        {
            // client triggered the listening(server) socket waiting for new conn
            if(ClientEvents[i].data.fd == ServerSocket)
            {
                ClientSocket = accept(ServerSocket, (sockaddr *)&ClientAddr, &AddrLen);

                if(ClientSocket < 0)
                {
                    cerr << "ERROR: Unable to accept client please try again\n";
                    break;
                }

                EnableNonBlocking(ClientSocket);

                Event.events = EPOLLIN | EPOLLET;
                Event.data.fd = ClientSocket;

                epoll_ctl(EpollFd, EPOLL_CTL_ADD, ClientSocket, &Event);

                cout << "New client connected with ip address : " << inet_ntoa(ClientAddr.sin_addr) << "\n";
            }
            // client sent command
            else
            {
                ClientSocket = ClientEvents[i].data.fd;
                char buffer[1024];

                while(true)
                {
                    memset(buffer, 0, sizeof(buffer));
                    iRet = recv(ClientSocket, buffer, sizeof(buffer) - 1, 0);

                    if(iRet == -1)
                    {
                        if (errno != EAGAIN) 
                        {
                            cout << "ERROR: " << strerror(errno) << "\n";
                            close(ClientSocket);
                        }
                        break;
                    }


                    if (iRet == 0) {
                        // client disconnected
                        printf("Client with fd %d disconnected\n", ClientSocket);
                        close(ClientSocket);
                        break;
                    }

                    string request(buffer, iRet);
                    string response = RCmd.CommandProcessor(request);
                    send(ClientSocket, response.c_str(), response.size(), 0);
                }

            }
        }
    }

    close(ServerSocket);

}