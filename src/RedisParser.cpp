#include "../include/RedisParser.h"

using namespace std;

// RESP parser format:
// example : SET Name Rahul
// *3/r/n$3\r\nSET\r\n$4\r\nName\r\n$5\r\nRahul
// *3 => array has 3 elements
// $3 => array[0] has 3 characters
// \r\n RESP seperater
// after parsing tokens array stores as
// SET Name Rahul

vector<string> RedisParser :: Parser(const string &input)
{
    size_t pos = 0, len = 0, crlf = 0;
    int iNo = 0;
    string token;
    vector<string> tokens;


    if (input.empty())
    {
        cerr << "ERROR: Invalid input\n";
        return tokens;
    }

    // if it doesnot start with '*' , then splitting by white space

    if(input[0] != '*') // not in RESP format
    {
        istringstream iss(input);
        while(iss >> token)
        {
            tokens.push_back(token);
        }

        return tokens;
    }

    // so here input[0] == "*";
    pos++; // skip '*'

    //crlf = carriage return (\r) Line feed (\n)
    crlf = input.find("\r\n", pos);

    if(crlf == string::npos)
    {
        return tokens;
    }

    iNo = stoi(input.substr(pos, crlf - pos));
    pos = crlf + 2;

    for(int i = 0; i < iNo; i++)
    {
        if(pos >= input.size() || input[pos] != '$')    //format error
        {
            cerr << "ERROR: Invalid input\n";
            break;
        }

        pos++;  //skip '$'

        crlf = input.find("\r\n", pos);
        if(crlf == string::npos)
        {
            break;
        }

        len = stoi(input.substr(pos, crlf - pos));
        pos = crlf + 2;

        if(pos + len > input.size())    //format error
        {
            break;
        }

        token = input.substr(pos, len);
        tokens.push_back(token);
        pos = pos + len + 2;    // skip token and crlf
        
    }

    return tokens;

}// this function is tokenising using RESP formatted string
