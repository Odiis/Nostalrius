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
SDName: Boss_Garr
SD%Complete: 50
SDComment: Adds NYI
SDCategory: Molten Core
EndScriptData */

#include "scriptPCH.h"
#include "molten_core.h"

enum
{
    // Garr spells
    SPELL_ANTIMAGICPULSE        = 19492,
    SPELL_MAGMASHACKLES         = 19496,
    SPELL_ENRAGE                = 19516,                   //Stacking enrage (stacks to 10 times)

    //Add spells
    SPELL_MASSIVE_ERUPTION      = 20483,
    SPELL_IMMOLATE              = 20294,
    SPELL_SEPARATION_ANXIETY    = 23492,
    SPELL_ADD_ERUPTION          = 19497,

    NPC_FIRESWORN               = 12099,
};

struct boss_garrAI : public ScriptedAI
{
    boss_garrAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 AntiMagicPulse_Timer;
    uint32 MagmaShackles_Timer;
    uint32 CheckAdds_Timer;
    uint32 Explode_Timer;

    ScriptedInstance* m_pInstance;

    void Reset()
    {
        AntiMagicPulse_Timer = 25000;                       //These times are probably wrong
        MagmaShackles_Timer = 15000;
        CheckAdds_Timer = 2000;
        Explode_Timer = urand(3000, 6000);

        if (m_pInstance && m_creature->isAlive())
            m_pInstance->SetData(TYPE_GARR, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_GARR) != DONE)
                m_pInstance->SetData(TYPE_GARR, IN_PROGRESS);
            else
                m_creature->DeleteLater();
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GARR, DONE);
    }

    void DoMassiveEruption()
    {
        std::list<Creature*> LigesListe;
        GetCreatureListWithEntryInGrid(LigesListe, m_creature, NPC_FIRESWORN, 150.0f);
        uint32 numLiges = LigesListe.size();
        if (!numLiges)
            return;

        int32 explodeIdx = urand(0, numLiges-1);
        Creature* validCreature = NULL;
        std::list<Creature*>::iterator itr = LigesListe.begin();
        while (explodeIdx > 0 || !validCreature)
        {
            if (itr == LigesListe.end())
                break;

            if ((*itr)->isAlive() && !(*itr)->HasAura(18647) && !(*itr)->HasAura(710))
                validCreature = *itr;
            ++itr;
            --explodeIdx;
        }
        if (validCreature)
            validCreature->CastSpell(validCreature, SPELL_MASSIVE_ERUPTION, false);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (AntiMagicPulse_Timer < diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_ANTIMAGICPULSE);
            AntiMagicPulse_Timer = urand(10000, 15000);
        }
        else
            AntiMagicPulse_Timer -= diff;

        if (MagmaShackles_Timer < diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_MAGMASHACKLES);
            MagmaShackles_Timer = urand(8000, 12000);
        }
        else
            MagmaShackles_Timer -= diff;

        if (m_creature->GetHealthPercent() < 50.0f)
        {
            if (Explode_Timer < diff)
            {
                DoMassiveEruption();
                Explode_Timer = urand(4500, 5500);
            }
            else
                Explode_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

struct mob_fireswornAI : public ScriptedAI
{
    mob_fireswornAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 Immolate_Timer;
    uint32 AnxietyTimer;
    bool Erupt;

    void Reset()
    {
        Immolate_Timer = 5000;
        AnxietyTimer = 10000;
        Erupt = false;
    }

    void JustDied(Unit*)
    {
        if (Creature* Garr = m_creature->FindNearestCreature(NPC_GARR, 100.0f, true))
            Garr->CastSpell(Garr, SPELL_ENRAGE, true);

        // Cast Eruption at death
        Creature* Garr = m_creature->FindNearestCreature(NPC_GARR, 100.0f, true);
        if (Garr && Garr->GetHealthPercent() > 50.0f)
            m_creature->CastSpell(m_creature, SPELL_ADD_ERUPTION, true);
    }

    void SpellHitTarget(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_ADD_ERUPTION || pSpell->Id == SPELL_MASSIVE_ERUPTION)
            Erupt = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (Erupt == true)
            m_creature->CastSpell(m_creature, 5, true);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Immolate_Timer
        if (Immolate_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_IMMOLATE);
            Immolate_Timer = urand(5000, 10000);
        }
        else
            Immolate_Timer -= diff;

        Creature* Garr = m_creature->FindNearestCreature(NPC_GARR, 100.0f, true);
        // Cast Eruption and let him die
        if (Garr && Garr->GetHealthPercent() > 50.0f && m_creature->GetHealthPercent() <= 10.0f)
            DoCastSpellIfCan(m_creature, SPELL_ADD_ERUPTION);

        // "Axiety"
        if (!m_creature->HasAura(SPELL_SEPARATION_ANXIETY))
        {
            if (Garr)
            {
                if (m_creature->GetDistance2d(Garr) > 45.0f)
                {
                    if (AnxietyTimer < diff)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_SEPARATION_ANXIETY);
                        AnxietyTimer = 5000;
                    }
                    else
                        AnxietyTimer -= diff;
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_garr(Creature* pCreature)
{
    return new boss_garrAI(pCreature);
}

CreatureAI* GetAI_mob_firesworn(Creature* pCreature)
{
    return new mob_fireswornAI(pCreature);
}

void AddSC_boss_garr()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_garr";
    newscript->GetAI = &GetAI_boss_garr;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_firesworn";
    newscript->GetAI = &GetAI_mob_firesworn;
    newscript->RegisterSelf();
}
