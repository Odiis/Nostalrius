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
SDName: Molten_Core
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

/* ContentData
mob_ancient_core_hound
EndContentData */

#include "scriptPCH.h"
#include "molten_core.h"

#define SPELL_CONE_OF_FIRE          19630
#define SPELL_BITE                  19771

//Random Debuff (each hound has only one of these)
#define SPELL_GROUND_STOMP          19364
#define SPELL_ANCIENT_DREAD         19365
#define SPELL_CAUTERIZING_FLAMES    19366
#define SPELL_WITHERING_HEAT        19367
#define SPELL_ANCIENT_DESPAIR       19369
#define SPELL_ANCIENT_HYSTERIA      19372

enum
{
    SPELL_FIREBLOSSOM         =   19637,
    SPELL_FIREBLOSSOM_CASTING =   19636,
    SPELL_INCITE_FLAMES       =   19635,
};

struct FirewalkerAI : public ScriptedAI
{
    FirewalkerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiFireBlossomCasting_Timer;
    uint32 m_uiFireBlossom_Timer;
    uint32 m_uiFireBlossomPreparing_Timer;
    uint32 m_uiInciteFlames_Timer;
    uint32 m_uiNbBlossom;

    void Reset()
    {
        m_uiFireBlossomCasting_Timer   =  6000;
        m_uiFireBlossom_Timer          =     0;
        m_uiFireBlossomPreparing_Timer =     0;
        m_uiInciteFlames_Timer         = 20000;
        m_uiNbBlossom                  =     0;
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFireBlossomCasting_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FIREBLOSSOM_CASTING) == CAST_OK)
                m_uiFireBlossomCasting_Timer   = 12000;

            m_uiNbBlossom = 6;
            m_uiFireBlossomPreparing_Timer = 1000;
        }
        else
            m_uiFireBlossomCasting_Timer -= uiDiff;

        if (m_uiInciteFlames_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_INCITE_FLAMES) == CAST_OK)
                m_uiInciteFlames_Timer = 20000;
        }
        else
            m_uiInciteFlames_Timer -= uiDiff;

        if (m_uiFireBlossomPreparing_Timer < uiDiff)
        {
            if (m_uiNbBlossom > 0)
            {
                if (Unit* pUnit = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCastSpellIfCan(pUnit, SPELL_FIREBLOSSOM);

                m_uiFireBlossomPreparing_Timer = 1000;
                m_uiNbBlossom = m_uiNbBlossom - 1;
            }
        }
        else
            m_uiFireBlossomPreparing_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_Firewalker(Creature* pCreature)
{
    return new FirewalkerAI(pCreature);
}

struct mob_ancient_core_houndAI : public ScriptedAI
{
    mob_ancient_core_houndAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 SouffleTimer;
    uint32 DebuffTimer;
    uint32 RandDebuff;
    uint32 BiteTimer;

    ScriptedInstance* m_pInstance;

    void Reset()
    {
        switch (urand(0, 5))
        {
            case 0 :
                RandDebuff = SPELL_GROUND_STOMP;
                break;
            case 1 :
                RandDebuff = SPELL_ANCIENT_DREAD;
                break;
            case 2 :
                RandDebuff = SPELL_CAUTERIZING_FLAMES;
                break;
            case 3 :
                RandDebuff = SPELL_WITHERING_HEAT;
                break;
            case 4 :
                RandDebuff = SPELL_ANCIENT_DESPAIR;
                break;
            case 5 :
                RandDebuff = SPELL_ANCIENT_HYSTERIA;
                break;
        }
        SouffleTimer = 10000;
        DebuffTimer = 15000;
        BiteTimer = 4000;
    }

    void JustDied(Unit* pKiller)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SouffleTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, SPELL_CONE_OF_FIRE, false);
            SouffleTimer = 7000;
        }
        else SouffleTimer -= uiDiff;

        if (DebuffTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, RandDebuff, false);
            DebuffTimer = 24000;
        }
        else DebuffTimer -= uiDiff;

        if (BiteTimer < uiDiff)
        {
            if (m_creature->IsWithinMeleeRange(m_creature->getVictim()))
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_BITE, false);
                BiteTimer = 6000;
            }
        }
        else BiteTimer -= uiDiff;

        if (m_creature->isAttackReady())
        {
            //If we are within range melee the target
            if (m_creature->IsWithinMeleeRange(m_creature->getVictim()))
            {
                m_creature->CastSpell(m_creature->getVictim(), 19319, true);
                m_creature->resetAttackTimer();
            }
        }
    }
};

CreatureAI* GetAI_mob_ancient_core_hound(Creature* pCreature)
{
    return new mob_ancient_core_houndAI(pCreature);
}

// Chiens du magma
/*
UPDATE creature_template SET ScriptName="mob_chien_magma" WHERE entry=11671;
*/
struct mob_chien_magmaAI : public ScriptedAI
{
    mob_chien_magmaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiCheckOtherDeathTimer = 0;
        bWasDead = false;
        Reset();
    }

    uint32 m_uiCheckOtherDeathTimer;
    uint32 m_uiMorsureTimer;
    bool bWasDead;

    void Reset()
    {
        m_uiCheckOtherDeathTimer = 10000;
        m_uiMorsureTimer = urand(5000, 10000);
    }

    void JustDied(Unit* pKiller)
    {
        m_uiCheckOtherDeathTimer = 10000;
        bWasDead = true;
        //m_creature->MonsterTextEmote("?Chien du magma commence à fondre?.", false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        bWasDead = false;
        if (m_uiMorsureTimer < uiDiff)
        {
            if (m_creature->IsWithinMeleeRange(m_creature->getVictim()))
            {
                DoCastSpellIfCan(m_creature->getVictim(), 19771);
                m_uiMorsureTimer = urand(10000, 15000);
            }
        }
        else
            m_uiMorsureTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void UpdateAI_corpse(const uint32 uiDiff)
    {
        if (!bWasDead)
            return;

        if (m_uiCheckOtherDeathTimer < uiDiff)
        {
            bWasDead = false;
            // Si la creature la plus proche vivante est en combat -> Respawn
            if (Creature* pCreature = m_creature->FindNearestCreature(m_creature->GetEntry(), 50.0f, true))
            {
                if (pCreature->isInCombat())
                {
                    m_creature->SetDeathState(ALIVE);
                    m_creature->SetHealthPercent(100.0f);
                    if (Unit* pVictim = pCreature->getVictim())
                        AttackStart(pVictim);
                }
            }
        }
        else
            m_uiCheckOtherDeathTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_chien_magma(Creature* pCreature)
{
    return new mob_chien_magmaAI(pCreature);
}

struct npc_surgisseur_laveAI : public ScriptedAI
{
    npc_surgisseur_laveAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 CheckTimer;
    ScriptedInstance* m_pInstance;

    void Reset()
    {
        CheckTimer = 10000;
    }

    void JustDied(Unit* pKiller)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        bool Ban = m_creature->GetFirstAuraBySpellIconAndVisual(96, 1305);

        if (Ban == false)
        {
            if (CheckTimer < uiDiff)
            {
                CheckTimer = urand(5000, 6000);
                Map::PlayerList const &liste = m_creature->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator i = liste.begin(); i != liste.end(); ++i)
                {
                    if (i->getSource()->isAlive() && i->getSource()->isInCombat())
                    {
                        if (m_creature->GetDistance2d(i->getSource()) > 8.0f && m_creature->GetDistance2d(i->getSource()) < 40.0f && m_creature->IsWithinLOSInMap(i->getSource()))
                        {
                            m_creature->CastSpell(i->getSource(), 19196, false);
                            m_creature->Relocate(i->getSource()->GetPositionX(), i->getSource()->GetPositionY(), i->getSource()->GetPositionZ());
                            m_creature->MonsterMove(i->getSource()->GetPositionX(), i->getSource()->GetPositionY(), i->getSource()->GetPositionZ());
                            break;
                        }
                    }
                }
            }
            else
                CheckTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_surgisseur_lave(Creature* pCreature)
{
    return new npc_surgisseur_laveAI(pCreature);
}

void AddSC_molten_core()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_firewalker";
    newscript->GetAI = &GetAI_Firewalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ancient_core_hound";
    newscript->GetAI = &GetAI_mob_ancient_core_hound;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_chien_magma";
    newscript->GetAI = &GetAI_mob_chien_magma;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_surgisseur_lave";
    newscript->GetAI = &GetAI_npc_surgisseur_lave;
    newscript->RegisterSelf();
}
