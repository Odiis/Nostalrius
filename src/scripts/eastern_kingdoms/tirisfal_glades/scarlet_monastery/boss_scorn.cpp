/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Scorn
SD%Complete: 100
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "scriptPCH.h"

#define SPELL_LICHSLAP                  28873
#define SPELL_FROSTBOLTVOLLEY           22643 //was 8398
#define SPELL_MINDFLAY                  17313
#define SPELL_FROSTNOVA                 29849 //was 15531

struct Locations
{
    float x, y, z;
};

static Locations ronde[] =
{
    {1797.668335, 1214.438965, 18.158447},
    {1798.763306, 1275.821533, 18.488573},
    {1800.726440, 1313.862305, 18.725443},
    {1805.438354, 1323.635010, 18.919188},
    {1799.388916, 1338.318481, 18.887611},
    {1797.709351, 1383.367432, 18.765408},
    {1796.630859, 1349.865845, 18.887638},
    {1791.184326, 1326.423096, 18.950106},
    {1793.545776, 1316.592651, 18.793835},
    {1796.110840, 1294.494141, 18.594208}
};

struct boss_scornAI : public ScriptedAI
{
    boss_scornAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        LastWayPoint = 0;
        Reset();
    }

    uint32 LichSlap_Timer;
    uint32 FrostboltVolley_Timer;
    uint32 MindFlay_Timer;
    uint32 FrostNova_Timer;
    uint32 LastWayPoint;

    void Reset()
    {
        LichSlap_Timer = 45000;
        FrostboltVolley_Timer = 30000;
        MindFlay_Timer = 30000;
        FrostNova_Timer = 30000;
        m_creature->GetMotionMaster()->MovePoint(LastWayPoint, ronde[LastWayPoint].x, ronde[LastWayPoint].y, ronde[LastWayPoint].z);
    }
    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (!m_creature->getVictim())
        {
            m_creature->SetWalk(true);
            if (uiPointId < 9)
                m_creature->GetMotionMaster()->MovePoint(uiPointId + 1, ronde[uiPointId + 1].x, ronde[uiPointId + 1].y, ronde[uiPointId + 1].z);
            else if (uiPointId == 9)
                m_creature->GetMotionMaster()->MovePoint(0, ronde[0].x, ronde[0].y, ronde[0].z);
        }
        if (uiPointId >= 0 && uiPointId < 10)
            LastWayPoint = uiPointId;
    }
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //LichSlap_Timer
        if (LichSlap_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_LICHSLAP);
            LichSlap_Timer = 45000;
        }
        else LichSlap_Timer -= diff;

        //FrostboltVolley_Timer
        if (FrostboltVolley_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FROSTBOLTVOLLEY);
            FrostboltVolley_Timer = 20000;
        }
        else FrostboltVolley_Timer -= diff;

        //MindFlay_Timer
        if (MindFlay_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_MINDFLAY);
            MindFlay_Timer = 20000;
        }
        else MindFlay_Timer -= diff;

        //FrostNova_Timer
        if (FrostNova_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FROSTNOVA);
            FrostNova_Timer = 15000;
        }
        else FrostNova_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_scorn(Creature* pCreature)
{
    return new boss_scornAI(pCreature);
}

void AddSC_boss_scorn()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_scorn";
    newscript->GetAI = &GetAI_boss_scorn;
    newscript->RegisterSelf();
}
