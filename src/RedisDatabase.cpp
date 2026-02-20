#include "../include/RedisDatabase.h"

RedisDatabase& RedisDatabase :: getInstance()
{
    static RedisDatabase instance;
    return instance;
}

//Common Commands
bool RedisDatabase :: FlushAll()
{
    kv_map.clear();
    return true;
}

//Key/Value commands
void RedisDatabase :: Set(const string& key, const string& value)
{
    kv_map[key] = value;
}

bool RedisDatabase :: Get(const string& key, string& value)
{
    auto it = kv_map.find(key);
    if(it != kv_map.end())
    {
        value = it->second;
        return true;
    }
    return false;
}

vector<string> RedisDatabase :: Keys()
{
    vector<string> result;

    for(const auto& pair : kv_map)
    {
        result.push_back(pair.first);
    }

    return result;
}

string RedisDatabase :: Type(const string& key)
{
    if(kv_map.find(key) != kv_map.end())
    {
        return "string";
    }

    return "none";

}

bool RedisDatabase :: UNLINK(const string& key)
{
    bool erased = false;

    if(kv_map.erase(key) > 0)
    {
        erased = true;
    }

    return erased;
}

bool RedisDatabase :: Rename(const string& oldKey, const string& newKey)
{
    
    bool found = false;
    
    auto itKv = kv_map.find(oldKey);
    if(itKv != kv_map.end())
    {
        kv_map[newKey] = itKv->second;
        kv_map.erase(itKv);
        found = true;
    }
    return found;
}

bool RedisDatabase :: Save(const string& filename)
{
    ofstream ofs(filename, ios::binary);
    if(!ofs)
    {
        return false;
    }

    for (const auto& kv : kv_map)
    {
        ofs << "K " << kv.first << " " << kv.second << "\n";
    }

    return true;
}

bool RedisDatabase::Load(const string& filename)
{
    ifstream ifs(filename, ios::binary);
    if(!ifs)
    {
        return false;
    }

    kv_map.clear();

    string line;

    while(getline(ifs, line))
    {
        istringstream iss(line);
        char type;
        iss >> type ;
        if(type == 'K')
        {
            string key , value;
            iss >> key >> value;
            kv_map[key] = value;
        }

    }
    return true;
}