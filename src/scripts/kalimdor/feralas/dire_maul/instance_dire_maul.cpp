/* Copyright (C) 2009 - 2010 Elysium <https://elysium-project.org/>
 * 1.12, Chakor
 * All rights reserved */

#include "scriptPCH.h"
#include "dire_maul.h"

//#define DEBUG_ON

void EnableCreature(Creature* pCreature)
{
    pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
}

instance_dire_maul::instance_dire_maul(Map* pMap) : ScriptedInstance(pMap),
    m_uiForceFieldGUID(0), m_uiTortheldrinGUID(0),
    m_uiTributeGUID(0),
    m_uiImmolTharGUID(0), m_uiTendrisGUID(0),
    m_uiOldIronbarkGUID(0), m_uiDoorAlzzinInGUID(0),
    m_uiGuardAliveCount(6), m_uiGordokTribute0GUID(0),
    m_uiGordokTribute1GUID(0), m_uiGordokTribute2GUID(0),
    m_uiGordokTribute3GUID(0), m_uiGordokTribute4GUID(0),
    m_uiGordokTribute5GUID(0), m_uiGordokTribute6GUID(0),
    m_bIsGordokTributeRespawned(false), m_uiMagicVortexGUID(0),
    m_uiSlipKikGUID(0)
{
    Initialize();
}

void instance_dire_maul::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    memset(&m_auiCristalsGUID, 0, sizeof(m_auiCristalsGUID));
}

void instance_dire_maul::OnPlayerEnter(Player* pPlayer)
{
    pPlayer->RemoveAurasDueToSpell(22799); //on retire le "Roi des Gordok" à l'entrée dans l'instance
}

void instance_dire_maul::OnPlayerLeave(Player* pPlayer)
{
    pPlayer->RemoveAurasDueToSpell(22799); //on retire le "Roi des Gordok" à l'entrée dans l'instance
}


void instance_dire_maul::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CRISTAL_1_EVENT :
            m_auiCristalsGUID[0]   = pGo->GetGUID();
            break;
        case GO_CRISTAL_2_EVENT :
            m_auiCristalsGUID[1]   = pGo->GetGUID();
            break;
        case GO_CRISTAL_3_EVENT :
            m_auiCristalsGUID[2]   = pGo->GetGUID();
            break;
        case GO_CRISTAL_4_EVENT :
            m_auiCristalsGUID[3]   = pGo->GetGUID();
            break;
        case GO_CRISTAL_5_EVENT :
            m_auiCristalsGUID[4]   = pGo->GetGUID();
            break;
        case GO_FORCE_FIELD     :
            m_uiForceFieldGUID     = pGo->GetGUID();
            break;
        case GO_MAGIC_VORTEX    :
            m_uiMagicVortexGUID    = pGo->GetGUID();
            break;
        case GO_DOOR_ALZZIN_IN  :
            m_uiDoorAlzzinInGUID   = pGo->GetGUID();
            break;
        case GO_GORDOK_TRIBUTE_0:
            m_uiGordokTribute0GUID = pGo->GetGUID();
            break;
        case GO_GORDOK_TRIBUTE_1:
            m_uiGordokTribute1GUID = pGo->GetGUID();
            break;
        case GO_GORDOK_TRIBUTE_2:
            m_uiGordokTribute2GUID = pGo->GetGUID();
            break;
        case GO_GORDOK_TRIBUTE_3:
            m_uiGordokTribute3GUID = pGo->GetGUID();
            break;
        case GO_GORDOK_TRIBUTE_4:
            m_uiGordokTribute4GUID = pGo->GetGUID();
            break;
        case GO_GORDOK_TRIBUTE_5:
            m_uiGordokTribute5GUID = pGo->GetGUID();
            break;
        case GO_GORDOK_TRIBUTE_6:
            m_uiGordokTribute6GUID = pGo->GetGUID();
            break;
        default:
            break;
    }
}

    void instance_dire_maul::OnCreatureDeath(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_KING_GORDOK:
                if (Creature* pTribute = instance->GetCreature(m_uiTributeGUID))
                    pTribute->CastSpell(pTribute, SPELL_TRIBUTE_EVENT, true);
                break;

            case NPC_IMMOL_THAR:
                if (Creature* pTortheldrin = instance->GetCreature(m_uiTortheldrinGUID))
                    pTortheldrin->MonsterYell("Who dares disrupt the sanctity of Eldre'Thalas? Face me, cowards!");
                break;
        }
    }

void instance_dire_maul::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_TRIBUTE:
            m_uiTributeGUID = pCreature->GetGUID();
            break;
        case NPC_IMMOL_THAR:
            m_uiImmolTharGUID   = pCreature->GetGUID();
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            break;
        case NPC_TORTHELDRIN:
            m_uiTortheldrinGUID = pCreature->GetGUID();
            break;
        case NPC_RESTE_MANA:
        case NPC_ARCANE_ABERRATION:
            m_lCristalsEventtMobGUIDList.push_back(pCreature->GetGUID());
            break;
        case NPC_IMMOL_THAR_GARDIEN:
            m_lImmolTharGardiensMobGUIDList.push_back(pCreature->GetGUID());
            break;
        case NPC_TENDRIS:
            m_uiTendrisGUID     = pCreature->GetGUID();
            break;
        case NPC_TENDRIS_PROTECTOR:
            m_lTendrisProtectorsMobGUIDList.push_back(pCreature->GetGUID());
            break;
        case NPC_OLD_IRONBARK:
            m_uiOldIronbarkGUID = pCreature->GetGUID();
            break;
        case NPC_SLIPKIK:
            m_uiSlipKikGUID = pCreature->GetGUID();
            break;
    }
}

void instance_dire_maul::OnCreatureRespawn(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_SLIPKIK)
        if (GetData(DATA_SLIPKIK_FROZEN) == SPECIAL)
        {
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            if (!pCreature->HasAura(22856)) // Ice Lock / Serrure de glace
                pCreature->AddAura(22856);
        }
}

void instance_dire_maul::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_CRISTAL_EVENT:
        {
            if (uiData == DONE)
            {
                // Desactivation du champs de force
                DoUseDoorOrButton(m_uiForceFieldGUID);
                DoUseDoorOrButton(m_uiMagicVortexGUID);
                // Le boss devient attaquable ...
                if (Creature* pImmolThar = instance->GetCreature(m_uiImmolTharGUID))
                {
                    EnableCreature(pImmolThar);
                    // ... et ses gardiens doivent l'attaquer.
#ifdef DEBUG_ON
                    sLog.outString("Immol'Thar (%u) rendu attaquable, %u gardiens trouves.", pImmolThar->GetGUIDLow(), m_lImmolTharGardiensMobGUIDList.size());
#endif
                    for (std::list<uint64>::const_iterator itr = m_lImmolTharGardiensMobGUIDList.begin(); itr != m_lImmolTharGardiensMobGUIDList.end(); itr++)
                    {
                        if (Creature* pCreature = instance->GetCreature(*itr))
                        {
                            // Ne pas non plus aggro toute l'instance.
                            if (pCreature->isAlive())
                            {
                                EnableCreature(pCreature);
                                if (pCreature->GetDistance(pImmolThar) > 100.0f)
                                    continue;

                                if (pCreature->AI())
                                    pCreature->AI()->AttackStart(pImmolThar);
                                pCreature->setFaction(90);
                            }
                        }
                    }
                }
                else
                    sLog.outError("Immol'Thar introuvable !! GUID %u", m_uiImmolTharGUID);
            }
            m_auiEncounter[TYPE_CRISTAL_EVENT] = uiData;
            break;
        }
        case TYPE_IMMOL_THAR:
        {
            if (uiData == DONE)
            {
                if (Creature* tortheldrin = instance->GetCreature(m_uiTortheldrinGUID))
                {
                    tortheldrin->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    tortheldrin->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    tortheldrin->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    sLog.outString("Tortheldrin (%u) rendu attaquable.", tortheldrin->GetGUIDLow());
                }
                else
                    sLog.outError("Tortheldrin introuvable !! GUID %u", m_uiTortheldrinGUID);
            }
            m_auiEncounter[TYPE_IMMOL_THAR] = uiData;
            break;
        }
        case TYPE_BOSS_ZEVRIM:
        {
            if (uiData == DONE)
            {
#ifdef DEBUG_ON
                sLog.outString("Zevrim DOWN");
#endif
            }
            m_auiEncounter[TYPE_BOSS_ZEVRIM] = uiData;
            break;
        }
        case DATA_TENDRIS_AGGRO:
        {
            break;
        }
        case TYPE_SPEAK_ECORCEFER:
        {
            if (uiData == DONE)
            {
#ifdef DEBUG_ON
                sLog.outString("EcorceFer Speak OK");
#endif
                DoUseDoorOrButton(m_uiDoorAlzzinInGUID);
            }
            m_auiEncounter[TYPE_SPEAK_ECORCEFER] = uiData;
            break;
        }
        case TYPE_GORDOK_TRIBUTE:
        {
            if (uiData == SPECIAL)
            {
                // Les gardes renvoient SPECIAL à leur mort via eventAI
                --m_uiGuardAliveCount;
                SetData(TYPE_GORDOK_TRIBUTE, IN_PROGRESS);
            }
            if (uiData == DONE)
            {
                if (m_bIsGordokTributeRespawned)
                    return;

                uint32 finalGuardStatus = m_uiGuardAliveCount;
                if (GetData(DATA_SLIPKIK_FROZEN) != SPECIAL && finalGuardStatus)
                    finalGuardStatus -= 1;

                switch (finalGuardStatus)
                {
                    case 0:
                        DoRespawnGameObject(m_uiGordokTribute6GUID);
                        break;
                    case 1:
                        DoRespawnGameObject(m_uiGordokTribute5GUID);
                        break;
                    case 2:
                        DoRespawnGameObject(m_uiGordokTribute4GUID);
                        break;
                    case 3:
                        DoRespawnGameObject(m_uiGordokTribute3GUID);
                        break;
                    case 4:
                        DoRespawnGameObject(m_uiGordokTribute2GUID);
                        break;
                    case 5:
                        DoRespawnGameObject(m_uiGordokTribute1GUID);
                        break;
                    case 6:
                        DoRespawnGameObject(m_uiGordokTribute0GUID);
                        break;
                }
                m_bIsGordokTributeRespawned = true;
            }
            m_auiEncounter[TYPE_GORDOK_TRIBUTE] = uiData;
            break;
        }
        case DATA_SLIPKIK_FROZEN:
        {
            if (uiData == SPECIAL)
            {
                Creature *SlipKik = instance->GetCreature(m_uiSlipKikGUID);
                SlipKik->DeleteThreatList();
                SlipKik->CombatStop(true);
                SlipKik->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                SlipKik->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SlipKik->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

                if (!SlipKik->HasAura(22856)) // Ice Lock / Serrure de glace
                    SlipKik->AddAura(22856);
            }
            m_auiEncounter[DATA_SLIPKIK_FROZEN] = uiData;
            break;
        }
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_dire_maul::SetData64(uint32 uiType, uint64 uiData)
{
#ifdef DEBUG_ON
    sLog.outString("SetData64(%u, %u) data is %u", uiType, uiData, GetData(TYPE_CRISTAL_EVENT));
#endif
    if (uiType == TYPE_CRISTAL_EVENT && GetData(TYPE_CRISTAL_EVENT) == NOT_STARTED)
        DoSortCristalsEventMobs();

    if (uiType == TYPE_CRISTAL_EVENT && GetData(TYPE_CRISTAL_EVENT) == IN_PROGRESS)
    {
        uint8 uiNotEmptyRoomsCount = 0;
        for (uint8 i = 0; i < MAX_CRISTALS; i++)
        {
            if (m_auiCristalsGUID[i])                       // This check is used, to ensure which runes still need processing
            {
                m_alCristalsEventtMobGUIDSorted[i].remove(uiData);
                if (m_alCristalsEventtMobGUIDSorted[i].empty())
                {
                    DoUseDoorOrButton(m_auiCristalsGUID[i]);
#ifdef DEBUG_ON
                    sLog.outString("ACTIVATION d'un cristal. Numero %u", i + 1);
#endif
                    m_auiCristalsGUID[i] = 0;
                }
                else
                    uiNotEmptyRoomsCount++;                 // found an not empty room
            }
        }
        if (!uiNotEmptyRoomsCount)
            SetData(TYPE_CRISTAL_EVENT, DONE);
    }
}

void instance_dire_maul::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[6] >> m_auiEncounter[7];

    for (uint8 i = 0; i < INSTANCE_DIRE_MAUL_MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_dire_maul::GetData(uint32 uiType)
{
    ASSERT(uiType < INSTANCE_DIRE_MAUL_MAX_ENCOUNTER);
    return m_auiEncounter[uiType];
}

uint64 instance_dire_maul::GetData64(uint32 uiType)
{
    switch (uiType)
    {
        case NPC_IMMOL_THAR:
            return m_uiImmolTharGUID;
        case NPC_TORTHELDRIN:
            return m_uiTortheldrinGUID;
        case GO_FORCE_FIELD:
            return m_uiForceFieldGUID;
        case GO_MAGIC_VORTEX:
            return m_uiMagicVortexGUID;
    }
    return 0;
}

void instance_dire_maul::DoSortCristalsEventMobs()
{
    if (GetData(TYPE_CRISTAL_EVENT) != NOT_STARTED)
        return;
#ifdef DEBUG_ON
    sLog.outString("instance_dire_maul::DoSortCristalsEventMobs");
#endif
    for (uint8 i = 0; i < MAX_CRISTALS; i++)
    {
        if (GameObject* pRune = instance->GetGameObject(m_auiCristalsGUID[i]))
        {
            for (std::list<uint64>::const_iterator itr = m_lCristalsEventtMobGUIDList.begin(); itr != m_lCristalsEventtMobGUIDList.end(); itr++)
            {
                if (Creature* pCreature = instance->GetCreature(*itr))
                {
                    if (pCreature->isAlive() && pCreature->GetDistance(pRune) < 10.0f)
                        m_alCristalsEventtMobGUIDSorted[i].push_back(*itr);
                }
            }
#ifdef DEBUG_ON
            sLog.outString("Cristal %u : %u mobs", i + 1,  m_alCristalsEventtMobGUIDSorted[i].size());
#endif
        }
    }

    SetData(TYPE_CRISTAL_EVENT, IN_PROGRESS);
}

InstanceData* GetInstanceData_instance_dire_maul(Map* pMap)
{
    return new instance_dire_maul(pMap);
}

// TRASH
// npc_reste_mana
enum
{
    SPELL_CHAINLIGHTNING     = 15659,
    SPELL_BLINK              = 14514
};
struct npc_reste_manaAI : public ScriptedAI
{
    npc_reste_manaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*) pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;
    uint32 m_uiChainLighting_Timer;
    uint32 m_uiBlink_Timer;

    void Reset()
    {
        m_uiBlink_Timer = urand(12000, 23000);
        m_uiChainLighting_Timer = urand(2000, 6000);
        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData64(TYPE_CRISTAL_EVENT, m_creature->GetGUID());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (m_uiBlink_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLINK);
            m_uiBlink_Timer = 6000;
            return;
        }   
        else
            m_uiBlink_Timer -= uiDiff;

        if (m_uiChainLighting_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAINLIGHTNING);
            m_uiChainLighting_Timer = 6000;
            return;
        }
        else
            m_uiChainLighting_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_reste_mana(Creature* pCreature)
{
    return new npc_reste_manaAI(pCreature);
}

// npc_arcane_aberration (11480)
enum
{
    SPELL_MANABURN           = 22936,
    SPELL_ARCANEBOLT         = 15979
};
struct npc_arcane_aberrationAI : public ScriptedAI
{
    npc_arcane_aberrationAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*) pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;
    uint32 m_uiArcaneBoltTimer;
    bool m_bManaBurnDone;

    void Reset()
    {
        m_uiArcaneBoltTimer = urand(0, 400);
        m_bManaBurnDone = false;
        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData64(TYPE_CRISTAL_EVENT, m_creature->GetGUID());
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (m_creature->GetHealthPercent() > 5.0f)
            return;

        if (!m_bManaBurnDone)
        {
            DoCastSpellIfCan(m_creature, SPELL_MANABURN);
            m_bManaBurnDone = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (m_uiArcaneBoltTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANEBOLT);
            m_uiArcaneBoltTimer = urand(2400, 3800);
            return;
        }
        else
            m_uiArcaneBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_arcane_aberration(Creature* pCreature)
{
    return new npc_arcane_aberrationAI(pCreature);
}

// npc_residual_montruosity (11484)
enum
{
    SPELL_ARCANEBLAST        = 22940,
    SPELL_SUMMON_MANABURSTS  = 22939,
    SPELL_ARCANEBOLT2        = 13748
};
struct npc_residual_montruosityAI : public ScriptedAI
{
    npc_residual_montruosityAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*) pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;
    uint32 m_uiArcaneBoltTimer;
    uint32 m_uiArcaneBlastTimer;

    void Reset()
    {
        m_uiArcaneBlastTimer = urand(1000, 2500);
        m_uiArcaneBoltTimer = urand(0, 400);
        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
    }

    void JustDied(Unit* pKiller)
    {
        DoCastSpellIfCan(m_creature, SPELL_SUMMON_MANABURSTS);
    }

    void UpdateFormationSpeed()
    {
        float newspeed = 1.79f;
        if (!m_creature->getVictim())
        {

            float closestbefore = 45.0f;
            float closestbehind = 45.0f;

            std::list<Creature*> montruosityList;
            GetCreatureListWithEntryInGrid(montruosityList, m_creature, 11484, 45.0f);
            if (!montruosityList.empty())
            {
                for (std::list<Creature*>::iterator itr = montruosityList.begin(); itr != montruosityList.end(); ++itr)
                {
                    if ((*itr) != m_creature)
                    {
                        if (!(*itr)->isInFrontInMap(m_creature, 45.0f, M_PI_F))
                        {
                            float distance = m_creature->GetDistance((*itr));
                            if (distance < closestbefore)
                                closestbefore = distance;
                        }
                        else
                        {
                            float distance = m_creature->GetDistance((*itr));
                            if (distance < closestbehind)
                                closestbehind = distance;
                        }
                    }
                }
            }
            if ((closestbefore > 36.0f) && (closestbehind < 32.0f))
                newspeed = 2.20f;
            else if ((closestbefore < 32.0f) && (closestbehind > 36.0f))
                newspeed = 1.00f;
        }
        m_creature->SetSpeedRate(MOVE_WALK, newspeed, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            UpdateFormationSpeed();
            return;
        }

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (m_uiArcaneBoltTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANEBOLT2);
            m_uiArcaneBoltTimer = urand(2400, 3800);
            return;
        }
        else
            m_uiArcaneBoltTimer -= uiDiff;

        if (m_uiArcaneBlastTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_ARCANEBLAST);
            m_uiArcaneBlastTimer = urand(3800, 5200);
            return;
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_residual_montruosity(Creature* pCreature)
{
    return new npc_residual_montruosityAI(pCreature);
}

/*######
## go_broken_trap
######*/

#define QUEST_REPAIR        1193

bool GOQuestRewarded_go_broken_trap(Player* pPlayer, GameObject* pGo, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_REPAIR)
        pGo->Delete();

    return true;
}

/*######
## npc_mizzle_the_crafty
######*/

#define GOSSIP_ITEM_KING     "So, now that I'm the king... What do you have for me?"
#define GOSSIP_ITEM_ASK      "Tell me more..."
#define GOSSIP_ITEM_TRIBUTE  "I want my tribute!"

#define SPELL_KING_OF_THE_GORDOK     22799

struct npc_mizzle_the_craftyAI : public ScriptedAI
{
    npc_mizzle_the_craftyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*) pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    void Reset()
    {
        m_creature->CastSpell(m_creature, 23319, false); // Lance un event qui le fait yell/déplacement/say
    }
};

CreatureAI* GetAI_npc_mizzle_the_crafty(Creature* pCreature)
{
    return new npc_mizzle_the_craftyAI(pCreature);
}

bool GossipHello_npc_mizzle_the_crafty(Player* pPlayer, Creature* pCreature)
{
    if (!pPlayer->HasAura(SPELL_KING_OF_THE_GORDOK))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(6882, pCreature->GetGUID());
    }

    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ASK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(6916, pCreature->GetGUID());
    }

    return true;
}

bool GossipSelect_npc_mizzle_the_crafty(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CastSpell(pPlayer, SPELL_KING_OF_THE_GORDOK, false);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ASK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(6916, pCreature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+2:
            if (instance_dire_maul* m_pInstance = (instance_dire_maul*) pCreature->GetInstanceData())
            {
                if (m_pInstance->GetData(TYPE_GORDOK_TRIBUTE) != DONE)
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ASK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            }
            pPlayer->SEND_GOSSIP_MENU(6876, pCreature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRIBUTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(6895, pCreature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CLOSE_GOSSIP_MENU();
            if (instance_dire_maul* m_pInstance = (instance_dire_maul*) pCreature->GetInstanceData())
                m_pInstance->SetData(TYPE_GORDOK_TRIBUTE, DONE);
            break;
    }
    return true;
}

/*######
## npc_knot_thimblejack
######*/

struct npc_knot_thimblejackAI : public ScriptedAI
{
    npc_knot_thimblejackAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*) pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    void Reset() {}

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (id)
        {
            case 1:
                me->GetMotionMaster()->MovePoint(2, 470.225372f, 542.596065f, -25.363186f);
                break;
            case 2:
                me->GetMotionMaster()->MovePoint(3, 465.367096f, 542.843689f, -23.911942f);
                break;
            case 3:
                me->GetMotionMaster()->MovePoint(4, 453.346649f, 544.004456f, -23.900503f);
                break;
            case 4:
                me->GetMotionMaster()->MovePoint(5, 435.055573f, 542.503967f, -18.395958f);
                break;
            case 5:
                me->GetMotionMaster()->MovePoint(6, 412.700775f, 537.009277f, -18.343367f);
                break;
            case 6:
                me->GetMotionMaster()->MovePoint(7, 401.076355f, 524.250061f, -12.787789f);
                break;
            case 7:
                me->GetMotionMaster()->MovePoint(8, 390.540833f, 502.830505f, -12.675946f);
                break;
            case 8:
                me->GetMotionMaster()->MovePoint(9, 386.112335f, 483.010040f, -7.232251f);
                break;
            case 9:
                me->GetMotionMaster()->MovePoint(10, 385.963501f, 442.606476f, -7.193601f);
                break;
            case 10:
                me->GetMotionMaster()->MovePoint(11, 385.531738f, 416.619385f, -1.703543f);
                break;
            case 11:
                me->GetMotionMaster()->MovePoint(12, 385.355988f, 375.940430f, -1.623023f);
                break;
            case 12:
                me->GetMotionMaster()->MovePoint(13, 385.620300f, 350.467163f, 3.825020f);
                break;
            case 13:
                me->ForcedDespawn(5000); //Despawn après 5 sec
                break;
        }
    }
};

CreatureAI* GetAI_npc_knot_thimblejack(Creature* pCreature)
{
    return new npc_knot_thimblejackAI(pCreature);
}

bool QuestRewarded_npc_knot_thimblejack(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == 5525 || pQuest->GetQuestId() == 7429)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->GetMotionMaster()->MovePoint(1, 518.325195f, 542.000183f, -23.900503f);
    }

    return true;
}

enum
{
    SPELL_BACK_HAND     =   6253,
    SPELL_ENRAGE        =   15716,
    SPELL_BRUISING_BLOW =   22572,
    SPELL_PUMMEL        =   15615,
    SPELL_UPPERCUT      =   18072,
};

/******************/
struct GordokBruteAI : public ScriptedAI
{
    GordokBruteAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        /** Save current equipment of the creature */
        m_uiEquipment_id = m_creature->GetEquipmentId();
        Reset();
    }

    uint32 m_uiEquipment_id;
    uint32 m_uiBackhand_Timer;
    uint32 m_uiBruisingBlow_Timer;
    uint32 m_uiPummel_Timer;
    uint32 m_uiUppercut_Timer;
    bool m_bEnrage;

    void Reset()
    {
        m_creature->LoadEquipment(m_uiEquipment_id, true);
        m_uiBackhand_Timer     = 2000;
        m_uiUppercut_Timer     = 0;
        m_uiBruisingBlow_Timer = 6000;
        m_uiPummel_Timer       = 5000;
        m_bEnrage = false;
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();

        uint32 yellChance = urand(0, 4);
        switch (yellChance)
        {
            case 0:
                m_creature->MonsterYell("Me smash! You die!");
                break;
            case 1:
                m_creature->MonsterYell("The Great One will smash you!");
                break;
            case 2:
                char eMessage[100];
                sprintf(eMessage, "Raaar!!! Me smash %s",pWho->GetName());
                m_creature->MonsterYell(eMessage);
                break;
            default:
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        /** Spell above 30% of life */
        if (m_uiBruisingBlow_Timer < uiDiff)
        {
        	if(m_creature->GetHealthPercent() > 30.0f)
        		if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BRUISING_BLOW) == CAST_OK)
        			m_uiBruisingBlow_Timer = urand(3000, 8000);
        }
        else
        	m_uiBruisingBlow_Timer -= uiDiff;

        if (m_uiPummel_Timer < uiDiff)
        {
        	if(m_creature->GetHealthPercent() > 30.0f && m_creature->getVictim()->IsNonMeleeSpellCasted(true))
        		if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PUMMEL) == CAST_OK)
        			m_uiPummel_Timer = urand(8000, 10000);
        }
        else
        	m_uiPummel_Timer -= uiDiff;

        
        if (m_creature->GetHealthPercent() < 30.0f && !m_bEnrage)
        {
            char eMessage[100];
            sprintf(eMessage, "Gordok Brute puts his club away and begins swinging wildly!");
            m_creature->LoadEquipment(0, true);
            m_creature->MonsterTextEmote(eMessage, NULL, false);

            m_creature->CastSpell(m_creature, SPELL_ENRAGE, false);
            m_bEnrage = true;
        }

        if (m_creature->GetHealthPercent() < 30.0f && m_bEnrage)
        {
            if (m_uiBackhand_Timer < uiDiff)
            {
           		if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BACK_HAND) == CAST_OK)
           			m_uiBackhand_Timer = urand(5000, 9000);
            }
            else
            	m_uiBackhand_Timer -= uiDiff;
        }

        if (m_uiUppercut_Timer < uiDiff)
        {
        	if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_UPPERCUT) == CAST_OK)
        		m_uiUppercut_Timer = urand(6000, 10000);
        }
        else
            m_uiUppercut_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_gordok_brute(Creature* pCreature)
{
    return new GordokBruteAI(pCreature);
}

void AddSC_instance_dire_maul()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_dire_maul";
    pNewScript->GetInstanceData = &GetInstanceData_instance_dire_maul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gordok_brute";
    pNewScript->GetAI = &GetAI_mob_gordok_brute;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_reste_mana";
    pNewScript->GetAI = &GetAI_npc_reste_mana;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arcane_aberration";
    pNewScript->GetAI = &GetAI_npc_arcane_aberration;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_residual_montruosity";
    pNewScript->GetAI = &GetAI_npc_residual_montruosity;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_broken_trap";
    pNewScript->pQuestRewardedGO = &GOQuestRewarded_go_broken_trap;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mizzle_the_crafty";
    pNewScript->GetAI = &GetAI_npc_mizzle_the_crafty;
    pNewScript->pGossipHello = &GossipHello_npc_mizzle_the_crafty;
    pNewScript->pGossipSelect = &GossipSelect_npc_mizzle_the_crafty;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_knot_thimblejack";
    pNewScript->GetAI = &GetAI_npc_knot_thimblejack;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_knot_thimblejack;
    pNewScript->RegisterSelf();
}
