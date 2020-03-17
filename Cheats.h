//
// Created by itayb on 3/16/2020.
//
#pragma once

#include <cinttypes>
class ProcessManager;
class HazeDumper;

class Cheats {
private:
    ProcessManager* proc;
    uintptr_t m_client;
    uintptr_t m_engine;
    uint32_t m_localPlayer;
    uint32_t m_localTeam;
private:
    uintptr_t dwLocalPlayer;
    uintptr_t dwEntityList;
    uintptr_t dwForceJump;
    uintptr_t dwForceAttack;
    uintptr_t dwGlowObjectManager;
    uintptr_t m_flFlashDuration;
    uintptr_t m_fFlags;
    uintptr_t m_iTeamNum;
    uintptr_t m_iCrosshairId;
private:
    bool glowEnabled;
    bool triggerEnabled;

public:
    Cheats(ProcessManager *proc,HazeDumper * dumper);
    ~Cheats();
    void run();
private:
    void bunnyHop();
    void glow();
    void triggerBot();
    void noSmoke();

};