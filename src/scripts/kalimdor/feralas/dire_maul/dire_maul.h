/* Copyright (C) 2009 - 2010 Elysium <https://elysium-project.org/>
 * 1.12, Chakor
 * All rights reserved */

#ifndef __DEF_DIRE_MAUL_HEADER
#define __DEF_DIRE_MAUL_HEADER

enum
{
    MAX_CRISTALS           = 5,

    TYPE_CRISTAL_EVENT     = 1,
    TYPE_IMMOL_THAR        = 2,
    DATA_TENDRIS_AGGRO     = 3,
    TYPE_BOSS_ZEVRIM       = 4,
    TYPE_SPEAK_ECORCEFER   = 5,
    TYPE_GORDOK_TRIBUTE    = 6,
    DATA_SLIPKIK_FROZEN    = 7,
    TYPE_EVENT_IMMOLTAR_PART1 = 8,
    TYPE_EVENT_IMMOLTAR_PART2 = 9,
    INSTANCE_DIRE_MAUL_MAX_ENCOUNTER          = 10,

    NPC_IMMOL_THAR_GARDIEN = 11466,
    NPC_IMMOL_THAR         = 11496,
    NPC_TORTHELDRIN        = 11486,
    NPC_RESTE_MANA         = 11483,
    NPC_ARCANE_ABERRATION  = 11480,

    NPC_TENDRIS            = 11489,
    NPC_TENDRIS_PROTECTOR  = 11459,

    // Vieil Ecorcefer
    NPC_OLD_IRONBARK       = 11491,
    NPC_ZEVRIM             = 11490, // Non utilise
    NPC_SLIPKIK            = 14323,

    GO_FORCE_FIELD         = 179503, 
    GO_MAGIC_VORTEX        = 179506,
    GO_CRISTAL_1_EVENT     = 177259,
    GO_CRISTAL_2_EVENT     = 177257,
    GO_CRISTAL_3_EVENT     = 177258,
    GO_CRISTAL_4_EVENT     = 179504,
    GO_CRISTAL_5_EVENT     = 179505,

    GO_DOOR_ALZZIN_IN      = 181496,

    // Chiffre après "GO_GORDOK_TRIBUTE_" = Nombre de Guardes Morts
    NPC_TRIBUTE            = 11500,
    NPC_KING_GORDOK        = 11501,
    SPELL_TRIBUTE_EVENT    = 23318,
    GO_GORDOK_TRIBUTE_0    = 179564,
    GO_GORDOK_TRIBUTE_1    = 300400,
    GO_GORDOK_TRIBUTE_2    = 300401,
    GO_GORDOK_TRIBUTE_3    = 300402,
    GO_GORDOK_TRIBUTE_4    = 300403,
    GO_GORDOK_TRIBUTE_5    = 300404,
    GO_GORDOK_TRIBUTE_6    = 300405,
};

class instance_dire_maul : public ScriptedInstance
{
    public:
        instance_dire_maul(Map* pMap);
        ~instance_dire_maul() {}

        void Initialize();

        void OnPlayerEnter(Player* pPlayer);
        void OnPlayerLeave(Player* pPlayer);
        void OnObjectCreate(GameObject* pGo);
        void OnCreatureDeath(Creature* pCreature);
        void OnCreatureCreate(Creature* pCreature);
        void OnCreatureRespawn(Creature* pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        void SetData64(uint32 uiType, uint64 uiData);
        uint32 GetData(uint32 uiType);
        uint64 GetData64(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void DoSortCristalsEventMobs();

    protected:
        uint32 m_auiEncounter[INSTANCE_DIRE_MAUL_MAX_ENCOUNTER];
        std::string strInstData;

        uint32 m_uiGuardAliveCount;

        uint64 m_uiMagicVortexGUID;
        uint64 m_uiForceFieldGUID;
        uint64 m_uiImmolTharGUID;
        uint64 m_uiTortheldrinGUID;
        uint64 m_uiTributeGUID;
        uint64 m_uiTendrisGUID;
        uint64 m_uiOldIronbarkGUID;
        uint64 m_uiSlipKikGUID;

        uint64 m_uiDoorAlzzinInGUID;

        uint64 m_uiGordokTribute0GUID;
        uint64 m_uiGordokTribute1GUID;
        uint64 m_uiGordokTribute2GUID;
        uint64 m_uiGordokTribute3GUID;
        uint64 m_uiGordokTribute4GUID;
        uint64 m_uiGordokTribute5GUID;
        uint64 m_uiGordokTribute6GUID;

        uint64 m_auiCristalsGUID[MAX_CRISTALS];
        std::list<uint64> m_alCristalsEventtMobGUIDSorted[MAX_CRISTALS];
        std::list<uint64> m_lCristalsEventtMobGUIDList;

        std::list<uint64> m_lImmolTharGardiensMobGUIDList;

        std::list<uint64> m_lTendrisProtectorsMobGUIDList;

        bool m_bIsGordokTributeRespawned;
};

#endif
