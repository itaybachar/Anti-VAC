//
// Created by itayb on 3/16/2020.
//
#pragma once

#include "windows.h"

class ProcessManager;
class HazeDumper;

class Cheats {
private:
    ProcessManager* proc;
    DWORD m_client;
    DWORD m_engine;
    DWORD m_localPlayer;
    DWORD m_localTeam;
private:
    DWORD dwLocalPlayer;
    DWORD dwEntityList;
    DWORD dwForceJump;
    DWORD dwForceAttack;
    DWORD dwGlowObjectManager;
    DWORD m_flFlashDuration;
    DWORD m_fFlags;
    DWORD m_iTeamNum;
    DWORD m_iCrosshairId;
    DWORD m_iGlowIndex;
private:
    bool glowEnabled;
    bool triggerEnabled;
    bool noFlashEnabled;

public:
    Cheats(ProcessManager *proc,HazeDumper * dumper);
    ~Cheats();
    void run();
private:
    void bunnyHop();
    void glow();
    void triggerBot();
    void noFlash();

};