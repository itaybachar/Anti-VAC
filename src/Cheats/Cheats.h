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
    DWORD dwClientState_GetLocalPlayer;
    DWORD dwClientState_ViewAngles;
    DWORD dwEntityList;
    DWORD dwForceJump;
    DWORD dwForceAttack;
    DWORD dwGlowObjectManager;
    DWORD m_flFlashDuration;
    DWORD m_fFlags;
    DWORD m_iTeamNum;
    DWORD m_iCrosshairId;
    DWORD m_iGlowIndex;
    DWORD m_vecOrigin;
    DWORD m_vecViewOffset;
    DWORD m_iHealth;
    DWORD m_lifeState;
    DWORD m_bDormant;
    DWORD m_bSpottedByMask;
    DWORD m_dwBoneMatrix;


    
private:
    bool glowEnabled;
    bool triggerEnabled;
    bool noFlashEnabled;
    bool aimbotEnabled;
    bool kill;

public:
    Cheats(ProcessManager *proc,HazeDumper * dumper);
    ~Cheats();
    void run();
private:
    void bunnyHop();
    void glow();
    void triggerBot();
    void aimBot();
    void noFlash();
    void printStatus();
    std::tuple<float,float> calcAngles(float* from, float* to);
    void getBoneLocation(DWORD* boneMatrix, uint8_t bId, float* out);

};