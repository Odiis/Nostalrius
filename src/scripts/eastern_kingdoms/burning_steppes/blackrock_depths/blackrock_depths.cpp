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
SDName: Blackrock_Depths
SD%Complete: 80
SDComment: Quest support: 4001, 4342, 7604. Vendor Lokhtos Darkbargainer.
SDCategory: Blackrock Depths
EndScriptData */

/* ContentData
go_shadowforge_brazier
at_ring_of_law
npc_grimstone
mob_phalanx
npc_kharan_mighthammer
npc_lokhtos_darkbargainer
EndContentData */

/* Elysium : Chakor
go_dark_keeper_portrait
go_thunderbrew_laguer_keg
go_relic_coffer_door
npc_watchman_doomgrip
npc_ribbly_fermevanne
npc_golem_lord_argelmach
*/
/* Elysium : Ivina
npc_GorShak
*/

#include "scriptPCH.h"
#include "blackrock_depths.h"

/*######
## go_shadowforge_brazier
######*/

bool GOHello_go_shadowforge_brazier(Player* pPlayer, GameObject* pGo)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_LYCEUM) == IN_PROGRESS)
            pInstance->SetData(TYPE_LYCEUM, DONE);
        else
            pInstance->SetData(TYPE_LYCEUM, IN_PROGRESS);
    }
    return false;
}

/*######
## npc_grimstone
######*/

enum
{
    NPC_GRIMSTONE       = 10096,
    NPC_THELDREN        = 16059,

    //4 or 6 in total? 1+2+1 / 2+2+2 / 3+3. Depending on this, code should be changed.
    MAX_MOB_AMOUNT      = 8
};

uint32 RingMob[] =
{
    8925,                                                   // Dredge Worm
    8926,                                                   // Deep Stinger
    8927,                                                   // Dark Screecher
    8928,                                                   // Burrowing Thundersnout
    8933,                                                   // Cave Creeper
    8932,                                                   // Borer Beetle
};

uint32 RingBoss[] =
{
    9027,                                                   // Gorosh
    9028,                                                   // Grizzle
    9029,                                                   // Eviscerator
    9030,                                                   // Ok'thor
    9031,                                                   // Anub'shiah
    9032,                                                   // Hedrum
};

bool AreaTrigger_at_ring_of_law(Player* pPlayer, const AreaTriggerEntry *at)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_RING_OF_LAW) == IN_PROGRESS || pInstance->GetData(TYPE_RING_OF_LAW) == DONE)
            return false;

        pInstance->SetData(TYPE_RING_OF_LAW, IN_PROGRESS);
        pPlayer->SummonCreature(NPC_GRIMSTONE, 625.559f, -205.618f, -52.735f, 2.609f, TEMPSUMMON_DEAD_DESPAWN, 0);

        return false;
    }
    return false;
}

/*######
## npc_grimstone
######*/

//TODO: implement quest part of event (different end boss)
struct npc_grimstoneAI : public npc_escortAI
{
    npc_grimstoneAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        MobSpawnId = urand(0, 5);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint8 EventPhase;
    uint32 Event_Timer;

    uint8 MobSpawnId;
    uint8 MobCount;
    uint32 MobDeath_Timer;

    uint64 RingMobGUID[MAX_MOB_AMOUNT];
    uint64 RingBossGUID;

    bool CanWalk;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        EventPhase = 0;
        Event_Timer = 1000;

        MobCount = 0;
        MobDeath_Timer = 0;

        for (uint8 i = 0; i < MAX_MOB_AMOUNT; ++i)
            RingMobGUID[i] = 0;

        RingBossGUID = 0;

        CanWalk = false;
    }

    void DoGate(uint32 id, uint32 state)
    {
        if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(id)))
            pGo->SetGoState(GOState(state));

        sLog.outDebug("npc_grimstone, arena gate update state.");
    }

    //TODO: move them to center
    void SummonRingMob()
    {
        // No array overflow
        if (MobCount >= MAX_MOB_AMOUNT)
            return;
        if (Creature* tmp = m_creature->SummonCreature(RingMob[MobSpawnId], 608.960f, -235.322f, -53.907f, 1.857f, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            RingMobGUID[MobCount] = tmp->GetGUID();
            tmp->GetMotionMaster()->MovePoint(1, 596.285156f, -188.698944f, -54.132176f);
            tmp->SetHomePosition(596.285156f, -188.698944f, -54.132176f, 0);
        }

        ++MobCount;

        if (MobCount == MAX_MOB_AMOUNT)
            MobDeath_Timer = 2500;
    }

    //TODO: move them to center
    void SummonRingBoss()
    {
        if (Creature* tmp = m_creature->SummonCreature(RingBoss[rand() % 6], 644.300f, -175.989f, -53.739f, 3.418f, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            RingBossGUID = tmp->GetGUID();
            tmp->GetMotionMaster()->MovePoint(1, 596.285156f, -188.698944f, -54.132176f);
            tmp->SetHomePosition(596.285156f, -188.698944f, -54.132176f, 0);
        }

        MobDeath_Timer = 2500;
    }

    void WaypointReached(uint32 i)
    {
        switch (i)
        {
            case 0:
                DoScriptText(-1000011, m_creature);
                CanWalk = false;
                Event_Timer = 5000;
                break;
            case 1:
                DoScriptText(-1000012, m_creature);
                CanWalk = false;
                Event_Timer = 5000;
                break;
            case 2:
                CanWalk = false;
                break;
            case 3:
                //DoScriptText(-1000013, m_creature);//5
                break;
            case 4:
                DoScriptText(-1000015, m_creature);
                CanWalk = false;
                Event_Timer = 5000;
                break;
            case 5:
                if (m_pInstance)
                {
                    m_pInstance->SetData(TYPE_RING_OF_LAW, DONE);
                    sLog.outDebug("npc_grimstone: event reached end and set complete.");
                }
                break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;

        if (MobDeath_Timer)
        {
            if (MobDeath_Timer <= diff)
            {
                MobDeath_Timer = 2500;

                if (RingBossGUID)
                {
                    Creature *boss = m_creature->GetMap()->GetCreature(RingBossGUID);
                    if (boss && !boss->isAlive() && boss->isDead())
                    {
                        RingBossGUID = 0;
                        Event_Timer = 5000;
                        MobDeath_Timer = 0;
                        return;
                    }
                    return;
                }

                for (uint8 i = 0; i < MAX_MOB_AMOUNT; ++i)
                {
                    Creature *mob = m_creature->GetMap()->GetCreature(RingMobGUID[i]);
                    if (mob && !mob->isAlive() && mob->isDead())
                    {
                        RingMobGUID[i] = 0;
                        --MobCount;

                        //seems all are gone, so set timer to continue and discontinue this
                        if (!MobCount)
                        {
                            Event_Timer = 5000;
                            MobDeath_Timer = 0;
                        }
                    }
                }
            }
            else MobDeath_Timer -= diff;
        }

        if (Event_Timer)
        {
            if (Event_Timer <= diff)
            {
                switch (EventPhase)
                {
                    case 0:
                        DoScriptText(-1000010, m_creature);
                        DoGate(DATA_ARENA4, 1);
                        Start(false, false);
                        CanWalk = true;
                        Event_Timer = 0;
                        break;
                    case 1:
                        CanWalk = true;
                        Event_Timer = 0;
                        break;
                    case 2:
                        Event_Timer = 2000;
                        break;
                    case 3:
                        DoGate(DATA_ARENA1, GO_STATE_ACTIVE);
                        Event_Timer = 3000;
                        break;
                    case 4:
                        CanWalk = true;
                        m_creature->SetVisibility(VISIBILITY_OFF);
                        SummonRingMob();
                        Event_Timer = 3000;
                        break;
                    case 5:
                        SummonRingMob();
                        SummonRingMob();
                        Event_Timer = 4000;
                        break;
                    case 6:
                        SummonRingMob();
                        Event_Timer = 12000;
                        break;
                    case 7:
                        MobSpawnId = urand(0, 5);
                        SummonRingMob();
                        Event_Timer = 3000;
                        break;
                    case 8:
                        SummonRingMob();
                        SummonRingMob();
                        m_creature->SetVisibility(VISIBILITY_ON);
                        DoScriptText(-1000013, m_creature);
                        Event_Timer = 4000;
                        break;
                    case 9:
                        SummonRingMob();
                        DoScriptText(-1000014, m_creature);
                        m_creature->SetVisibility(VISIBILITY_OFF);
                        Event_Timer = 0;
                        break;
                    case 10:
                        m_creature->SetVisibility(VISIBILITY_ON);
                        DoGate(DATA_ARENA1, GO_STATE_READY);
                        CanWalk = true;
                        Event_Timer = 0;
                        break;
                    case 11:
                        DoGate(DATA_ARENA2, GO_STATE_ACTIVE);
                        Event_Timer = 5000;
                        break;
                    case 12:
                        m_creature->SetVisibility(VISIBILITY_OFF);
                        SummonRingBoss();
                        Event_Timer = 0;
                        break;
                    case 13:
                        //if quest, complete
                        DoGate(DATA_ARENA2, GO_STATE_READY);
                        DoGate(DATA_ARENA3, GO_STATE_ACTIVE);
                        DoGate(DATA_ARENA4, GO_STATE_ACTIVE);
                        CanWalk = true;
                        Event_Timer = 0;
                        break;
                }
                ++EventPhase;
            }
            else Event_Timer -= diff;
        }

        if (CanWalk)
            npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_grimstone(Creature* pCreature)
{
    if (!pCreature->GetInstanceData())
        return NULL;
    return new npc_grimstoneAI(pCreature);
}

/*######
## mob_phalanx
######*/

#define SPELL_THUNDERCLAP       15588
#define SPELL_FIREBALLVOLLEY    15285
#define SPELL_MIGHTYBLOW        14099

struct mob_phalanxAI : public ScriptedAI
{
    mob_phalanxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 ThunderClap_Timer;
    uint32 FireballVolley_Timer;
    uint32 MightyBlow_Timer;

    void Reset()
    {
        ThunderClap_Timer = 12000;
        FireballVolley_Timer = 0;
        MightyBlow_Timer = 15000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //ThunderClap_Timer
        if (ThunderClap_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_THUNDERCLAP);
            ThunderClap_Timer = 10000;
        }
        else ThunderClap_Timer -= diff;

        //FireballVolley_Timer
        if (m_creature->GetHealthPercent() < 51.0f)
        {
            if (FireballVolley_Timer < diff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALLVOLLEY);
                FireballVolley_Timer = 15000;
            }
            else FireballVolley_Timer -= diff;
        }

        //MightyBlow_Timer
        if (MightyBlow_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIGHTYBLOW);
            MightyBlow_Timer = 10000;
        }
        else MightyBlow_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_phalanx(Creature* pCreature)
{
    return new mob_phalanxAI(pCreature);
}

/*######
## npc_kharan_mighthammer
######*/

#define QUEST_4001      4001
#define QUEST_4342      4342

#define GOSSIP_ITEM_KHARAN_1    "I need to know where the princess are, Kharan!"
#define GOSSIP_ITEM_KHARAN_2    "All is not lost, Kharan!"

#define GOSSIP_ITEM_KHARAN_3    "Gor'shak is my friend, you can trust me."
#define GOSSIP_ITEM_KHARAN_4    "Not enough, you need to tell me more."
#define GOSSIP_ITEM_KHARAN_5    "So what happened?"
#define GOSSIP_ITEM_KHARAN_6    "Continue..."
#define GOSSIP_ITEM_KHARAN_7    "So you suspect that someone on the inside was involved? That they were tipped off?"
#define GOSSIP_ITEM_KHARAN_8    "Continue with your story please."
#define GOSSIP_ITEM_KHARAN_9    "Indeed."
#define GOSSIP_ITEM_KHARAN_10   "The door is open, Kharan. You are a free man."

bool GossipHello_npc_kharan_mighthammer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_4001) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetQuestStatus(4342) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

    if (pPlayer->GetTeam() == HORDE)
        pPlayer->SEND_GOSSIP_MENU(2473, pCreature->GetGUID());
    else
        pPlayer->SEND_GOSSIP_MENU(2474, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_kharan_mighthammer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(2475, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(2476, pCreature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(2477, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(2478, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(2479, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(2480, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            pPlayer->SEND_GOSSIP_MENU(2481, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+8:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KHARAN_10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            pPlayer->SEND_GOSSIP_MENU(2482, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+9:
            pPlayer->CLOSE_GOSSIP_MENU();
            if (pPlayer->GetTeam() == HORDE)
                pPlayer->AreaExploredOrEventHappens(QUEST_4001);
            else
                pPlayer->AreaExploredOrEventHappens(QUEST_4342);
            break;
    }
    return true;
}

/*######
## npc_lokhtos_darkbargainer
######*/

#define ITEM_THRORIUM_BROTHERHOOD_CONTRACT               18628
#define ITEM_SULFURON_INGOT                              17203
#define QUEST_A_BINDING_CONTRACT                         7604
#define SPELL_CREATE_THORIUM_BROTHERHOOD_CONTRACT_DND    23059

#define GOSSIP_ITEM_SHOW_ACCESS     "Show me what I have access to, Lothos."
#define GOSSIP_ITEM_GET_CONTRACT    "Get Thorium Brotherhood Contract"

bool GossipHello_npc_lokhtos_darkbargainer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isVendor() && pPlayer->GetReputationRank(59) >= REP_FRIENDLY)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_ITEM_SHOW_ACCESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->GetQuestRewardStatus(QUEST_A_BINDING_CONTRACT) != 1 &&
            !pPlayer->HasItemCount(ITEM_THRORIUM_BROTHERHOOD_CONTRACT, 1, true) &&
            pPlayer->HasItemCount(ITEM_SULFURON_INGOT, 1))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GET_CONTRACT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetReputationRank(59) < REP_FRIENDLY)
        pPlayer->SEND_GOSSIP_MENU(3673, pCreature->GetGUID());
    else
        pPlayer->SEND_GOSSIP_MENU(3677, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_lokhtos_darkbargainer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, SPELL_CREATE_THORIUM_BROTHERHOOD_CONTRACT_DND, false);
    }

    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*######
## npc_rocknot
######*/

#define SAY_GOT_BEER        -1230000
#define SPELL_DRUNKEN_RAGE  14872
#define QUEST_ALE           4295

//positions when opening door for Nagmara
struct sPositionInformation
{
    float fX, fY, fZ, fO;
};
static const sPositionInformation RocknotPositionsForNagmara[9] =
{
    {874.87359, -198.372, -43.703, 4.338482},
    {866.8894, -205.99,   -43.7037, 4.86626},
    {866.5805, -216.69,   -43.7037, 4.95816},
    {868.7742, -228.3334, -43.7399, 5.23933}, //(pause: open door)
    {872.9965, -233.0496, -43.752,  0.61177},
    {889.4429, -221.512,  -49.944,  2.57212},
    {885.3575, -218.535,  -49.9454, 3.64655},
    {877.7158, -222.768,  -49.9736, 4.89768},
    {878.592041, -226.941788, -49.982887, 3.619845} //(last point under stairs)
};
enum
{
    NPC_NAGMARA             = 9500,
    NPC_ROCKNOT              = 9503,

    SPELL_LOVE_POTION    = 14928,
    SPELL_NAGMARA_VANISH = 15341,
    QUEST_LOVE_POTION    = 4201
};

void NagmaraEndFollowRocknot(Creature* pCreature);

struct npc_rocknotAI : public npc_escortAI
{
    npc_rocknotAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 BreakKeg_Timer;
    uint32 BreakDoor_Timer;
    uint32 NagmaraEventTimer;
    uint8 NagmaraEvent;

    bool noNagmaraEvent()
    {
        return !NagmaraEvent;
    }
    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        BreakKeg_Timer = 0;
        BreakDoor_Timer = 0;
        NagmaraEvent = 0;
    }
    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_LOVE_POTION)
        {
            if(NagmaraEvent == 0)
            {
                currPoint = 0;
                NagmaraEvent = 1;
                m_creature->GetMotionMaster()->MovePoint(1, RocknotPositionsForNagmara[0].fX, RocknotPositionsForNagmara[0].fY, RocknotPositionsForNagmara[0].fZ, MOVE_PATHFINDING, RocknotPositionsForNagmara[0].fO);
            }
        }
    }
    uint32 currPoint;
    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(NagmaraEvent>0)
        {
            if (uiType == POINT_MOTION_TYPE)
            {
                currPoint = uiPointId;
                if(uiPointId >0 && uiPointId <9 && uiPointId!=4)
                {
                    m_creature->GetMotionMaster()->MovePoint(uiPointId+1, RocknotPositionsForNagmara[uiPointId].fX, RocknotPositionsForNagmara[uiPointId].fY, RocknotPositionsForNagmara[uiPointId].fZ, MOVE_PATHFINDING, 0, RocknotPositionsForNagmara[uiPointId].fO);

                }
                if(uiPointId == 8)
                {
                    if(Creature* nagmara= m_creature->FindNearestCreature(NPC_NAGMARA, 40.0f))
                        NagmaraEndFollowRocknot(nagmara);
                }
                if(uiPointId == 9)
                {
                    m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);
                    m_creature->GetMotionMaster()->MoveIdle();
                }
                if(uiPointId == 4)
                {
                    NagmaraEventTimer = 4000;
                }
            }
        }
        else
            npc_escortAI::MovementInform(uiType, uiPointId);
    }
    void UpdateAI(uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if(NagmaraEvent == 1)
            {
                if(currPoint == 4)
                {
                    if (NagmaraEventTimer < uiDiff)
                    {
                        DoGo(DATA_GO_BAR_DOOR, 0);
                        m_creature->GetMotionMaster()->MovePoint(currPoint+1, RocknotPositionsForNagmara[currPoint].fX, RocknotPositionsForNagmara[currPoint].fY, RocknotPositionsForNagmara[currPoint].fZ, MOVE_PATHFINDING);
                        m_pInstance->SetData(TYPE_NAGMARA, DONE);
                    }
                    else {NagmaraEventTimer -= uiDiff;}
                }
            }
        }
        npc_escortAI::UpdateAI(uiDiff);
    }


    void DoGo(uint32 id, uint32 state)
    {
        if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(id)))
            pGo->SetGoState(GOState(state));
    }

    void WaypointReached(uint32 i)
    {
        if (!m_pInstance)
            return;

        switch (i)
        {
            case 1:
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                break;
            case 2:
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_ATTACKUNARMED);
                break;
            case 3:
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_ATTACKUNARMED);
                break;
            case 4:
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                break;
            case 5:
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                BreakKeg_Timer = 2000;
                break;
        }
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;

        if (BreakKeg_Timer)
        {
            if (BreakKeg_Timer <= diff)
            {
                DoGo(DATA_GO_BAR_KEG, 0);
                BreakKeg_Timer = 0;
                BreakDoor_Timer = 1000;
            }
            else BreakKeg_Timer -= diff;
        }

        if (BreakDoor_Timer)
        {
            if (BreakDoor_Timer <= diff)
            {
                DoGo(DATA_GO_BAR_DOOR, 2);
                DoGo(DATA_GO_BAR_KEG_TRAP, 0);              //doesn't work very well, leaving code here for future
                //spell by trap has effect61, this indicate the bar go hostile

                if (Unit *tmp = m_creature->GetMap()->GetUnit(m_pInstance->GetData64(DATA_PHALANX)))
                {
                    tmp->GetMotionMaster()->MovePoint(1, 865.495850f, -219.225037f, -43.702477f);
                    tmp->setFaction(14);
                }

                //for later, this event(s) has alot more to it.
                //optionally, DONE can trigger bar to go hostile.
                m_pInstance->SetData(TYPE_BAR, DONE);

                BreakDoor_Timer = 0;
            }
            else BreakDoor_Timer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_rocknot(Creature* pCreature)
{
    return new npc_rocknotAI(pCreature);
}

bool QuestRewarded_npc_rocknot(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());

    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_BAR) == DONE)
        return true;
    if (pInstance->GetData(TYPE_NAGMARA) == IN_PROGRESS || pInstance->GetData(TYPE_NAGMARA) == DONE )
        return true;

    if (pQuest->GetQuestId() == QUEST_ALE)
    {
        pInstance->SetData(TYPE_BAR, IN_PROGRESS);

        //keep track of amount in instance script, returns SPECIAL if amount ok and event in progress
        if (pInstance->GetData(TYPE_BAR) == SPECIAL)
        {
            DoScriptText(SAY_GOT_BEER, pCreature);
            pCreature->CastSpell(pCreature, SPELL_DRUNKEN_RAGE, false);

            if (npc_rocknotAI* pEscortAI = dynamic_cast<npc_rocknotAI*>(pCreature->AI()))
            {
                if(pEscortAI->noNagmaraEvent() == true)
                {
                    pEscortAI->Start(false, false, 0, NULL, true);
                }
            }
        }
    }

    return true;
}

struct npc_nagmaraAI : public ScriptedAI
{
    npc_nagmaraAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        eventPhase = 0;
        phaseTimer = 120000;
    }
    void Reset() { vanishTimer= urand(2000,9000);}
    uint8 eventPhase;//0:nothing,
    //1: waiting to be at right WP to start.
    //2: at right WP, waiting.
    //3: /follow rocknot (in Rocknot's code, when door opens, save to instance! stop from launching the other door opening events)
    //4: last point & wait
    //5: Ended (despawned)
    uint32 phaseTimer;
    uint64 GuidRocknot;
    uint32 vanishTimer;

    // bool CanStartEvent()
    // {
        // if (ScriptedInstance* pInstance = ((ScriptedInstance*)m_creature->GetInstanceData()))
        // {
            // if (pInstance->GetData(TYPE_BAR) == SPECIAL || pInstance->GetData(TYPE_BAR) == DONE)
                // return false;
        // }
        // return !eventPhase;
    // }
    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType == WAYPOINT_MOTION_TYPE)
        {
            if(uiPointId >1 && uiPointId <5)
            {
                if(eventPhase == 1)
                {
                    if(Creature* rocknot =  m_creature->FindNearestCreature(NPC_ROCKNOT, 20.0f))
                    {
                        GuidRocknot = rocknot->GetGUID();
                        m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_creature->SetFacingToObject(rocknot);
                        eventPhase=2;
                        phaseTimer=5000;
                    }
                }
            }
        }
        else if (uiType == POINT_MOTION_TYPE)
        {
            m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);
            m_creature->GetMotionMaster()->MoveIdle();
        }
    }
    bool StartEvent()
    {
        if (eventPhase)
            return false;
        if (ScriptedInstance* pInstance = ((ScriptedInstance*)m_creature->GetInstanceData()))
        {
            if (pInstance->GetData(TYPE_BAR) == SPECIAL || pInstance->GetData(TYPE_BAR) == DONE || pInstance->GetData(TYPE_NAGMARA) == IN_PROGRESS || pInstance->GetData(TYPE_NAGMARA) == DONE )
                return false;

            pInstance->SetData(TYPE_NAGMARA,IN_PROGRESS);
        }
        eventPhase = 1;
        if(Creature* rocknot =  m_creature->FindNearestCreature(NPC_ROCKNOT, 20.0f))
        {
            GuidRocknot = rocknot->GetGUID();
            m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFacingToObject(rocknot);
            eventPhase=2;
            phaseTimer=5000;
        }
        return true;
    }
    void stopFollow()
    {
        m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);
        m_creature->GetMotionMaster()->MovePoint(2, 877.065918f, -227.647369f, -49.993839f, MOVE_PATHFINDING,0,0.619626);
        eventPhase = 4;
        phaseTimer = 120000;
    }

    void UpdateAI(uint32 uiDiff)
    {
        if (eventPhase > 0)
        {
            if (eventPhase ==2)
            {
                if (phaseTimer < uiDiff)
                {
                    if (Creature* rocknot = m_creature->GetMap()->GetCreature(GuidRocknot))
                    {
                        DoCastSpellIfCan(rocknot, SPELL_LOVE_POTION);
                        m_creature->GetMotionMaster()->MoveFollow(rocknot, 0.3, M_PI);//follow rocknot
                        m_creature->SetSpeedRate(MOVE_WALK, 1, true);
                        eventPhase = 3;
                    }
                }
                else {phaseTimer -= uiDiff;}
            }
            if (eventPhase == 4)
            {
                if(phaseTimer<uiDiff)
                {
                    eventPhase = 5;
                    if (Creature* rocknot = m_creature->GetMap()->GetCreature(GuidRocknot))
                    {
                        rocknot->DisappearAndDie();
                        m_creature->DisappearAndDie();
                    }
                }
                else {phaseTimer -= uiDiff;}
            }
        }
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        if(vanishTimer<uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_LOVE_POTION);
            vanishTimer= urand(2000,9000);
        }
        else {vanishTimer -= uiDiff;}
        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_nagmara(Creature* pCreature)
{
    return new npc_nagmaraAI(pCreature);
}

void NagmaraEndFollowRocknot(Creature* pCreature)
{
    if (npc_nagmaraAI* pEmiAI = dynamic_cast<npc_nagmaraAI*>(pCreature->AI()))
    {
        pEmiAI->stopFollow();
    }
}

bool GossipSelect_npc_nagmara(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (pPlayer->GetQuestStatus(QUEST_LOVE_POTION) == QUEST_STATUS_COMPLETE)
    {
        if (npc_nagmaraAI* pEmiAI = dynamic_cast<npc_nagmaraAI*>(pCreature->AI()))
        {
            if (pEmiAI->StartEvent())
            {
                // send this gossip menu: 2075 //npc_text corresponding id 2728
                pPlayer->SEND_GOSSIP_MENU(2728, pCreature->GetGUID());
                return true;
            }
        }
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}
/*######
## go_dark_keeper_portrait
######*/

enum
{
    NPC_DARK_KEEPER_VORFALK    = 9437,
    NPC_DARK_KEEPER_BETHEK     = 9438,
    NPC_DARK_KEEPER_UGGEL      = 9439,
    NPC_DARK_KEEPER_ZIMREL     = 9441,
    NPC_DARK_KEEPER_OFGUT      = 9442,
    NPC_DARK_KEEPER_PELVER     = 9443,

    GO_VORFALK                 = 164820,
    GO_BETHEK                  = 164821,
    GO_UGGEL                   = 164822,
    GO_ZIMREL                  = 164823,
    GO_OFGUT                   = 164824,
    GO_PELVER                  = 164825,
};

bool GOHello_go_dark_keeper_portrait(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)pGo->GetInstanceData());

    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_VAULT) != DONE)
    {
        switch (urand(0, 5))
        {
            case 0:
                pPlayer->SummonCreature(NPC_DARK_KEEPER_VORFALK, 815.60f, -168.54f, -49.75f, 5.97f, TEMPSUMMON_DEAD_DESPAWN, 0);
                pPlayer->SummonGameObject(GO_VORFALK, 831.54f, -339.529f, -46.682f, 0.802851f, 0, 0, 0, 0, 0);
                pInstance->SetData(TYPE_VAULT, DONE);
                break;
            case 1:
                pPlayer->SummonCreature(NPC_DARK_KEEPER_BETHEK, 846.66f, -317.18f, -50.29f, 3.90f, TEMPSUMMON_DEAD_DESPAWN, 0);
                pPlayer->SummonGameObject(GO_BETHEK, 831.54f, -339.529f, -46.682f, 0.802851f, 0, 0, 0, 0, 0);
                pInstance->SetData(TYPE_VAULT, DONE);
                break;
            case 2:
                pPlayer->SummonCreature(NPC_DARK_KEEPER_UGGEL, 963.27f, -343.73f, -71.74f, 2.22f, TEMPSUMMON_DEAD_DESPAWN, 0);
                pPlayer->SummonGameObject(GO_UGGEL, 831.54f, -339.529f, -46.682f, 0.802851f, 0, 0, 0, 0, 0);
                pInstance->SetData(TYPE_VAULT, DONE);
                break;
            case 3:
                pPlayer->SummonCreature(NPC_DARK_KEEPER_ZIMREL, 545.49f, -162.49f, -35.46f, 5.86f, TEMPSUMMON_DEAD_DESPAWN, 0);
                pPlayer->SummonGameObject(GO_ZIMREL, 831.54f, -339.529f, -46.682f, 0.802851f, 0, 0, 0, 0, 0);
                pInstance->SetData(TYPE_VAULT, DONE);
                break;
            case 4:
                pPlayer->SummonCreature(NPC_DARK_KEEPER_OFGUT, 681.52f, -11.55f, -60.06f, 1.98f, TEMPSUMMON_DEAD_DESPAWN, 0);
                pPlayer->SummonGameObject(GO_OFGUT, 831.54f, -339.529f, -46.682f, 0.802851f, 0, 0, 0, 0, 0);
                pInstance->SetData(TYPE_VAULT, DONE);
                break;
            case 5:
                pPlayer->SummonCreature(NPC_DARK_KEEPER_PELVER, 803.64f, -248.00f, -43.30f, 2.60f, TEMPSUMMON_DEAD_DESPAWN, 0);
                pPlayer->SummonGameObject(GO_PELVER, 831.54f, -339.529f, -46.682f, 0.802851f, 0, 0, 0, 0, 0);
                pInstance->SetData(TYPE_VAULT, DONE);
                break;
        }
    }
    return false;
}

/*######
## go_thunderbrew_laguer_keg
######*/

enum
{
    HURLEY_ENTRY    = 9537,
    GUARDS_ENTRY    = 9541,
};

bool GOHello_go_thunderbrew_laguer_keg(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)pGo->GetInstanceData());

    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_THUNDERBREW) == DONE)
        return false;

    if (pInstance->GetData(TYPE_THUNDERBREW) == NOT_STARTED)
        pInstance->SetData(TYPE_THUNDERBREW, IN_PROGRESS);

    if (pInstance->GetData(TYPE_THUNDERBREW) == DONE)
    {
        Creature* pHurley = pPlayer->SummonCreature(HURLEY_ENTRY,
                            892.42f, -145.03f, -49.76f, 0.44f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
        //pHurley->MonsterYell("Who dare stealing my beer?!", 0, pPlayer);
        pHurley->MonsterYell(ELYSIUM_TEXT(252), 0, pPlayer);
        pHurley->AI()->AttackStart(pPlayer);

        Creature* pGuards1 = pPlayer->SummonCreature(GUARDS_ENTRY,
                             892.55f, -149.18f, -49.76f, 0.44f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
        pGuards1->AI()->AttackStart(pPlayer);

        Creature* pGuards2 = pPlayer->SummonCreature(GUARDS_ENTRY,
                             891.57f, -146.99f, -49.76f, 0.44f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
        pGuards2->AI()->AttackStart(pPlayer);

        Creature* pGuards3 = pPlayer->SummonCreature(GUARDS_ENTRY,
                             890.44f, -144.86f, -49.76f, 0.44f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
        pGuards3->AI()->AttackStart(pPlayer);

        Creature* pGuards4 = pPlayer->SummonCreature(GUARDS_ENTRY,
                             889.56f, -143.23f, -49.76f, 0.44f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
        pGuards4->AI()->AttackStart(pPlayer);
    }

    return false;
}

/*######
## go_relic_coffer_door
######*/

enum
{
    RUINEPOIGNE_ENTRY    = 9476,
};

bool GOHello_go_relic_coffer_door(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)pGo->GetInstanceData());

    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_RELIC_COFFER) != IN_PROGRESS || pInstance->GetData(TYPE_RELIC_COFFER) != DONE)
        pInstance->SetData(TYPE_RELIC_COFFER, IN_PROGRESS);

    pInstance->SetData(TYPE_RELIC_COFFER, SPECIAL);

    if (pInstance->GetData(TYPE_RELIC_COFFER) == DONE)
    {
        Creature* pCreature = pPlayer->SummonCreature(RUINEPOIGNE_ENTRY,
                              819.45f, -348.96f, -50.49f, 0.35f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
//        pCreature->MonsterYell("Ne les laissez pas s'emparer du Coeur de la montagne!!", 0, pPlayer);
        // pCreature->MonsterYell("Don't let them take the moutain hearth!", 0, pPlayer);
        pCreature->MonsterYell(ELYSIUM_TEXT(153), 0, pPlayer);
        pCreature->AI()->AttackStart(pPlayer);
    }

    return false;
}

/*######
## npc_watchman_doomgrip
######*/

#define SPELL_BOIRE_LA_POTION_DE_SOINS    15504
#define SPELL_FRACASSER_ARMURE    11971

struct npc_watchman_doomgripAI : public ScriptedAI
{
    npc_watchman_doomgripAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 BoireLaPotionDeSoins_Timer;
    uint32 FracasserArmure_Timer;

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DOOMGRIP, DONE);
    }

    void Reset()
    {
        BoireLaPotionDeSoins_Timer = 0;
        FracasserArmure_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //BoireLaPotionDeSoins_Timer
        if (m_creature->GetHealthPercent() < 51.0f)
        {
            if (BoireLaPotionDeSoins_Timer < diff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_BOIRE_LA_POTION_DE_SOINS);
                BoireLaPotionDeSoins_Timer = 15000;
            }
            else BoireLaPotionDeSoins_Timer -= diff;
        }

        //FracasserArmure_Timer
        if (FracasserArmure_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FRACASSER_ARMURE);
            FracasserArmure_Timer = 10000;
        }
        else FracasserArmure_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_watchman_doomgrip(Creature* pCreature)
{
    return new npc_watchman_doomgripAI(pCreature);
}

/*######
## npc_ribbly_fermevanne
######*/

//#define GOSSIP_ITEM_ATTAQUE  "On pay bien pour votre tête..."
#define GOSSIP_ITEM_ATTAQUE "We pay a lot for your head..."

#define SPELL_BRISE_GENOU    9080
#define SPELL_SURINER    12540

struct npc_ribbly_fermevanneAI : public ScriptedAI
{
    npc_ribbly_fermevanneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 BiseGenou_Timer;
    uint32 Suriner_Timer;

    void Reset()
    {
        BiseGenou_Timer = urand(3000, 9000);
        Suriner_Timer = urand(2000, 10000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //BiseGenou_Timer
        if (BiseGenou_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BRISE_GENOU);
            BiseGenou_Timer = urand(9000, 15000);
        }
        else BiseGenou_Timer -= diff;

        //Suriner_Timer
        if (Suriner_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SURINER);
            Suriner_Timer = urand(9000, 12000);
        }
        else Suriner_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ribbly_fermevanne(Creature* pCreature)
{
    return new npc_ribbly_fermevanneAI(pCreature);
}

bool GossipHello_npc_ribbly_fermevanne(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(4136) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GOSSIP_ITEM_ATTAQUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_ribbly_fermevanne(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
//        pCreature->MonsterYell("Encore un coup de ma chère soeur, vous n'aurez pas ma tête si facilement!!", 0, pPlayer);
//        pCreature->MonsterYell("Another drink, you won't have my head easily!", 0, pPlayer);
        pCreature->MonsterYell(ELYSIUM_TEXT(154), 0, pPlayer);
        pCreature->setFaction(14);
        pCreature->AI()->AttackStart(pPlayer);

        if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
            pInstance->SetData(TYPE_RIBBLY, DONE);
    }

    return true;
}

/*######
## npc_golem_lord_argelmach
######*/

#define SPELL_BOUCLIER_DE_FOUDRE    15507
#define SPELL_CHAINE_D_ECLAIRES    15305
#define SPELL_HORION    15605

struct npc_golem_lord_argelmachAI : public ScriptedAI
{
    npc_golem_lord_argelmachAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 BouclierDeFoudre_Timer;
    uint32 ChaineDEclaires_Timer;
    uint32 Horion_Timer;

    void Aggro(Unit* pWho)
    {
        m_creature->GetMotionMaster()->MovePoint(0, 846.801025f, 16.280600f, -53.639500f);
        //m_creature->MonsterYell("Golems, votre Seigneur a besoin de vous!", 0, pWho);
        m_creature->MonsterYell(ELYSIUM_TEXT(155), 0, pWho);

        if (m_pInstance)
            m_pInstance->SetData(DATA_ARGELMACH_AGGRO, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_ARGELMACH_AGGRO, DONE);
    }

    void Reset()
    {
        BouclierDeFoudre_Timer = 0;
        ChaineDEclaires_Timer = 5000;
        Horion_Timer = 2000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //BouclierDeFoudre_Timer
        if (BouclierDeFoudre_Timer < diff)
        {
            if (!m_creature->HasAura(SPELL_BOUCLIER_DE_FOUDRE))
                if (DoCastSpellIfCan(m_creature, SPELL_BOUCLIER_DE_FOUDRE) == CAST_OK)
                    BouclierDeFoudre_Timer = 15000;
        }
        else BouclierDeFoudre_Timer -= diff;

        //ChaineDEclaires_Timer
        if (ChaineDEclaires_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAINE_D_ECLAIRES) == CAST_OK)
                ChaineDEclaires_Timer = 14000;
        }
        else ChaineDEclaires_Timer -= diff;

        //Horion_Timer
        if (Horion_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HORION) == CAST_OK)
                Horion_Timer = 6000;
        }
        else Horion_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_golem_lord_argelmach(Creature* pCreature)
{
    return new npc_golem_lord_argelmachAI(pCreature);
}

// Elysium : Ivina

/*######
## npc_GorShak
######*/

enum
{
    NPC_GARDE_RAGENCLUME        = 8891,
    NPC_GARDIEN_RAGENCLUME      = 8890,
    NPC_TORTIONNAIRE            = 8912,

    QUEST_WHATISGOINGON         = 3982
};

struct npc_GorShakAI : public ScriptedAI
{
    npc_GorShakAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiTimer;
    uint32 uiPhase;
    int iSumCreaCnt;
    int iSumCreaDead;
    ObjectGuid playerGuid;
    bool bQuestActive;

    void Reset()
    {
        uiPhase = 0;
        iSumCreaCnt = 0;
        iSumCreaDead = 0;
        playerGuid.Clear();
        bQuestActive = false;
    }

    void JustDied(Unit* pKiller)
    {
        if (bQuestActive)
        {
            if (Player* player = m_creature->GetMap()->GetPlayer(playerGuid))
                if (player->GetQuestStatus(QUEST_WHATISGOINGON) == QUEST_STATUS_INCOMPLETE)
                    player->FailQuest(QUEST_WHATISGOINGON);
            Reset();
        }
    }

    void OnQuestAccept(Player* pPlayer, const Quest* pQuest)
    {
        if (!bQuestActive && (pQuest->GetQuestId() == QUEST_WHATISGOINGON))
        {
            playerGuid = pPlayer->GetObjectGuid();
            uiPhase = 1;
            uiTimer = 10;
            bQuestActive = true;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        iSumCreaCnt++;
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        iSumCreaDead++;
    }

    void DoSpawnDwarves()
    {
        float x = 374.45f;
        float y = -185.57f;
        float z = -70.11f;

        m_creature->SummonCreature(
            NPC_GARDE_RAGENCLUME,
            x, y, z, 5.0f,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        m_creature->SummonCreature(
            NPC_GARDE_RAGENCLUME,
            x, y, z, 5.0f,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        m_creature->SummonCreature(
            NPC_GARDIEN_RAGENCLUME,
            x, y, z, 5.0f,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        m_creature->SummonCreature(
            NPC_TORTIONNAIRE,
            x, y, z, 5.0f,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (uiPhase)
        {
            if (uiTimer <= uiDiff)
            {
                switch (uiPhase)
                {
                    case 1:
                        //DoScriptText(SAY_QUEST_ACCEPT_ATTACK, me);
                        uiTimer = 2000;
                        uiPhase = 2;
                        break;
                    case 2:
                        DoSpawnDwarves();
                        uiTimer = 32000;
                        uiPhase = 3;
                        break;
                    case 3:
                        DoSpawnDwarves();
                        uiTimer = 2000;
                        uiPhase = 4;
                        break;
                    case 4:
                        uiTimer = 500;
                        // Waves killed
                        if (iSumCreaDead >= iSumCreaCnt)
                        {
                            // DoScriptText(SAY_END, m_creature);
                            // Validate Quest :
                            if (Player* player = m_creature->GetMap()->GetPlayer(playerGuid))
                                player->AreaExploredOrEventHappens(QUEST_WHATISGOINGON);
                            Reset();
                        }
                        break;
                    default:
                        break;
                }
            }
            else uiTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_GorShak(Creature* pCreature)
{
    return new npc_GorShakAI(pCreature);
}

bool QuestAccept_npc_GorShak(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (npc_GorShakAI* pScriptedAI = dynamic_cast<npc_GorShakAI*>(pCreature->AI()))
        pScriptedAI->OnQuestAccept(pPlayer, pQuest);
    return true;
}

// FIN Elysium : Ivina
//ALITA
enum
{
    GOUTTE_DE_FLAMMES   = 15529,
    SAOUL_EFFET_VISUEL  = 15533, //turns you black
    ROOT                = 23973 //in DB
};

struct npc_ironhand_guardianAI : public ScriptedAI
{
    npc_ironhand_guardianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        GoutteDeFlammes_Timer = 10000;
        Saoul_Timer = 1000;
        Reset();
    }

    uint32 GoutteDeFlammes_Timer;
    uint32 Saoul_Timer;

    void Reset()  {    }

    void UpdateAI(const uint32 diff)
    {
        if (GoutteDeFlammes_Timer < diff)
        {
            DoCastSpellIfCan(m_creature, GOUTTE_DE_FLAMMES);
            GoutteDeFlammes_Timer = 16000;
        }
        else GoutteDeFlammes_Timer -= diff;

        if (Saoul_Timer < 30000) //test une seule fois. <= mwai.
        {
            if (Saoul_Timer < diff) //en fait je vois pas à quoi ca sert, et puisque ça interomp....
            {
                DoCastSpellIfCan(m_creature, SAOUL_EFFET_VISUEL);
                Saoul_Timer = 40000;
            }
            else Saoul_Timer -= diff;
        }
    }
};
CreatureAI* GetAI_npc_ironhand_guardian(Creature* pCreature)
{
    return new npc_ironhand_guardianAI(pCreature);
}


void AddSC_blackrock_depths()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_shadowforge_brazier";
    newscript->pGOHello = &GOHello_go_shadowforge_brazier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_ring_of_law";
    newscript->pAreaTrigger = &AreaTrigger_at_ring_of_law;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_grimstone";
    newscript->GetAI = &GetAI_npc_grimstone;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_phalanx";
    newscript->GetAI = &GetAI_mob_phalanx;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kharan_mighthammer";
    newscript->pGossipHello =  &GossipHello_npc_kharan_mighthammer;
    newscript->pGossipSelect = &GossipSelect_npc_kharan_mighthammer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lokhtos_darkbargainer";
    newscript->pGossipHello =  &GossipHello_npc_lokhtos_darkbargainer;
    newscript->pGossipSelect = &GossipSelect_npc_lokhtos_darkbargainer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_rocknot";
    newscript->GetAI = &GetAI_npc_rocknot;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_rocknot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_dark_keeper_portrait";
    newscript->pGOHello = &GOHello_go_dark_keeper_portrait;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_thunderbrew_laguer_keg";
    newscript->pGOHello = &GOHello_go_thunderbrew_laguer_keg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_relic_coffer_door";
    newscript->pGOHello = &GOHello_go_relic_coffer_door;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_watchman_doomgrip";
    newscript->GetAI = &GetAI_npc_watchman_doomgrip;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ribbly_fermevanne";
    newscript->GetAI = &GetAI_npc_ribbly_fermevanne;
    newscript->pGossipHello = &GossipHello_npc_ribbly_fermevanne;
    newscript->pGossipSelect = &GossipSelect_npc_ribbly_fermevanne;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_golem_lord_argelmach";
    newscript->GetAI = &GetAI_npc_golem_lord_argelmach;
    newscript->RegisterSelf();

    // Elysium : added by Ivina
    newscript = new Script;
    newscript->Name = "npc_GorShak";
    newscript->GetAI = &GetAI_npc_GorShak;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_GorShak;
    newscript->RegisterSelf();//FIN

    //Alita
    newscript = new Script;
    newscript->Name = "npc_ironhand_guardian";
    newscript->GetAI = &GetAI_npc_ironhand_guardian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_nagmara";
    newscript->GetAI = &GetAI_npc_nagmara;
    newscript->pGossipSelect = &GossipSelect_npc_nagmara;
    newscript->RegisterSelf();
}
