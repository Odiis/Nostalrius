#include "scriptPCH.h"
#include "MoveMapSharedDefines.h"

//#define DEBUG_ELEMENTAL_INVASIONS

enum
{
    NPC_ADD_FIRE        = 14460,
    NPC_LEADER_FIRE     = 14461,

    NPC_ADD_WATER       = 14458,
    NPC_LEADER_WATER    = 14457,

    NPC_ADD_LAND        = 14462,
    NPC_LEADER_LAND     = 14464,

    NPC_ADD_WHIRL       = 14455,
    NPC_LEADER_WHIRL    = 14454,
};


class elemental_invasion_portalAI: public GameObjectAI
{
public:
    elemental_invasion_portalAI(GameObject* gobj, uint32 spawnMobEntry, ObjectGuid leaderGuid) : GameObjectAI(gobj), m_summonEntry(spawnMobEntry), m_leaderGuid(leaderGuid)
    {
        m_summonTimer  = 10000;
        m_visibleTimer = urand(1, 5 * 60) * 60; // Random spawn within 5 hours
    }

    ObjectGuid m_leaderGuid;
    uint32 m_summonEntry;
    uint32 m_summonTimer;
    uint32 m_visibleTimer;
    const static uint32 MAX_NPC_PER_SPAWN = 12;
    ObjectGuid m_summoned[MAX_NPC_PER_SPAWN];

    void UpdateAI(const uint32 diff)
    {
        if (m_summonTimer < diff)
        {
            Creature* leader = me->GetMap()->GetCreature(m_leaderGuid);
            if (!leader)
                return;

            time_t now = time(NULL);
            me->SetVisible(!leader->GetRespawnTime() || leader->GetRespawnTime() < now + m_visibleTimer);
            if (!me->IsVisible())
                return;

            // How many should we spawn
            uint32 spawnCount = MAX_NPC_PER_SPAWN;
            if (leader->GetRespawnTime())
                spawnCount = MAX_NPC_PER_SPAWN * (leader->GetRespawnTime() - now) / (5 * 60 * 60);
            if (spawnCount > MAX_NPC_PER_SPAWN) // Should not happen ...
                spawnCount = MAX_NPC_PER_SPAWN;

            // Find available slot
            for (int i = 0; i < spawnCount; ++i)
                if (!me->GetMap()->GetCreature(m_summoned[i]))
                {
                    float x, y, z;
                    me->GetPosition(x, y, z);
                    Creature* add = me->SummonCreature(m_summonEntry, x, y, z, 0.0, TEMPSUMMON_CORPSE_DESPAWN);
                    if (!add)
                        return;
                    for (int attempts = 0; attempts < 20; ++attempts)
                    {
                        me->GetMap()->GetWalkRandomPosition(NULL, x, y, z, 55.0f, NAV_GROUND | NAV_STEEP_SLOPES | NAV_WATER);
                        if (me->GetDistance2d(x, y) > 20.0f)
                            break;
                    }
                    add->SetHomePosition(x, y, z, frand(0, 2 * M_PI));
                    add->SetRespawnRadius(20.0f);
                    add->SetDefaultMovementType(RANDOM_MOTION_TYPE);
                    add->GetMotionMaster()->MoveTargetedHome();

                    m_summoned[i] = add->GetObjectGuid();
                    break;
                }
#ifndef DEBUG_ELEMENTAL_INVASIONS
            m_summonTimer = urand(30000, 120000); // Between 30 secs and 2 minutes spawn timer
#else
            m_summonTimer = 5000;
#endif
        }
        else
            m_summonTimer -= diff;
    }
};

GameObjectAI* GetAI_go_elemental_invasion_portal_fire(GameObject* gobj)
{
    return new elemental_invasion_portalAI(gobj, NPC_ADD_FIRE, sObjectMgr.GetOneCreatureByEntry(NPC_LEADER_FIRE));
}

GameObjectAI* GetAI_go_elemental_invasion_portal_water(GameObject* gobj)
{
    return new elemental_invasion_portalAI(gobj, NPC_ADD_WATER, sObjectMgr.GetOneCreatureByEntry(NPC_LEADER_WATER));
}

GameObjectAI* GetAI_go_elemental_invasion_portal_land(GameObject* gobj)
{
    return new elemental_invasion_portalAI(gobj, NPC_ADD_LAND, sObjectMgr.GetOneCreatureByEntry(NPC_LEADER_LAND));
}

GameObjectAI* GetAI_go_elemental_invasion_portal_whirl(GameObject* gobj)
{
    return new elemental_invasion_portalAI(gobj, NPC_ADD_WHIRL, sObjectMgr.GetOneCreatureByEntry(NPC_LEADER_WHIRL));
}

void AddSC_elemental_invasions()
{
    Script* newscript = new Script;
    newscript->Name = "go_elemental_invasion_portal_fire";
    newscript->GOGetAI = &GetAI_go_elemental_invasion_portal_fire;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_elemental_invasion_portal_water";
    newscript->GOGetAI = &GetAI_go_elemental_invasion_portal_water;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_elemental_invasion_portal_land";
    newscript->GOGetAI = &GetAI_go_elemental_invasion_portal_land;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_elemental_invasion_portal_whirl";
    newscript->GOGetAI = &GetAI_go_elemental_invasion_portal_whirl;
    newscript->RegisterSelf();
}
