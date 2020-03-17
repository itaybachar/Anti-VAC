//
// Created by itayb on 3/16/2020.
//

#include "HazeDumper.h"
#include <fstream>
#include <iostream>
#include <string>

HazeDumper::HazeDumper(const char * fileName):
dumpFile(fileName){
    readDump();
}

HazeDumper::~HazeDumper() {}

void HazeDumper::readDump() {
    std::ifstream stream(this->dumpFile);

    if (stream.is_open()) {
        //Ignore nonsense
        skipLine(&stream, 3);

        while (std::getline(stream, curLine)) {
            if (curLine == "  },")
                skipLine(&stream, 2);
            if (curLine == "  }")
                break;
            addItem();
        }
    } else {
        puts("Couldn't Find Dump File");
        system("pause");
        exit(3);
    }
}

void HazeDumper::addItem(){
    uint32_t begOffset = 5;
    uint32_t nameEnd = curLine.find_last_of('\"');
    std::string name = curLine.substr(begOffset,nameEnd-begOffset);
    int value = std::stoi(curLine.substr(nameEnd+3,curLine.length()-(nameEnd+4)));
    gameVars[name] = value;
}

void HazeDumper::skipLine(std::ifstream * s,uint32_t count){
    for(uint32_t i = 0; i<count;i++)
        std::getline(*s,this->curLine);
}

uint32_t HazeDumper::getPointer(const char * varName) {
    auto pair = gameVars.find(varName);
    if(pair == gameVars.end()) {
        puts("Could not find Key");
        system("pause");
        exit(4);
    }
    return gameVars.find(varName)->second;
}


