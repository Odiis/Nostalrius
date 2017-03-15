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
SDName: Boss_Ragnaros
SD%Complete: 75
SDComment: Intro Dialog and event NYI
SDCategory: Molten Core
EndScriptData */

#include "scriptPCH.h"
#include "molten_core.h"

#define SAY_REINFORCEMENTS1         -1409013
#define SAY_REINFORCEMENTS2         -1409014
#define SAY_HAND                    -1409015
#define SAY_WRATH                   -1409016
#define SAY_KILL                    -1409017
#define SAY_MAGMABURST              -1409018

#define SPELL_HANDOFRAGNAROS        19780
#define SPELL_WRATHOFRAGNAROS       20566
#define SPELL_LAVABURST             21158

#define SPELL_MAGMABURST            20565                   //Ranged attack

#define SPELL_SONSOFFLAME_DUMMY     21108                   //Server side effect
#define SPELL_RAGSUBMERGE           21107                   //Stealth aura
#define SPELL_RAGEMERGE             20568
#define SPELL_MELTWEAPON            21388
#define SPELL_ELEMENTALFIRE         20564
#define SPELL_ERRUPTION             17731
#define SPELL_LAVASHIELD            21857
#define SPELL_HAND_OF_RAGNAROS      21154                   //might of ragnaros???

#define MAX_ADDS_IN_SUBMERGE        8
#define NPC_SON_OF_FLAME            12143

#define TIMER_PHASE1                180000
#define TIMER_PHASE2                90000

struct Coords
{
    float x;
    float y;
    float z;
    float o;
};

static Coords RagnaGO[] =
{
    {829.867f, -877.96f, -227.096f, 2.9048f},
    {861.73f, -868.952f, -227.484f, 3.36269f},
    {872.134f, -854.459f, -227.479f, 4.36879f},
    {871.205f, -826.667f, -227.797f, 4.85104f},
    {861.927f, -802.512f, -227.119f, 5.25709f},
    {834.44f, -797.015f, -227.81f, 6.05427f},
    {818.413f, -807.652f, -227.479f, 2.43516f},
    {810.823f, -824.293f, -227.252f, 2.97938f}
};

class ThreatListCopier : public ThreatListProcesser
{
public:
    ThreatListCopier(Unit* destination) : _dest(destination) {}
    bool Process(Unit* unit)
    {
        _dest->AI()->AttackStart(unit);
        return false;
    }
    Unit* _dest;
};

struct boss_ragnarosAI : public ScriptedAI
{
    boss_ragnarosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        SetCombatMovement(false);
        Reset();
    }

    uint32 WrathOfRagnaros_Timer;
    uint32 HandOfRagnaros_Timer;
    uint32 MagmaBurst_Timer;
    uint32 ElementalFire_Timer;
    uint32 Eruption_Timer;
    uint32 Submerge_Timer;
    uint32 Attack_Timer;

    bool HasYelledMagmaBurst;
    bool HasSubmergedOnce;
    bool IsBanished;
    bool HasAura;
    bool Explosion;

    ScriptedInstance* m_pInstance;

    void Reset()
    {
        WrathOfRagnaros_Timer = 30000;
        HandOfRagnaros_Timer = 25000;
        MagmaBurst_Timer = 2000;
        Eruption_Timer = (10 + rand() % 20) * 1000;
        ElementalFire_Timer = 3000;
        Submerge_Timer = TIMER_PHASE1; // P1 dure 3 min
        Attack_Timer = TIMER_PHASE2;    // P2 dure 1min30 max
        HasYelledMagmaBurst = false;
        HasSubmergedOnce = false;
        IsBanished = false;
        Explosion = false;

        m_creature->CastSpell(m_creature, SPELL_MELTWEAPON, true);
        HasAura = true;

        if (m_pInstance && m_creature->isAlive())
        {
            m_pInstance->SetData(TYPE_RAGNAROS, NOT_STARTED);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        }
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_RAGNAROS, IN_PROGRESS);
            if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE))
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAGNAROS, DONE);
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (done_by && urand(0, 100) < 15 && done_by->GetDistance2d(m_creature) < 5.0f)
            m_creature->CastSpell(done_by, 21388, true);
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetEntry() == 12018)
            return;

        DoScriptText(SAY_KILL, m_creature);
    }

    void SummonSonOfFlame(int i)
    {
        // Note : il existe des sorts pour cela. "Invocation du Fils de la Flamme A" jusqu'a H (spell 21110 -> 21117)
        // A quoi servent-ils ?
        ASSERT(i < MAX_ADDS_IN_SUBMERGE);
        ThreatListCopier* dataCopier = new ThreatListCopier(m_creature);
        if (Creature* Crea = m_creature->SummonCreature(NPC_SON_OF_FLAME, RagnaGO[i].x, RagnaGO[i].y, RagnaGO[i].z - 2.0f, RagnaGO[i].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
        {
            Crea->CastSpell(Crea, SPELL_LAVASHIELD, true);
            m_creature->ProcessThreatList(dataCopier);
            if (Unit* randomTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                Crea->getThreatManager().modifyThreatPercent(randomTarget, 90);
                Crea->AI()->AttackStart(randomTarget);
                Crea->GetMotionMaster()->MoveChase(randomTarget);
            }
        }
        delete dataCopier;
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_creature->GetUInt32Value(UNIT_FIELD_FLAGS) == UNIT_FLAG_NON_ATTACKABLE)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE))
            return;

        if (Eruption_Timer < diff)
        {
            uint32 Rand = rand() % 8;
            if (GameObject* Gob = m_creature->SummonGameObject(178088, RagnaGO[Rand].x, RagnaGO[Rand].y, RagnaGO[Rand].z, RagnaGO[Rand].o, 0, 0, 0, 0, 0))
            {
                Gob->Use(m_creature);
                if (Explosion)
                {
                    Eruption_Timer = (10 + rand() % 20) * 1000;
                    Explosion = false;
                }
                else
                {
                    Eruption_Timer = 1000;
                    Explosion = true;
                }
            }
        }
        else
            Eruption_Timer -= diff;

        if (IsBanished && Attack_Timer < diff)
        {
            if (m_creature->HasAura(SPELL_RAGSUBMERGE))
                m_creature->RemoveAurasDueToSpell(SPELL_RAGSUBMERGE);
            if (DoCastSpellIfCan(m_creature, SPELL_RAGEMERGE) == CAST_OK)
            {
                //Become unbanished again
                m_creature->setFaction(14);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                IsBanished = false;
            }
            else
                sLog.outError("[MC/Ragna] Cast %u impossible.", SPELL_RAGEMERGE);
        }
        else if (IsBanished)
        {
            Attack_Timer -= diff;
            bool Allbanished = true;
            std::list<Creature*> FilsListe;
            GetCreatureListWithEntryInGrid(FilsListe, m_creature, NPC_SON_OF_FLAME, 150.0f);

            for (std::list<Creature*>::iterator itr = FilsListe.begin(); itr != FilsListe.end(); ++itr)
            {
                if ((*itr)->isAlive())
                {
                    if (!(*itr)->hasUnitState(UNIT_STAT_ISOLATED)) // banished
                    {
                        Allbanished = false;
                        break;
                    }
                }
            }
            if (Allbanished)
                Attack_Timer = 0;

            //Do nothing while banished
            if (!m_creature->HasAura(SPELL_RAGSUBMERGE))
                m_creature->CastSpell(m_creature, SPELL_RAGSUBMERGE, false);
            return;
        }

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;


        //Submerge_Timer
        if (!IsBanished && Submerge_Timer < diff)
        {
            //Creature spawning and ragnaros becomming unattackable
            //is not very well supported in the core
            //so added normaly spawning and banish workaround and attack again after 90 secs.

            m_creature->InterruptNonMeleeSpells(false);
            //Root self
            DoCastSpellIfCan(m_creature, 23973);
            m_creature->setFaction(35);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);
            m_creature->CastSpell(m_creature, SPELL_RAGSUBMERGE, true);

            if (!HasSubmergedOnce)
            {
                DoScriptText(SAY_REINFORCEMENTS1, m_creature);

                for (uint8 i = 0; i < MAX_ADDS_IN_SUBMERGE; ++i)
                    SummonSonOfFlame(i);
            }
            else
            {
                DoScriptText(SAY_REINFORCEMENTS2, m_creature);

                for (uint8 i = 0; i < MAX_ADDS_IN_SUBMERGE; ++i)
                    SummonSonOfFlame(i);
            }
            HasSubmergedOnce = true;
            IsBanished = true;
            Attack_Timer = TIMER_PHASE2;
            Submerge_Timer = TIMER_PHASE1;
        }
        else if (Submerge_Timer >= diff)
            Submerge_Timer -= diff;

        Map::PlayerList const &liste = m_creature->GetMap()->GetPlayers();
        bool CacPresents = false;
        //If we are within range melee the target
        if (m_creature->IsWithinMeleeRange(m_creature->getVictim()) && m_creature->IsWithinLOSInMap(m_creature->getVictim()))
        {
            //Make sure our attack is ready and we arn't currently casting
            if (m_creature->isAttackReady() && !m_creature->IsNonMeleeSpellCasted(false))
            {
                m_creature->AttackerStateUpdate(m_creature->getVictim());
                m_creature->resetAttackTimer();
                if (HasYelledMagmaBurst)
                {
                    HasYelledMagmaBurst = false;
                    MagmaBurst_Timer = 5000;
                }
            }
            CacPresents = true;
        }
        else for (Map::PlayerList::const_iterator i = liste.begin(); i != liste.end(); ++i)
            {
                if (i->getSource()->isGameMaster() || !i->getSource()->isAlive())
                    continue;
                if (!HasYelledMagmaBurst)
                {
                    DoScriptText(SAY_MAGMABURST, m_creature);
                    HasYelledMagmaBurst = true;
                }
                if (m_creature->GetDistance2d(i->getSource()) > 5.0f)
                    m_creature->getThreatManager().modifyThreatPercent(i->getSource(), -100);
                else if (m_creature->IsWithinLOSInMap(i->getSource()))
                    CacPresents = true;
            }

        if (!CacPresents)
        {
            //MagmaBurst_Timer
            if (MagmaBurst_Timer < diff)
            {
                // Can target pets. Can't target totems.
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(target, SPELL_MAGMABURST) == CAST_OK)
                        MagmaBurst_Timer = 2500;
            }
            else
                MagmaBurst_Timer -= diff;
        }
        else
        {
            //WrathOfRagnaros_Timer
            if (WrathOfRagnaros_Timer < diff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_WRATHOFRAGNAROS) == CAST_OK)
                {
                    WrathOfRagnaros_Timer = 28000;
                    DoScriptText(SAY_WRATH, m_creature);
                }
            }
            else
                WrathOfRagnaros_Timer -= diff;

            //HandOfRagnaros_Timer
            if (HandOfRagnaros_Timer < diff)
            {
                std::vector<Player*> manaPlayers;

                ThreatList const& tList = m_creature->getThreatManager().getThreatList();
                for (ThreatList::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                {
                    Player* pPlayer = m_creature->GetMap()->GetPlayer((*itr)->getUnitGuid());
                    if (pPlayer && pPlayer->isAlive() && pPlayer->getPowerType() == POWER_MANA && !pPlayer->isGameMaster() && pPlayer->GetDistance2d(m_creature) > 5.0f)
                        manaPlayers.push_back(pPlayer);
                }
                if (!manaPlayers.empty())
                {
                    if (Player* pTarget = manaPlayers[urand(0, manaPlayers.size() - 1)])
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_HAND_OF_RAGNAROS) == CAST_OK)
                        {
                            m_creature->SetFacingToObject(pTarget);
                            DoScriptText(SAY_HAND, m_creature);
                            HandOfRagnaros_Timer = 25000;
                        }
                    }
                }
                //===============================================================
            }
            else
                HandOfRagnaros_Timer -= diff;

            //ElementalFire_Timer
            if (ElementalFire_Timer < diff)
            {
                if (m_creature->getVictim())
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ELEMENTALFIRE, CAST_AURA_NOT_PRESENT) == CAST_OK)
                        ElementalFire_Timer = urand(10000, 14000);
            }
            else
                ElementalFire_Timer -= diff;
        }
    }
};

CreatureAI* GetAI_boss_ragnaros(Creature* pCreature)
{
    return new boss_ragnarosAI(pCreature);
}

struct boss_flame_of_ragnarosAI : public ScriptedAI
{
    boss_flame_of_ragnarosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        SetCombatMovement(false);
    }

    ScriptedInstance* m_pInstance;
    bool Explode;

    void Reset()
    {
        m_creature->addUnitState(UNIT_STAT_ROOT);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetLevel(63);
        m_creature->setFaction(14);
        m_creature->CastSpell(m_creature, 21155, false);
        Explode = false;
    }

    void SpellHitTarget(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == 21155)
            Explode = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (Explode == true)
            m_creature->CastSpell(m_creature, 5, true);
    }
};

CreatureAI* GetAI_boss_flame_of_ragnaros(Creature* pCreature)
{
    return new boss_flame_of_ragnarosAI(pCreature);
}

void AddSC_boss_ragnaros()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_ragnaros";
    newscript->GetAI = &GetAI_boss_ragnaros;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_flame_of_ragnaros";
    newscript->GetAI = &GetAI_boss_flame_of_ragnaros;
    newscript->RegisterSelf();
}
