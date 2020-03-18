//
// Created by itayb on 3/16/2020.
//
#pragma once

#include <unordered_map>
#include <iosfwd>


class HazeDumper {
private:
    std::unordered_map<std::string,uint32_t> gameVars;
    std::string curLine;
    const char * dumpFile;

private:
    void skipLine(std::ifstream * s,uint32_t count);
    void addItem();
    void fetchFile();
    bool readDump();
    
public:
    HazeDumper(const char * fileName = "csgo.json");
    ~HazeDumper();
    uint32_t getPointer(const char * varName);

};
