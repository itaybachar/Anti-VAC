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

    //Flags
    glowEnabled(false),
    triggerEnabled(false)
{
    delete dumper;
}

Cheats::~Cheats() {
    delete proc;
}

void Cheats::run() {
    std::thread* bhop,*glow,*tbot;
    bool init = false;
    while(proc->isRunning() || (GetAsyncKeyState(VK_ESCAPE) && GetAsyncKeyState(VK_MENU))){
        //Local Player
        proc->read(m_client+dwLocalPlayer,&m_localPlayer);
        //Local Team
        proc->read(m_localPlayer+m_iTeamNum,&m_localTeam);

        if(GetAsyncKeyState(VK_F1) & 1)
            glowEnabled = !glowEnabled;

        if(!init){
            bhop = new std::thread(&Cheats::bunnyHop,this);
            glow = new std::thread(&Cheats::glow,this);
            init = true;
        }
        Sleep(1000);
    }
    delete bhop;
    delete glow;
    delete tbot;
}

void Cheats::bunnyHop() {
    while (true) {
        uint32_t flag;
        uint8_t val = 6;
        if (GetAsyncKeyState(VK_SPACE)) {
            proc->read(m_localPlayer + m_fFlags, &flag);

            if (flag & FL_ONGROUND)
                proc->write(m_client + dwForceJump, &val);

        }
    }
}

void Cheats::glow() {
    while (true) {
        if (!glowEnabled) {
            Sleep(500);
            continue;
        }
        uint32_t glowmngr, count;
        //Glow Manager
        proc->read(m_client + dwGlowObjectManager, &glowmngr);
        //Entity Count
        proc->read(m_client + dwGlowObjectManager + sizeof(uint32_t), &count);

        //Loop through entities
        for (uint32_t i = 0; i < count; i++) {
            uint32_t entity;
            //Get entity
            proc->read(glowmngr + i * 0x38, &entity);

            if (entity != m_localPlayer) {
                uint32_t team;
                //Get the team
                proc->read(entity + m_iTeamNum, &team);
                float r, g, b, a;
                if (team == m_localTeam) {
                    r = 0;
                    g = 2;
                    b = 0;
                    a = 1.7;
                } else {
                    r = 2;
                    g = 0;
                    b = 0;
                    a = 1.7;
                }
                uint32_t offset = glowmngr + i * 0x38;
                uint8_t on = 1;

                proc->write(offset + 0x4, &r);
                proc->write(offset + 0x8, &g);
                proc->write(offset + 0xC, &b);
                proc->write(offset + 0x10, &a);

                proc->write(offset + 0x36, &on);

            }
        }
        Sleep(5);
    }
}

void Cheats::triggerBot() {

}

void Cheats::noSmoke() {

}
