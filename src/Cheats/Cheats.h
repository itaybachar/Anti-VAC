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
    DWORD dwClientState;
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
    static constexpr uint8_t ENTITY_COUNT = 32;

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
    void noRecoil();
    void printStatus();
    void getBoneLocation(DWORD* boneMatrix, uint8_t bId, float* out);
    void smoothAim(float* from, float* to, uint8_t steps, DWORD& clientState);
    float getDistance3D(float* from, float* to, float* deltaOut);
    float getDistance2D(float* from, float* to);
    std::tuple<float,float> calcAngles(float* from, float* to);
};