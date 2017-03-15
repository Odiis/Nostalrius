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
SDName: Boss_Magmadar
SD%Complete: 75
SDComment: Conflag on ground nyi, fear causes issues without VMAPs
SDCategory: Molten Core
EndScriptData */

#include "scriptPCH.h"
#include "molten_core.h"

enum
{
    EMOTE_GENERIC_FRENZY_KILL   = -1000001,

    SPELL_FRENZY                = 19451,
    SPELL_MAGMASPIT             = 19449,                    //This is actually a buff he gives himself
    SPELL_PANIC                 = 19408,
    SPELL_LAVABOMB              = 19411,                    //This calls a dummy server side effect that isn't implemented yet
    SPELL_LAVABOMB_ALT          = 19428,                    //This is the spell that the lava bomb casts
    SPELL_LAVA_BREATH           = 19272,

    NPC_LAVABOMB                = 20006,

    MODEL_INVISIBLE             = 11686,
};

/*
DELETE FROM creature_template WHERE entry=20006 OR ScriptName="boss_magmadar_lavabomb";
INSERT INTO creature_template
SET entry=20006, modelid_1=11686, modelid_2=11686, name="LavaBomb", subname="Script", minhealth=4000, maxhealth=5000, faction_A=14,faction_H=14,
speed_walk=0.01, speed_run=0.01, ScriptName="boss_magmadar_lavabomb";
*/
struct boss_magmadarAI : public ScriptedAI
{
    boss_magmadarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 Frenzy_Timer;
    uint32 Panic_Timer;
    uint32 Lavabomb_Timer;
    uint32 Lavabomb_alt_Timer;
    uint32 LavaBreath_Timer;
    uint32 uiCrachatMagmaTimer;

    ScriptedInstance* m_pInstance;

    void Reset()
    {
        Frenzy_Timer = 30000;
        Panic_Timer = 20000;
        Lavabomb_Timer = 12000;
        LavaBreath_Timer = 30000;
        uiCrachatMagmaTimer = 10000;
        Lavabomb_alt_Timer = 3000;

        if (!m_creature->HasAura(SPELL_MAGMASPIT))
            m_creature->CastSpell(m_creature, SPELL_MAGMASPIT, true);

        if (m_pInstance && m_creature->isAlive())
            m_pInstance->SetData(TYPE_MAGMADAR, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAGMADAR, IN_PROGRESS);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAGMADAR, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!Lavabomb_alt_Timer || !m_creature->getVictim())
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

        if (uiCrachatMagmaTimer < diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), 19450) == CAST_OK)
                uiCrachatMagmaTimer = urand(10000, 20000);
        }
        else
            uiCrachatMagmaTimer -= diff;

        //Frenzy_Timer
        if (Frenzy_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
            {
                DoScriptText(EMOTE_GENERIC_FRENZY_KILL, m_creature);
                Frenzy_Timer = 15000 + rand() % 5000;
            }
        }
        else
            Frenzy_Timer -= diff;

        //Panic_Timer
        if (Panic_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PANIC) == CAST_OK)
                Panic_Timer = 35000;
        }
        else
            Panic_Timer -= diff;

        //Lavabomb_Timer
        if (Lavabomb_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, nullptr, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(target, SPELL_LAVABOMB) == CAST_OK)
                {
                    m_creature->SetInFront(target);
                    m_creature->SetTargetGuid(target->GetObjectGuid());
                    if (Creature* Cre = m_creature->SummonCreature(NPC_LAVABOMB,
                                        target->GetPositionX(),
                                        target->GetPositionY(),
                                        target->GetPositionZ(),
                                        target->GetOrientation(),
                                        TEMPSUMMON_TIMED_DESPAWN,
                                        30000))
                    {
                        //scale at 0 is necessary for spell animation
                        Cre->SetObjectScale(0.0f);
                        Cre->setFaction(m_creature->getFaction());
                    }
                    Lavabomb_Timer = 10000 + rand() % 2000;

                    Lavabomb_alt_Timer = 700;
                }
            }
        }
        else
            Lavabomb_Timer -= diff;

        if (Lavabomb_alt_Timer)
        {
            if (Lavabomb_alt_Timer <= diff)
            {
                std::list<Creature*> CreListe;
                GetCreatureListWithEntryInGrid(CreListe, m_creature, NPC_LAVABOMB, 100.0f);
                for (std::list<Creature*>::iterator itr = CreListe.begin(); itr != CreListe.end(); ++itr)
                    (*itr)->CastSpell((*itr), SPELL_LAVABOMB_ALT, false);
                m_creature->SetInFront(m_creature->getVictim());
                m_creature->SetTargetGuid(m_creature->getVictim()->GetObjectGuid());
                Lavabomb_alt_Timer = 0;
            }
            else
                Lavabomb_alt_Timer -= diff;
        }

        if (LavaBreath_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_LAVA_BREATH) == CAST_OK)
                LavaBreath_Timer = urand(10000, 30000);
        }
        else
            LavaBreath_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_magmadar(Creature* pCreature)
{
    return new boss_magmadarAI(pCreature);
}

void AddSC_boss_magmadar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_magmadar";
    newscript->GetAI = &GetAI_boss_magmadar;
    newscript->RegisterSelf();
}
