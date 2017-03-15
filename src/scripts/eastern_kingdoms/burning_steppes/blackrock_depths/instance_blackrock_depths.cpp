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
SDName: Instance_Blackrock_Depths
SD%Complete: 20
SDComment: events: ring of law
SDCategory: Blackrock Depths
EndScriptData */

#include "scriptPCH.h"
#include "blackrock_depths.h"

enum
{
    MAX_ENCOUNTER              = 14,

    NPC_EMPEROR                = 9019,
    NPC_PRINCESS               = 8929,
    NPC_PHALANX                = 9502,
    NPC_HATEREL                = 9034,
    NPC_ANGERREL               = 9035,
    NPC_VILEREL                = 9036,
    NPC_GLOOMREL               = 9037,
    NPC_SEETHREL               = 9038,
    NPC_DOOMREL                = 9039,
    NPC_DOPEREL                = 9040,
    NPC_RUINEPOIGNE_PROTECTOR  = 8905,
    NPC_RIBBLY_S_CRONY         = 10043,
    NPC_ARENA_SPECTATOR        = 8916,
    //NPC_PANZOR                 = 8923, Ustaag : gérer par la DB
    NPC_GOLEM_RAVAGE           = 8906,
    NPC_WRATH_HAMMER_CONSTRUCT = 8907,
    NPC_GOLEM_LORD_ARGELMACH   = 8983,
    //NPC_PLUGGER_SPAZZRING      = 9499,
    NPC_GUZZLING_PATRON        = 9547,
    NPC_GRIM_PATRON            = 9545,
    NPC_HAMMERED_PATRON        = 9554,

    NPC_THELDREN               = 16059,

    GO_ARENA1                  = 161525,
    GO_ARENA2                  = 161522,
    GO_ARENA3                  = 161524,
    GO_ARENA4                  = 161523,
    GO_SHADOW_LOCK             = 161460,
    GO_SHADOW_MECHANISM        = 161461,
    GO_SHADOW_GIANT_DOOR       = 157923,
    GO_SHADOW_DUMMY            = 161516,
    GO_BAR_KEG_SHOT            = 170607,
    GO_BAR_KEG_TRAP            = 171941,
    GO_BAR_DOOR                = 170571,
    GO_TOMB_ENTER              = 170576,
    GO_TOMB_EXIT               = 170577,
    GO_LYCEUM                  = 170558,
    GO_GOLEM_ROOM_N            = 170573,
    GO_GOLEM_ROOM_S            = 170574,
    GO_THRONE_ROOM             = 170575,

    GO_SPECTRAL_CHALICE        = 164869,
    GO_CHEST_SEVEN             = 169243,
    GO_SECRET_DOOR             = 174553
};

struct instance_blackrock_depths : public ScriptedInstance
{
    instance_blackrock_depths(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiEmperorGUID;
    uint64 m_uiPrincessGUID;
    uint64 m_uiPhalanxGUID;
    uint64 m_uiHaterelGUID;
    uint64 m_uiAngerrelGUID;
    uint64 m_uiVilerelGUID;
    uint64 m_uiGloomrelGUID;
    uint64 m_uiSeethrelGUID;
    uint64 m_uiDoomrelGUID;
    uint64 m_uiDoperelGUID;

    uint64 m_uiTheldrenGUID;

    uint64 m_uiGoArena1GUID;
    uint64 m_uiGoArena2GUID;
    uint64 m_uiGoArena3GUID;
    uint64 m_uiGoArena4GUID;
    uint64 m_uiGoShadowLockGUID;
    uint64 m_uiGoShadowMechGUID;
    uint64 m_uiGoShadowGiantGUID;
    uint64 m_uiGoShadowDummyGUID;
    uint64 m_uiGoBarKegGUID;
    uint64 m_uiGoBarKegTrapGUID;
    uint64 m_uiGoBarDoorGUID;
    uint64 m_uiGoTombEnterGUID;
    uint64 m_uiGoTombExitGUID;
    uint64 m_uiGoLyceumGUID;
    uint64 m_uiGoGolemNGUID;
    uint64 m_uiGoGolemSGUID;
    uint64 m_uiGoThroneGUID;

    uint64 m_uiGoMagnusGUID;

    // Elysium
    //uint64 m_uiPanzorGUID;
    uint64 m_uiGolemLordArgelmachGUID;
    //uint64 m_uiPluggerSpazzringGUID;

    uint64 m_uiSpectralChaliceGUID;
    uint64 m_uiSevensChestGUID;
    uint64 m_uiGoSecretDoorGUID;

    uint32 m_uiBarAleCount;
    uint32 m_uiThunderbrewCount;
    uint32 m_uiRelicCofferDoorCount;

    std::list<uint64> m_lRuinepoigneProtectorsMobGUIDList;
    std::list<uint64> m_lRibblySCronyMobGUIDList;
    std::list<uint64> m_lArenaSpectatorMobGUIDList;
    std::list<uint64> m_lArgelmachProtectorsMobGUIDList;
    std::list<uint64> m_lPluggerSpazzringProtectorsMobGUIDList;

    bool m_bIsTheldrenInvocated;

    void EnableCreature(Creature* pCreature)
    {
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiEmperorGUID = 0;
        m_uiPrincessGUID = 0;
        m_uiPhalanxGUID = 0;
        m_uiHaterelGUID = 0;
        m_uiAngerrelGUID = 0;
        m_uiVilerelGUID = 0;
        m_uiGloomrelGUID = 0;
        m_uiSeethrelGUID = 0;
        m_uiDoomrelGUID = 0;
        m_uiDoperelGUID = 0;

        m_uiTheldrenGUID = 0;
        m_uiGoMagnusGUID = 0;

        m_uiGoArena1GUID = 0;
        m_uiGoArena2GUID = 0;
        m_uiGoArena3GUID = 0;
        m_uiGoArena4GUID = 0;
        m_uiGoShadowLockGUID = 0;
        m_uiGoShadowMechGUID = 0;
        m_uiGoShadowGiantGUID = 0;
        m_uiGoShadowDummyGUID = 0;
        m_uiGoBarKegGUID = 0;
        m_uiGoBarKegTrapGUID = 0;
        m_uiGoBarDoorGUID = 0;
        m_uiGoTombEnterGUID = 0;
        m_uiGoTombExitGUID = 0;
        m_uiGoLyceumGUID = 0;
        m_uiGoGolemNGUID = 0;
        m_uiGoGolemSGUID = 0;
        m_uiGoThroneGUID = 0;

        m_uiBarAleCount = 0;

        // Elysium
        //m_uiPanzorGUID = 0;
        m_uiGolemLordArgelmachGUID = 0;
        //m_uiPluggerSpazzringGUID = 0;

        m_uiSpectralChaliceGUID = 0;
        m_uiSevensChestGUID = 0;
        m_uiGoSecretDoorGUID = 0;

        m_uiThunderbrewCount = 0;
        m_uiRelicCofferDoorCount = 0;
        m_bIsTheldrenInvocated = false;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_EMPEROR:
                m_uiEmperorGUID = pCreature->GetGUID();
                break;
            case NPC_PRINCESS:
                m_uiPrincessGUID = pCreature->GetGUID();
                break;
            case NPC_PHALANX:
                m_uiPhalanxGUID = pCreature->GetGUID();
                break;
            case NPC_HATEREL:
                m_uiHaterelGUID = pCreature->GetGUID();
                break;
            case NPC_ANGERREL:
                m_uiAngerrelGUID = pCreature->GetGUID();
                break;
            case NPC_VILEREL:
                m_uiVilerelGUID = pCreature->GetGUID();
                break;
            case NPC_GLOOMREL:
                m_uiGloomrelGUID = pCreature->GetGUID();
                break;
            case NPC_SEETHREL:
                m_uiSeethrelGUID = pCreature->GetGUID();
                break;
            case NPC_DOOMREL:
                m_uiDoomrelGUID = pCreature->GetGUID();
                break;
            case NPC_DOPEREL:
                m_uiDoperelGUID = pCreature->GetGUID();
                break;
            case NPC_THELDREN:
                m_uiTheldrenGUID = pCreature->GetGUID();
                break;
            case NPC_RUINEPOIGNE_PROTECTOR:
                m_lRuinepoigneProtectorsMobGUIDList.push_back(pCreature->GetGUID());
                break;
            case NPC_RIBBLY_S_CRONY:
                m_lRibblySCronyMobGUIDList.push_back(pCreature->GetGUID());
                break;
            case 9938:
                m_uiGoMagnusGUID = pCreature->GetGUID();
                break;
            case NPC_ARENA_SPECTATOR:
                m_lArenaSpectatorMobGUIDList.push_back(pCreature->GetGUID());
                break;
            /*case NPC_PANZOR: m_uiPanzorGUID = pCreature->GetGUID();
                switch (urand (0,1))
                {
                    case 0:
                        pCreature->SetVisibility(VISIBILITY_OFF);
                        break;
                    case 1:
                        pCreature->SetVisibility(VISIBILITY_ON);
                        EnableCreature(pCreature);
                        break;
                }
                break;*/
            case NPC_WRATH_HAMMER_CONSTRUCT:
            case NPC_GOLEM_RAVAGE:
                m_lArgelmachProtectorsMobGUIDList.push_back(pCreature->GetGUID());
                break;
            case NPC_GOLEM_LORD_ARGELMACH:
                m_uiGolemLordArgelmachGUID = pCreature->GetGUID();
                break;
            //case NPC_PLUGGER_SPAZZRING: m_uiPluggerSpazzringGUID = pCreature->GetGUID(); break;
            case NPC_GUZZLING_PATRON:
            case NPC_GRIM_PATRON:
            case NPC_HAMMERED_PATRON:
                m_lPluggerSpazzringProtectorsMobGUIDList.push_back(pCreature->GetGUID());
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch (pGo->GetEntry())
        {
            case GO_ARENA1:
                m_uiGoArena1GUID = pGo->GetGUID();
                break;
            case GO_ARENA2:
                m_uiGoArena2GUID = pGo->GetGUID();
                break;
            case GO_ARENA3:
                m_uiGoArena3GUID = pGo->GetGUID();
                break;
            case GO_ARENA4:
                m_uiGoArena4GUID = pGo->GetGUID();
                break;
            case GO_SHADOW_LOCK:
                m_uiGoShadowLockGUID = pGo->GetGUID();
                break;
            case GO_SHADOW_MECHANISM:
                m_uiGoShadowMechGUID = pGo->GetGUID();
                break;
            case GO_SHADOW_GIANT_DOOR:
                m_uiGoShadowGiantGUID = pGo->GetGUID();
                break;
            case GO_SHADOW_DUMMY:
                m_uiGoShadowDummyGUID = pGo->GetGUID();
                break;
            case GO_BAR_KEG_SHOT:
                m_uiGoBarKegGUID = pGo->GetGUID();
                break;
            case GO_BAR_KEG_TRAP:
                m_uiGoBarKegTrapGUID = pGo->GetGUID();
                break;
            case GO_BAR_DOOR:
                m_uiGoBarDoorGUID = pGo->GetGUID();
                if (GetData(TYPE_BAR) == DONE)
                    pGo->SetGoState(GOState(2));
                if(GetData(TYPE_NAGMARA) == DONE)
                    pGo->SetGoState(GOState(0));
                break;
            case GO_TOMB_ENTER:
                m_uiGoTombEnterGUID = pGo->GetGUID();
                break;
            case GO_TOMB_EXIT:
                m_uiGoTombExitGUID = pGo->GetGUID();
                if (GetData(TYPE_TOMB_OF_SEVEN) == DONE)
                    pGo->UseDoorOrButton();
                break;
            case GO_LYCEUM:
                m_uiGoLyceumGUID = pGo->GetGUID();
                break;
            case GO_GOLEM_ROOM_N:
                m_uiGoGolemNGUID = pGo->GetGUID();
                if (GetData(TYPE_LYCEUM) == DONE)
                    pGo->UseDoorOrButton();
                break;
            case GO_GOLEM_ROOM_S:
                m_uiGoGolemSGUID = pGo->GetGUID();
                if (GetData(TYPE_LYCEUM) == DONE)
                    pGo->UseDoorOrButton();
                break;
            case GO_THRONE_ROOM:
                m_uiGoThroneGUID = pGo->GetGUID();
                if (GetData(TYPE_IRON_HALL) == DONE)
                    pGo->UseDoorOrButton();
                break;
            case GO_SPECTRAL_CHALICE:
                m_uiSpectralChaliceGUID = pGo->GetGUID();
                break;
            case GO_CHEST_SEVEN:
                m_uiSevensChestGUID = pGo->GetGUID();
                break;
            case GO_SECRET_DOOR:
                m_uiGoSecretDoorGUID = pGo->GetGUID();
                break;
        }
    }

    void CustomSpellCasted(uint32 spellId, Unit* caster, Unit* target)
    {
        sLog.outString("Spell %u caste par '%s' sur '%s'", spellId, caster->GetName(), (target) ? target->GetName() : "<Personne>");
        switch (spellId)
        {
            // BRD : Invocation de Theldren
            case 27517:
            {
                // On invoque pas 2 fois ...
                if (m_bIsTheldrenInvocated)
                    return;

                if (Player *pPlayer = caster->ToPlayer())
                {
                    m_bIsTheldrenInvocated = true;
                    BeginTheldrenEvent(pPlayer);
                }
                break;
            }
            default:
                break;
        }
    }

    void DoSummonCreatureAndAttack(uint32 entry, Unit* who, uint32 num)
    {
        float posx = 586.0f;
        float posy = -152.0f;
        float posz = -52.0f;
        switch (num)
        {
            case 0:
                posx = 598.0f;
                posy = -153.0f;
                break;
            case 1:
                posx = 577.0f;
                posy = -162.0f;
                break;
            case 2:
                posx = 583.0f;
                posy = -158.0f;
                break;
            case 3:
                posx = 593.0f;
                posy = -154.0f;
                break;
        }
        if (Creature* crea = instance->SummonCreature(entry, posx, posy, posz, 4.8f))
        {
            crea->SetInCombatWithZone();
            crea->AI()->AttackStart(who);
            crea->setFaction(16);
        }
    }
    void BeginTheldrenEvent(Player* pPlayer)
    {
        uint32 MobsEntries[4];
        //uint32 MobsEntries[8];
        switch (pPlayer->GetTeam())
        {
            case ALLIANCE:
                MobsEntries[0] = 16053;
                MobsEntries[1] = 16055;
                MobsEntries[2] = 16050;
                MobsEntries[3] = 16051;
                break;
            case HORDE:
                MobsEntries[0] = 16049;
                MobsEntries[1] = 16052;
                MobsEntries[2] = 16054;
                MobsEntries[3] = 16058;
                break;
        }
        /*MobsEntries[0] = 16053;
        MobsEntries[1] = 16055;
        MobsEntries[2] = 16050;
        MobsEntries[3] = 16051;
        MobsEntries[4] = 16049;
        MobsEntries[5] = 16052;
        MobsEntries[6] = 16054;
        MobsEntries[7] = 16058;

        uint8 uiMob1 = urand(0, 7);
        uint8 uiMob2 = (uiMob1 + urand(1, 7)) % 8;
        uint8 uiMob3 = (uiMob2 + urand(2, 7)) % 8;
        uint8 uiMob4 = (uiMob3 + urand(3, 7)) % 8;

        DoSummonCreatureAndAttack(MobsEntries[uiMob1], pPlayer, 0);
        DoSummonCreatureAndAttack(MobsEntries[uiMob2], pPlayer, 1);
        DoSummonCreatureAndAttack(MobsEntries[uiMob3], pPlayer, 2);
        DoSummonCreatureAndAttack(MobsEntries[uiMob4], pPlayer, 3);*/

        for (uint8 i = 0; i < 4; ++i)
        {
            if (i != 0)
                DoSummonCreatureAndAttack(MobsEntries[i], pPlayer, i);
        }
        DoSummonCreatureAndAttack(16059, pPlayer, 4);
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        sLog.outDebug("Instance Blackrock Depths: SetData update (Type: %u Data %u)", uiType, uiData);

        switch (uiType)
        {
            case TYPE_RING_OF_LAW:
                if (uiData == DONE)
                {
                    for (std::list<uint64>::const_iterator itr = m_lArenaSpectatorMobGUIDList.begin(); itr != m_lArenaSpectatorMobGUIDList.end(); itr++)
                    {
                        if (Creature* pCreature = instance->GetCreature(*itr))
                        {
                            if (pCreature->isAlive())
                                pCreature->setFaction(674);
                        }
                    }
                }
                m_auiEncounter[TYPE_RING_OF_LAW] = uiData;
                break;
            case TYPE_VAULT:
                m_auiEncounter[TYPE_VAULT] = uiData;
                break;
            case TYPE_BAR:
                if (uiData == IN_PROGRESS)
                    ++m_uiBarAleCount;
                m_auiEncounter[TYPE_BAR] = uiData;
                break;
            case TYPE_TOMB_OF_SEVEN:
                switch (uiData)
                {
                    case IN_PROGRESS:
                        DoUseDoorOrButton(m_uiGoTombEnterGUID);
                        break;
                    case FAIL:
                        if (m_auiEncounter[3] == IN_PROGRESS)//prevent use more than one time
                            DoUseDoorOrButton(m_uiGoTombEnterGUID);
                        break;
                    case DONE:
                        DoRespawnGameObject(m_uiSevensChestGUID, HOUR * IN_MILLISECONDS);
                        DoUseDoorOrButton(m_uiGoTombExitGUID);
                        DoUseDoorOrButton(m_uiGoTombEnterGUID);
                        break;
                }
                m_auiEncounter[TYPE_TOMB_OF_SEVEN] = uiData;
                break;
            case TYPE_LYCEUM:
                if (uiData == IN_PROGRESS && GetData(TYPE_LYCEUM) == DONE)
                    uiData = DONE;
                if (uiData == DONE)
                {
                    DoOpenDoor(m_uiGoGolemNGUID);
                    DoOpenDoor(m_uiGoGolemSGUID);

                    if (Creature* magnus = instance->GetCreature(m_uiGoMagnusGUID))
                    {
                        std::list<Creature*> AnvilrageList;
                        GetCreatureListWithEntryInGrid(AnvilrageList, magnus, 8901, 400.0f);

                        for (std::list<Creature*>::iterator it = AnvilrageList.begin(); it != AnvilrageList.end(); ++it)
                            (*it)->SetRespawnDelay(345600);
                    }
                }
                m_auiEncounter[TYPE_LYCEUM] = uiData;
                break;
            case TYPE_IRON_HALL:
                switch (uiData)
                {
                    case IN_PROGRESS:
                        DoResetDoor(m_uiGoGolemNGUID);
                        DoResetDoor(m_uiGoGolemSGUID);
                        break;
                    case FAIL:
                        DoOpenDoor(m_uiGoGolemNGUID);
                        DoOpenDoor(m_uiGoGolemSGUID);
                        break;
                    case DONE:
                        DoOpenDoor(m_uiGoGolemNGUID);
                        DoOpenDoor(m_uiGoGolemSGUID);
                        DoOpenDoor(m_uiGoThroneGUID);
                        break;
                }
                m_auiEncounter[TYPE_IRON_HALL] = uiData;
                break;
            case TYPE_THUNDERBREW:
                if (uiData == IN_PROGRESS)
                {
                    m_uiThunderbrewCount++;
                    if (m_uiThunderbrewCount == 3)
                        m_auiEncounter[TYPE_THUNDERBREW] = DONE;
                }
                break;
            case TYPE_RELIC_COFFER:
                if (uiData == IN_PROGRESS)
                {
                    m_uiRelicCofferDoorCount++;
                    if (m_uiRelicCofferDoorCount == 12)
                    {
                        for (std::list<uint64>::const_iterator itr = m_lRuinepoigneProtectorsMobGUIDList.begin(); itr != m_lRuinepoigneProtectorsMobGUIDList.end(); itr++)
                        {
                            if (Creature* pCreature = instance->GetCreature(*itr))
                            {
                                if (pCreature->isAlive() && pCreature->getVictim())
                                {
                                    EnableCreature(pCreature);
                                    if (pCreature->AI())
                                        pCreature->AI()->AttackStart(pCreature->getVictim());
                                    if (pCreature->HasAura(10255))
                                        pCreature->RemoveAurasDueToSpell(10255);
                                }
                            }
                        }
                        m_auiEncounter[TYPE_RELIC_COFFER] = DONE;
                    }
                }
                break;
            case TYPE_DOOMGRIP:
                if (uiData == DONE)
                    DoUseDoorOrButton(m_uiGoSecretDoorGUID);
                m_auiEncounter[TYPE_DOOMGRIP] = uiData;
                break;
            case TYPE_RIBBLY:
                if (uiData == DONE)
                {
                    for (std::list<uint64>::const_iterator itr = m_lRibblySCronyMobGUIDList.begin(); itr != m_lRibblySCronyMobGUIDList.end(); itr++)
                    {
                        if (Creature* pCreature = instance->GetCreature(*itr))
                        {
                            if (pCreature->isAlive())
                            {
                                pCreature->setFaction(14);
                                Unit* pVictim = pCreature->getVictim();
                                if (pCreature->AI())
                                    pCreature->AI()->AttackStart(pVictim);
                            }
                        }
                    }
                }
                m_auiEncounter[TYPE_RIBBLY] = uiData;
                break;
            case DATA_ARGELMACH_AGGRO:
                if (uiData == IN_PROGRESS)
                {
                    if (Creature* argelmach = instance->GetCreature(m_uiGolemLordArgelmachGUID))
                        if (Unit* pVictim = argelmach->getVictim())
                            for (std::list<uint64>::const_iterator itr = m_lArgelmachProtectorsMobGUIDList.begin(); itr != m_lArgelmachProtectorsMobGUIDList.end(); itr++)
                                if (Creature* protector = instance->GetCreature(*itr))
                                    if (protector->isAlive() && protector->AI() && protector->IsWithinDist(argelmach, 80.0f))
                                        protector->AI()->AttackStart(pVictim);
                }
                m_auiEncounter[DATA_ARGELMACH_AGGRO] = uiData;
                break;
            case DATA_SPAZZRING_AGGRO:
                if (uiData == IN_PROGRESS)
                {
                    for (std::list<uint64>::const_iterator itr = m_lPluggerSpazzringProtectorsMobGUIDList.begin(); itr != m_lPluggerSpazzringProtectorsMobGUIDList.end(); itr++)
                    {
                        if (Creature* patron = instance->GetCreature(*itr))
                        {
                            if (patron->isAlive() && patron->AI())
                                patron->setFaction(14);
                        }
                    }
                }
                m_auiEncounter[11] = uiData;
                break;

            case DATA_THELDREN:
                m_auiEncounter[12] = uiData;
                break;
            case TYPE_NAGMARA:
                m_auiEncounter[13] = uiData;
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                       << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                       << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                       << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " " << m_auiEncounter[12] << " " << m_auiEncounter[13];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch (uiType)
        {
            case TYPE_RING_OF_LAW:
                return m_auiEncounter[0];
            case TYPE_VAULT:
                return m_auiEncounter[1];
            case TYPE_BAR:
                if (m_auiEncounter[2] == IN_PROGRESS && m_uiBarAleCount == 3)
                    return SPECIAL;
                else
                    return m_auiEncounter[2];
            case TYPE_TOMB_OF_SEVEN:
                return m_auiEncounter[3];
            case TYPE_LYCEUM:
                return m_auiEncounter[4];
            case TYPE_IRON_HALL:
                return m_auiEncounter[5];
            case TYPE_THUNDERBREW:
                return m_auiEncounter[6];
            case TYPE_RELIC_COFFER:
                return m_auiEncounter[7];
            case TYPE_DOOMGRIP:
                return m_auiEncounter[8];
            case TYPE_RIBBLY:
                return m_auiEncounter[9];
            case DATA_ARGELMACH_AGGRO:
                return m_auiEncounter[10];
            case DATA_SPAZZRING_AGGRO:
                return m_auiEncounter[11];
            case DATA_THELDREN:
                return m_auiEncounter[12];
            case TYPE_NAGMARA:
                return m_auiEncounter[13];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch (uiData)
        {
            case DATA_EMPEROR:
                return m_uiEmperorGUID;
            case DATA_PRINCESS:
                return m_uiPrincessGUID;
            case DATA_PHALANX:
                return m_uiPhalanxGUID;
            case DATA_HATEREL:
                return m_uiHaterelGUID;
            case DATA_ANGERREL:
                return m_uiAngerrelGUID;
            case DATA_VILEREL:
                return m_uiVilerelGUID;
            case DATA_GLOOMREL:
                return m_uiGloomrelGUID;
            case DATA_SEETHREL:
                return m_uiSeethrelGUID;
            case DATA_DOOMREL:
                return m_uiDoomrelGUID;
            case DATA_DOPEREL:
                return m_uiDoperelGUID;

            case DATA_ARENA1:
                return m_uiGoArena1GUID;
            case DATA_ARENA2:
                return m_uiGoArena2GUID;
            case DATA_ARENA3:
                return m_uiGoArena3GUID;
            case DATA_ARENA4:
                return m_uiGoArena4GUID;

            case DATA_GO_BAR_KEG:
                return m_uiGoBarKegGUID;
            case DATA_GO_BAR_KEG_TRAP:
                return m_uiGoBarKegTrapGUID;
            case DATA_GO_BAR_DOOR:
                return m_uiGoBarDoorGUID;
            case DATA_GO_CHALICE:
                return m_uiSpectralChaliceGUID;
            case DATA_GO_TOMB_EXIT:
                return m_uiGoTombExitGUID;
        }
        return 0;
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
                   >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
                   >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11] >> m_auiEncounter[12] >> m_auiEncounter[13];

        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_blackrock_depths(Map* pMap)
{
    return new instance_blackrock_depths(pMap);
}

void AddSC_instance_blackrock_depths()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_blackrock_depths";
    newscript->GetInstanceData = &GetInstanceData_instance_blackrock_depths;
    newscript->RegisterSelf();
}
