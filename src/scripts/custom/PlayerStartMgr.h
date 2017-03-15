#ifndef _NPC_WELCOME_STOCKAGE_HEADER
#define _NPC_WELCOME_STOCKAGE_HEADER

#include "scriptPCH.h"
#include "ProgressBar.h"
#include "Policies/Singleton.h"
#include "Database/DatabaseEnv.h"


/*
REPLACE INTO elysium_string (entry, content_loc2, content_default) VALUES
(56, "Sergent recruteur semble perdu dans ses pensées.",
    "Recruiting sergeant seems lost in his thoughts."),
(57, "Sergent recruteur semble perdu dans ses pensées.",
    "Recruiting sergeant seems lost in his thoughts."),
(58, "%s : approchez, j'ai une proposition à vous faire.",
    "%s ! Approach, I have a proposition for you."),
(59, "Je ne peux que saluer votre courage.",
    "I can only applaud your courage."),
(60, "A vrai dire, non... J'ai tant de chose à decouvrir !", "Actually, no ... I have so much to discover!"),
(61, "Oui, c'est comme si je l'avais déjà vécu. Je me sens prêt(e). Enseignez-moi tout ce que je dois savoir.",
    "Yes, it is as if I had already lived. I feel ready. Teach me everything I need to know.");

*/
enum
{
    NPC_SPEECH_JAI_PROPOSITION = ELYSIUM_TEXT(58),
    NPC_SPEECH_0           = ELYSIUM_TEXT(55),

    NPC_SPEECH_H_1         = ELYSIUM_TEXT(64),
    NPC_SPEECH_H_2         = ELYSIUM_TEXT(65),
    NPC_SPEECH_H_3         = ELYSIUM_TEXT(66),
    NPC_SPEECH_H_4_ORC     = ELYSIUM_TEXT(67),
    NPC_SPEECH_H_4_TAUREN  = ELYSIUM_TEXT(68),
    NPC_SPEECH_H_4_UNDEAD  = ELYSIUM_TEXT(69),
    NPC_SPEECH_H_4_TROLL   = ELYSIUM_TEXT(76),

    NPC_SPEECH_A_1         = ELYSIUM_TEXT(70),
    NPC_SPEECH_A_2         = ELYSIUM_TEXT(71),
    NPC_SPEECH_A_3_HUMAIN  = ELYSIUM_TEXT(72),
    NPC_SPEECH_A_3_NAIN    = ELYSIUM_TEXT(73),
    NPC_SPEECH_A_3_ELFE    = ELYSIUM_TEXT(74),
    NPC_SPEECH_A_3_GNOME   = ELYSIUM_TEXT(75),

    NPC_SPEECH_LOST_MINDS_H= ELYSIUM_TEXT(56),
    NPC_SPEECH_LOST_MINDS_A= ELYSIUM_TEXT(57),

    NPC_SPEECH_CONGRATS    = ELYSIUM_TEXT(59),
    GOSSIP_ITEM_NOTREADY   = ELYSIUM_TEXT(60),
    GOSSIP_ITEM_IAMREADY   = ELYSIUM_TEXT(61),

    GOSSIP_ITEM_WELCOME    = ELYSIUM_TEXT(150),

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

// Elysium : 12 empl.
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
