#ifndef _NPC_WELCOME_STOCKAGE_HEADER
#define _NPC_WELCOME_STOCKAGE_HEADER

#include "scriptPCH.h"
#include "ProgressBar.h"
#include "Policies/Singleton.h"
#include "Database/DatabaseEnv.h"


/*
REPLACE INTO nostalrius_string (entry, content_loc2, content_default) VALUES
(56, "Sergent recruteur semble perdu dans ses pens�es.",
    "Recruiting sergeant seems lost in his thoughts."),
(57, "Sergent recruteur semble perdu dans ses pens�es.",
    "Recruiting sergeant seems lost in his thoughts."),
(58, "%s : approchez, j'ai une proposition � vous faire.",
    "%s ! Approach, I have a proposition for you."),
(59, "Je ne peux que saluer votre courage.",
    "I can only applaud your courage."),
(60, "A vrai dire, non... J'ai tant de chose � decouvrir !", "Actually, no ... I have so much to discover!"),
(61, "Oui, c'est comme si je l'avais d�j� v�cu. Je me sens pr�t(e). Enseignez-moi tout ce que je dois savoir.",
    "Yes, it is as if I had already lived. I feel ready. Teach me everything I need to know.");

*/
enum
{
    NPC_SPEECH_JAI_PROPOSITION = NOST_TEXT(58),
    NPC_SPEECH_0           = NOST_TEXT(55),

    NPC_SPEECH_H_1         = NOST_TEXT(64),
    NPC_SPEECH_H_2         = NOST_TEXT(65),
    NPC_SPEECH_H_3         = NOST_TEXT(66),
    NPC_SPEECH_H_4_ORC     = NOST_TEXT(67),
    NPC_SPEECH_H_4_TAUREN  = NOST_TEXT(68),
    NPC_SPEECH_H_4_UNDEAD  = NOST_TEXT(69),
    NPC_SPEECH_H_4_TROLL   = NOST_TEXT(76),

    NPC_SPEECH_A_1         = NOST_TEXT(70),
    NPC_SPEECH_A_2         = NOST_TEXT(71),
    NPC_SPEECH_A_3_HUMAIN  = NOST_TEXT(72),
    NPC_SPEECH_A_3_NAIN    = NOST_TEXT(73),
    NPC_SPEECH_A_3_ELFE    = NOST_TEXT(74),
    NPC_SPEECH_A_3_GNOME   = NOST_TEXT(75),

    NPC_SPEECH_LOST_MINDS_H= NOST_TEXT(56),
    NPC_SPEECH_LOST_MINDS_A= NOST_TEXT(57),

    NPC_SPEECH_CONGRATS    = NOST_TEXT(59),
    GOSSIP_ITEM_NOTREADY   = NOST_TEXT(60),
    GOSSIP_ITEM_IAMREADY   = NOST_TEXT(61),

    GOSSIP_ITEM_WELCOME    = NOST_TEXT(150),

    GOSSIP_MENU_WELCOME     = 55001,
    GOSSIP_MENU_QUESTGIVING = 55002,
    GOSSIP_MENU_ALREADYHL   = 55003
};


enum
{
    OPTION_SET_55            = (GOSSIP_ACTION_INFO_DEF + 1),
    OPTION_GET_REWARD        = (GOSSIP_ACTION_INFO_DEF + 3),
    OPTION_SET_60            = (GOSSIP_ACTION_INFO_DEF + 4),
};

enum
{
    OPTION_SET_20            = (GOSSIP_ACTION_INFO_DEF + 5),
    OPTION_SET_30            = (GOSSIP_ACTION_INFO_DEF + 6),
    OPTION_SET_40            = (GOSSIP_ACTION_INFO_DEF + 7),
    OPTION_SET_50            = (GOSSIP_ACTION_INFO_DEF + 8),
};

enum
{
    TO_FACTION_CHIEF   = 1,
    AT_FACTION_CHIEF   = 2,
    TO_BLACKSMITHER    = 3,
    TO_MOUNT_VENDOR    = 4,
    GET_PEX_REWARD     = 5
};

enum
{
    GS_FACTION_CHIEF_1   = 11,
    GS_FACTION_CHIEF_2   = 12,
    GS_FACTION_CHIEF_3   = 13,
    GS_FACTION_CHIEF_4   = 14,
};

// Nostalrius : 12 empl.
#define BAG_ADD_ID 30000
//#define BAG_ADD_ID2 55601

#define MOUNT_SPELL 33389
#define SPELL_SLEEP 9159
#define SPELL_ELECVISUAL 28136

#define GOSSIP_ACTION_NOTREADY (GOSSIP_ACTION_INFO_DEF + 41)
#define GOSSIP_ACTION_IAMREADY (GOSSIP_ACTION_INFO_DEF + 42)

struct PlayerStartSpell
{
    uint32 spellId;
    uint32 playerClass;
    uint32 playerRace;
};
struct PlayerStartEquip
{
    uint32 entry;
    uint8 playerClass;
    uint32 itemId;
    uint32 itemAmount;
    uint8 equipmentSlot;
};

class PlayerStartMgr
{
    public:
        PlayerStartMgr();
        ~PlayerStartMgr();

        void load();

        std::map<uint32,PlayerStartEquip*> GetEquipment(uint32 version);
        std::map<uint32,PlayerStartSpell*> GetSpells()
        { return spells;}

        std::map<uint32,PlayerStartSpell*> GetLowSpells()
        { return lowSpells;}

    private:
        std::map<uint32,PlayerStartSpell*> spells;
        std::map<uint32,PlayerStartSpell*> lowSpells;
        std::map<uint32,PlayerStartEquip*> equip;
        std::map<uint32,PlayerStartEquip*> equip2;
        std::map<uint32,PlayerStartEquip*> equip3; 
};

uint32 PSM_GetMountId(Player *player);
uint32 PSM_GetQuestID(Player *player, uint32 questType);
uint32 PSM_GetGossipMessID(Player *player, uint32 type);

bool PSM_CheckOptionForPlayer(Player *player, uint32 uiOption);

void PSM_AddItems(Player *player);
void PSM_AddSpells(Player *player);
void PSM_PexReward(Player *player);

#define sPlayerStartMgr MaNGOS::Singleton<PlayerStartMgr>::Instance()

#endif
