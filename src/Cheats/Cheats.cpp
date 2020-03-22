//
// Created by itayb on 3/16/2020.
//

#include <thread>
#include <iostream>
#include <math.h>

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
dwClientState(dumper->getPointer(VAR2STR(dwClientState))),
dwClientState_GetLocalPlayer(dumper->getPointer(VAR2STR(dwClientState_GetLocalPlayer))),
dwClientState_ViewAngles(dumper->getPointer(VAR2STR(dwClientState_ViewAngles))),
dwEntityList(dumper->getPointer(VAR2STR(dwEntityList))),
dwForceJump(dumper->getPointer(VAR2STR(dwForceJump))),
dwForceAttack(dumper->getPointer(VAR2STR(dwForceAttack))),
dwGlowObjectManager(dumper->getPointer(VAR2STR(dwGlowObjectManager))),
m_flFlashDuration(dumper->getPointer(VAR2STR(m_flFlashDuration))),
m_fFlags(dumper->getPointer(VAR2STR(m_fFlags))),
m_iTeamNum(dumper->getPointer(VAR2STR(m_iTeamNum))),
m_iCrosshairId(dumper->getPointer(VAR2STR(m_iCrosshairId))),
m_iGlowIndex(dumper->getPointer(VAR2STR(m_iGlowIndex))),
m_vecOrigin(dumper->getPointer(VAR2STR(m_vecOrigin))),
m_vecViewOffset(dumper->getPointer(VAR2STR(m_vecViewOffset))),
m_iHealth(dumper->getPointer(VAR2STR(m_iHealth))),
m_lifeState(dumper->getPointer(VAR2STR(m_lifeState))),
m_bDormant(dumper->getPointer(VAR2STR(m_bDormant))),
m_bSpottedByMask(dumper->getPointer(VAR2STR(m_bSpottedByMask))),
m_dwBoneMatrix(dumper->getPointer(VAR2STR(m_dwBoneMatrix))),

    //Flags
glowEnabled(false),
triggerEnabled(false),
noFlashEnabled(false),
aimbotEnabled(false),
kill(false)
{
    delete dumper;
}

Cheats::~Cheats() {
    delete proc;
}

void Cheats::run() {
    std::thread* bhop,*glow,*tbot,*aimbot,*noflash,*recoil;
    bool init = false;
    printStatus();

    //Initial Read
    proc->read(m_client+dwLocalPlayer,&m_localPlayer,sizeof(DWORD));

    while(proc->isRunning()){ 
        //rt Alt and Q to quit
        if(GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(0x51)){
            kill = true;
            break;
        }

        //Read local player health or team if bogus reread the localplayer
        //Local Team
        proc->read(m_localPlayer+m_iTeamNum,&m_localTeam,sizeof(DWORD));

        //Update local player if team not 2 or 3
        if(m_localTeam != 2 && m_localTeam != 3)
            proc->read(m_client+dwLocalPlayer,&m_localPlayer,sizeof(DWORD));
        

        if(GetAsyncKeyState(VK_F1) & 1) {
            glowEnabled = !glowEnabled;
            printStatus();
        }

        if(GetAsyncKeyState(VK_F2) & 1) {
            triggerEnabled = !triggerEnabled;
            printStatus();
        }

        if(GetAsyncKeyState(VK_F3) & 1) {
            noFlashEnabled = !noFlashEnabled;
            printStatus();
        }

        if(GetAsyncKeyState(VK_F4) & 1) {
            aimbotEnabled = !aimbotEnabled;
            printStatus();
        }

        if(!init){
            bhop = new std::thread(&Cheats::bunnyHop,this);
            glow = new std::thread(&Cheats::glow,this);
            noflash = new std::thread(&Cheats::noFlash,this);
            aimbot = new std::thread(&Cheats::aimBot,this);
            tbot = new std::thread(&Cheats::triggerBot,this);
           // recoil = new std::thread(&Cheats::noRecoil,this);
            init = true;
        }
        Sleep(500);
    }
    kill = true;

    //Wait for threads to exit
    bhop->join();
    glow->join();
    tbot->join();
    aimbot->join();
    noflash->join();

    //Delete Threads
    delete bhop;
    delete glow;
    delete tbot;
    delete aimbot;
    delete noflash;
}

void Cheats::bunnyHop() {
    while (!kill) {
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
    while (!kill) {
        if (!glowEnabled) {
            Sleep(500);
            continue;
        }
        DWORD glowmngr;
        //Glow Manager
        proc->read(m_client + dwGlowObjectManager, &glowmngr,sizeof(DWORD));

        //Loop through entities
        for (uint8_t i = 0; i < ENTITY_COUNT; i++) {
            DWORD entity;
            uint8_t lifestate;
            //Get entity
            proc->read(m_client + dwEntityList + i * 0x10, &entity,sizeof(DWORD));
            proc->read(entity + m_lifeState,&lifestate,sizeof(uint8_t));

            if (lifestate == 0 && entity > 0 && entity != m_localPlayer) {
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
    uint8_t val = 6;
    while(!kill){
        if(!triggerEnabled){
            Sleep(500);
            continue;
        }
        uint8_t id,eTeam;
        DWORD entity;
            //Get ID in crossair
        proc->read(m_localPlayer + m_iCrosshairId, &id, sizeof(uint8_t));
        if(id>0 && id<64){
            //Get Entity
            proc->read(m_client + dwEntityList + ((id-1) * 0x10), &entity, sizeof(DWORD));
            //Check Entity team
            proc->read(entity + m_iTeamNum, &eTeam, sizeof(uint8_t));
            //If not in current team, shoot.
            if(eTeam != m_localTeam){
                proc->write(m_client + dwForceAttack, &val, sizeof(uint8_t));
                Sleep(250);
            }   
        }
    }
}


void Cheats::aimBot() {
    uint8_t aimToBone[] = {5,8};
    float aimToLocations[ENTITY_COUNT*2];
    float localAngles[2];
    float org[3];
    float playerHead[3];
    float clipRange = 1.5;

    while (!kill) {
        if (!aimbotEnabled) {
            Sleep(500);
            continue;
        }

        uint8_t aimToCount = 0;

        //Get local player head location
        proc->read(m_localPlayer + m_vecOrigin, &org, sizeof(org));
        proc->read(m_localPlayer + m_vecViewOffset, &playerHead, sizeof(playerHead));
        playerHead[0] += org[0];
        playerHead[1] += org[1];
        playerHead[2] += org[2];

        //Get angles
        DWORD clientState;
        proc->read(m_engine + dwClientState, &clientState, sizeof(DWORD));
        proc->read(clientState + dwClientState_ViewAngles, localAngles,sizeof(float) * 2);
        //Get client's ID
        uint32_t localID;
        proc->read(clientState + dwClientState_GetLocalPlayer, &localID, sizeof(uint32_t));

        //Get current player head pos
        for (uint8_t i = 0; i < ENTITY_COUNT; i++) {
            DWORD entity;
            uint8_t lifestate, eTeam;
            bool dormant;

            proc->read(m_client + dwEntityList + (i * 0x10), &entity, sizeof(DWORD));

            //Check dormant
            proc->read(entity + m_bDormant, &dormant, sizeof(bool));
            if (dormant)
                continue;
            

            //Check team
            proc->read(entity + m_iTeamNum, &eTeam, sizeof(uint8_t));
            if (eTeam == m_localTeam)
                continue;

            //Check health
            proc->read(entity + m_lifeState, &lifestate, sizeof(uint8_t));
            if (lifestate != 0)
                continue;

            //Check if player is visible
            uint32_t spottedMask;
            proc->read(entity + m_bSpottedByMask, &spottedMask, sizeof(uint32_t));
            //if not spotted
            if (!(spottedMask & (1 << localID)))
                continue;
            DWORD boneMatrix;


            proc->read(entity + m_dwBoneMatrix, &boneMatrix, sizeof(DWORD));
            float boneLocation[3];
            for(uint8_t j : aimToBone){
                getBoneLocation(&boneMatrix, j, boneLocation);
                std::tuple<float, float> angles = calcAngles(playerHead, boneLocation);
                float viewAngles[2] = {std::get<0>(angles), std::get<1>(angles)};
                if (viewAngles[0] >= -89 && viewAngles[0] <= 89 && viewAngles[1] >= -180 && viewAngles[1] <= 180) {
                    aimToLocations[aimToCount*2] = viewAngles[0];
                    aimToLocations[aimToCount*2 + 1]= viewAngles[1];
                    aimToCount++;
                }
            }
        }
        //All Angles calculated, Choose closest one.
        uint8_t bestIndex = 0;
        float bestDist = 10000;

        //Get closest angle to aim at
        for(uint8_t i = 0; i<aimToCount; i++){
            float dist = getDistance2D(localAngles,&aimToLocations[i*2]);
            if(dist<clipRange){
                bestDist = dist;
                bestIndex = i;
            }
        }
        //Aim
        if(bestDist<clipRange)
            smoothAim(localAngles,&aimToLocations[bestIndex*2],15,clientState);
    }
}

std::tuple<float,float> Cheats::calcAngles(float* from, float* to){
    float delta[3];
    float len = getDistance3D(from,to,delta);

    float pitch = -asin(delta[2]/len)*(180.0/M_PI);
    float yaw = atan2(delta[1],delta[0]) * (180.0/M_PI);

    return {pitch,yaw};
}

float Cheats::getDistance3D(float* from, float* to, float* deltaOut){
    deltaOut[0] = to[0] - from[0];
    deltaOut[1] = to[1] - from[1];
    deltaOut[2] = to[2] - from[2];
    return sqrt(deltaOut[0]*deltaOut[0] + deltaOut[1]*deltaOut[1] + deltaOut[2]*deltaOut[2]);
}

float Cheats::getDistance2D(float* from, float* to){
    float delta[2] = {to[0] - from[0], to[1] - from[1]};
    return sqrt(delta[0]*delta[0] + delta[1]*delta[1]);
}

void Cheats::smoothAim(float* from, float* to, uint8_t steps, DWORD& clientState){
    //Get deltas
    float dX = (to[0] - from[0])/steps;
    float dY = (to[1] - from[1])/steps;
    for(uint8_t i = 1; i <=steps; i++){
        from[0]+=dX;
        from[1]+=dY;

        //Write new location
        if(!GetAsyncKeyState(VK_MBUTTON)){
            proc->write(clientState + dwClientState_ViewAngles, from,sizeof(float) * 2);
        }
    }
}

void Cheats::getBoneLocation(DWORD* boneMatrix, uint8_t bId, float* out){
    proc->read(*boneMatrix + (0x30 * bId) + 0x0C,out,sizeof(out));
    proc->read(*boneMatrix + (0x30 * bId) + 0x1C,&out[1],sizeof(float));
    proc->read(*boneMatrix + (0x30 * bId) + 0x2C,&out[2],sizeof(float));
}

void Cheats::noRecoil(){
    float oldAimPunch[2] = { 0.0, 0.0};
    while(!kill){
        //Get angles
        float localAngles[2];
        DWORD clientState;
        uint8_t val;
        proc->read(m_engine + dwClientState, &clientState, sizeof(DWORD));
        proc->read(clientState + dwClientState_ViewAngles, localAngles,sizeof(float) * 2);
        proc->read(m_client + dwForceAttack, &val, sizeof(uint8_t));
        float aimPunch[2];
        if(val !=4){
            if(oldAimPunch[0] == 0){
                proc->read(m_localPlayer + 12332,&oldAimPunch,sizeof(aimPunch));
                oldAimPunch[0] *= 2;
                oldAimPunch[0] *= 2;

            }
            proc->read(m_localPlayer + 12332,&aimPunch,sizeof(aimPunch));
            aimPunch[0] *= 2;
            aimPunch[0] *= 2;
            localAngles[0] = localAngles[0] + oldAimPunch[0] - aimPunch[0];
            localAngles[1] = localAngles[1] + oldAimPunch[1] - aimPunch[1];

            proc->write(clientState + dwClientState_ViewAngles, localAngles,sizeof(float) * 2);
            oldAimPunch[0] = aimPunch[0];
            oldAimPunch[1] = aimPunch[1];
        }else{
            oldAimPunch[0] = 0;
            oldAimPunch[1] = 0;
        }
    }
}

void Cheats::noFlash() {
    while(!kill){
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

void Cheats::printStatus(){
    //Clear Screen
    system("cls");

    //Display Bunny Hop
    puts("Hold Space to Bunny Hop");

    //Display Glow
    std::cout<<std::boolalpha;
    std::cout<<"Press F1 to toggle Glow: "<< glowEnabled <<std::endl;
    std::cout<<"Press F2 to toggle Trigger Bot: "<< triggerEnabled <<std::endl;
    std::cout<<"Press F3 to toggle No Flash: "<< noFlashEnabled <<std::endl;
    std::cout<<"Press F4 to toggle Aim Bot(Middle wheel to unclip): "<< aimbotEnabled <<std::endl;
}
