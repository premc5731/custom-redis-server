#include "../include/RedisCmd.h"

string RedisCmd::CommandProcessor(const string &Command)
{
    // use RESP parser
    RedisParser RP;
    string Cmd;
    vector<string> tokens = RP.Parser(Command);
    ostringstream Response;

    if(tokens.empty())
    {
        return "ERROR: Empty command\r\n";
    }
    
    Cmd = tokens[0];

    // making first string(command) to upper case
    transform(Cmd.begin(), Cmd.end(), Cmd.begin(), :: toupper);

    // connect to database 
    RedisDatabase& Rdb = RedisDatabase::getInstance();

    // Common Commands
    if(Cmd == "PING")
    {
        Response << "+PONG\r\n";
    }
    else if(Cmd == "ECHO")
    {
        if(tokens.size() != 2)
        {
            Response << "-ERR ECHO requires a message\r\n";
        }
        else
        {
            Response << "+" << tokens[1] << "\r\n";
        }
    }
    else if(Cmd == "COMMAND")
    {
  
        Response << "*0\r\n";
        
    }
    else if(Cmd == "FLUSHALL")
    {
        if(tokens.size() != 1)
        {
            Response << "-ERR Invalid FLUSHALL command\r\n";
        }
        else
        {
            Rdb.FlushAll();
            Response << "+OK\r\n";
        }
    }

    // Key/Value operations
    else if(Cmd == "SET")
    {
        if(tokens.size() != 3)
        {
            Response << "-ERR SET requires key and value\r\n";
        }
        else
        {
            Rdb.Set(tokens[1], tokens[2]);
            Response << "+OK\r\n";
        }
    }
    else if(Cmd == "GET")
    {
       if(tokens.size() != 2)
        {
            Response << "-ERR GET requires key\r\n";
        }
        else
        {
            string value;
            if(Rdb.Get(tokens[1], value))
            {
                Response << "$" << value.size() << "\r\n" << value << "\r\n";
            }
            else
            {
                Response << "$-1\r\n";
            }
        } 
    }
    else if (Cmd == "KEYS")
    {
        vector<string> allKeys = Rdb.Keys();
        Response << "*" << allKeys.size() << "\r\n";
        for(const auto& key : allKeys)
        {
            Response << "$" << key.size() << "\r\n" << key << "\r\n";
        }
    }
    else if(Cmd == "TYPE")
    {
        if(tokens.size() != 2)
        {
            Response << "-ERR TYPE requires key\r\n";
        }
        else
        {
            Response << "+" << Rdb.Type(tokens[1]) << "\r\n";
        }
    }
    else if(Cmd == "DEL" || Cmd == "UNLINK")
    {
        if(tokens.size() != 2)
        {
            Response << "-ERR "<< Cmd << " requires key\r\n";
        }
        else
        {
            bool res = Rdb.UNLINK(tokens[1]);
            Response << ":" << (res ? 1: 0) << "\r\n";
        }
    }
    else if(Cmd == "RENAME")
    {
        if(tokens.size() != 3)
        {
           Response << "-ERR EXPIRE requires old key name and new key name\r\n"; 
        }
        else
        {
            if(Rdb.Rename(tokens[1], tokens[2]))
            {
                Response << "+OK\r\n";   
            }
        } 
    } 
    else
    {
        return "-ERR unknown command\r\n";
    }

    return Response.str();

}