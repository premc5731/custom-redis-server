#include "./RedisParser.h"
#include "./RedisDatabase.h"

#include <vector>
#include <sstream>
#include <iostream> 
#include <string>
#include <algorithm>  // for transform
#include <cctype>     // for toupper
using namespace std;

class RedisCmd
{
    public :
        RedisCmd() = default;
        
        // process a command from a client and return RESP formatted response
        string CommandProcessor(const string &commandLine);
};
