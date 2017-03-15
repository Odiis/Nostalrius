#include "scriptPCH.h"
#include "PlayerStartMgr.h"

INSTANTIATE_SINGLETON_1(PlayerStartMgr);

PlayerStartMgr::PlayerStartMgr()
{
}

PlayerStartMgr::~PlayerStartMgr()
{
}

void PlayerStartMgr::load()
{
    // 2 - Clear
    spells.clear();
    lowSpells.clear();
    equip.clear();
    equip2.clear();
    equip3.clear();

    // 3' - LowSpell
    QueryResult* resultSpell = WorldDatabase.Query(
                                   "SELECT spell, class, race"
                                   " FROM playerlowstart_spell ");
    if (resultSpell)
    {
        uint32 uiSpellCount = 0;

        do
        {
            Field* pFields = resultSpell->Fetch();

            PlayerStartSpell* pTemp = new PlayerStartSpell;

            pTemp->spellId   = pFields[0].GetUInt32();
            uint32 uiEntry          = uiSpellCount;//pTemp->spellId*1000+pFields[1].GetUInt32();
            pTemp->playerClass         = pFields[1].GetUInt32();
            pTemp->playerRace          = pFields[2].GetUInt32();

            lowSpells[uiEntry] = pTemp;
            ++uiSpellCount;
        }
        while (resultSpell->NextRow());

        delete resultSpell;

        sLog.outString("");
        sLog.outString(">> %u sorts de demarrage charge", uiSpellCount);
    }
    else
    {
        sLog.outString("");
        sLog.outString(">> 0 Sorts au demarrage charges. DB vide ?");
    }


    // 3 - Spell
    QueryResult* result = WorldDatabase.Query(
                              "SELECT spell, class, race"
                              " FROM playerstart_spell ");
    if (result)
    {
        uint32 uiSpellCount = 0;

        do
        {
            Field* pFields = result->Fetch();

            PlayerStartSpell* pTemp = new PlayerStartSpell;

            pTemp->spellId   = pFields[0].GetUInt32();
            uint32 uiEntry          = uiSpellCount;//pTemp->spellId*1000+pFields[1].GetUInt32();
            pTemp->playerClass         = pFields[1].GetUInt32();
            pTemp->playerRace          = pFields[2].GetUInt32();

            spells[uiEntry] = pTemp;
            ++uiSpellCount;
        }
        while (result->NextRow());

        delete result;

        sLog.outString("");
        sLog.outString(">> %u sorts de demarrage charge", uiSpellCount);
    }
    else
    {
        sLog.outString("");
        sLog.outString(">> 0 Sorts au demarrage charges. DB vide ?");
    }

    // 4 - Stuff 1
    QueryResult* resultEquip = WorldDatabase.Query(
                                   "SELECT item, slot, class"
                                   " FROM playerstart_equip ");
    if (resultEquip)
    {
        uint32 uiSpellCount = 0;

        do
        {
            Field* pFields = resultEquip->Fetch();

            PlayerStartEquip* pTemp = new PlayerStartEquip;

            pTemp->itemId   = pFields[0].GetUInt32();
            pTemp->equipmentSlot  = pFields[1].GetUInt8();
            pTemp->playerClass    = pFields[2].GetUInt8();
            uint32 uiEntry       = pTemp->itemId * 10000 + pTemp->equipmentSlot * 100 + pTemp->playerClass;

            equip[uiEntry] = pTemp;
            ++uiSpellCount;
        }
        while (resultEquip->NextRow());

        delete resultEquip;

        sLog.outString("");
        sLog.outString(">> %u equipements de demarrage charge", uiSpellCount);
    }
    else
    {
        sLog.outString("");
        sLog.outString(">> 0 equipements au demarrage charges. DB vide ?");
    }

    // 5 - Stuff 2
    QueryResult* resultEquip2 = WorldDatabase.Query(
                                    "SELECT entry, class, item, itemAmount, slot"
                                    " FROM playerstart_equip2 ");
    if (resultEquip2)
    {
        uint32 uiSpellCount = 0;

        do
        {
            Field* pFields = resultEquip2->Fetch();

            PlayerStartEquip* pTemp = new PlayerStartEquip;

            pTemp->entry          = pFields[0].GetUInt32();
            pTemp->playerClass    = pFields[1].GetUInt8();
            pTemp->itemId         = pFields[2].GetUInt32();
            pTemp->itemAmount     = pFields[3].GetUInt32();
            pTemp->equipmentSlot  = pFields[4].GetUInt8();

            equip2[pTemp->entry] = pTemp;
            ++uiSpellCount;
        }
        while (resultEquip2->NextRow());

        delete resultEquip2;

        sLog.outString("");
        sLog.outString(">> %u equipements de demarrage charge", uiSpellCount);
    }
    else
    {
        sLog.outString("");
        sLog.outString(">> 0 equipements au demarrage charges. DB vide ?");
    }

    // 6 - Stuff 1
    QueryResult* resultEquip3 = WorldDatabase.Query(
                                    "SELECT item, slot, class"
                                    " FROM playerstart_equip3 ");
    if (resultEquip)
    {
        uint32 uiSpellCount = 0;

        do
        {
            Field* pFields = resultEquip3->Fetch();

            PlayerStartEquip* pTemp = new PlayerStartEquip;

            pTemp->itemId   = pFields[0].GetUInt32();
            pTemp->equipmentSlot  = pFields[1].GetUInt8();
            pTemp->playerClass    = pFields[2].GetUInt8();
            uint32 uiEntry       = pTemp->itemId * 10000 + pTemp->equipmentSlot * 100 + pTemp->playerClass;

            equip3[uiEntry] = pTemp;
            ++uiSpellCount;
        }
        while (resultEquip3->NextRow());

        delete resultEquip3;

        sLog.outString("");
        sLog.outString(">> %u equipements de demarrage charge", uiSpellCount);
    }
    else
    {
        sLog.outString("");
        sLog.outString(">> 0 equipements au demarrage charges. DB vide ?");
    }


}

std::map<uint32, PlayerStartEquip*> PlayerStartMgr::GetEquipment(uint32 version)
{
    switch (version)
    {
        case 3 :
            return equip3;
            break;
        case 2 :
            return equip2;
            break;
        case 1 :
        default :
            return equip;
            break;
    }
}

uint32 PSM_GetMountId(Player *player)
{
    uint32 mountId = 0;
    uint8 race = player->getRace();
    switch (race)
    {
        case RACE_HUMAN :
            mountId = 5656;
            break;
        case RACE_DWARF :
            mountId = 5864;
            break;
        case RACE_NIGHTELF :
            mountId = 8629;
            break;
        case RACE_GNOME :
            mountId = 8563;
            break;
        case RACE_ORC :
            mountId = 5665;
            break;
        case RACE_UNDEAD :
            mountId = 13331;
            break;
        case RACE_TAUREN :
            mountId = 15277;
            break;
        case RACE_TROLL :
            mountId = 8588;
            break;
    }
    return mountId;
}

uint32 PSM_GetQuestID(Player *player, uint32 questType)
{
    uint32 questId = 55000;
    uint8 race = player->getRace();
    uint32 raceoffset = 0;
    uint32 typeoffset = 0;

    switch (race)
    {
        case RACE_HUMAN :
            raceoffset = 0;
            break;
        case RACE_ORC :
            raceoffset = 10;
            break;
        case RACE_DWARF :
            raceoffset = 20;
            break;
        case RACE_NIGHTELF :
            raceoffset = 30;
            break;
        case RACE_UNDEAD :
            raceoffset = 40;
            break;
        case RACE_TAUREN :
            raceoffset = 50;
            break;
        case RACE_GNOME :
            raceoffset = 60;
            break;
        case RACE_TROLL :
            raceoffset = 70;
            break;
    }

    switch (questType)
    {
        case TO_FACTION_CHIEF :
            typeoffset = 1;
            break;
        case TO_BLACKSMITHER :
            typeoffset = 2;
            break;
        case TO_MOUNT_VENDOR :
            typeoffset = 3;
            break;
        case GET_PEX_REWARD :
            typeoffset = 101;
            break;
    }
    questId = questId + raceoffset + typeoffset;
    return questId;
}

uint32 PSM_GetGossipMessID(Player *player, uint32 type)
{
    uint32 npctextId = 55000;
    uint8 race = player->getRace();
    uint32 raceoffset = 0;
    uint32 typeoffset = 0;

    switch (race)
    {
        case RACE_HUMAN :
            raceoffset = 0;
            break;
        case RACE_ORC :
            raceoffset = 30;
            break;
        case RACE_DWARF :
            raceoffset = 0;
            break;
        case RACE_NIGHTELF :
            raceoffset = 0;
            break;
        case RACE_UNDEAD :
            raceoffset = 30;
            break;
        case RACE_TAUREN :
            raceoffset = 30;
            break;
        case RACE_GNOME :
            raceoffset = 0;
            break;
        case RACE_TROLL :
            raceoffset = 30;
            break;
    }

    switch (type)
    {
        case GS_FACTION_CHIEF_1 :
            typeoffset = 101;
            break;
        case GS_FACTION_CHIEF_2 :
            typeoffset = 102;
            break;
        case GS_FACTION_CHIEF_3 :
            typeoffset = 103;
            break;
    }

    npctextId = npctextId + raceoffset + typeoffset;
    return npctextId;
}

bool PSM_CheckOptionForPlayer(Player *player, uint32 uiOption)
{
    if (player->isGameMaster())
        return true;

    uint32 uiCustomFlags = player->GetCustomFlags();
    switch (uiOption)
    {
        case OPTION_SET_55:
            return (!player->HasCustomFlag(CUSTOM_FLAG_HL) && player->getLevel() < 55);
        case OPTION_GET_REWARD:
            return (!player->HasCustomFlag(CUSTOM_FLAG_HL)) && (player->getLevel() >= 60);
        case OPTION_SET_20:
        case OPTION_SET_30:
        case OPTION_SET_40:
        case OPTION_SET_50:
        case OPTION_SET_60:
            return (!player->HasCustomFlag(CUSTOM_FLAG_INSTANT60));
        default:
            return false;
    }
}


void PSM_AddItems(Player *player)
{
    std::map<uint32, PlayerStartEquip*> equip = sPlayerStartMgr.GetEquipment(3);
    uint8 myClass = player->getClass();
    uint8 myRace = player->getRace();


    std::map<uint32, PlayerStartEquip*>::iterator iterEquip;
    for (iterEquip = equip.begin(); iterEquip != equip.end(); ++iterEquip)
    {
        uint8 playerClass   = iterEquip->second->playerClass;
        uint32 itemId       = iterEquip->second->itemId;
        uint32 itemAmount   = iterEquip->second->itemAmount;
        uint8 equipmentSlot = iterEquip->second->equipmentSlot;

        if (myClass != iterEquip->second->playerClass)
            continue;

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
    }
    player->SaveToDB();
}

void PSM_AddSpells(Player *player)
{
    PlayerSpellMap const& mySpells = player->GetSpellMap();

    // Maxinus : Suppression des eventuels métiers + ajout du flag empêchant d'en avoir de nouveaux avant le lvl 60
    for (PlayerSpellMap::const_iterator spellItr = mySpells.begin(); spellItr != mySpells.end() ; ++spellItr)
    {
        if (sSpellMgr.IsPrimaryProfessionFirstRankSpell(spellItr->first))
            player->removeSpell(spellItr->first, false, false);
    }

    player->AddCustomFlag(CUSTOM_FLAG_HL | CUSTOM_FLAG_NO_JOB);
    player->RemoveCustomFlag(CUSTOM_FLAG_IN_PEX | CUSTOM_FLAG_PEX_FINISHED);

    player->GiveLevel(60);

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

    player->UpdateSkillsToMaxSkillsForLevel();

    for (int i = 0; i < 4; ++i)
        player->StoreNewItemInBestSlots(BAG_ADD_ID, 1);

    // Ajout des flys
    if (player->GetTeam() == ALLIANCE)
        player->m_taxi.LoadTaxiMask("3456411898 2148078928 49991 0 0 0 0 0 ");
    else
        player->m_taxi.LoadTaxiMask("561714688 282102432 52408 0 0 0 0 0 ");

    player->SaveToDB();

    player->SetDrunkValue(0x5FFF, 0);
    player->CastSpell(player, SPELL_ELECVISUAL, true);
    player->CastSpell(player, SPELL_SLEEP, true);
}

void PSM_PexReward(Player *player)
{
    player->RemoveCustomFlag(CUSTOM_FLAG_IN_PEX);
    player->AddCustomFlag(CUSTOM_FLAG_PEX_FINISHED);
    player->AddCustomFlag(CUSTOM_FLAG_HL);
}
