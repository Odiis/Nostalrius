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
SDName: Boss_Baron_Geddon
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "scriptPCH.h"
#include "molten_core.h"

#define EMOTE_SERVICE               -1409000

#define SPELL_INFERNO               19695
#define SPELL_IGNITEMANA            19659
#define SPELL_LIVINGBOMB            20475
#define SPELL_ARMAGEDDOM            20478

struct boss_baron_geddonAI : public ScriptedAI
{
    boss_baron_geddonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 Inferno_Timer;
    uint32 IgniteMana_Timer;
    uint32 LivingBomb_Timer;
    uint32 InfCount;
    uint32 Tick;
    bool Inferno;
    bool Armageddon;

    ScriptedInstance* m_pInstance;

    void EnterEvadeMode()
    {
        m_creature->clearUnitState(UNIT_STAT_ROOT);
        ScriptedAI::EnterEvadeMode();
    }
    void Reset()
    {
        Inferno_Timer    = 15000;                              //These times are probably wrong
        IgniteMana_Timer = 30000;
        LivingBomb_Timer = 20000;
        Inferno = false;
        Armageddon = false;

        if (m_pInstance && m_creature->isAlive())
            m_pInstance->SetData(TYPE_GEDDON, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GEDDON, IN_PROGRESS);
        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GEDDON, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->HasAura(SPELL_ARMAGEDDOM) || Armageddon)
            return;

        //If we are <2% hp cast Armageddom
        if (!Armageddon)
        {
            if (m_creature->GetHealthPercent() <= 2.0f)
            {
                m_creature->InterruptNonMeleeSpells(true);

                m_creature->CastSpell(m_creature, SPELL_ARMAGEDDOM, true);
                DoScriptText(EMOTE_SERVICE, m_creature);
                Armageddon = true;
                return;
            }
        }

        //LivingBomb_Timer
        if (LivingBomb_Timer < diff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                pTarget->CastSpell(pTarget, SPELL_LIVINGBOMB, false);
                LivingBomb_Timer = 13000 + rand() % 4000;
            }
        }
        else
            LivingBomb_Timer -= diff;

        //IgniteMana_Timer
        if (IgniteMana_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_IGNITEMANA) == CAST_OK)
                IgniteMana_Timer = 30000;
        }
        else
            IgniteMana_Timer -= diff;

        //Inferno_Timer
        if (Inferno_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INFERNO) == CAST_OK)
            {
                Inferno_Timer = 15000 + rand() % 5000;
                InfCount = 0;
                Tick = 1000;
                Inferno = true;
            }
        }
        else if (!m_creature->HasAura(SPELL_INFERNO))
        {
            Inferno_Timer -= diff;
            m_creature->clearUnitState(UNIT_STAT_ROOT);
        }

        if (Inferno)
        {
            m_creature->addUnitState(UNIT_STAT_ROOT);

            if (Tick >= 1000)
            {
                int Damage = 0;
                switch (InfCount)
                {
                    case 8:
                        Inferno = false;
                        Damage = 2500;
                        break;
                    case 7:
                    case 6:
                        Damage = 2000;
                        break;
                    case 5:
                    case 4:
                        Damage = 1500;
                        break;
                    case 3:
                    case 2:
                        Damage = 1000;
                        break;
                    case 1:
                    case 0:
                        Damage = 500;
                        break;
                }
                m_creature->CastCustomSpell(m_creature, 5857, 0, 0, 0, false);
                m_creature->CastCustomSpell(m_creature, 19698, &Damage, NULL, NULL, true);
                InfCount++;
                Tick = 0;
            }
            Tick += diff;
            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_baron_geddon(Creature* pCreature)
{
    return new boss_baron_geddonAI(pCreature);
}

void AddSC_boss_baron_geddon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_baron_geddon";
    newscript->GetAI = &GetAI_boss_baron_geddon;
    newscript->RegisterSelf();
}
