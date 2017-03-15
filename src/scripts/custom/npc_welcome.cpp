
#include "scriptPCH.h"
#include "./../custom/PlayerStartMgr.h"


#define SHOW_GOSSIP(a) player->SEND_GOSSIP_MENU(a,_Creature->GetObjectGuid())

/*######
## npc_welcome2
######*/

struct npc_welcome2AI : public ScriptedAI
{
    npc_welcome2AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        uiCheckViewCD = 10000;
    }

    uint32 faction;
    uint32 uiCheckViewCD;
    std::list<ObjectGuid> lNearbyPlayers;
    std::list<ObjectGuid> lAllPlayers;
    uint64 currentPlayerGUID;
    uint8 currentPlayerRace;
    uint32 uiSpeechTimer;
    uint32 uiPhase;

    void Reset()
    {
        m_creature->SetWorldMask(WORLD_SEE_ALL);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        currentPlayerGUID = 0;
        currentPlayerRace = 0;
        uiSpeechTimer = 3000;
        uiPhase = 0;
    }

    void MoveInLineOfSight(Unit* who)
    {
        // On n'interpelle que les nouveaux joueurs.
        if (Player* pPlayer = who->ToPlayer())
        {
            if (pPlayer->HasCustomFlag(CUSTOM_FLAG_PEX_FINISHED | CUSTOM_FLAG_HL))
                return;

            bool inList = false;
            for (std::list<ObjectGuid>::iterator itr = lNearbyPlayers.begin(); itr != lNearbyPlayers.end(); ++itr)
            {
                if ((*itr) == pPlayer->GetObjectGuid())
                {
                    inList = true;
                    break;
                }
            }
            if (inList)
                return;

            for (std::list<ObjectGuid>::iterator itr = lAllPlayers.begin(); itr != lAllPlayers.end(); ++itr)
            {
                if ((*itr) == pPlayer->GetObjectGuid())
                {
                    inList = true;
                    break;
                }
            }
            if (inList)
                return;

            lNearbyPlayers.push_back(pPlayer->GetObjectGuid());
            lAllPlayers.push_back(pPlayer->GetObjectGuid());
        }
    }

    void StartSpeech(Player* player, uint32 phase)
    {
        player->AddCustomFlag(CUSTOM_FLAG_SPEECH_OK);
        currentPlayerGUID = player->GetGUID();
        currentPlayerRace = player->getRace();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        player->CLOSE_GOSSIP_MENU();
        uiSpeechTimer = 1000;
        uiPhase = phase;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (Player* pCurrentPlayer = m_creature->GetMap()->GetPlayer(currentPlayerGUID))
        {
            if (uiSpeechTimer < uiDiff)
            {
                switch (uiPhase)
                {
                    case 00:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_creature->MonsterSay(NPC_SPEECH_0, 0, 0);
                        uiSpeechTimer = 10000;
                        if (pCurrentPlayer->GetTeam() == HORDE)
                            uiPhase = 11;
                        else
                            uiPhase = 01;
                        break;
                    case 11:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_creature->MonsterSay(NPC_SPEECH_H_1, 0, 0);
                        uiSpeechTimer = 10000;
                        uiPhase = 12;
                        break;
                    case 12:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_creature->MonsterSay(NPC_SPEECH_H_2, 0, 0);
                        uiSpeechTimer = 10000;
                        uiPhase = 13;
                        m_creature->MonsterTextEmote(NPC_SPEECH_LOST_MINDS_H, NULL);
                        break;
                    case 13:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_creature->MonsterSay(NPC_SPEECH_H_3, 0, 0);
                        uiSpeechTimer = 10000;
                        uiPhase = 20;
                        break;
                    case 01:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_creature->MonsterSay(NPC_SPEECH_A_1, 0, 0);
                        uiSpeechTimer = 10000;
                        uiPhase = 02;
                        m_creature->MonsterTextEmote(NPC_SPEECH_LOST_MINDS_A, NULL);
                        break;
                    case 02:
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_creature->MonsterSay(NPC_SPEECH_A_2, 0, 0);
                        uiSpeechTimer = 10000;
                        uiPhase = 20;
                        break;
                    case 20:
                        switch (currentPlayerRace)
                        {
                            case RACE_HUMAN:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_creature->MonsterSay(NPC_SPEECH_A_3_HUMAIN, 0, 0);
                                break;
                            case RACE_ORC:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_creature->MonsterSay(NPC_SPEECH_H_4_ORC, 0, 0);
                                break;
                            case RACE_DWARF:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_creature->MonsterSay(NPC_SPEECH_A_3_NAIN, 0, 0);
                                break;
                            case RACE_NIGHTELF:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_creature->MonsterSay(NPC_SPEECH_A_3_ELFE, 0, 0);
                                break;
                            case RACE_UNDEAD:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_creature->MonsterSay(NPC_SPEECH_H_4_UNDEAD, 0, 0);
                                break;
                            case RACE_TAUREN:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_creature->MonsterSay(NPC_SPEECH_H_4_TAUREN, 0, 0);
                                break;
                            case RACE_GNOME:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_creature->MonsterSay(NPC_SPEECH_A_3_GNOME, 0, 0);
                                break;
                            case RACE_TROLL:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_creature->MonsterSay(NPC_SPEECH_H_4_TROLL, 0, 0);
                                break;
                        }
                        uiSpeechTimer = 10000;
                        uiPhase = 21;
                        break;
                    case 21:
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        currentPlayerGUID = 0;
                        if (m_creature->isQuestGiver())
                            pCurrentPlayer->PrepareQuestMenu(m_creature->GetGUID());
                        pCurrentPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_QUESTGIVING, m_creature->GetGUID());
                        break;
                }
            }
            else
                uiSpeechTimer -= uiDiff;
        }
        else
        {
            Reset();
            if (uiCheckViewCD < uiDiff)
            {
                std::string plNames;
                bool foundOne = false;
                for (std::list<ObjectGuid>::iterator itr = lNearbyPlayers.begin(); itr != lNearbyPlayers.end(); ++itr)
                {
                    if (Unit* pUnit = Unit::GetUnit(*m_creature, *itr))
                    {
                        if (pUnit->GetTypeId() != TYPEID_PLAYER)
                            continue;
                        if (foundOne)
                            plNames += ", ";

                        plNames += pUnit->GetName();
                        foundOne = true;
                    }
                }
                if (foundOne)
                {
                    m_creature->PMonsterSay(NPC_SPEECH_JAI_PROPOSITION, plNames.c_str());
                    lNearbyPlayers.clear();
                }
                uiCheckViewCD = 40000;
            }
            else
                uiCheckViewCD -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_welcome2AI(Creature* pCreature)
{
    return new npc_welcome2AI(pCreature);
}

bool GossipHello_npc_welcome2(Player *player, Creature *pCreature)
{
    if ((!player->HasCustomFlag(CUSTOM_FLAG_SPEECH_OK)) && (PSM_CheckOptionForPlayer(player, OPTION_SET_60)))
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WELCOME, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(GOSSIP_MENU_WELCOME, pCreature->GetGUID());
    }
    else if (PSM_CheckOptionForPlayer(player, OPTION_SET_60))
    {
        if (pCreature->isQuestGiver())
            player->PrepareQuestMenu(pCreature->GetGUID());
        player->SEND_GOSSIP_MENU(GOSSIP_MENU_QUESTGIVING, pCreature->GetGUID());
    }
    else
    {
        if (!player->HasCustomFlag(CUSTOM_FLAG_SPEECH_OK))
            player->AddCustomFlag(CUSTOM_FLAG_SPEECH_OK);
        player->SEND_GOSSIP_MENU(GOSSIP_MENU_ALREADYHL, pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_welcome2(Player *player, Creature *pCreature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            if (npc_welcome2AI* pScriptedAI = dynamic_cast<npc_welcome2AI*>(pCreature->AI()))
                pScriptedAI->StartSpeech(player, 0);
            break;
    }
    return true;
}

/*######
## npc_welcome
######*/

enum // TODO
{
    NPC_WELCOME_HELLO = 1,
    NPC_WELCOME_ERROR = 1,
    NPC_WELCOME_OK    = 1
};

bool GossipHello_npc_welcome(Player *player, Creature *_Creature)
{
    // Phase : Aucune
    if (PSM_CheckOptionForPlayer(player, OPTION_SET_55))
        player->ADD_GOSSIP_ITEM(0, "Commencer niveau 55 et jouer Blizzlike ensuite", GOSSIP_SENDER_MAIN, OPTION_SET_55);
    if (PSM_CheckOptionForPlayer(player, OPTION_GET_REWARD))
        player->ADD_GOSSIP_ITEM(0, "Obtenir ma recompense, et passer en phase HL",   GOSSIP_SENDER_MAIN, OPTION_GET_REWARD);

    player->SEND_GOSSIP_MENU(NPC_WELCOME_HELLO, _Creature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_welcome(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;
    if (!PSM_CheckOptionForPlayer(player, action))
        return false;

    switch (action)
    {
        case OPTION_SET_55:
        {
            std::map<uint32, PlayerStartEquip*> equip = sPlayerStartMgr.GetEquipment(1);
            std::map<uint32, PlayerStartSpell*> spells = sPlayerStartMgr.GetSpells();
            uint8 myClass = player->getClass();
            uint8 myRace = player->getRace();
            uint8 i = 0;

            /* Ajout des sorts */
            std::map<uint32, PlayerStartSpell*>::iterator iter;
            for (iter = spells.begin(); iter != spells.end(); ++iter)
            {
                if (myClass != iter->second->playerClass)
                    continue;
                else if (myRace != iter->second->playerRace && iter->second->playerRace)
                    continue;

                SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(iter->second->spellId);
                if (!spellInfo) // Sort inexistant
                    continue;
                if (!player->HasSpell(iter->second->spellId))
                    player->learnSpell(iter->second->spellId, false);
                ++i;
            }
            //sLog.outString("- %u sorts ajoutes : class %u",i,myClass);
            // Ajout de la monture : Code by Chakor
            uint32 mount = PSM_GetMountId(player);

            if (!player->HasItemCount(mount, 1, true))
            {
                ItemPosCountVec dest;
                InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, mount, 1);
                if (msg != 0)
                    player->SendEquipError(msg, NULL, NULL);

                Item* monture = player->StoreNewItem(dest, mount, true);

                if (monture)
                    player->SendNewItem(monture, 1, true, false);
            }

            //if(!spellOnly)
            {
                i = 0;

                /* Ajout du stuff */
                std::map<uint32, PlayerStartEquip*>::iterator iterEquip;
                for (iterEquip = equip.begin(); iterEquip != equip.end(); ++iterEquip)
                {
                    if (myClass != iterEquip->second->playerClass)
                        continue;
                    /* Déplacement au préalable du stuff de départ équipé .*/
                    player->RemoveItem(INVENTORY_SLOT_BAG_0, iterEquip->second->equipmentSlot, true);

                    // @TODO : Vérifier si l'item existe dans la DB
                    if (!player->GetItemByPos(INVENTORY_SLOT_BAG_0, iterEquip->second->equipmentSlot)
                            && iterEquip->second->equipmentSlot <= 20)
                    {
                        // Pas d'item équipé la.
                        player->EquipNewItem(iterEquip->second->equipmentSlot, iterEquip->second->itemId, true);
                    }
                    else //Déja un item a la position => Go dans le sac, si on peut.
                    {
                        ItemPosCountVec dest;
                        InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, iterEquip->second->itemId, 1);
                        if (msg != 0)
                            player->SendEquipError(msg, NULL, NULL);
                        else
                        {
                            Item* item = player->StoreNewItem(dest, iterEquip->second->itemId, true);
                            if (item)
                                player->SendNewItem(item, 1, true, false);
                        }
                    }
                    ++i;
                }
                player->CastSpell(player, MOUNT_SPELL, false);
                player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
                player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
                player->GiveLevel(55);
                player->ModifyMoney(20000);
            }
            player->UpdateSkillsToMaxSkillsForLevel();

            player->RemoveCustomFlag(CUSTOM_FLAG_IN_PEX | CUSTOM_FLAG_PEX_FINISHED);
            player->AddCustomFlag(CUSTOM_FLAG_HL);

            // Ajout des flys
            if (player->GetTeam() == ALLIANCE)
                player->m_taxi.LoadTaxiMask("3456411898 2148078928 49991 0 0 0 0 0 ");
            else
                player->m_taxi.LoadTaxiMask("561714688 282102432 52408 0 0 0 0 0 ");

            player->SaveToDB();
            // Affichage d'un petit message :
            char sMessage[200];
            sprintf(sMessage, "Bienvenue sur Elysium, %s ! En esperant que vous apprecierez notre royaume.", player->GetName());
            _Creature->MonsterSay(sMessage, 0, 0);

            SHOW_GOSSIP(NPC_WELCOME_OK);
            return true;
        }
        break;
        case OPTION_GET_REWARD:
        {
            // Recompense
            ItemPosCountVec dest;
            InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 19160, 1);
            if (msg != 0)
            {
                player->SendEquipError(msg, NULL, NULL);
                return true;
            }
            Item* recompense = player->StoreNewItem(dest, 19160, true);

            if (recompense)
                player->SendNewItem(recompense, 1, true, false);

            player->RemoveCustomFlag(CUSTOM_FLAG_IN_PEX);
            player->AddCustomFlag(CUSTOM_FLAG_PEX_FINISHED);
            player->AddCustomFlag(CUSTOM_FLAG_HL);

            player->SaveToDB();
            // Affichage d'un petit message :
            char sMessage[200];
            sprintf(sMessage, "Bravo pour votre courage, %s ! Votre determination ne sera pas oubliee.", player->GetName());
            _Creature->MonsterSay(sMessage, 0, 0);

            SHOW_GOSSIP(NPC_WELCOME_OK);
            return true;
        }
        break;
    } // End Switch
    SHOW_GOSSIP(NPC_WELCOME_ERROR);
    return false;
}

/*######
## npc_welcomeblacksmith
######*/

bool QuestRewarded_npc_welcomeblacksmith(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, TO_BLACKSMITHER))
    {
        PSM_AddItems(pPlayer);
        pCreature->MonsterSay(NPC_SPEECH_CONGRATS);
    }
    return true;
}

/*######
## npc_factionchief
######*/

bool GossipHello_npc_factionchief(Player* pPlayer, Creature* pCreature)
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

bool GossipSelect_npc_factionchief(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
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

bool QuestRewarded_npc_factionchief(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, TO_FACTION_CHIEF))
    {
        PSM_AddSpells(pPlayer);
        pCreature->HandleEmote(66);
    }
    else if (pQuest->GetQuestId() == PSM_GetQuestID(pPlayer, GET_PEX_REWARD))
        PSM_PexReward(pPlayer);
    return true;
}

/*######
## npc_welcome 3
######*/

bool GossipHello_npc_welcome3(Player *player, Creature *_Creature)
{
    if (!PSM_CheckOptionForPlayer(player, OPTION_SET_60))
    {
        SHOW_GOSSIP(19997);
        return true;
    }
    player->ADD_GOSSIP_ITEM(0, "I understand these rules and I will respect them.", GOSSIP_SENDER_MAIN, OPTION_SET_60);
    player->SEND_GOSSIP_MENU(19999, _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_welcome3(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;
    if (!PSM_CheckOptionForPlayer(player, action))
    {
        SHOW_GOSSIP(19997);
        return true;
    }

    switch (action)
    {
        case OPTION_SET_60:
        {
            std::map<uint32, PlayerStartEquip*> equip = sPlayerStartMgr.GetEquipment(3);
            std::map<uint32, PlayerStartSpell*> spells = sPlayerStartMgr.GetSpells();
            uint8 myClass = player->getClass();
            uint8 myRace = player->getRace();
            uint32 i = 0;

            /* Ajout des sorts */
            std::map<uint32, PlayerStartSpell*>::iterator iter;
            for (iter = spells.begin(); iter != spells.end(); ++iter)
            {
                if (myClass != iter->second->playerClass)
                    continue;
                else if (myRace != iter->second->playerRace && iter->second->playerRace)
                    continue;

                SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(iter->second->spellId);
                if (!spellInfo) // Sort inexistant
                    continue;
                if (!player->HasSpell(iter->second->spellId))
                    player->learnSpell(iter->second->spellId, false);
                ++i;
            }

            if (player->getClass() == CLASS_SHAMAN)
            {
                player->SetSkill(160, 300, 300);
                player->SetSkill(172, 300, 300);
            }
            else if (player->getClass() == CLASS_PRIEST)
            {
                switch (player->getRace())
                {
                    case RACE_DWARF:
                        player->learnSpell(19243, false);
                        player->learnSpell(6346, false);
                        break;
                    case RACE_HUMAN:
                        player->learnSpell(19243, false);
                        player->learnSpell(19275, false);
                        break;
                    case RACE_NIGHTELF:
                        player->learnSpell(19305, false);
                        player->learnSpell(19293, false);
                        break;
                    case RACE_UNDEAD:
                        player->learnSpell(19280, false);
                        player->learnSpell(19266, false);
                        break;
                    case RACE_TROLL:
                        player->learnSpell(19312, false);
                        player->learnSpell(19285, false);
                        break;
                }
            }
            player->CastSpell(player, MOUNT_SPELL, false);
            player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
            player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
            player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
            player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
            player->GiveLevel(60);

            player->UpdateSkillsToMaxSkillsForLevel();

            player->RemoveCustomFlag(CUSTOM_FLAG_IN_PEX | CUSTOM_FLAG_PEX_FINISHED);
            player->AddCustomFlag(CUSTOM_FLAG_INSTANT60);

            // Ajout des flys
            if (player->GetTeam() == ALLIANCE)
                player->m_taxi.LoadTaxiMask("3456411898 2148078928 49991 0 0 0 0 0 ");
            else
                player->m_taxi.LoadTaxiMask("561714688 282102432 52408 0 0 0 0 0 ");

            // Give AV reputation
            if (player->GetTeam() == ALLIANCE)
                player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(730), 21000);
            else
                player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(729), 21000);

            // Set rank
            player->SetStoredHonor(45000);
            player->UpdateHonor();
            player->ModifyMoney(9000000);

            SHOW_GOSSIP(19998);
            return true;
        }
        break;
    } // End Switch
    SHOW_GOSSIP(19997);
    return false;
}

/* NEW NPC */
bool GossipHello_npc_welcome4(Player *player, Creature *_Creature)
{
    if (PSM_CheckOptionForPlayer(player, OPTION_SET_60))
    {
        player->ADD_GOSSIP_ITEM(0, "Instant level 20", GOSSIP_SENDER_MAIN, OPTION_SET_20);
        player->ADD_GOSSIP_ITEM(0, "Instant level 30", GOSSIP_SENDER_MAIN, OPTION_SET_30);
        player->ADD_GOSSIP_ITEM(0, "Instant level 40", GOSSIP_SENDER_MAIN, OPTION_SET_40);
        player->ADD_GOSSIP_ITEM(0, "Instant level 50", GOSSIP_SENDER_MAIN, OPTION_SET_50);
    }

    player->SEND_GOSSIP_MENU(NPC_WELCOME_HELLO, _Creature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_welcome4(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    uint32 m_uiLevel = 0;
    std::map<uint32, PlayerStartSpell*> spells = sPlayerStartMgr.GetLowSpells();
    uint8 myClass = player->getClass();
    uint8 myRace = player->getRace();
    uint32 i = 0;

    if (sender != GOSSIP_SENDER_MAIN)
        return false;

    if (!PSM_CheckOptionForPlayer(player, action))
        return false;

    switch (action)
    {
        case OPTION_SET_20:
            m_uiLevel = 20;
            break;
        case OPTION_SET_30:
            m_uiLevel = 30;
            break;
        case OPTION_SET_40:
            m_uiLevel = 40;
            break;
        case OPTION_SET_50:
            m_uiLevel = 50;
            break;
        default :
            return false;
            break;
    }

    /* Ajout des sorts */
    std::map<uint32, PlayerStartSpell*>::iterator iter;
    for (iter = spells.begin(); iter != spells.end(); ++iter)
    {
        if (myClass != iter->second->playerClass)
            continue;
        else if (myRace != iter->second->playerRace && iter->second->playerRace)
            continue;

        SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(iter->second->spellId);
        if (!spellInfo) // Sort inexistant
            continue;
        if (!player->HasSpell(iter->second->spellId))
            player->learnSpell(iter->second->spellId, false);
        ++i;
    }

    if (player->getClass() == CLASS_SHAMAN)
    {
        player->SetSkill(160, 300, 300);
        player->SetSkill(172, 300, 300);
    }
    else if (player->getClass() == CLASS_PRIEST)
    {
        switch (player->getRace())
        {
            case RACE_DWARF:
                player->learnSpell(19243, false);
                player->learnSpell(6346, false);
                break;
            case RACE_HUMAN:
                player->learnSpell(19243, false);
                player->learnSpell(19275, false);
                break;
            case RACE_NIGHTELF:
                player->learnSpell(19305, false);
                player->learnSpell(19293, false);
                break;
            case RACE_UNDEAD:
                player->learnSpell(19280, false);
                player->learnSpell(19266, false);
                break;
            case RACE_TROLL:
                player->learnSpell(19312, false);
                player->learnSpell(19285, false);
                break;
        }
    }


    player->CastSpell(player, MOUNT_SPELL, false);

    player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
    player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
    player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
    player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);
    player->GiveLevel(m_uiLevel);
    player->ModifyMoney(9000000);
    player->UpdateSkillsToMaxSkillsForLevel();

    player->RemoveCustomFlag(CUSTOM_FLAG_IN_PEX | CUSTOM_FLAG_PEX_FINISHED);
    player->AddCustomFlag(CUSTOM_FLAG_INSTANT60);

    // Ajout des flys
    if (player->GetTeam() == ALLIANCE)
        player->m_taxi.LoadTaxiMask("3456411898 2148078928 49991 0 0 0 0 0 ");
    else
        player->m_taxi.LoadTaxiMask("561714688 282102432 52408 0 0 0 0 0 ");

    char sMessage[200];
    sprintf(sMessage, "%s is now tuned as a %d level characters.", player->GetName(), m_uiLevel);
    _Creature->MonsterSay(sMessage, 0, 0);

    SHOW_GOSSIP(NPC_WELCOME_OK);
    return true;
}

/* NEW NPC */


void AddSC_npc_welcome()
{
    sPlayerStartMgr.load();
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_welcome";
    newscript->pGossipHello   = &GossipHello_npc_welcome;
    newscript->pGossipSelect  = &GossipSelect_npc_welcome;
    newscript->GetAI          = &GetAI_npc_welcome2AI;
    newscript->RegisterSelf();

    // Instant 60
    newscript = new Script;
    newscript->Name = "npc_welcome2";
    newscript->pGossipHello   = &GossipHello_npc_welcome2;
    newscript->pGossipSelect  = &GossipSelect_npc_welcome2;
    newscript->GetAI          = &GetAI_npc_welcome2AI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_welcomeblacksmith";
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_welcomeblacksmith;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_factionchief";
    newscript->pGossipHello   = &GossipHello_npc_factionchief;
    newscript->pGossipSelect  = &GossipSelect_npc_factionchief;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_factionchief;
    newscript->RegisterSelf();

    // ??
    newscript = new Script;
    newscript->Name = "npc_welcome3";
    newscript->pGossipHello   = &GossipHello_npc_welcome3;
    newscript->pGossipSelect  = &GossipSelect_npc_welcome3;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_welcome_instants";
    newscript->pGossipHello   = &GossipHello_npc_welcome4;
    newscript->pGossipSelect  = &GossipSelect_npc_welcome4;
    newscript->RegisterSelf();

}
