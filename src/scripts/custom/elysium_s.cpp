/*
Copyright Elysium :
https://elysium-project.org/
*/
#include "scriptPCH.h"
#include "PlayerAI.h"
#include "Config/Config.h"
#include "MapManager.h"

#define SPELL_AGGRO 4507

struct inge_leurreAI : public ScriptedAI
{
    uint32 stayTime;
    bool active;

    inge_leurreAI(Creature *c, uint32 delay = 15000) : ScriptedAI(c)
    {
        stayTime = delay;
        c->CastSpell(c, SPELL_AGGRO, false);
        active = true;
        b_isAggro = false;
        m_uiAggro_Timer = 0;
        m_creature->addUnitState(UNIT_STAT_ROOT);
    }

    uint32 m_uiAggro_Timer;
    bool b_isAggro;

    void Reset()
    {
        m_creature->addUnitState(UNIT_STAT_ROOT);

        if (b_isAggro)
        {
            m_uiAggro_Timer = 3000;
            b_isAggro = false;
        }
    }


    void Aggro(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
//        m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);

//        if (!m_creature->hasUnitState(UNIT_STAT_ROOT))
//            m_creature->addUnitState(UNIT_STAT_ROOT);


        if (!active)
            return;

        stayTime - diff;
        if (stayTime < diff)
        {
            // Despawn
            m_creature->CombatStop();
            m_creature->AddObjectToRemoveList();
            active = false;
            return;
        }
        stayTime -= diff;


        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);

        if (!m_creature->hasUnitState(UNIT_STAT_ROOT))
            m_creature->addUnitState(UNIT_STAT_ROOT);


        if (m_uiAggro_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature, 4044, false) == CAST_OK)
            {
                m_uiAggro_Timer = 3000;
                b_isAggro = true;
            }
        }
        else
            m_uiAggro_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_inge_leurre(Creature* pCreature)
{
    return new inge_leurreAI(pCreature, 15000);
}

enum
{
    SPELL_TELEPORT_VISUAL       = 21649,
    SPELL_LIGHTNING_VISUAL      = 19513,
    SPELL_KNOCKBACK_VISUAL      = 10689,
    SPELL_SLOW_FALL             = 12438,
    GO_KNOCKBACK_TRACE          = 164870,
};

class PlayerRephaseAI: public PlayerAI
{
public:
    PlayerRephaseAI(Player* player, uint32 recallTimer): PlayerAI(player), rephaseTimer(recallTimer), teleportDelay(1000)
    {
        me->SetDrunkValue(0xFF);
        me->SendSpellGo(me, SPELL_TELEPORT_VISUAL);
        me->SendSpellGo(me, SPELL_LIGHTNING_VISUAL);
    }
    void GenerateTeleportPositions(float& x, float &y, float &z)
    {
        switch (urand(0, 7))
        {
            case 0:
                x=-6768;y=1272;z=55;
                mapId = 1;
                break;
            case 1:
                x=7392;y=-4495;z=748;
                mapId = 1;
                break;
            case 2:
                x=-1286;y=1937;z=160;
                mapId = 1;
                break;
            case 3:
                x=-7927;y=3511;z=184;
                mapId = 1;
                break;
            case 4:
                x=2079;y=-4258;z=184;
                mapId = 0;
                break;
            case 5:
                x=-1280;y=-2517;z=137;
                mapId = 0;
                break;
            case 6:
                x=-11816;y=203;z=108;
                mapId = 0;
                break;
            case 7:
                x=-6862;y=-1421;z=354;
                mapId = 0;
                break;
        }
        x += frand(-200, 200);
        y += frand(-200, 200);
    }
    void DoTeleport()
    {
        float x, y, z;
        GenerateTeleportPositions(x, y, z);
        instanceId = mapId + 20;
        me->TeleportTo(mapId, x, y, z, 0.0f);
        me->AddAura(SPELL_SLOW_FALL);
    }
    void UpdateAI(uint32 diff)
    {
        if (teleportDelay)
        {
            if (teleportDelay <= diff)
            {
                DoTeleport();
                teleportDelay = 0;
            }
            else
                teleportDelay -= diff;
            return;
        }
        if (me->IsInWorld() && me->GetMapId() == mapId && me->GetInstanceId() != instanceId && !me->IsBeingTeleported())
        {
            me->SetAutoInstanceSwitch(false);
            me->SendSpellGo(me, SPELL_TELEPORT_VISUAL);
            me->SendSpellGo(me, SPELL_LIGHTNING_VISUAL);
            sMapMgr.ScheduleInstanceSwitch(me, instanceId);
            return;
        }
        if (rephaseTimer < diff)
        {
            me->SendSpellGo(me, SPELL_TELEPORT_VISUAL);
            me->SetDrunkValue(0);
            me->SetAutoInstanceSwitch(true);
            Remove();
            return;
        }
        else
            rephaseTimer -= diff;
        me->SetPvP(false); // PvE server simulation ;)
    }
    uint32 rephaseTimer;
    uint32 mapId;
    uint32 instanceId;
    uint32 teleportDelay;
};

class go_kryll_teleporterAI: public GameObjectAI
{
public:
    go_kryll_teleporterAI(GameObject* gobj) : GameObjectAI(gobj)
    {

    }

    void UpdateAI(uint32 diff)
    {
        float knockbackdist = sConfig.GetFloatDefault("Kryll.Knockback.Radius", -1.0f);
        float knockbackchance = sConfig.GetFloatDefault("Kryll.Knockback.Chance", -1.0f);
        float lightningdist = sConfig.GetFloatDefault("Kryll.Lightning.Radius", -1.0f);
        float lightningchance = sConfig.GetFloatDefault("Kryll.Lightning.Chance", -1.0f);
        float teleportdist = sConfig.GetFloatDefault("Kryll.Teleport.Radius", -1.0f);
        float teleportchance = sConfig.GetFloatDefault("Kryll.Teleport.Chance", -1.0f);
        uint32 myZone = me->GetZoneId();
        Map::PlayerList const& mapPlayers = me->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator it = mapPlayers.begin(); it != mapPlayers.end(); ++it)
        {
            Player* player = it->getSource();
            if (!player->IsInWorld() || player->isGameMaster())
                continue;
            if (player->GetCachedZoneId() == myZone)
                player->SetPvP(false);
            if (lightningdist > 0 && player->IsWithinDist(me, lightningdist))
                if (frand(0, 100) <= lightningchance)
                    player->SendSpellGo(player, SPELL_LIGHTNING_VISUAL);
            if (teleportdist > 0 && player->IsWithinDist(me, teleportdist))
            {
                if (!player->AI() && frand(0, 100) <= teleportchance)
                {
                    player->setAI(new PlayerRephaseAI(player, urand(60, 180) * 1000));
                    player->SummonGameObject(GO_KNOCKBACK_TRACE, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), frand(0, 3) + player->GetOrientation(), 0, 0, 0, 60000);
                }
            }
            else if (knockbackdist > 0 && player->IsWithinDist(me, knockbackdist))
            {
                if (frand(0, 100) <= knockbackchance /*&& !player->GetCheatData()->IsInKnockBack()*/)
                {
                    player->SendSpellGo(player, SPELL_KNOCKBACK_VISUAL);
                    player->SummonGameObject(GO_KNOCKBACK_TRACE, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), frand(0, 3) + player->GetOrientation(), 0, 0, 0, 60000);
                    player->KnockBackFrom(me, frand(20, 60), frand(100, 200));
                }
            }
        }
    }
};

GameObjectAI* GetAI_go_kryll_teleporter(GameObject* gobj)
{
    return new go_kryll_teleporterAI(gobj);
}

void AddSC_elysium()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_target_dummy";
    newscript->GetAI = &GetAI_inge_leurre;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_kryll_teleporter";
    newscript->GOGetAI = &GetAI_go_kryll_teleporter;
    newscript->RegisterSelf();
}
