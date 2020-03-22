//
// Created by itayb on 3/16/2020.
//
#pragma once

#include <windows.h>
#include <string>
#include <tlhelp32.h>

class ProcessManager {
private:
    std::string name;
    DWORD id;
    HANDLE handle;
    bool attached;
public:
    ProcessManager(const char *name);
    ~ProcessManager();

public:
    void read(uintptr_t add, void *buff, size_t size);
    void write(uintptr_t add, void *buff, size_t size);

public:
    bool wait(); //Wait for program to open in order to attach
    bool isRunning();
    bool attach();
    bool detach();

public:
    uintptr_t getModule(const char *modName);
    uintptr_t waitModule(const char *modName);

private:
    bool findProcess();
    bool findModule(const char *modName, MODULEENTRY32& modEntry);
};
