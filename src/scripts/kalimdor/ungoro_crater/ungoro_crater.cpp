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
SDName: Ungoro_Crater
SD%Complete: 100
SDComment: Quest support: 4245, 4491
SDCategory: Un'Goro Crater
EndScriptData */

/* ContentData
npc_ringo
EndContentData */

#include "scriptPCH.h"

/*######
## npc_ame01
######*/

enum
{
    SAY_AME_START           = -1000446,
    SAY_AME_PROGRESS        = -1000447,
    SAY_AME_END             = -1000448,
    SAY_AME_AGGRO1          = -1000449,
    SAY_AME_AGGRO2          = -1000450,
    SAY_AME_AGGRO3          = -1000451,

    QUEST_CHASING_AME       = 4245
};

struct npc_ame01AI : public npc_escortAI
{
    npc_ame01AI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 0:
                DoScriptText(SAY_AME_START, m_creature);
                break;
            case 19:
                DoScriptText(SAY_AME_PROGRESS, m_creature);
                break;
            case 37:
                DoScriptText(SAY_AME_END, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_CHASING_AME, m_creature);
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
            return;

        if (Player* pPlayer = GetPlayerForEscort())
        {
            if (pPlayer->getVictim() && pPlayer->getVictim() == pWho)
                return;

            switch (urand(0, 2))
            {
                case 0:
                    DoScriptText(SAY_AME_AGGRO1, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_AME_AGGRO2, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_AME_AGGRO3, m_creature);
                    break;
            }
        }
    }
};

bool QuestAccept_npc_ame01(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CHASING_AME)
    {
        if (npc_ame01AI* pAmeAI = dynamic_cast<npc_ame01AI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);

            if (pPlayer->GetTeam() == ALLIANCE)
                pCreature->setFaction(FACTION_ESCORT_A_PASSIVE);
            else if (pPlayer->GetTeam() == HORDE)
                pCreature->setFaction(FACTION_ESCORT_H_PASSIVE);

            pAmeAI->Start(false, false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_ame01(Creature* pCreature)
{
    return new npc_ame01AI(pCreature);
}

/*####
# npc_ringo
####*/

enum
{
    SAY_RIN_START_1             = -1000416,
    SAY_RIN_START_2             = -1000417,

    SAY_FAINT_1                 = -1000418,
    SAY_FAINT_2                 = -1000419,
    SAY_FAINT_3                 = -1000420,
    SAY_FAINT_4                 = -1000421,

    SAY_WAKE_1                  = -1000422,
    SAY_WAKE_2                  = -1000423,
    SAY_WAKE_3                  = -1000424,
    SAY_WAKE_4                  = -1000425,

    SAY_RIN_END_1               = -1000426,
    SAY_SPR_END_2               = -1000427,
    SAY_RIN_END_3               = -1000428,
    EMOTE_RIN_END_4             = -1000429,
    EMOTE_RIN_END_5             = -1000430,
    SAY_RIN_END_6               = -1000431,
    SAY_SPR_END_7               = -1000432,
    EMOTE_RIN_END_8             = -1000433,

    SPELL_REVIVE_RINGO          = 15591,
    QUEST_A_LITTLE_HELP         = 4491,
    NPC_SPRAGGLE                = 9997
};

struct npc_ringoAI : public FollowerAI
{
    npc_ringoAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiFaintTimer;
    uint32 m_uiEndEventProgress;
    uint32 m_uiEndEventTimer;

    Unit* pSpraggle;

    void Reset()
    {
        m_uiFaintTimer = urand(30000, 60000);
        m_uiEndEventProgress = 0;
        m_uiEndEventTimer = 1000;
        pSpraggle = NULL;
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_SPRAGGLE)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                if (Player* pPlayer = GetLeaderForFollower())
                {
                    if (pPlayer->GetQuestStatus(QUEST_A_LITTLE_HELP) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->GroupEventHappens(QUEST_A_LITTLE_HELP, m_creature);
                }

                pSpraggle = pWho;
                SetFollowComplete(true);
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (HasFollowState(STATE_FOLLOW_INPROGRESS | STATE_FOLLOW_PAUSED) && pSpell->Id == SPELL_REVIVE_RINGO)
            ClearFaint();
    }

    void SetFaint()
    {
        if (!HasFollowState(STATE_FOLLOW_POSTEVENT))
        {
            SetFollowPaused(true);

            switch (urand(0, 3))
            {
                case 0:
                    DoScriptText(SAY_FAINT_1, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_FAINT_2, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_FAINT_3, m_creature);
                    break;
                case 3:
                    DoScriptText(SAY_FAINT_4, m_creature);
                    break;
            }
        }

        //what does actually happen here? Emote? Aura?
        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
    }

    void ClearFaint()
    {
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        if (HasFollowState(STATE_FOLLOW_POSTEVENT))
            return;

        switch (urand(0, 3))
        {
            case 0:
                DoScriptText(SAY_WAKE_1, m_creature);
                break;
            case 1:
                DoScriptText(SAY_WAKE_2, m_creature);
                break;
            case 2:
                DoScriptText(SAY_WAKE_3, m_creature);
                break;
            case 3:
                DoScriptText(SAY_WAKE_4, m_creature);
                break;
        }

        SetFollowPaused(false);
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasFollowState(STATE_FOLLOW_POSTEVENT))
            {
                if (m_uiEndEventTimer < uiDiff)
                {
                    if (!pSpraggle || !pSpraggle->isAlive())
                    {
                        SetFollowComplete();
                        return;
                    }

                    switch (m_uiEndEventProgress)
                    {
                        case 1:
                            DoScriptText(SAY_RIN_END_1, m_creature);
                            m_uiEndEventTimer = 3000;
                            break;
                        case 2:
                            DoScriptText(SAY_SPR_END_2, pSpraggle);
                            m_uiEndEventTimer = 5000;
                            break;
                        case 3:
                            DoScriptText(SAY_RIN_END_3, m_creature);
                            m_uiEndEventTimer = 1000;
                            break;
                        case 4:
                            DoScriptText(EMOTE_RIN_END_4, m_creature);
                            SetFaint();
                            m_uiEndEventTimer = 9000;
                            break;
                        case 5:
                            DoScriptText(EMOTE_RIN_END_5, m_creature);
                            ClearFaint();
                            m_uiEndEventTimer = 1000;
                            break;
                        case 6:
                            DoScriptText(SAY_RIN_END_6, m_creature);
                            m_uiEndEventTimer = 3000;
                            break;
                        case 7:
                            DoScriptText(SAY_SPR_END_7, pSpraggle);
                            m_uiEndEventTimer = 10000;
                            break;
                        case 8:
                            DoScriptText(EMOTE_RIN_END_8, m_creature);
                            m_uiEndEventTimer = 5000;
                            break;
                        case 9:
                            SetFollowComplete();
                            break;
                    }

                    ++m_uiEndEventProgress;
                }
                else
                    m_uiEndEventTimer -= uiDiff;
            }
            else if (HasFollowState(STATE_FOLLOW_INPROGRESS))
            {
                if (!HasFollowState(STATE_FOLLOW_PAUSED))
                {
                    if (m_uiFaintTimer < uiDiff)
                    {
                        SetFaint();
                        m_uiFaintTimer = urand(60000, 120000);
                    }
                    else
                        m_uiFaintTimer -= uiDiff;
                }
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ringo(Creature* pCreature)
{
    return new npc_ringoAI(pCreature);
}

bool QuestAccept_npc_ringo(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_A_LITTLE_HELP)
    {
        if (npc_ringoAI* pRingoAI = dynamic_cast<npc_ringoAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pRingoAI->StartFollow(pPlayer, FACTION_ESCORT_N_FRIEND_PASSIVE, pQuest);
        }
    }

    return true;
}

// Elysium :
// ungoro_eggs_trigger
struct ungoro_eggs_triggerAI : public ScriptedAI
{
    ungoro_eggs_triggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    void Reset()
    {
        checkTimer = 10000;
        summon_x = m_creature->GetPositionX();
        summon_y = m_creature->GetPositionY();
        summon_z = m_creature->GetPositionZ();
        summon_o = m_creature->GetOrientation();
    }

    uint32 checkTimer;
    float summon_x, summon_y, summon_z, summon_o;

    Player* GetPlayerNear(float maxDist)
    {
        Map::PlayerList const &pl = m_creature->GetMap()->GetPlayers();
        uint32 myArea = m_creature->GetAreaId();
        for (Map::PlayerList::const_iterator it = pl.begin(); it != pl.end(); ++it)
        {
            Player* currPlayer =  it->getSource();
            if (currPlayer && m_creature->GetAreaId() == myArea && m_creature->IsWithinDist(currPlayer, maxDist, false))
                if (currPlayer->isAlive())
                    return currPlayer;
        }
        return NULL;
    }
    void UpdateAI(const uint32 diff)
    {
        if (checkTimer < diff)
        {
            Player* target = GetPlayerNear(10.0f);
            if (target)
            {
                m_creature->SummonCreature(9683, summon_x, summon_y, summon_z, summon_o, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000);
                //sLog.outString("Spawn 9683 !");
                // Histoire de pas respawn tout de suite un autre mob ...
                checkTimer = 60000;
            }
            else
            {
                //sLog.outString("No target :(");
                checkTimer = 10000;
            }
        }
        else
            checkTimer -= diff;
    }
};


CreatureAI* GetAI_eggs_trigger(Creature* pCreature)
{
    return new ungoro_eggs_triggerAI(pCreature);
}

// Elysium - quete épique chasseur

enum
{
    PRECIOUS_SPELL          = 23200,
    DESESPOIR_IDIOT         = 23503,
    BAISER_TENTATRICE       = 23205,
    TRAIT_FOUDRE            = 23206,
    SILENCE                 = 23207,
    //?ZERK?                = 23505,
    FOOL_PLIGHT             = 23504,
    SIMONE_DESPAWN_TIMER    = 2400000, //Shall despawn after 40 minutes if turned into a demon
    SIMONE_COMBAT_TIMER     = 1200000, //Combat shall not last more than 20 minutes

    NPC_CLEANER             = 14503,
    NPC_SIMONE              = 14527,
    NPC_PRECIOUS            = 14528,
    NPC_SIMONE_EVIL         = 14533,
    NPC_PRECIOUS_EVIL       = 14538
};

struct mob_SimoneAI : public ScriptedAI
{
    mob_SimoneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        _eventBegin       = false;
        _eventEnd         = true;
        _boredOrInterfere = false;
        _combatTimer      = 10000;
        _combatDespawnTimer = SIMONE_COMBAT_TIMER;
        _despawnTimer       = SIMONE_DESPAWN_TIMER;

        Reset();
    }

    bool isEngaged;
    bool _eventBegin;
    bool _eventEnd;
    bool _boredOrInterfere;
    uint32 _boltTimer;
    uint32 _combatTimer;
    uint32 _despawnTimer;
    uint32 _combatDespawnTimer;
    uint64 _playerGuid;
    uint32 _cleanerTimer;
    ObjectGuid _preciousGuid;
    ObjectGuid _cleanerGuid;


    /// Debug information
    void GetAIInformation(ChatHandler& out)
    {
        out.PSendSysMessage("event begin:%u end:%u interf:%u", _eventBegin, _eventEnd, _boredOrInterfere);
        out.PSendSysMessage("_combatTimer:%u _despawnTimer:%u _cleanerTimer:%u", _combatTimer, _despawnTimer, _cleanerTimer);
        out.PSendSysMessage("_preciousGuid:%s _cleanerGuid:%s", _preciousGuid.GetString().c_str(), _cleanerGuid.GetString().c_str());
    }

    void Reset()
    {
        isEngaged = false;
        _boltTimer = 15000;
        _cleanerTimer = 0;
        _playerGuid = 0;

        if (Creature* precious = m_creature->GetMap()->GetCreature(_preciousGuid))
            if (precious->isAlive())
                precious->DisappearAndDie();
        _preciousGuid.Clear();

        if (_boredOrInterfere || _eventEnd)
        {
            _eventBegin       = false;
            _eventEnd         = false;
            _boredOrInterfere = false;
            _combatTimer      = 10000;
            _despawnTimer     = SIMONE_DESPAWN_TIMER;

            m_creature->UpdateEntry(NPC_SIMONE);
            m_creature->SetHomePosition(-7070.37f, -933.852f, -269.139f, 3.21862f);
            m_creature->NearTeleportTo( -7070.37f, -933.852f, -269.139f, 3.21862f);
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
            m_creature->GetMotionMaster()->MoveWaypoint();
        }
        else if (_eventBegin)
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);
            m_creature->GetMotionMaster()->MoveIdle();

            if (_despawnTimer < 1000)
            {
                m_creature->SetRespawnDelay(10000);
                m_creature->DisappearAndDie();
                _eventEnd = true;
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        Creature* precious = m_creature->GetMap()->GetCreature(_preciousGuid);
        if (!precious)
            precious = m_creature->SummonCreature(NPC_PRECIOUS, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 
                                                  m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
        _preciousGuid.Clear();
        if (!precious)
            return;

        /** Combat shall not last more than 20 minutes */
        if(!isEngaged)
        {
            isEngaged     = true;
            _combatDespawnTimer = SIMONE_DESPAWN_TIMER;
        }

        precious->UpdateEntry(NPC_PRECIOUS_EVIL);
        precious->setFaction(m_creature->getFaction());
        _preciousGuid = precious->GetObjectGuid();
        ScriptedAI::Aggro(pWho);
    }

    void JustDied(Unit* pKiller)
    {
        if (!_boredOrInterfere)
            _eventEnd = true;
        ScriptedAI::JustDied(pKiller);
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (!_eventBegin)
            m_creature->CastSpell(done_by, DESESPOIR_IDIOT, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!_eventBegin)
            return;
        else 
        {
            if (_despawnTimer <= uiDiff)
            {
                if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                {
                    m_creature->SetRespawnDelay(10000);
                    m_creature->DisappearAndDie();
                    _eventEnd = true;
                }
           }
           else
               _despawnTimer -= uiDiff;
        }

        if (_combatTimer && _combatTimer <= uiDiff)
        {
            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(_playerGuid))
            {
                _combatTimer = 0;
                m_creature->UpdateEntry(NPC_SIMONE_EVIL);
            }
            else
                EnterEvadeMode();
        }
        else if (_combatTimer)
        {
            _combatTimer -= uiDiff;
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        /** If combat last for too long, force creature respawn */
        if (_combatDespawnTimer < uiDiff)
        {
            if(isEngaged)
            {
                _boredOrInterfere = true;
                m_creature->SetRespawnDelay(10000);
                m_creature->DisappearAndDie();
            }
        }
        else
            _combatDespawnTimer -= uiDiff;

        if (m_creature->getThreatManager().getThreatList().size() > 1 && !m_creature->GetMap()->GetCreature(_cleanerGuid))
        {
            if (Creature* cleaner = m_creature->SummonCreature(NPC_CLEANER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 
                                                               m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000))
            {
                _cleanerGuid = cleaner->GetObjectGuid();
                cleaner->AddThreatsOf(m_creature);
                cleaner->MonsterYell(ELYSIUM_TEXT(206), 0);

                m_creature->MonsterSay(ELYSIUM_TEXT(207), 0);
//                m_creature->SetRespawnDelay(25200); // Ustaag : prochain respawn fix a 7h 25200
                m_creature->CastSpellOnNearestVictim(FOOL_PLIGHT, 0.0f, 20.0f, false);
                _boredOrInterfere = true;

                if (Creature* precious = m_creature->GetMap()->GetCreature(_preciousGuid))
                    precious->DisappearAndDie();
            }
        }

        Creature* cleaner = m_creature->GetMap()->GetCreature(_cleanerGuid);
        if (cleaner && cleaner->isAlive())
        {
            _cleanerTimer += uiDiff;
            if (_cleanerTimer > 2000)
                m_creature->DisappearAndDie();
            return;
        }
        else
            _cleanerTimer = 0;

        if (m_creature->GetDistance2d(m_creature->getVictim()) > 8.0f && !m_creature->getVictim()->HasAura(BAISER_TENTATRICE))
            m_creature->getVictim()->AddAura(BAISER_TENTATRICE);

        // Ustaag : interrupt cast if Simone has aura Viper Sting
        if (Spell* spell = m_creature->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            if (m_creature->HasAura(3034) || m_creature->HasAura(14279) || m_creature->HasAura(14280)) // Viper Sting rank 1-2-3
                m_creature->InterruptSpell(CURRENT_GENERIC_SPELL, true);

        if (!m_creature->HasAura(3034) && !m_creature->HasAura(14279) && !m_creature->HasAura(14280)) // Viper Sting rank 1-2-3
        {
            if (_boltTimer < uiDiff)
            {
                // Si le hunt est en aspect de la nature, fixer a 200 les degats de la bolt
                // Ustaag : ajout check mana suffisante pour lancer le spell. pas trouvé d'équivalent à DoCastSpellIfCan pour CastCustomSpell
                if (m_creature->getVictim()->HasAura(20190) && m_creature->GetPower(POWER_MANA) > 150)
                {
                    int damageFoudre = 40; // dégâts finaux du spell en fonction du lvl du caster.. fix dégats initiaux pour coller a 200 degats max
                    m_creature->CastCustomSpell(m_creature->getVictim(), TRAIT_FOUDRE, &damageFoudre, NULL, NULL, false);
                    _boltTimer = 5000;
                }
                else if (DoCastSpellIfCan(m_creature->getVictim(), TRAIT_FOUDRE) == CAST_OK)
                    _boltTimer = 5000;
            }
            else
                _boltTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_mob_Simone(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(7636) == QUEST_STATUS_INCOMPLETE && pPlayer->getClass() == CLASS_HUNTER)
        pPlayer->ADD_GOSSIP_ITEM(0, ELYSIUM_TEXT(210), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        //pPlayer->ADD_GOSSIP_ITEM(0, "Ce deguisement ne me trompe pas seductrice. Ton heure est venue !", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(7041, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_mob_Simone(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;

    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
        {
            if (mob_SimoneAI* pSimoneEventAI = dynamic_cast<mob_SimoneAI*>(pCreature->AI()))
            {
                pSimoneEventAI->_eventBegin = true;
                pSimoneEventAI->_playerGuid = pPlayer->GetGUID();
                //pCreature->MonsterSay("Comme vous voudrez, Chasseur.", 0, 0);
                pCreature->MonsterSay(ELYSIUM_TEXT(209), 0, 0);
                pCreature->SetHomePosition(pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), pCreature->GetOrientation());
                pCreature->GetMotionMaster()->Clear(false);
                pCreature->SetDefaultMovementType(IDLE_MOTION_TYPE);
                pCreature->GetMotionMaster()->MoveIdle();
                pCreature->SaveToDB();
            }
            pPlayer->CLOSE_GOSSIP_MENU();
        }
        break;
    }
    return true;
}

CreatureAI* GetAI_mob_Simone(Creature* pCreature)
{
    return new mob_SimoneAI(pCreature);
}
enum
{
	SPELL_MERGING_OOZES = 16032,
	NPC_PRIMAL_OOZE = 6557,
	NPC_CAPTURED_OOZE = 10290,
	NPC_GARGANTUAN_OOZE = 9621
};
//melding of influences (Alita)
bool EffectDummyCreature_primal_ooze(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget)/*, ObjectGuid*/ /*originalCasterGuid*//*)*/
{
    if (uiSpellId == SPELL_MERGING_OOZES && uiEffIndex == EFFECT_INDEX_0)
    {
        if (pCaster->GetTypeId() != TYPEID_PLAYER && pCreatureTarget->GetTypeId() != TYPEID_PLAYER)
        {
            pCaster->SummonCreature(NPC_GARGANTUAN_OOZE, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 420000);
            ((Creature*)pCaster)->DisappearAndDie();
            ((Creature*)pCreatureTarget)->DisappearAndDie();
        }
        return true;
    }
    return true;
}
struct mob_captured_felwood_oozeAI : public ScriptedAI
{
    mob_captured_felwood_oozeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    void Reset()
    {
        initialTimer=5000;
    }
    uint32 initialTimer;
    void UpdateAI(const uint32 uiDiff)
    {
        if (initialTimer < uiDiff)
        {
            if( Creature* primalOoze = m_creature->FindNearestCreature(NPC_PRIMAL_OOZE, 5.0f))
            {
                DoCastSpellIfCan(primalOoze, SPELL_MERGING_OOZES);
            }
            initialTimer = 5000;
        }
        else
            initialTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_captured_felwood_ooze(Creature* pCreature)
{
    return new mob_captured_felwood_oozeAI(pCreature);
}
void AddSC_ungoro_crater()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_ame01";
    newscript->GetAI = &GetAI_npc_ame01;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_ame01;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ringo";
    newscript->GetAI = &GetAI_npc_ringo;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_ringo;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "ungoro_eggs_trigger";
    newscript->GetAI = &GetAI_eggs_trigger;
    newscript->RegisterSelf();

    // Elysium

    newscript = new Script;
    newscript->Name = "mob_Simone";
    newscript->GetAI = &GetAI_mob_Simone;
    newscript->pGossipHello = &GossipHello_mob_Simone;
    newscript->pGossipSelect = &GossipSelect_mob_Simone;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_primal_ooze";
    newscript->pEffectDummyCreature = &EffectDummyCreature_primal_ooze;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_captured_felwood_ooze";
    newscript->GetAI = &GetAI_mob_captured_felwood_ooze;
    newscript->RegisterSelf();
}
