#include "../include/RedisServer.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
using namespace std;

int main(int argc, char * argv[])
{
    int port = 6379;   
 
    if(RedisDatabase::getInstance().Load("Redisdb.bin"))
    {
        cout << "Database Loaded From Redisdb.bin\n";
    }
    else
    {
        cout << "Starting with an empty database Redisdb.bin\n";
    }

    RedisServer server(port);

    //background persistence : dump the database every 300 seconds

    thread persistenceThread([]()
    {
        while(true)
        {
            this_thread::sleep_for(chrono::seconds(300));
            // dump the database
            if(!RedisDatabase::getInstance().Save("Redisdb.bin"))
            {
                cerr << "ERROR: Saving Database\n";
            }
            else
            {
                cout << "Data Saved to Redisdb.bin\n";
            }
        }
    });

    persistenceThread.detach();

    server.Start();
    return 0;
}
