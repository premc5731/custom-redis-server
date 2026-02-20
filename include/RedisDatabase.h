#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;

#pragma once

class RedisDatabase
{
    public:
        //Get the singelton instance
        static RedisDatabase& getInstance();

        //Common Commands
        bool FlushAll();

        //Key/Value commands
        void Set(const string& key, const string& value);
        bool Get(const string& key, string& value);
        vector<string> Keys();
        string Type(const string& key);
        bool UNLINK(const string& key);
        bool Rename(const string& oldKey, const string& newKey);

        // Persistence: Dump / load databse from a file 
        bool Save(const string& filename);
        bool Load(const string& filename);

    private:
        RedisDatabase() = default;
        ~RedisDatabase() = default;

        //This deletes the copy constructor.
        RedisDatabase(const RedisDatabase&) = delete;

        // This deletes the copy assignment operator.
        RedisDatabase& operator = (const RedisDatabase&) = delete;

        unordered_map <string, string > kv_map;

};
