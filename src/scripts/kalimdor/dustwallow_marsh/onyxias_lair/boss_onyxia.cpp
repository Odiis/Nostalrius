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
SDName: Boss_Onyxia
SD%Complete: 65
SDComment: Phase 3 need additianal code. Phase 2 requires entries in spell_target_position with specific locations. See bottom of file.
SDCategory: Onyxia's Lair
EndScriptData */

#include "scriptPCH.h"
#include "instance_onyxia_lair.h"

enum
{
    SAY_AGGRO                   = -1249000,
    SAY_KILL                    = -1249001,
    SAY_PHASE_2_TRANS           = -1249002,
    SAY_PHASE_3_TRANS           = -1249003,
    EMOTE_BREATH                = -1249004,
    EMOTE_ROAR                  = -1249005,

    SPELL_WINGBUFFET            = 18500,
    SPELL_FLAMEBREATH           = 18435,
    SPELL_CLEAVE                = 19983,
    SPELL_TAILSWEEP             = 15847,
    SPELL_KNOCK_AWAY            = 19633,

    SPELL_ENGULFINGFLAMES       = 20019,
    SPELL_DEEPBREATH            = 23461, // + Apply 6sec aura DoT
    SPELL_FIREBALL              = 18392,

    SPELL_BREATH_NORTH_TO_SOUTH = 17086,                  // 20x in "array"
    SPELL_BREATH_SOUTH_TO_NORTH = 18351,                  // 11x in "array"

    SPELL_BREATH_EAST_TO_WEST   = 18576,                    // 7x in "array"
    SPELL_BREATH_WEST_TO_EAST   = 18609,                    // 7x in "array"

    SPELL_BREATH_SE_TO_NW       = 18564,                    // 12x in "array"
    SPELL_BREATH_NW_TO_SE       = 18584,                    // 12x in "array"

    SPELL_BREATH_SW_TO_NE       = 18596,                    // 12x in "array"
    SPELL_BREATH_NE_TO_SW       = 18617,                    // 12x in "array"

    //SPELL_BREATH                = 21131,                  // 8x in "array", different initial cast than the other arrays

    SPELL_BELLOWINGROAR         = 18431,
    //SPELL_HEATED_GROUND         = 22191,

    SPELL_SUMMONWHELP           = 17646,
    NPC_WHELP                   = 11262,

    DEPART_VOL                  = 20,
    ATTERISSAGE_VOL             = 21
};

enum
{
    ONYXIA_P1 = 1,
    ONYXIA_P2 = 2,
    ONYXIA_P3 = 3,
};

struct sOnyxMove
{
    uint32 uiLocId;
    uint32 uiLocIdEnd;
    uint32 uiSpellId;
    float fX, fY, fZ, fZGround;
};

static sOnyxMove aMoveData[] =
{
    {0, 4, SPELL_BREATH_NE_TO_SW,        10.2191f, -247.912f, -71.896f,  -85.84668f},
    {1, 5, SPELL_BREATH_EAST_TO_WEST,   -31.4963f, -250.123f, -75.1278f, -89.127853f},
    {2, 6, SPELL_BREATH_SE_TO_NW,       -63.5156f, -240.096f, -72.477f,  -85.066696f},
    {3, 7, SPELL_BREATH_SOUTH_TO_NORTH, -65.8444f, -213.809f, -69.655f,  -84.298462f},
    {4, 0, SPELL_BREATH_SW_TO_NE,       -58.2509f, -189.020f, -72.790f,  -85.292267f},
    {5, 1, SPELL_BREATH_WEST_TO_EAST,   -33.5561f, -182.682f, -74.9457f, -88.945686f},
    {6, 2, SPELL_BREATH_NW_TO_SE,         6.8951f, -180.246f, -71.896f,  -85.634293f},
    {7, 3, SPELL_BREATH_NORTH_TO_SOUTH,  22.8763f, -217.152f, -71.896f,  -85.054054f},
};

static float afSpawnLocations[2][3] =
{
    { -30.127f, -254.463f, -89.440f},
    { -30.817f, -177.106f, -89.258f}
};

struct boss_onyxiaAI : public ScriptedAI
{
    boss_onyxiaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 m_uiPhase;
    bool transitoire;
    uint32 TransTimer;
    uint32 TransCount;

    bool EruptPhase;
    uint32 EruptTimer;

    uint32 SummonCount;
    uint32 PhaseDeuxVar;
    uint32 PhaseDeuxTimer;

    uint32 m_uiFlameBreathTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiWingBuffetTimer;
    uint32 m_uiKnockAwayTimer;

    int m_uiMovePoint;
    uint32 m_uiMovementTimer;
    sOnyxMove* m_pPointData;
    bool Move;

    uint32 m_uiEngulfingFlamesTimer;
    uint32 m_uiSummonWhelpsTimer;
    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiWhelpTimer;

    uint8 m_uiSummonCount;
    bool m_bIsSummoningWhelps;

    ScriptedInstance* m_pInstance;
    std::list<GameObject*> GOListe;

    void Reset()
    {
        if (!IsCombatMovement())
            SetCombatMovement(true);

        m_creature->SetSpeedRate(MOVE_RUN, 1.14286f, true);
        m_uiPhase = ONYXIA_P1;
        transitoire = false;
        TransTimer = 0;
        TransCount = 0;

        EruptPhase = false;
        EruptTimer = 15000;

        SummonCount = 16;
        PhaseDeuxVar = 0;
        PhaseDeuxTimer = 10000;

        m_uiFlameBreathTimer = urand(10000, 20000);
        m_uiTailSweepTimer = 5000; //urand(15000, 20000);
        m_uiCleaveTimer = urand(2000, 5000);
        m_uiWingBuffetTimer = urand(10000, 20000);
        m_uiKnockAwayTimer = urand(15000, 25000);

        m_uiMovePoint = urand(0, 5);
        m_uiMovementTimer = 20000;
        m_pPointData = GetMoveData();
        Move = false;

        m_uiEngulfingFlamesTimer = 15000;
        m_uiSummonWhelpsTimer = 5000;
        m_uiBellowingRoarTimer = 30000;
        m_uiWhelpTimer = 1000;

        m_uiSummonCount = 0;
        m_bIsSummoningWhelps = false;
        if (m_pInstance)
            m_pInstance->SetData(DATA_ONYXIA_EVENT, NOT_STARTED);

        // remise en mode "dort"
        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
        m_creature->SetFly(false);
        m_creature->SetWalk(true);
        m_creature->SetLevitate(false);
    }

    void DelayEventIfNeed(uint32& event, uint32 delay)
    {
        if (event < (delay + 150)) // Le temps s'ecoule par tranches de 150ms
            event = delay + 150;
    }

    void DelayCastEvents(uint32 delay)
    {
        DelayEventIfNeed(m_uiFlameBreathTimer, delay);
        DelayEventIfNeed(m_uiTailSweepTimer, delay);
        DelayEventIfNeed(m_uiCleaveTimer, delay);
        DelayEventIfNeed(m_uiWingBuffetTimer, delay);
        DelayEventIfNeed(m_uiKnockAwayTimer, delay);
    }

    void SpellHitTarget(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_BELLOWINGROAR)
        {
            if (EruptPhase == false)
            {
                GetGameObjectListWithEntryInGrid(GOListe, m_creature, 177985, 100.0f);
                GetGameObjectListWithEntryInGrid(GOListe, m_creature, 177984, 100.0f);
            }
            EruptPhase = true;
            EruptTimer = 500;
        }
    }

    void Aggro(Unit* pWho)
    {
        //: Fix orientation.
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(DATA_ONYXIA_EVENT, IN_PROGRESS);

        std::list<Creature*> GardiensListe;
        GetCreatureListWithEntryInGrid(GardiensListe, m_creature, 12129, 200.0f);
        for (std::list<Creature*>::iterator itr = GardiensListe.begin(); itr != GardiensListe.end(); ++itr)
            if (!(*itr)->isAlive())
                (*itr)->Respawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_ONYXIA_EVENT, DONE);

        while (Creature* Add = m_creature->FindNearestCreature(12129, 200.0f, true))
            Add->DisappearAndDie();
    }

    bool isOnyxiaFlying()
    {
        return m_creature->HasAura(17131);
    }

    void JustSummoned(Creature *pSummoned)
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);

        ++m_uiSummonCount;
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    sOnyxMove* GetMoveData()
    {
        uint32 uiMaxCount = sizeof(aMoveData) / sizeof(sOnyxMove);
        for (uint32 i = 0; i < uiMaxCount; ++i)
        {
            if (aMoveData[i].uiLocId == m_uiMovePoint)
                return &aMoveData[i];
        }

        return NULL;
    }

    bool SetNextRandomPoint()
    {
        bool Souffle = false;
        switch (rand() % 3)
        {
            case 0:
                m_creature->SetSpeedRate(MOVE_RUN, 1.14286f, true);
                m_uiMovePoint = (m_uiMovePoint - 1) % 8;
                break;
            case 1:
                m_creature->SetSpeedRate(MOVE_RUN, 1.14286f, true);
                m_uiMovePoint = (m_uiMovePoint + 1) % 8;
                break;
            case 2:
                m_uiMovePoint = (m_uiMovePoint + 4) % 8;
                m_creature->SetSpeedRate(MOVE_RUN, 3.0f, true);
                DoScriptText(EMOTE_BREATH, m_creature);
                m_creature->CastSpell(m_creature, m_pPointData->uiSpellId, true);
                Souffle = true;
                break;
        }
        if (m_uiMovePoint < 0)
            m_uiMovePoint = 7;
        if (m_uiMovePoint > 7)
            m_uiMovePoint = 0;
        return Souffle;
    }

    void PhaseUn(uint32 uiDiff)
    {
        if (m_creature->IsFlying())
        {
            m_creature->SetWalk(true);
            m_creature->SetFly(false);
            m_creature->SetLevitate(false);
        }

        if (m_uiFlameBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAMEBREATH) == CAST_OK)
            {
                DelayCastEvents(2000); // 2sec de cast
                m_uiFlameBreathTimer = urand(10000, 20000);
            }
        }
        else
            m_uiFlameBreathTimer -= uiDiff;

        if (m_uiTailSweepTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TAILSWEEP) == CAST_OK)
                m_uiTailSweepTimer = 5000; //urand(15000, 20000);
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(2000, 5000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiWingBuffetTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WINGBUFFET) == CAST_OK)
            {
                DelayCastEvents(1500); // 1.5 sec de cast
                m_uiWingBuffetTimer = urand(15000, 30000);
            }
        }
        else
            m_uiWingBuffetTimer -= uiDiff;

        if (m_uiKnockAwayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY) == CAST_OK)
            {
                m_uiKnockAwayTimer = urand(15000, 30000);
                m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -25);
            }
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void PhaseDeux(uint32 uiDiff)
    {
        if (Move == true)
        {
            if (PhaseDeuxTimer < 5000)
            {
                m_pPointData = GetMoveData();
                if (!m_pPointData)
                    return;
                else
                    m_creature->GetMotionMaster()->MovePoint(m_pPointData->uiLocId, m_pPointData->fX, m_pPointData->fY, m_pPointData->fZ, MOVE_PATHFINDING | MOVE_FLY_MODE);

                PhaseDeuxVar = 0;
                PhaseDeuxTimer += 2000;
                Move = false;
            }
            else PhaseDeuxTimer -= uiDiff;
        }
        else if (PhaseDeuxTimer < uiDiff)
        {
            switch (PhaseDeuxVar)
            {
                case 0:
                    BouleDeFeu();
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                    if (rand() % 2)
                    {
                        if (BouleDeFeu()) break;
                        else PhaseDeuxVar--;
                    }
                    else Deplacement();
                    break;
                case 5:
                    Deplacement();
                    break;
            }
        }
        else PhaseDeuxTimer -= uiDiff;

        if (m_bIsSummoningWhelps == true)
        {
            if (m_uiSummonCount < SummonCount)
            {
                if (m_uiWhelpTimer < uiDiff)
                {
                    m_creature->SummonCreature(NPC_WHELP, afSpawnLocations[0][0], afSpawnLocations[0][1], afSpawnLocations[0][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                    m_creature->SummonCreature(NPC_WHELP, afSpawnLocations[1][0], afSpawnLocations[1][1], afSpawnLocations[1][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                    m_uiWhelpTimer = 1000;
                }
                else
                    m_uiWhelpTimer -= uiDiff;
            }
            else
            {
                m_bIsSummoningWhelps = false;
                m_uiSummonCount = 0;
                SummonCount = 5 + rand() % 2;
                m_uiSummonWhelpsTimer = 30000;
            }
        }
        else
        {
            if (m_uiSummonWhelpsTimer < uiDiff)
                m_bIsSummoningWhelps = true;
            else
                m_uiSummonWhelpsTimer -= uiDiff;
        }
    }

    bool BouleDeFeu()
    {
        PhaseDeuxVar++;
        PhaseDeuxTimer = 5000;
        std::vector<Player*> playersList;
        Map::PlayerList const &liste = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator iter = liste.begin(); iter != liste.end(); ++iter)
        {
            if (iter->getSource()->isAlive())
                playersList.push_back(iter->getSource());
        }
        if (playersList.empty())
            return false;
        uint32 rnd = urand(0, playersList.size() - 1);
        Player* pPlayer = playersList[rnd];
        if (pPlayer)
        {
            if (DoCastSpellIfCan(pPlayer, SPELL_FIREBALL) == CAST_OK)
            {
                DoResetThreat();
                m_creature->getThreatManager().addThreat(pPlayer, 10000.0f);
                m_creature->SetFacingToObject(pPlayer);
                return true;
            }
        }
        return false;
    }

    void Deplacement()
    {
        m_pPointData = GetMoveData();
        Move = true;

        if (SetNextRandomPoint())
            PhaseDeuxTimer = 10000;
        else PhaseDeuxTimer = 5000;
    }

    void PhaseTrois(uint32 uiDiff)
    {
        if (m_bIsSummoningWhelps == true)
        {
            if (m_uiSummonCount < SummonCount)
            {
                if (m_uiWhelpTimer < uiDiff)
                {
                    m_creature->SummonCreature(NPC_WHELP, afSpawnLocations[0][0], afSpawnLocations[0][1], afSpawnLocations[0][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                    m_creature->SummonCreature(NPC_WHELP, afSpawnLocations[1][0], afSpawnLocations[1][1], afSpawnLocations[1][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                    m_uiWhelpTimer = 1000;
                }
                else
                    m_uiWhelpTimer -= uiDiff;
            }
            else
            {
                m_bIsSummoningWhelps = false;
                m_uiSummonCount = 0;
                SummonCount = 4 + rand() % 3;
                m_uiSummonWhelpsTimer = 60000;
            }
        }
        else
        {
            if (m_uiSummonWhelpsTimer < uiDiff)
                m_bIsSummoningWhelps = true;
            else
                m_uiSummonWhelpsTimer -= uiDiff;
        }

        if (m_uiBellowingRoarTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BELLOWINGROAR, true) == CAST_OK)
            {
                DoScriptText(EMOTE_ROAR, m_creature);
                m_uiBellowingRoarTimer = 30000;
                // Ne pas etre interrompu par d'autres casts.
                DelayCastEvents(2000);
            }
        }
        else
            m_uiBellowingRoarTimer -= uiDiff;

        if (EruptPhase == true)
        {
            if (EruptTimer < uiDiff)
            {
                EruptTimer = 500;
                if (Erupt() == false)
                {
                    EruptPhase = false;
                    EruptTimer = 100000;
                }
            }
            else
                EruptTimer -= uiDiff;
        }
        PhaseUn(uiDiff);
    }

    bool Erupt()
    {
        uint32 Var = 0;
        while (Var < 10 && !GOListe.empty())
        {
            std::list<GameObject*>::iterator itr = GOListe.begin();
            std::advance(itr, rand() % GOListe.size());
            if (GameObject* GO = *itr)
            {
                GO->Use(m_creature);
                if (Creature* Cre = m_creature->SummonCreature(20006, GO->GetPositionX(), GO->GetPositionY(), GO->GetPositionZ(), GO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 2000))
                {
                    Cre->SetName("Onyxia");
                    Cre->SetLevel(63);
                    Cre->CastSpell(Cre, 17731, true);
                    Cre->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    Cre->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    Cre->AI()->EnterEvadeMode();
                }
                GOListe.erase(itr);
                Var++;
            }
        }
        if (GOListe.empty())
            return false;
        else return true;
    }

    void PhaseTransitoire(uint32 uiDiff, bool Debut)
    {
//        m_creature->CombatStop(true);
        m_creature->clearUnitState(UNIT_STAT_MELEE_ATTACKING);

        /** If a new phase begins, reset aggro + ignore new aggro */
        if (Debut == true)
        {
            Map::PlayerList const &liste = m_creature->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator i = liste.begin(); i != liste.end(); ++i)
                if (m_creature->GetDistance2d(i->getSource()) > 5.0f)
                    m_creature->getThreatManager().modifyThreatPercent(i->getSource(), -100);
        }
        /** P2 Event to take off */
        if (m_uiPhase == ONYXIA_P2)
        {
            /** Stop combat, and move to the last ground waypoint before taking off */
            if (Debut == true && TransCount == 0)
            {
                SetCombatMovement(false);
                //m_creature->HandleEmote(0);
                DoScriptText(SAY_PHASE_2_TRANS, m_creature);
                m_creature->InterruptNonMeleeSpells(false);

                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();

                m_creature->SetSpeedRate(MOVE_RUN, 1.14286f, true);

                m_creature->GetMotionMaster()->MovePoint(DEPART_VOL, -57.750641f, -215.610077f, -85.094727f, MOVE_PATHFINDING);
                TransTimer = 60000; // Cf MovementInform normalement
            }
            /** Take off in progress */
            else if (TransTimer < uiDiff && TransCount == 1)
            {
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->GetMotionMaster()->MovePoint(0, -57.204933f, -215.592148f, -85.156929f, MOVE_PATHFINDING | MOVE_FLY_MODE);
                TransTimer = 2000;
                TransCount = 2;
            }
            /** Fly mode added, move to the first random point */
            else if (TransTimer < uiDiff && TransCount == 2)
            {
                m_creature->CastSpell(m_creature, 17131, true); /** Start flying */
                transitoire = false;
                TransTimer = 0;
                float X = m_creature->GetPositionX() - m_pPointData->fX;
                float Y = m_creature->GetPositionY() - m_pPointData->fY;
                PhaseDeuxTimer = sqrt((X * X) + (Y * Y)) / (m_creature->GetSpeed(MOVE_RUN) * 0.001f);

                if (rand() % 2) m_uiMovePoint = 0;
                else m_uiMovePoint = 5;

                m_pPointData = GetMoveData();

                m_creature->GetMotionMaster()->MovePoint(m_pPointData->uiLocId, m_pPointData->fX, m_pPointData->fY, m_pPointData->fZ, MOVE_PATHFINDING | MOVE_FLY_MODE);
            }
        }
        /** P3 event to land */
        else if (m_uiPhase == ONYXIA_P3)
        {
            if (Debut == true && TransCount == 2)
            {
                m_creature->InterruptNonMeleeSpells(false);
                m_pPointData = GetMoveData();
                m_creature->GetMotionMaster()->MovePoint(ATTERISSAGE_VOL, m_pPointData->fX, m_pPointData->fY, m_pPointData->fZGround, MOVE_FLY_MODE);
                DoScriptText(SAY_PHASE_3_TRANS, m_creature);
                TransTimer = 60000; // Cf MovementInform normalement
            }
            else if (TransTimer < uiDiff && TransCount == 3)
            {
                m_creature->SetLevitate(false);
                DoResetThreat();
                TransCount = 4;
            }
            else if (TransTimer < uiDiff && TransCount == 4)
            {
                m_creature->RemoveAurasDueToSpell(17131); /** Stop flying */
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                transitoire = false;
                TransTimer = 0;
                m_uiBellowingRoarTimer = 5000;
                m_uiFlameBreathTimer = urand(10000, 20000);
                m_uiTailSweepTimer = urand(15000, 20000);
                m_uiCleaveTimer = urand(2000, 5000);
                m_uiWingBuffetTimer = urand(10000, 20000);
                m_uiKnockAwayTimer = urand(15000, 25000);
            }
        }

        if (TransTimer >= uiDiff)
            TransTimer -= uiDiff;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == DEPART_VOL)
        {
            m_creature->SetOrientation(0.0f);
            TransTimer = 1000;
            TransCount = 1;
            m_creature->SetFly(true);
            m_creature->SetLevitate(true);
            m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
        }
        else if (uiPointId == ATTERISSAGE_VOL)
        {
            m_creature->SetOrientation(0.0f);
            TransTimer = 1000;
            TransCount = 3;
            m_creature->SetFly(false);
            m_creature->SetLevitate(false);
            m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if ((m_uiPhase == ONYXIA_P2 || m_uiPhase == ONYXIA_P3) && transitoire)
        {
            PhaseTransitoire(uiDiff, false);
            return;
        }

        if (Unit* target = m_creature->getVictim())
        {
            /** Teleport victim if too far away from Onyxia */
            if ((!target->isInAccessablePlaceFor(m_creature) ||
                    m_creature->GetDistance2d(target) > 120.0f) &&
                    m_uiPhase != ONYXIA_P2 && !isOnyxiaFlying())
            {
                float Xp = m_creature->GetPositionX() + 5.0f * sin(m_creature->GetOrientation());
                float Yp = m_creature->GetPositionY() + 5.0f * cos(m_creature->GetOrientation());
                float Zp = m_creature->GetPositionZ() + 5.0f;
                float Op = m_creature->GetOrientation() + M_PI;
                if (Op > 2 * M_PI)
                    Op -= 2 * M_PI;

                target->NearTeleportTo(Xp, Yp, Zp, Op);
            }
        }
        else if (m_creature->isInCombat() && m_creature->getThreatManager().getThreatList().empty())
        {
            EnterEvadeMode();
            m_creature->CombatStop();
        }

        if (!isOnyxiaFlying() && !(m_creature->SelectHostileTarget() && m_creature->getVictim()))
            return;

        /** Switch to P3 */
        if (m_creature->GetHealthPercent() < 40.0f && m_uiPhase == ONYXIA_P2 && !m_creature->IsMoving())
        {
            m_uiPhase = ONYXIA_P3;
            transitoire = true;
            PhaseTransitoire(0, true);
        }
        /** Switch to P2 */
        else if (m_creature->GetHealthPercent() < 65.0f && m_uiPhase == ONYXIA_P1)
        {
            m_uiPhase = ONYXIA_P2;
            transitoire = true;
            PhaseTransitoire(0, true);
        }

        /** TODO: Issue during transition => should ignore aggro, not really the case actually.
         * A Flags_extra in creature_template 0x00000002 is a solution, but cannot be modified actually.
         * Test to solve the problem by using the function StopAttacking() & StartAttacking() */
        if (transitoire == true)
            PhaseTransitoire(uiDiff, false);
        else
        {
            switch (m_uiPhase)
            {
                case ONYXIA_P1:
                    PhaseUn(uiDiff);
                    break;
                case ONYXIA_P2:
                    PhaseDeux(uiDiff);
                    break;
                case ONYXIA_P3:
                    PhaseTrois(uiDiff);
                    break;
                default:
                    break;
            }
        }
    }
};

CreatureAI* GetAI_boss_onyxiaAI(Creature* pCreature)
{
    return new boss_onyxiaAI(pCreature);
}

void AddSC_boss_onyxia()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_onyxia";
    newscript->GetAI = &GetAI_boss_onyxiaAI;
    newscript->RegisterSelf();
}

/*
-- SPELL_BREATH_EAST_TO_WEST
DELETE FROM spell_target_position WHERE id IN (18576, 18578, 18579, 18580, 18581, 18582, 18583);
INSERT INTO spell_target_position VALUES (18576, 249, -37.743851, -243.667923, -88.217651, 1.416);
INSERT INTO spell_target_position VALUES (18578, 249, -35.805332, -232.028900, -87.749153, 1.416);
INSERT INTO spell_target_position VALUES (18579, 249, -34.045738, -224.714661, -85.529465, 1.416);
INSERT INTO spell_target_position VALUES (18580, 249, -32.081570, -214.916962, -88.327438, 1.416);
INSERT INTO spell_target_position VALUES (18581, 249, -36.611721, -202.684677, -85.653786, 1.416);
INSERT INTO spell_target_position VALUES (18582, 249, -37.067261, -195.758652, -87.745834, 1.416);
INSERT INTO spell_target_position VALUES (18583, 249, -37.728523, -188.616806, -88.074898, 1.416);
-- SPELL_BREATH_WEST_TO_EAST
DELETE FROM spell_target_position WHERE id IN (18609, 18611, 18612, 18613, 18614, 18615, 18616);
INSERT INTO spell_target_position VALUES (18609, 249, -37.728523, -188.616806, -88.074898, 4.526);
INSERT INTO spell_target_position VALUES (18611, 249, -37.067261, -195.758652, -87.745834, 4.526);
INSERT INTO spell_target_position VALUES (18612, 249, -36.611721, -202.684677, -85.653786, 4.526);
INSERT INTO spell_target_position VALUES (18613, 249, -32.081570, -214.916962, -88.327438, 4.526);
INSERT INTO spell_target_position VALUES (18614, 249, -34.045738, -224.714661, -85.529465, 4.526);
INSERT INTO spell_target_position VALUES (18615, 249, -35.805332, -232.028900, -87.749153, 4.526);
INSERT INTO spell_target_position VALUES (18616, 249, -37.743851, -243.667923, -88.217651, 4.526);
-- SPELL_BREATH_NW_TO_SE
DELETE FROM spell_target_position WHERE id IN (18584, 18585, 18586, 18587, 18588, 18589, 18590, 18591, 18592, 18593, 18594, 18595);
INSERT INTO spell_target_position VALUES (18584, 249, 6.016711, -181.305771, -85.654648, 3.776);
INSERT INTO spell_target_position VALUES (18585, 249, 3.860220, -183.227249, -86.375381, 3.776);
INSERT INTO spell_target_position VALUES (18586, 249, -2.529650, -188.690491, -87.172859, 3.776);
INSERT INTO spell_target_position VALUES (18587, 249, -8.449303, -193.957962, -87.564957, 3.776);
INSERT INTO spell_target_position VALUES (18588, 249, -14.321238, -199.462219, -87.922478, 3.776);
INSERT INTO spell_target_position VALUES (18589, 249, -15.602085, -216.893936, -88.403183, 3.776);
INSERT INTO spell_target_position VALUES (18590, 249, -23.650263, -221.969086, -89.172699, 3.776);
INSERT INTO spell_target_position VALUES (18591, 249, -29.495876, -213.014359, -88.910423, 3.776);
INSERT INTO spell_target_position VALUES (18592, 249, -35.439922, -217.260284, -87.336311, 3.776);
INSERT INTO spell_target_position VALUES (18593, 249, -41.762104, -221.896545, -86.114113, 3.776);
INSERT INTO spell_target_position VALUES (18594, 249, -51.067528, -228.909988, -85.765556, 3.776);
INSERT INTO spell_target_position VALUES (18595, 249, -56.559654, -241.223923, -85.423607, 3.776);
-- SPELL_BREATH_SE_TO_NW
DELETE FROM spell_target_position WHERE id IN (18564, 18565, 18566, 18567, 18568, 18569, 18570, 18571, 18572, 18573, 18574, 18575);
INSERT INTO spell_target_position VALUES (18564, 249, -56.559654, -241.223923, -85.423607, 0.666);
INSERT INTO spell_target_position VALUES (18565, 249, -51.067528, -228.909988, -85.765556, 0.666);
INSERT INTO spell_target_position VALUES (18566, 249, -41.762104, -221.896545, -86.114113, 0.666);
INSERT INTO spell_target_position VALUES (18567, 249, -35.439922, -217.260284, -87.336311, 0.666);
INSERT INTO spell_target_position VALUES (18568, 249, -29.495876, -213.014359, -88.910423, 0.666);
INSERT INTO spell_target_position VALUES (18569, 249, -23.650263, -221.969086, -89.172699, 0.666);
INSERT INTO spell_target_position VALUES (18570, 249, -15.602085, -216.893936, -88.403183, 0.666);
INSERT INTO spell_target_position VALUES (18571, 249, -14.321238, -199.462219, -87.922478, 0.666);
INSERT INTO spell_target_position VALUES (18572, 249, -8.449303, -193.957962, -87.564957, 0.666);
INSERT INTO spell_target_position VALUES (18573, 249, -2.529650, -188.690491, -87.172859, 0.666);
INSERT INTO spell_target_position VALUES (18574, 249, 3.860220, -183.227249, -86.375381, 0.666);
INSERT INTO spell_target_position VALUES (18575, 249, 6.016711, -181.305771, -85.654648, 0.666);
-- SPELL_BREATH_SW_TO_NE
DELETE FROM spell_target_position WHERE id IN (18596, 18597, 18598, 18599, 18600, 18601, 18602, 18603, 18604, 18605, 18606, 18607);
INSERT INTO spell_target_position VALUES (18596, 249, -58.250900, -189.020004, -85.292267, 5.587);
INSERT INTO spell_target_position VALUES (18597, 249, -52.006271, -193.796570, -85.808769, 5.587);
INSERT INTO spell_target_position VALUES (18598, 249, -46.135464, -198.548553, -85.901764, 5.587);
INSERT INTO spell_target_position VALUES (18599, 249, -40.500187, -203.001053, -85.555107, 5.587);
INSERT INTO spell_target_position VALUES (18600, 249, -30.907579, -211.058197, -88.592125, 5.587);
INSERT INTO spell_target_position VALUES (18601, 249, -20.098139, -218.681427, -88.937088, 5.587);
INSERT INTO spell_target_position VALUES (18602, 249, -12.223192, -224.666168, -87.856300, 5.587);
INSERT INTO spell_target_position VALUES (18603, 249, -6.475297, -229.098724, -87.076401, 5.587);
INSERT INTO spell_target_position VALUES (18604, 249, -2.010256, -232.541992, -86.995140, 5.587);
INSERT INTO spell_target_position VALUES (18605, 249, 2.736300, -236.202347, -86.790367, 5.587);
INSERT INTO spell_target_position VALUES (18606, 249, 7.197779, -239.642868, -86.307297, 5.587);
INSERT INTO spell_target_position VALUES (18607, 249, 12.120926, -243.439407, -85.874260, 5.587);
-- SPELL_BREATH_NE_TO_SW
DELETE FROM spell_target_position WHERE id IN (18617, 18619, 18620, 18621, 18622, 18623, 18624, 18625, 18626, 18627, 18628, 18618);
INSERT INTO spell_target_position VALUES (18617, 249, 12.120926, -243.439407, -85.874260, 2.428);
INSERT INTO spell_target_position VALUES (18619, 249, 7.197779, -239.642868, -86.307297, 2.428);
INSERT INTO spell_target_position VALUES (18620, 249, 2.736300, -236.202347, -86.790367, 2.428);
INSERT INTO spell_target_position VALUES (18621, 249, -2.010256, -232.541992, -86.995140, 2.428);
INSERT INTO spell_target_position VALUES (18622, 249, -6.475297, -229.098724, -87.076401, 2.428);
INSERT INTO spell_target_position VALUES (18623, 249, -12.223192, -224.666168, -87.856300, 2.428);
INSERT INTO spell_target_position VALUES (18624, 249, -20.098139, -218.681427, -88.937088, 2.428);
INSERT INTO spell_target_position VALUES (18625, 249, -30.907579, -211.058197, -88.592125, 2.428);
INSERT INTO spell_target_position VALUES (18626, 249, -40.500187, -203.001053, -85.555107, 2.428);
INSERT INTO spell_target_position VALUES (18627, 249, -46.135464, -198.548553, -85.901764, 2.428);
INSERT INTO spell_target_position VALUES (18628, 249, -52.006271, -193.796570, -85.808769, 2.428);
INSERT INTO spell_target_position VALUES (18618, 249, -58.250900, -189.020004, -85.292267, 2.428);

-- Below is not needed for current script
-- SPELL_BREATH_SOUTH_TO_NORTH
DELETE FROM spell_target_position WHERE id IN (18351, 18352, 18353, 18354, 18355, 18356, 18357, 18358, 18359, 18360, 18361);
INSERT INTO spell_target_position VALUES (18351, 249, -68.834236, -215.036163, -84.018875, 6.280);
INSERT INTO spell_target_position VALUES (18352, 249, -61.834255, -215.051910, -84.673416, 6.280);
INSERT INTO spell_target_position VALUES (18353, 249, -53.343277, -215.071014, -85.597191, 6.280);
INSERT INTO spell_target_position VALUES (18354, 249, -42.619305, -215.095139, -86.663605, 6.280);
INSERT INTO spell_target_position VALUES (18355, 249, -35.899323, -215.110245, -87.196548, 6.280);
INSERT INTO spell_target_position VALUES (18356, 249, -28.248341, -215.127457, -89.191750, 6.280);
INSERT INTO spell_target_position VALUES (18357, 249, -20.324360, -215.145279, -88.963997, 6.280);
INSERT INTO spell_target_position VALUES (18358, 249, -11.189384, -215.165833, -87.817093, 6.280);
INSERT INTO spell_target_position VALUES (18359, 249, -2.047405, -215.186386, -86.279655, 6.280);
INSERT INTO spell_target_position VALUES (18360, 249, 7.479571, -215.207809, -86.075531, 6.280);
INSERT INTO spell_target_position VALUES (18361, 249, 20.730539, -215.237610, -85.254387, 6.280);
-- SPELL_BREATH_NORTH_TO_SOUTH
DELETE FROM spell_target_position WHERE id IN (17086, 17087, 17088, 17089, 17090, 17091, 17092, 17093, 17094, 17095, 17097, 22267, 22268, 21132, 21133, 21135, 21136, 21137, 21138, 21139);
INSERT INTO spell_target_position VALUES (17086, 249, 20.730539, -215.237610, -85.254387, 3.142);
INSERT INTO spell_target_position VALUES (17087, 249, 7.479571, -215.207809, -86.075531, 3.142);
INSERT INTO spell_target_position VALUES (17088, 249, -2.047405, -215.186386, -86.279655, 3.142);
INSERT INTO spell_target_position VALUES (17089, 249, -11.189384, -215.165833, -87.817093, 3.142);
INSERT INTO spell_target_position VALUES (17090, 249, -20.324360, -215.145279, -88.963997, 3.142);
INSERT INTO spell_target_position VALUES (17091, 249, -28.248341, -215.127457, -89.191750, 3.142);
INSERT INTO spell_target_position VALUES (17092, 249, -35.899323, -215.110245, -87.196548, 3.142);
INSERT INTO spell_target_position VALUES (17093, 249, -42.619305, -215.095139, -86.663605, 3.142);
INSERT INTO spell_target_position VALUES (17094, 249, -53.343277, -215.071014, -85.597191, 3.142);
INSERT INTO spell_target_position VALUES (17095, 249, -61.834255, -215.051910, -84.673416, 3.142);
INSERT INTO spell_target_position VALUES (17097, 249, -68.834236, -215.036163, -84.018875, 3.142);
INSERT INTO spell_target_position VALUES (22267, 249, -75.736046, -214.984970, -83.394188, 3.142);
INSERT INTO spell_target_position VALUES (22268, 249, -84.087578, -214.857834, -82.640053, 3.142);
INSERT INTO spell_target_position VALUES (21132, 249, -90.424416, -214.601974, -82.482697, 3.142);
INSERT INTO spell_target_position VALUES (21133, 249, -96.572411, -214.353745, -82.239967, 3.142);
INSERT INTO spell_target_position VALUES (21135, 249, -102.069931, -214.131775, -80.571190, 3.142);
INSERT INTO spell_target_position VALUES (21136, 249, -107.385597, -213.917145, -77.447037, 3.142);
INSERT INTO spell_target_position VALUES (21137, 249, -114.281258, -213.866486, -73.851128, 3.142);
INSERT INTO spell_target_position VALUES (21138, 249, -123.328560, -213.607910, -71.559921, 3.142);
INSERT INTO spell_target_position VALUES (21139, 249, -130.788300, -213.424026, -70.751007, 3.142);
*/
