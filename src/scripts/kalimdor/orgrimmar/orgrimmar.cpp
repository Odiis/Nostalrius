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
SDName: Orgrimmar
SD%Complete: 100
SDComment: Quest support: 2460, 5727, 6566
SDCategory: Orgrimmar
EndScriptData */

/* ContentData
npc_neeru_fireblade     npc_text + gossip options text missing
npc_shenthul
npc_thrall_warchief
EndContentData */

#include "scriptPCH.h"
#include "./../../custom/PlayerStartMgr.h"

/*######
## npc_neeru_fireblade
######*/

#define QUEST_5727  5727

bool GossipHello_npc_neeru_fireblade(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_5727) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "You may speak frankly, Neeru...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(4513, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_neeru_fireblade(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] ...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(4513, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(QUEST_5727);
            break;
    }
    return true;
}

/*######
## npc_shenthul
######*/

enum
{
    QUEST_SHATTERED_SALUTE  = 2460
};

struct npc_shenthulAI : public ScriptedAI
{
    npc_shenthulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool CanTalk;
    bool CanEmote;
    uint32 Salute_Timer;
    uint32 Reset_Timer;
    uint64 playerGUID;

    void Reset()
    {
        CanTalk = false;
        CanEmote = false;
        Salute_Timer = 6000;
        Reset_Timer = 0;
        playerGUID = 0;
    }

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_SALUTE && pPlayer->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
        {
            if (CanEmote)
            {
                pPlayer->AreaExploredOrEventHappens(QUEST_SHATTERED_SALUTE);
                Reset();
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CanEmote)
        {
            if (Reset_Timer < diff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(playerGUID))
                {
                    if (pPlayer->GetTypeId() == TYPEID_PLAYER && pPlayer->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->FailQuest(QUEST_SHATTERED_SALUTE);
                }
                Reset();
            }
            else Reset_Timer -= diff;
        }

        if (CanTalk && !CanEmote)
        {
            if (Salute_Timer < diff)
            {
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                CanEmote = true;
                Reset_Timer = 60000;
            }
            else Salute_Timer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_shenthul(Creature* pCreature)
{
    return new npc_shenthulAI(pCreature);
}

bool QuestAccept_npc_shenthul(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SHATTERED_SALUTE)
    {
        ((npc_shenthulAI*)pCreature->AI())->CanTalk = true;
        ((npc_shenthulAI*)pCreature->AI())->playerGUID = pPlayer->GetGUID();
    }
    return true;
}

/********************/
/* SAURFANG *********/

enum
{
    SPELL_SF_EXECUTE      = 7160,  //OK
    SPELL_SF_CLEAVE       = 15284, //OK
    SPELL_SF_CHARGE       = 22886, //OK
    SPELL_SF_THUNDERCLAP  = 23931, //?
    SPELL_SF_MORTALSTRIKE = 12294, //?
    SPELL_SF_SAURFANGRAGE = 26339,
};
struct npc_saurfangAI : public ScriptedAI
{
    npc_saurfangAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiExecute_Timer;
    uint32 m_uiCleave_Timer;
    uint32 m_uiCharge_Timer;
    uint32 m_uiThunderClap_Timer;
    uint32 m_uiMortalStrike_Timer;
    uint32 m_uiSaurfangRage_Timer;
    bool isInCombat;

    void Reset()
    {
        isInCombat = false;
        m_uiExecute_Timer = 0;
        m_uiCleave_Timer = 7000;
        m_uiCharge_Timer = 0;
        m_uiThunderClap_Timer = 5000;
        m_uiMortalStrike_Timer = 13000;
        m_uiSaurfangRage_Timer = urand(3000, 8000);
    }

    void Aggro(Unit* pWho)
    {
        if (isInCombat == false)
        {
            m_creature->MonsterTextEmote("Saurfang's eyes glow red for a brief moment");
            isInCombat = true;
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            m_creature->MonsterSay("Is that the best you can do?", 0, 0);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->getVictim()->GetHealth() <= m_creature->getVictim()->GetMaxHealth() * 0.2f && m_uiExecute_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_EXECUTE);
            m_uiExecute_Timer = 2000;
        }
        else m_uiExecute_Timer -= diff;

        if (m_uiMortalStrike_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_MORTALSTRIKE);
            m_uiMortalStrike_Timer = 13000;
        }
        else m_uiMortalStrike_Timer -= diff;

        /*        if (m_uiCleave_Timer < diff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_CLEAVE);
                    m_uiCleave_Timer = 7000;
                }
                else m_uiCleave_Timer -= diff;
        */
        if (m_uiCharge_Timer < diff && m_creature->GetDistance(m_creature->getVictim()->GetPositionX(),
                m_creature->getVictim()->GetPositionY(),
                m_creature->getVictim()->GetPositionZ()) >= 8.0f)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_CHARGE);
            m_uiCharge_Timer = 9000;
        }
        else m_uiCharge_Timer -= diff;
        /*
                if (m_uiThunderClap_Timer < diff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_THUNDERCLAP);
                    m_uiThunderClap_Timer = 9000;
                }
                else m_uiThunderClap_Timer -= diff;
        */
        if (m_uiSaurfangRage_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_SAURFANGRAGE) ;
            m_uiSaurfangRage_Timer = 8000;
        }
        else m_uiSaurfangRage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_npc_saurfang(Creature* pCreature)
{
    return new npc_saurfangAI(pCreature);
}

/*######
## npc_thrall_warchief
######*/

#define QUEST_6566              6566

#define SPELL_CHAIN_LIGHTNING   16033
#define SPELL_SHOCK             16034

//TODO: verify abilities/timers
struct npc_thrall_warchiefAI : public ScriptedAI
{
    npc_thrall_warchiefAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 ChainLightning_Timer;
    uint32 Shock_Timer;

    void Reset()
    {
        ChainLightning_Timer = 2000;
        Shock_Timer = 8000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (ChainLightning_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING);
            ChainLightning_Timer = 9000;
        }
        else ChainLightning_Timer -= diff;

        if (Shock_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOCK);
            Shock_Timer = 15000;
        }
        else Shock_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_thrall_warchief(Creature* pCreature)
{
    return new npc_thrall_warchiefAI(pCreature);
}

bool GossipHello_npc_thrall_warchief(Player* pPlayer, Creature* pCreature)
{
    if ((pPlayer->GetQuestStatus(PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF)) == QUEST_STATUS_INCOMPLETE) && (PSM_CheckOptionForPlayer(pPlayer, OPTION_SET_55)))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NOTREADY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_NOTREADY);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_IAMREADY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_IAMREADY);
        pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_1), pCreature->GetGUID());
    }
    else
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        if (pPlayer->GetQuestStatus(QUEST_6566) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Please share your wisdom with me, Warchief.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_thrall_warchief(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_NOTREADY:
            pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_2), pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_IAMREADY:
            pPlayer->AreaExploredOrEventHappens(PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF));
            if (pCreature->isQuestGiver())
                pPlayer->PrepareQuestMenu(pCreature->GetGUID());
            pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_3), pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What discoveries?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(5733, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Usurper?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(5734, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "With all due respect, Warchief - why not allow them to be destroyed? Does this not strengthen our position?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(5735, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I... I did not think of it that way, Warchief.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(5736, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I live only to serve, Warchief! My life is empty and meaningless without your guidance.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(5737, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Of course, Warchief!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(5738, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(QUEST_6566);
            break;
    }
    return true;
}

bool QuestRewarded_npc_thrall_warchief(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF))
        PSM_AddSpells(pPlayer);
    else if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, GET_PEX_REWARD))
        PSM_PexReward(pPlayer);
    return true;
}

bool GossipHello_npc_eitrigg(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(4941) == QUEST_STATUS_INCOMPLETE)
        pPlayer->AreaExploredOrEventHappens(4941);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

/*######
## npc_voljin
######*/

bool GossipHello_npc_voljin(Player* pPlayer, Creature* pCreature)
{


    if ((pPlayer->GetQuestStatus(PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF)) == QUEST_STATUS_INCOMPLETE) && (PSM_CheckOptionForPlayer(pPlayer, OPTION_SET_55)))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NOTREADY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_NOTREADY);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_IAMREADY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_IAMREADY);
        pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_1), pCreature->GetGUID());
    }
    else
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_voljin(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_NOTREADY:
            pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_2), pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_IAMREADY:
            pPlayer->AreaExploredOrEventHappens(PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF));
            if (pCreature->isQuestGiver())
                pPlayer->PrepareQuestMenu(pCreature->GetGUID());
            pPlayer->SEND_GOSSIP_MENU(PSM_GetGossipMessID(pPlayer, GS_FACTION_CHIEF_3), pCreature->GetGUID());
            break;
    }
    return true;
}

bool QuestRewarded_npc_voljin(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF))
        PSM_AddSpells(pPlayer);
    else if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, GET_PEX_REWARD))
        PSM_PexReward(pPlayer);
    return true;
}


enum eSpells
{
    SPELL_HEX               = 16097,
    SPELL_SHADOW_SHOCK      = 17289,
    SPELL_SHADOW_WORD_PAIN  = 17146,
    SPELL_SHOOT             = 20463,
    SPELL_VEIL_OF_SHADOW    = 17820,
};

struct boss_vol_jinAI : public ScriptedAI
{
public:
    boss_vol_jinAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() override
    {
        m_uiHexTimer = 10000;
        m_uiShadowShockTimer = 8000;
        m_uiShadowWordPainTimer = 12000;
        m_uiShootTimer = 10000;
        m_uiVeilOfShadowTimer = 18000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHexTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HEX) == CAST_OK)
                m_uiHexTimer = 10000;
        }
        else
            m_uiHexTimer -= uiDiff;

        if (m_uiShadowShockTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_SHOCK) == CAST_OK)
                m_uiShadowShockTimer = urand(8000, 12000);
        }
        else
            m_uiShadowShockTimer -= uiDiff;

        if (m_uiShadowWordPainTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_WORD_PAIN) == CAST_OK)
                    m_uiShadowWordPainTimer = 12000;
            }
        }
        else
            m_uiShadowWordPainTimer -= uiDiff;

        if (m_uiShootTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT) == CAST_OK)
                m_uiShootTimer = 10000;
        }
        else
            m_uiShootTimer -= uiDiff;

        if (m_uiVeilOfShadowTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_VEIL_OF_SHADOW) == CAST_OK)
                m_uiVeilOfShadowTimer = 18000;
        }
        else
            m_uiVeilOfShadowTimer -= uiDiff;

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(uiDiff);
    }

private:
    uint32 m_uiHexTimer;
    uint32 m_uiShadowShockTimer;
    uint32 m_uiShadowWordPainTimer;
    uint32 m_uiShootTimer;
    uint32 m_uiVeilOfShadowTimer;
};

CreatureAI* GetAI_boss_vol_jin(Creature* pCreature)
{
    return new boss_vol_jinAI(pCreature);
}


void AddSC_orgrimmar()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_saurfang";
    newscript->GetAI = &GetAI_npc_saurfang;
//    newscript->pGossipHello =  &GossipHello_npc_thrall_warchief;
//    newscript->pGossipSelect = &GossipSelect_npc_thrall_warchief;
//    newscript->pQuestRewardedNPC = &QuestRewarded_npc_thrall_warchief;
    newscript->RegisterSelf();


    newscript = new Script;
    newscript->Name = "npc_neeru_fireblade";
    newscript->pGossipHello =  &GossipHello_npc_neeru_fireblade;
    newscript->pGossipSelect = &GossipSelect_npc_neeru_fireblade;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_shenthul";
    newscript->GetAI = &GetAI_npc_shenthul;
    newscript->pQuestAcceptNPC =  &QuestAccept_npc_shenthul;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_thrall_warchief";
    newscript->GetAI = &GetAI_npc_thrall_warchief;
    newscript->pGossipHello =  &GossipHello_npc_thrall_warchief;
    newscript->pGossipSelect = &GossipSelect_npc_thrall_warchief;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_thrall_warchief;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_voljin";
    newscript->GetAI = &GetAI_boss_vol_jin;
    newscript->pGossipHello   = &GossipHello_npc_voljin;
    newscript->pGossipSelect  = &GossipSelect_npc_voljin;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_voljin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_eitrigg";
    newscript->pGossipHello =  &GossipHello_npc_eitrigg;
    newscript->RegisterSelf();
}
