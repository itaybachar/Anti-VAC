//
// Created by itayb on 3/16/2020.
//

#include "ProcessManager.h"

#include <windows.h>


ProcessManager::ProcessManager(const char * name):
    name(name),
    id(0),
    handle(INVALID_HANDLE_VALUE),
    attached(false)
    {}

ProcessManager::~ProcessManager() {
    this->detach();
}

void ProcessManager::read(uintptr_t add, void *buff,size_t size) {
    ReadProcessMemory(handle,(void*)add,buff,size,nullptr);
}

void ProcessManager::write(uintptr_t add, void *buff,size_t size) {
    WriteProcessMemory(handle,(void*)add,buff,size,nullptr);
}

bool ProcessManager::wait() {
    while(!isRunning()){
        if(GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(0x51))
            return false;
        Sleep(500);//Wait half a second before checking for program again
    }
    return attach();
}

bool ProcessManager::isRunning() {
    if(attached){
       DWORD exitCode;
       GetExitCodeProcess(handle,&exitCode);
       return exitCode == STILL_ACTIVE;
    }else return findProcess();
}

bool ProcessManager::attach() {
    if(!attached){
        if(id == 0) isRunning();

        handle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,id);
        if(handle == NULL){
            puts("ERROR: Bad Process ID");
            exit(2);
        }
        attached = true;
        return true;
    }else{
        puts("ERROR: Process is already attached");
        return false;
    }
}

bool ProcessManager::detach() {
    if (attached) {
        CloseHandle(handle);
        attached = false;
        return true;
    } else {
        puts("ERROR: Process is already detached!");
        return false;
    }
}

uintptr_t ProcessManager::getModule(const char *modName) {
    MODULEENTRY32 modEntry;
    if(findModule(modName,modEntry)){
        return (uintptr_t)modEntry.modBaseAddr;
    }
    return 0;
}

uintptr_t ProcessManager::waitModule(const char *modName) {
    uintptr_t mod = 0;
    while(!(mod = getModule(modName))){
        Sleep(1000);
    }
    return mod;
}

bool ProcessManager::findProcess() {
    PROCESSENTRY32 pEntry;

    ZeroMemory(&pEntry,sizeof(pEntry));
    pEntry.dwSize = sizeof(PROCESSENTRY32);

    //Get a snapshot of all processes and loop through the entries to find
    //a process with a matching name and return its id
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hSnap && Process32Next(hSnap,&pEntry)){
        do{
            if(strcmp(pEntry.szExeFile,this->name.c_str()) == 0){
                this->id = pEntry.th32ProcessID;
                CloseHandle(hSnap);
                return true;
            }
        }while(Process32Next(hSnap,&pEntry));
    }
    return false;
}

bool ProcessManager::findModule(const char * modName, MODULEENTRY32& modEntry) {
    MODULEENTRY32 mEntry;

    ZeroMemory(&mEntry, sizeof(mEntry));
    mEntry.dwSize = sizeof(MODULEENTRY32);
    //Get Snapshot of all modules in a process and loop through entries to find
    // a module with a matching name and return the Module Struct
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->id);
    if (hSnap && Module32Next(hSnap, &mEntry)) {
        do {
            if (strcmp(mEntry.szModule, modName) == 0) {
                modEntry = mEntry;
                CloseHandle(hSnap);
                return true;
            }
        } while (Module32Next(hSnap, &mEntry));
    }
    return false;
}
