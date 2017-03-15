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
SDName: boss_cannon_master_willey
SD%Complete: 100
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "scriptPCH.h"

//front, left
//previously
//3536.851807f -17
//-2958.885986f +13
#define ADD_1X  3537.2725f
#define ADD_1Y  -2958.18f
#define ADD_1Z 125.001015f
#define ADD_1O 0.592007f
//front, right
#define ADD_2X 3542.206299f
#define ADD_2Y -2965.929932f
#define ADD_2Z 125.001015f
#define ADD_2O 0.592007f
//mid, left
#define ADD_3X 3539.417480f
#define ADD_3Y -2959.667236f
#define ADD_3Z 125.001015f
#define ADD_3O 0.592007f
//mid, right
#define ADD_4X 3540.651855f
#define ADD_4Y -2964.519043f
#define ADD_4Z 125.001015f
#define ADD_4O 0.592007f
//back, left
#define ADD_5X 3531.927246f
#define ADD_5Y -2962.977295f
#define ADD_5Z 125.001015f
#define ADD_5O 0.592007f
//back, mid
#define ADD_6X 3538.094697f
#define ADD_6Y -2963.123291f
#define ADD_6Z 125.001015f
#define ADD_6O 0.592007f
//back, right
#define ADD_7X 3535.727539f
#define ADD_7Y -2969.776123f
#define ADD_7Z 125.001015f
#define ADD_7O 0.592007f
//behind, left
#define ADD_8X 3532.156250f
#define ADD_8Y -2966.162354f
#define ADD_8Z 125.001015f
#define ADD_8O 0.592007f
//behind, right
#define ADD_9X 3533.202148f
#define ADD_9Y -2969.437744f
#define ADD_9Z 125.001015f
#define ADD_9O 0.592007f

#define SPELL_KNOCKAWAY    10101
#define SPELL_PUMMEL    15615
#define SPELL_SHOOT    20463
//#define SPELL_SUMMONCRIMSONRIFLEMAN    17279

struct boss_cannon_master_willeyAI : public ScriptedAI
{
    boss_cannon_master_willeyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 KnockAway_Timer;
    uint32 Pummel_Timer;
    uint32 Shoot_Timer;
    uint32 SummonRifleman_Timer;

    void Reset()
    {
        Shoot_Timer = 1000;
        Pummel_Timer = 7000;
        KnockAway_Timer = 11000;
        SummonRifleman_Timer = 20000;
    }

    void Aggro(Unit* pWho)
    {
        SummonRifleman_Timer = 1000;
    }

    void JustDied(Unit* Victim)
    {
        m_creature->SummonCreature(11054, ADD_1X, ADD_1Y, ADD_1Z, ADD_1O, TEMPSUMMON_TIMED_DESPAWN, 60000);
        m_creature->SummonCreature(11054, ADD_2X, ADD_2Y, ADD_2Z, ADD_2O, TEMPSUMMON_TIMED_DESPAWN, 60000);
        m_creature->SummonCreature(11054, ADD_3X, ADD_3Y, ADD_3Z, ADD_3O, TEMPSUMMON_TIMED_DESPAWN, 60000);
        m_creature->SummonCreature(11054, ADD_4X, ADD_4Y, ADD_4Z, ADD_4O, TEMPSUMMON_TIMED_DESPAWN, 60000);
        m_creature->SummonCreature(11054, ADD_5X, ADD_5Y, ADD_5Z, ADD_5O, TEMPSUMMON_TIMED_DESPAWN, 60000);
        m_creature->SummonCreature(11054, ADD_7X, ADD_7Y, ADD_7Z, ADD_7O, TEMPSUMMON_TIMED_DESPAWN, 60000);
        m_creature->SummonCreature(11054, ADD_9X, ADD_9Y, ADD_9Z, ADD_9O, TEMPSUMMON_TIMED_DESPAWN, 60000);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Pummel
        if (Pummel_Timer < diff)
        {
            //Cast
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PUMMEL) == CAST_OK)
                Pummel_Timer = 12000;
        }
        else Pummel_Timer -= diff;

        //KnockAway
        if (KnockAway_Timer < diff)
        {
            //Cast
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKAWAY) == CAST_OK)
                KnockAway_Timer = 14000;
        }
        else KnockAway_Timer -= diff;

        //Shoot
        if (Shoot_Timer < diff)
        {
            //Cast
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT);
            //1 seconds until we should cast this again
            Shoot_Timer = 1000;
        }
        else Shoot_Timer -= diff;

        //SummonRifleman
        if (SummonRifleman_Timer < diff)
        {
            //Cast
            switch (urand(0, 8))
            {
                case 0:
                    m_creature->SummonCreature(11054, ADD_1X, ADD_1Y, ADD_1Z, ADD_1O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_2X, ADD_2Y, ADD_2Z, ADD_2O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_4X, ADD_4Y, ADD_4Z, ADD_4O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 1:
                    m_creature->SummonCreature(11054, ADD_2X, ADD_2Y, ADD_2Z, ADD_2O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_3X, ADD_3Y, ADD_3Z, ADD_3O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_5X, ADD_5Y, ADD_5Z, ADD_5O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 2:
                    m_creature->SummonCreature(11054, ADD_3X, ADD_3Y, ADD_3Z, ADD_3O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_4X, ADD_4Y, ADD_4Z, ADD_4O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_6X, ADD_6Y, ADD_6Z, ADD_6O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 3:
                    m_creature->SummonCreature(11054, ADD_4X, ADD_4Y, ADD_4Z, ADD_4O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_5X, ADD_5Y, ADD_5Z, ADD_5O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_7X, ADD_7Y, ADD_7Z, ADD_7O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 4:
                    m_creature->SummonCreature(11054, ADD_5X, ADD_5Y, ADD_5Z, ADD_5O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_6X, ADD_6Y, ADD_6Z, ADD_6O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_8X, ADD_8Y, ADD_8Z, ADD_8O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 5:
                    m_creature->SummonCreature(11054, ADD_6X, ADD_6Y, ADD_6Z, ADD_6O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_7X, ADD_7Y, ADD_7Z, ADD_7O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_9X, ADD_9Y, ADD_9Z, ADD_9O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 6:
                    m_creature->SummonCreature(11054, ADD_7X, ADD_7Y, ADD_7Z, ADD_7O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_8X, ADD_8Y, ADD_8Z, ADD_8O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_1X, ADD_1Y, ADD_1Z, ADD_1O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 7:
                    m_creature->SummonCreature(11054, ADD_8X, ADD_8Y, ADD_8Z, ADD_8O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_9X, ADD_9Y, ADD_9Z, ADD_9O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_2X, ADD_2Y, ADD_2Z, ADD_2O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
                case 8:
                    m_creature->SummonCreature(11054, ADD_9X, ADD_9Y, ADD_9Z, ADD_9O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_1X, ADD_1Y, ADD_1Z, ADD_1O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    m_creature->SummonCreature(11054, ADD_3X, ADD_3Y, ADD_3Z, ADD_3O, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    break;
            }
            SummonRifleman_Timer = 20000;
        }
        else SummonRifleman_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_cannon_master_willey(Creature* pCreature)
{
    return new boss_cannon_master_willeyAI(pCreature);
}

struct go_scarlet_cannonAI: public GameObjectAI
{
    go_scarlet_cannonAI(GameObject* pGo) : GameObjectAI(pGo) {}

    bool OnUse(Unit* pUser)
    {
        if (Creature* CannonBall = pUser->SummonCreature(160018, 3541.06f, -2951.33f, 126.381f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 5000)) // Trigger Cannonball
        {
            CannonBall->CastSpell(CannonBall, 17278, false);
            return true;
        }
        else
            return false;
    }
};

GameObjectAI* GetAIgo_scarlet_cannon(GameObject *pGo)
{
    return new go_scarlet_cannonAI(pGo);
}

void AddSC_boss_cannon_master_willey()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_cannon_master_willey";
    newscript->GetAI = &GetAI_boss_cannon_master_willey;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_scarlet_cannon";
    newscript->GOGetAI = &GetAIgo_scarlet_cannon;
    newscript->RegisterSelf();
}
