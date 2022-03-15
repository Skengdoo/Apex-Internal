#pragma once
#include "../../global.h"

struct UserCmd_t
{
    uintptr_t pVft;                // 0x00
    int32_t   m_iCmdNumber;        // 0x04
    int32_t   m_iTickCount;        // 0x08
    Vector3 m_vecViewAngles;     // 0x0C
    Vector3 m_vecAimDirection;   // 0x18
    float     m_flForwardmove;     // 0x24
    float     m_flSidemove;        // 0x28
    float     m_flUpmove;          // 0x2C
    int32_t   m_iButtons;          // 0x30
    uint8_t   m_bImpulse;          // 0x34
    uint8_t   Pad1[3];
    int32_t   m_iWeaponSelect;     // 0x38
    int32_t   m_iWeaponSubtype;    // 0x3C
    int32_t   m_iRandomSeed;       // 0x40
    int16_t   m_siMouseDx;         // 0x44
    int16_t   m_siMouseDy;         // 0x46
    bool      m_bHasBeenPredicted; // 0x48
    uint8_t   Pad2[27];
}; // size is 100 or 0x64