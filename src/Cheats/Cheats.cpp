//
// Created by itayb on 3/16/2020.
//

#include <thread>
#include <iostream>

#include "Cheats.h"
#include "ProcessManager.h"
#include "HazeDumper.h"

#define VAR2STR(Variable) (#Variable)
#define FL_ONGROUND (1<<0)

Cheats::Cheats(ProcessManager *proc, HazeDumper * dumper):
    proc(proc),
    m_client(proc->waitModule("client_panorama.dll")),
    m_engine(proc->waitModule("engine.dll")),
    m_localPlayer(0),
    m_localTeam(0),

    //Variables
    dwLocalPlayer(dumper->getPointer(VAR2STR(dwLocalPlayer))),
    dwEntityList(dumper->getPointer(VAR2STR(dwEntityList))),
    dwForceJump(dumper->getPointer(VAR2STR(dwForceJump))),
    dwForceAttack(dumper->getPointer(VAR2STR(dwForceAttack))),
    dwGlowObjectManager(dumper->getPointer(VAR2STR(dwGlowObjectManager))),
    m_flFlashDuration(dumper->getPointer(VAR2STR(m_flFlashDuration))),
    m_fFlags(dumper->getPointer(VAR2STR(m_fFlags))),
    m_iTeamNum(dumper->getPointer(VAR2STR(m_iTeamNum))),
    m_iCrosshairId(dumper->getPointer(VAR2STR(m_iCrosshairId))),
    m_iGlowIndex(dumper->getPointer(VAR2STR(m_iGlowIndex))),

    //Flags
    glowEnabled(false),
    triggerEnabled(false),
    noFlashEnabled(false)
{
    delete dumper;
}

Cheats::~Cheats() {
    delete proc;
}

void Cheats::run() {
    std::thread* bhop,*glow,*tbot,*noflash;
    bool init = false;
    while(proc->isRunning() && !(GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(0x51))){ //rt Alt and Q to quit
        //Local Player
        proc->read(m_client+dwLocalPlayer,&m_localPlayer,sizeof(DWORD));
        //Local Team
        proc->read(m_localPlayer+m_iTeamNum,&m_localTeam,sizeof(DWORD));

        if(GetAsyncKeyState(VK_F1) & 1) {
            glowEnabled = !glowEnabled;
        }

        if(GetAsyncKeyState(VK_F2) & 1) {
            triggerEnabled = !triggerEnabled;
        }

        if(GetAsyncKeyState(VK_F3) & 1) {
            noFlashEnabled = !noFlashEnabled;
        }

        if(!init){
            bhop = new std::thread(&Cheats::bunnyHop,this);
            glow = new std::thread(&Cheats::glow,this);
            noflash = new std::thread(&Cheats::noFlash,this);
            //tbot = new std::thread(&Cheats::triggerBot,this);
            init = true;
        }
        Sleep(1000);
    }

    delete bhop;
    delete glow;
    delete tbot;
    delete noflash;
}

void Cheats::bunnyHop() {
    while (true) {
        uint32_t flag;
        uint8_t val = 6;
        if (GetAsyncKeyState(VK_SPACE)) {
            proc->read(m_localPlayer + m_fFlags, &flag,sizeof(uint32_t));
            if (flag & FL_ONGROUND)
                proc->write(m_client + dwForceJump, &val,sizeof(uint8_t));
            Sleep(5);
        }
    }
}

void Cheats::glow() {
    while (true) {
        if (!glowEnabled) {
            Sleep(500);
            continue;
        }
        DWORD glowmngr;
        //Glow Manager
        proc->read(m_client + dwGlowObjectManager, &glowmngr,sizeof(DWORD));

        //Loop through entities
        for (uint32_t i = 0; i < 16; i++) {
            DWORD entity;
            //Get entity
            proc->read(m_client + dwEntityList + i * 0x10, &entity,sizeof(DWORD));

            if (entity > 0 && entity != m_localPlayer) {
                uint32_t team;
                DWORD glowIndex;
                //Get the team
                proc->read(entity + m_iTeamNum, &team,sizeof(uint32_t));
                proc->read(entity + m_iGlowIndex, &glowIndex,sizeof(DWORD));
                float r, g, b, a;
                if (team == m_localTeam) {
                    r = 0;
                    g = 0.5;
                    b = 0;
                    a = 0.9;
                } else {
                    r = 0.5;
                    g = 0;
                    b = 0;
                    a = 0.9;
                }
                bool on = true;
                bool off = false;
                DWORD offset = glowmngr + (glowIndex * 0x38);
                proc->write(offset + 0x4, &r,sizeof(float));
                proc->write(offset + 0x8, &g,sizeof(float));
                proc->write(offset + 0xC, &b,sizeof(float));
                proc->write(offset + 0x10, &a,sizeof(float));

                proc->write(offset + 0x24, &on,sizeof(bool));
                proc->write(offset + 0x25, &off,sizeof(bool));
            }
        }
        Sleep(5);
    }
}

void Cheats::triggerBot() {

}

void Cheats::noFlash() {
    while(true){
        if (!noFlashEnabled) {
            Sleep(500);
            continue;
        }
        uint32_t flashDuration = 0;
        float val = 0;
        proc->read(m_localPlayer + m_flFlashDuration,&flashDuration,sizeof(uint32_t));
        if(flashDuration>0)
            proc->write(m_localPlayer + m_flFlashDuration,&val,sizeof(float));
    }
}
