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
SDName: Western_Plaguelands
SD%Complete: 90
SDComment: Quest support: 5216,5219,5222,5225,5229,5231,5233,5235. To obtain Vitreous Focuser (could use more spesifics about gossip items)
SDCategory: Western Plaguelands
EndScriptData */

/* ContentData
npcs_dithers_and_arbington
npc_myranda_hag
npc_the_scourge_cauldron
EndContentData */

#include "scriptPCH.h"

/*######
## npcs_dithers_and_arbington
######*/

bool GossipHello_npcs_dithers_and_arbington(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->GetQuestRewardStatus(5237) || pPlayer->GetQuestRewardStatus(5238))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What does the Felstone Field Cauldron need?",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What does the Dalson's Tears Cauldron need?",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What does the Writhing Haunt Cauldron need?",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What does the Gahrron's Withering Cauldron need?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        pPlayer->SEND_GOSSIP_MENU(3985, pCreature->GetGUID());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npcs_dithers_and_arbington(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, i need a Vitreous Focuser", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(3980, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, i need a Vitreous Focuser", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(3981, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, i need a Vitreous Focuser", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(3982, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, i need a Vitreous Focuser", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(3983, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, 17529, false);
            break;
    }
    return true;
}

/*######
## npc_myranda_the_hag
######*/

enum
{
    QUEST_SUBTERFUGE        = 5862,
    QUEST_IN_DREAMS         = 5944,
    SPELL_SCARLET_ILLUSION  = 17961
};

#define GOSSIP_ITEM_ILLUSION    "I am ready for the illusion, Myranda."

bool GossipHello_npc_myranda_the_hag(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_SUBTERFUGE) == QUEST_STATUS_COMPLETE &&
            !pPlayer->GetQuestRewardStatus(QUEST_IN_DREAMS) && !pPlayer->HasAura(SPELL_SCARLET_ILLUSION))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ILLUSION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(4773, pCreature->GetGUID());
        return true;
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_myranda_the_hag(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_SCARLET_ILLUSION, false);
    }
    return true;
}

/*######
## npc_the_scourge_cauldron
######*/

struct npc_the_scourge_cauldronAI : public ScriptedAI
{
    npc_the_scourge_cauldronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() {}

    void DoDie()
    {
        //summoner dies here
        m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        //override any database `spawntimesecs` to prevent duplicated summons
        uint32 rTime = m_creature->GetRespawnDelay();
        if (rTime < 600)
            m_creature->SetRespawnDelay(600);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            switch (m_creature->GetAreaId())
            {
                case 199:                                   //felstone
                    if (((Player*)who)->GetQuestStatus(5216) == QUEST_STATUS_INCOMPLETE ||
                            ((Player*)who)->GetQuestStatus(5229) == QUEST_STATUS_INCOMPLETE)
                    {
                        m_creature->SummonCreature(11075, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                        DoDie();
                    }
                    break;
                case 200:                                   //dalson
                    if (((Player*)who)->GetQuestStatus(5219) == QUEST_STATUS_INCOMPLETE ||
                            ((Player*)who)->GetQuestStatus(5231) == QUEST_STATUS_INCOMPLETE)
                    {
                        m_creature->SummonCreature(11077, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                        DoDie();
                    }
                    break;
                case 201:                                   //gahrron
                    if (((Player*)who)->GetQuestStatus(5225) == QUEST_STATUS_INCOMPLETE ||
                            ((Player*)who)->GetQuestStatus(5235) == QUEST_STATUS_INCOMPLETE)
                    {
                        m_creature->SummonCreature(11078, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                        DoDie();
                    }
                    break;
                case 202:                                   //writhing
                    if (((Player*)who)->GetQuestStatus(5222) == QUEST_STATUS_INCOMPLETE ||
                            ((Player*)who)->GetQuestStatus(5233) == QUEST_STATUS_INCOMPLETE)
                    {
                        m_creature->SummonCreature(11076, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                        DoDie();
                    }
                    break;
            }
        }
    }
};
CreatureAI* GetAI_npc_the_scourge_cauldron(Creature* pCreature)
{
    return new npc_the_scourge_cauldronAI(pCreature);
}

enum eAndorhalTower
{
    GO_BEACON_TORCH                             = 176093
};

struct npc_andorhal_towerAI : public Scripted_NoMovementAI
{
    npc_andorhal_towerAI(Creature *c) : Scripted_NoMovementAI(c) {}

    void Reset()
    {

    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (GetClosestGameObjectWithEntry(m_creature, GO_BEACON_TORCH, 20.0f))
            ((Player*)pWho)->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetGUID());
    }
};

CreatureAI* GetAI_npc_andorhal_tower(Creature* pCreature)
{
    return new npc_andorhal_towerAI(pCreature);
}

/*
En Reve :
[SQL]
UPDATE creature_template SET `resistance1` = 0, `dmgschool` = 1, `armor` = 4795, `mindmg` = 688, `maxdmg` = 769, minhealth=26650, maxhealth=26650, speed_run='1.14286', speed_walk=1, ScriptName="npc_taelan_fordring", spell1=14518, spell2=15493, spell3=17143, spell4=17232 WHERE entry=1842;
UPDATE creature_template SET ScriptName="npc_isillien" WHERE entry=1840;
UPDATE creature_template SET ScriptName="npc_tirion_fordring_in_dreams" WHERE entry=12126;
DELETE FROM creature_involvedrelation WHERE quest=5944;
INSERT INTO creature_involvedrelation (id, quest) VALUES (12126, 5944);
*/
// INSERT INTO  elysium_string(entry, content_default, content_loc2) VALUES
// (170,"Highlord Fordring, are you alright?","Généralissime Fordring, est-ce que ça va ?"),
// (171,"What the...","Que ..."),
// (172,"Sir?","Monsieur ?"),
// (173,"Hey, what's going on over there? Sir, are you alright? Sir??","Hé, qu'est ce qui se passe, ici ? Monsieur, est-ce que ça va ? Monsieur ??"),
// (174,"Remove your disguise, lest you feel the bite of my blade when the fury has taken control.","Enlevez votre déguisement, à moins que vous ne vouliez tâter de ma lame quand je serai bien énervé."),
// (175,"Halt.","Halte !"),
// (176,"It's not much further. The main road is just up ahead.","Ce n'est plus très loin. La route principale est juste devant."),
// (177,"Isillien!","Isillien !"),
// (178,"I will lead us through Hearthglen to the forest's edge. From there, you will take me to my father.","Je vous guiderai à travers Hearthglen jusqu'au seuil de la forêt. De là, vous me mènerez jusqu'à mon père."),
// (179,"Something is wrong with the Highlord. Do something!","Quelque chose qui ne va pas avec le généralissime. Faites quelque chose !"),
// (180,"Noooooooooo!"","NOOOOOOOOOOOON !"),
// (181,"You carry the taint of the scourge. Prepare to enter the twisting nether.","Vous portez la souillure du fléau. Préparez vous à entrer dans le Néant distordu."),
// (182,"There is no escape for you. The Crusade shall destroy all who carry the scourge's taint.","Vous ne pouvez vous échapper. La Croisade détruira quiconque porte la souillure du fléau."),
// (183,"This is not your fight, stranger. Protect yourself from the attacks of the Crimson Elite. I shall battle the Grand Inquisitor.","Ceci n'est pas votre combat. Protégez vous contre les attaques de l'élite cramoisie. Je combattrai le grand inquisiteur."),
// (184,"It is as they say; Like father, like son. You are as weak of will as Tirion... perhaps more so. I can only hope my assassins finally succeeded in ending his pitiful life.","Comme on dit : tel père, tel fils. Vous avez aussi peu de volonté que Tirion... Et c'est peut-être pire encore. J'espère seulement que mes assassins ont réussi à mettre fin à sa pitoyable existence."),
// (185,"The Grand Crusader has charged me with destroying you and your newfound friends, Taelan, but know this: I do this for pleasure, not of obligation or duty.","Le Grand croisé m'a chargé de vous détruire ainsi que vos nouveaux amis, Taelan, mais sachez ceci : je le fais par plaisir, pas par devoir."),
// (186,"Grand Inquisitor Isillien calls for his guardsman.","Grand Inquisiteur Isillien appelle ses gardes."),
// (187,"The end is now, Fordring.","La fin est proche, Fordring."),
// (188,"Enough!","Assez !"),
// (189,"Grand Inquisitor Isillien laughs.","Grand Inquisiteur Isillien rit."),
// (190,"Did you really believe that you could defeat me? Your friends are soon to join you, Taelan.","Vous pensiez vraiment que vous pouviez me battre ? Vos amis vous rejoindront bientôt, Taelan."),
// (191,"Grand Inquisitor Isillien turns his attention towards you.","Grand Inquisiteur Isillien tourne son attention vers vous."),
// (192,"What have you done, Isillien? You once fought with honor, for the good of our people... and now... you have murdered my boy...","Qu'avez-vous fait, Isillien ? Vous combattiez naguère dans l'honneur, pour le bien de notre peuple... Et à présent... Vous avez tué mon petit..."),
// (193,"Tragic. The elder Fordring lives on... You are too late, old man. Retreat back to your cave, hermit, unless you wish to join your son in the Twisting Nether.","Tragique. Le vieux Fordring vit toujours... Vous venez trop tard, vieillard... Retournez dans votre grotte, ermite, à moins que vous ne vouliez retrouver votre fils dans le Néant distordu."),
// (194,"May your soul burn in anguish, Isillien! Light give me strength to battle this fiend.","Que votre âme brûle d'angoisse, Isillien ! La Lumière me donnera la force de combattre cette crapule !"),
// (195,"Face me, coward. Face the faith and strength that you once embodied.","Regardez-moi, espèce de lâche. Affrontez la foi et la force qui furent vôtres naguère !"),
// (196,"Then come, hermit!","Alors venez, ermite ! "),
// (197,"A thousand more like him exist. Ten thousand. Should one fall, another will rise to take the seat of power.","Il en existe encore des milliers comme lui. Dix mille en fait... Si l'un devait tomber, un autre prendrait sa place sur le siège du pouvoir."),
// (198,"Lord Tirion Fordring falls to one knee.","Seigneur Tirion Fordring met un genou à terre."),
// (199,"Look what they did to my boy.","Regardez ce qu'ils ont fait à mon enfant."),
// (200,"Lord Tirion Fordring holds the limp body of Taelan Fordring and softly sobs.","Seigneur Tirion Fordring serre contre lui le corps inerte de Taelan Fordring et sanglote."),
// (201,"Too long have I sat idle, gripped in this haze... this malaise, lamenting what could have been... what should have been.","Je suis demeuré trop longtemps immobile, pris dans ce malaise, me lamentant sur ce qui aurait pu être... Sur ce qui aurait dû être..."),
// (202,"Your death will not have been in vain, Taelan. A new Order is born on this day... an Order which will dedicate itself to extinguising the evil that plagues this world. An evil that cannot hide behind politics and pleasantries.","Ta mort n'aura pas été vaine, Taelan. Un nouvel Ordre est né aujourd'hui... Un Ordre qui se consacrera à la destruction du mal qui ravage ce monde. Un mal qui ne peut se camoufler derrière la politique et les mondanités."),
// (203,"This I promise... This I vow...","Je le promets... J'en fais le voeu...");

//There are 2 lines missing totaly from this script:
//Grand Inquisitor Isillien says: You will not make it to the forest's edge, Fordring.
//Grand Inquisitor Isillien says: You disappoint me, Taelan. I had plans for you... grand plans. Alas, it was only a matter of time before your filthy bloodline would catch up with you.
// INSERT INTO  elysium_string(entry, content_default) VALUES
// (204,"You will not make it to the forest's edge, Fordring."),
// (205,"You disappoint me, Taelan. I had plans for you... grand plans. Alas, it was only a matter of time before your filthy bloodline would catch up with you.");


enum
{
//    QUEST_SUBTERFUGE        = 5862,
//    QUEST_IN_DREAMS         = 5944,


    NPC_CAVALIER_ECARLATE   = 1836,
    NPC_TAELAN_FORDRING     = 1842,
    NPC_TIRION_FORDRING     = 12126, // npc_tirion_fordring
    NPC_ISILLIEN            = 1840,
    NPC_ELITE_CRAMOISIE     = 12128,
    NPC_MYRANDA             = 11872,

    SPELL_FORDRING_MOUNT    = 458, // Pas sur
    SPELL_TIRION_FORDRING_MOUNT = 458, // Pas sur
    SPELL_REVERS            = 6253, // Knockback
//    SPELL_SCARLET_ILLUSION = 17961

    // Source : old.wowhead pour l'ID, et video.
    SPELL_ISILLIEN_MANA_BURN  = 15800,
    SPELL_ISILLIEN_MIND_BLAST = 17194,
    SPELL_ISILLIEN_MIND_FLAY  = 17165,

    SPELL_AURA_DEVOTION       = 17232,
};

struct npc_taelan_fordringAI : public npc_escortAI
{
    npc_taelan_fordringAI(Creature *pCrea) : npc_escortAI(pCrea)
    {
        uiLayOnHands_Cooldown  = 0;
        uiCurrAction = 0;
        uiCurrWaypoint = 0;
        JustRespawned();
    }

    void JustRespawned()
    {
        // Reset status for next user.
        m_creature->RestoreFaction();
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
        summonedGuids.clear();
        npc_escortAI::JustRespawned();
    }

    std::list<uint64> lCavaliersGUIDs;
    uint32 uiNextActionTimer;
    uint32 uiCurrAction;
    uint32 uiCurrWaypoint;

    void QuestCompleted(Player* pPlayer, Quest const* pQuest)
    {
        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        std::list<Creature*> lCavaliers;
        m_creature->GetCreatureListWithEntryInGrid(lCavaliers, NPC_CAVALIER_ECARLATE, 10.0f);
        uint8 i = 0;
        bool bCavaliers = true;
        for (std::list<Creature*>::iterator it = lCavaliers.begin(); it != lCavaliers.end(); ++it)
        {
            if (!(*it)->IsWithinLOSInMap(m_creature) || !(*it)->isAlive())
            {
                bCavaliers = false;
                continue;
            }

            ++i;
            switch (i)
            {
                case 1:
                    (*it)->MonsterSay(ELYSIUM_TEXT(170));
                    break;
                case 2:
                    (*it)->MonsterSay(ELYSIUM_TEXT(171));
                    break;
                case 3:
                    (*it)->MonsterSay(ELYSIUM_TEXT(172));
                    break;
                case 4:
                    (*it)->MonsterSay(ELYSIUM_TEXT(173));
                    break;
            }
            (*it)->SetFacingToObject(m_creature);
            float x, y, z;
            m_creature->GetContactPoint((*it), x, y, z, 3.0f);
            (*it)->GetMotionMaster()->MovePoint(0, x, y, z, MOVE_PATHFINDING);
            lCavaliersGUIDs.push_back((*it)->GetGUID());
        }
        uiCurrAction = bCavaliers ? 2 : 1;
        uiNextActionTimer = 5000;
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell && spell->Id == SPELL_FORDRING_MOUNT)
            SetEscortPaused(false);
    }

    void WaypointReached(uint32 pointId)
    {
        uiCurrWaypoint = pointId;
        switch (pointId)
        {
            case 1:
                if (Player* pEscorter = GetPlayerForEscort())
                {
                    if (pEscorter->HasAura(SPELL_SCARLET_ILLUSION))
                    {
                        m_creature->MonsterSay(ELYSIUM_TEXT(174));
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                }
                break;
            case 15:
                m_creature->MonsterSay(ELYSIUM_TEXT(175));
                if (Player* pEscorter = GetPlayerForEscort())
                    m_creature->SetFacingToObject(pEscorter);
                break;
            case 36:
                // Une embuscade !
                // TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN pour ne pas avoir les loots de strat eca
                if (Creature* Crea = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2712.37f, -1710.58f, 124.594f, 0.862388f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5 * MINUTE * IN_MILLISECONDS))
                    Crea->GetMotionMaster()->MovePoint(0, 2743.2f, -1666.78f, 127.423f, MOVE_PATHFINDING);
                if (Creature* Crea = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2709.54f, -1708.17f, 124.03f, 0.86553f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5 * MINUTE * IN_MILLISECONDS))
                    Crea->GetMotionMaster()->MovePoint(0, 2741.0f, -1665.15f, 126.932f, MOVE_PATHFINDING);
                if (Creature* Crea = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2707.37f, -1706.32f, 124.018f, 0.86553f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5 * MINUTE * IN_MILLISECONDS))
                    Crea->GetMotionMaster()->MovePoint(0, 2738.72f, -1663.46f, 126.689f, MOVE_PATHFINDING);
                if (Creature* Crea = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2705.5f, -1704.47f, 124.745f, 0.86553f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5 * MINUTE * IN_MILLISECONDS))
                    Crea->GetMotionMaster()->MovePoint(0, 2736.35f, -1661.7f, 126.766f, MOVE_PATHFINDING);
                break;
            case 44:
                m_creature->MonsterSay(ELYSIUM_TEXT(176));
                break;
            case 46:
                if (m_creature->HasAura(SPELL_FORDRING_MOUNT))
                    m_creature->RemoveAurasDueToSpell(SPELL_FORDRING_MOUNT);
                m_creature->MonsterYell(ELYSIUM_TEXT(177));
                m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                uiCurrAction = 9;
                uiNextActionTimer = 3000;
                break;
        }
    }

    void QuestAccepted(Player* pPlayer, Quest const* pQuest)
    {
        m_creature->setFaction(FACTION_ESCORT_N_FRIEND_PASSIVE);
        npc_escortAI::Start(false, false, pPlayer->GetGUID(), pQuest, false);
        npc_escortAI::SetMaxPlayerDistance(150.0f);
        m_creature->MonsterSay(ELYSIUM_TEXT(178));
        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
    }

    void MoveInLineOfSight(Unit* unit)
    {
        if (unit && unit->GetEntry() == NPC_ELITE_CRAMOISIE)
        {
            // Ne pas attaquer les elites cramoisies. Seul le joueur s'en occupe pendant le combat final.
            if (uiCurrWaypoint >= 45)
                return;
            // Sinon c'est qu'il s'agit de l'embuscade. On attaque.
            if (unit->GetDistance(m_creature) < 20.0f && summonedGuids.find(unit->GetObjectGuid()) != summonedGuids.end())
                unit->AI()->AttackStart(m_creature);
        }
        npc_escortAI::MoveInLineOfSight(unit);
    }
    void Taelan_DoAction()
    {
        switch (uiCurrAction)
        {
            /* Partie I : Rendre la quete "le subterfuge ecarlate" */
            case 1:
            {
                uint8 i = 0;
                for (std::list<uint64>::iterator it = lCavaliersGUIDs.begin(); it != lCavaliersGUIDs.end(); ++it)
                {
                    if (Creature* pCavalier = m_creature->GetMap()->GetCreature(*it))
                    {
                        if (pCavalier->isAlive())
                        {
                            ++i;
                            switch (i)
                            {
                                case 1:
                                    pCavalier->HandleEmote(EMOTE_ONESHOT_TALK);
                                    pCavalier->MonsterSay(ELYSIUM_TEXT(179));
                                    break;
                                case 2:
                                    pCavalier->HandleEmote(UNIT_STAND_STATE_KNEEL);
                                    break;
                                case 3:
                                    pCavalier->HandleEmote(EMOTE_ONESHOT_TALK);
                                    break;
                                default :
                                    break;
                            }
                        }
                    }
                }
                uiNextActionTimer = 6000;
                break;
            }
            case 2:
            {
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
                for (std::list<uint64>::iterator it = lCavaliersGUIDs.begin(); it != lCavaliersGUIDs.end(); ++it)
                {
                    if (Creature* pCavalier = m_creature->GetMap()->GetCreature(*it))
                    {
                        if (pCavalier->isAlive())
                        {
                            pCavalier->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                            pCavalier->CastSpell(pCavalier, SPELL_REVERS, true);
                            pCavalier->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);
                        }
                    }
                }
                m_creature->MonsterYell(ELYSIUM_TEXT(180));
                uiNextActionTimer = 2500;
                break;
            }
            case 3:
            {
                uint8 i = 0;
                for (std::list<uint64>::iterator it = lCavaliersGUIDs.begin(); it != lCavaliersGUIDs.end(); ++it)
                {
                    if (Creature* pCavalier = m_creature->GetMap()->GetCreature(*it))
                    {
                        if (pCavalier->isAlive())
                        {
                            if (i == 0)
                                pCavalier->MonsterSay(ELYSIUM_TEXT(181));
                            else if (i == 1)
                                pCavalier->MonsterSay(ELYSIUM_TEXT(182));
                            else
                                break;
                            ++i;
                        }
                    }
                }
                m_creature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);
                uiNextActionTimer = 500;
                break;
            }
            case 4:
            {
                for (std::list<uint64>::iterator it = lCavaliersGUIDs.begin(); it != lCavaliersGUIDs.end(); ++it)
                    if (Creature* pCavalier = m_creature->GetMap()->GetCreature(*it))
                        if (pCavalier->isAlive())
                            pCavalier->AI()->AttackStart(m_creature);
                lCavaliersGUIDs.clear();
                uiCurrAction = 0;
                return;
            }
            /* Partie II : A la fin de "En reve" */
            case 9:
            {
                // Invocation de Isillien et de 2 adds.
                Creature* Isillien = m_creature->SummonCreature(NPC_ISILLIEN, 2698.7f, -1951.7f, 72.15f, 2.18f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000, true);
                Creature* add1 = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2700.7f, -1949.7f, 72.15f, 2.18f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000, true);
                Creature* add2 = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2692.7f, -1951.7f, 72.15f, 2.18f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000, true);
                if (!add1 || !add2 || !Isillien)
                {
                    sLog.elysium("[q5944][FORDRING] Impossible d'invoquer Isillien ou ses adds.");
                    uiCurrAction = 0;
                    return;
                }
                Isillien->AI()->InformGuid(m_creature->GetObjectGuid(), NPC_TAELAN_FORDRING);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                Isillien->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                add1->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                add2->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetFacingToObject(Isillien);
                Isillien->SetWalk(true);
                Isillien->GetMotionMaster()->MovePoint(1, 2676.6f, -1917.4f, 67.81f, MOVE_PATHFINDING);
                add1->GetMotionMaster()->MovePoint(1, 2680.9f, -1916.34f, 68.27f, MOVE_PATHFINDING);
                add2->GetMotionMaster()->MovePoint(1, 2672.3f, -1920.06f, 67.8f, MOVE_PATHFINDING);
                // D'autres mobs pour attaquer le joueur.
                if (Player* player = GetPlayerForEscort())
                {
                    add1 = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2696.5f, -1809.7f, 85.42f, 4.77f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000, true);
                    add2 = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2696.5f, -1809.7f, 85.42f, 4.77f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000, true);
                    Creature* add3 = m_creature->SummonCreature(NPC_ELITE_CRAMOISIE, 2696.5f, -1809.7f, 85.42f, 4.77f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000, true);
                    if (add1 && add2 && add3)
                    {
                        add1->AI()->AttackStart(player);
                        add2->AI()->AttackStart(player);
                        add3->AI()->AttackStart(player);
                    }
                }
                uiNextActionTimer = 3000;
                break;
            }
            case 10:
            {
                if (Player* player = GetPlayerForEscort())
                    m_creature->SetFacingToObject(player);
                m_creature->MonsterSay(ELYSIUM_TEXT(183));
                break;
            }
            default:
            {
                uiNextActionTimer = 4000;
                break;
            }
        }
        ++uiCurrAction;
    }

    /*
    Variables de combat
    */
    uint32 uiDiff_Add;

    uint32 uiLayOnHands_Cooldown;
    uint32 uiHeal_Cooldown;
    uint32 uiGlobalCooldown;

    void Reset()
    {
        uiGlobalCooldown       = 0;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }
    void EnterCombat(Unit*)
    {
        if (m_creature->HasAura(SPELL_FORDRING_MOUNT))
            m_creature->RemoveAurasDueToSpell(SPELL_FORDRING_MOUNT);
    }
    void JustDied(Unit* pKiller)
    {
        if (uiCurrWaypoint <= 45) // EscortAI: do not fail the quest at death if part of the event
            npc_escortAI::JustDied(pKiller);
        uiCurrWaypoint = 0;
        uiCurrAction = 0;
        uiLayOnHands_Cooldown  = 0;
    }
    void UpdateAI(uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            // Pas en combat.
            if (uiCurrAction)
            {
                if (uiNextActionTimer < uiDiff)
                    Taelan_DoAction();
                else
                    uiNextActionTimer -= uiDiff;
            }
            if (uiCurrWaypoint <= 45)
            {
                if (uiCurrWaypoint >= 15)
                {
                    if (!m_creature->HasAura(SPELL_FORDRING_MOUNT))
                    {
                        m_creature->CastSpell(m_creature, SPELL_FORDRING_MOUNT, false);
                        SetEscortPaused(true);
                    }
                }
                npc_escortAI::UpdateAI(uiDiff);
            }
            return;
        }


        // Quand on cast un sort, les timer ne peuvent pas se declencher.
        if (m_creature->IsNonMeleeSpellCasted(false))
        {
            uiDiff_Add += uiDiff;
            return;
        }
        // GlobalCD non ecoule.
        if (uiGlobalCooldown > uiDiff)
        {
            uiDiff_Add += uiDiff;
            uiGlobalCooldown -= uiDiff;
            return;
        }

        uiDiff     += uiDiff_Add;
        uiDiff_Add  = 0;

        if (uiLayOnHands_Cooldown < uiDiff)
            uiLayOnHands_Cooldown = 0;
        else
            uiLayOnHands_Cooldown -= uiDiff;

        if (uiHeal_Cooldown < uiDiff)
            uiHeal_Cooldown = 0;
        else
            uiHeal_Cooldown -= uiDiff;

        // 1 : Check des buffs
        if (!m_creature->HasAura(17232))
        {
            m_creature->CastSpell(m_creature, 17232, false);
            return;
        }

        // 2 : Check des heals
        if (!uiHeal_Cooldown && m_creature->GetHealthPercent() < 50.0f)
        {
            // Plus trop de mana : Impo des mains prete ?
            if (m_creature->GetPower(POWER_MANA) < 150 && !uiLayOnHands_Cooldown)
            {
                m_creature->CastSpell(m_creature, 17233, false);
                uiHeal_Cooldown = 10000;
                uiLayOnHands_Cooldown = 600000; // 10 minutes de CD.
                return;
            }
            // Sinon lumiere sacree.
            else if (m_creature->GetPower(POWER_MANA) > 150)
            {
                m_creature->CastSpell(m_creature, 15493, false);
                uiHeal_Cooldown = 5000;
                return;
            }
            uiHeal_Cooldown = 20000;
        }

        // 3 : Ensuite, les attaques
        switch (urand(0, 4))
        {
            case 0:
            case 1:
                // Crusader Strike
                m_creature->CastSpell(m_creature->getVictim(), 14518, false);
                break;
            case 2:
            case 3:
                // Holy Strike
                m_creature->CastSpell(m_creature->getVictim(), 17143, false);
                break;
            case 4:
                // Holy Cleave
                m_creature->CastSpell(m_creature->getVictim(), 18819, false);
                break;
        }

        uiGlobalCooldown = 1500;
        DoMeleeAttackIfReady();
    }
    void GetAIInformation(ChatHandler& reader)
    {
        reader.PSendSysMessage("Taelan: Action%u Combat%u", uiCurrAction, m_creature->isInCombat());
        npc_escortAI::GetAIInformation(reader);
    }
    std::set<ObjectGuid> summonedGuids;
    void JustSummoned(Creature* summoned)
    {
        summonedGuids.insert(summoned->GetObjectGuid());
    }
};

bool QuestAccept_npc_taelan_fordring(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_IN_DREAMS)
        if (npc_taelan_fordringAI* pEscortAI = dynamic_cast<npc_taelan_fordringAI*>(pCreature->AI()))
            pEscortAI->QuestAccepted(pPlayer, pQuest);

    return true;
}

CreatureAI* GetAI_npc_taelan_fordring(Creature* pCreature)
{
    return new npc_taelan_fordringAI(pCreature);
}

bool QuestComplete_npc_taelan_fordring(Player* pPlayer, Creature* pQuestGiver, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SUBTERFUGE)
    {
        if (npc_taelan_fordringAI* pTaelan = dynamic_cast<npc_taelan_fordringAI*>(pQuestGiver->AI()))
        {
            pTaelan->QuestCompleted(pPlayer, pQuest);
            return true;
        }
    }
    return false;
}

// Script de Isillien
struct npc_isillienAI : public ScriptedAI
{
    npc_isillienAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        speechStep = 0;
        speechTimer = 0;
        Reset();
    }

    ObjectGuid _fordringGuid;
    uint32 speechStep;
    uint32 speechTimer;
    uint32 spellsTimer;

    void Reset()
    {
        spellsTimer = 5000;
    }

    void InformGuid(const ObjectGuid guid, uint32 type)
    {
        if (type == NPC_TAELAN_FORDRING)
            _fordringGuid = guid;
    }
    void MovementInform(uint32 movementType, uint32 moveId)
    {
        if (moveId == 1)
        {
            speechStep = 0;
            speechTimer = 1;
            m_creature->SaveHomePosition();
        }
    }
    void UpdateAI(uint32 uiDiff)
    {
        if (speechTimer && speechTimer <= uiDiff)
        {
            speechTimer = 0;
            ++speechStep;
            switch (speechStep - 1)
            {
                case 0:
                    m_creature->MonsterSay(ELYSIUM_TEXT(184));
                    speechTimer = 5000;
                    break;
                case 1:
                    m_creature->MonsterSay(ELYSIUM_TEXT(185));
                    speechTimer = 4000;
                    break;
                case 2:
                    m_creature->MonsterTextEmote(ELYSIUM_TEXT(186), NULL);
                    speechTimer = 2000;
                    break;
                case 3:
                {
                    // Les adds et Isillien attaquent Fordring.
                    Creature* fordring = m_creature->GetMap()->GetCreature(_fordringGuid);
                    if (!fordring)
                    {
                        sLog.elysium("[q5944] Fordring introuvable.");
                        break;
                    }
                    speechTimer = 10000;
                    std::list<Creature*> creaList;
                    m_creature->GetCreatureListWithEntryInGrid(creaList, NPC_ELITE_CRAMOISIE, 20.0f);
                    fordring->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                    for (std::list<Creature*>::iterator it = creaList.begin(); it != creaList.end(); ++it)
                    {
                        (*it)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        (*it)->AI()->AttackStart(fordring);
                        (*it)->SaveHomePosition();
                    }
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_UNK_16); // Ne pas être attaqué par les joueurs. Ce combat est un duel à mort.
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    AttackStart(fordring);
                    break;
                }
                case 4:
                {
                    m_creature->MonsterSay(ELYSIUM_TEXT(187));
                    m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                    speechTimer = 70000; // 1 min 10
                    break;
                }
                case 5:
                {
                    // On ne bouge plus.
                    m_creature->addUnitState(UNIT_STAT_ROOT);
                    m_creature->MonsterSay(ELYSIUM_TEXT(188));
                    speechTimer = 2000;
                    break;
                }
                case 6:
                {
                    // On tue Taelan.
                    if (Creature* taelanFordring = m_creature->GetMap()->GetCreature(_fordringGuid))
                        m_creature->DoKillUnit(taelanFordring);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    speechTimer = 3000;
                    break;
                }
                case 7:
                {
                    m_creature->MonsterTextEmote(ELYSIUM_TEXT(189), NULL);
                    m_creature->HandleEmote(EMOTE_STATE_LAUGH);
                    speechTimer = 1000;
                    break;
                }
                case 8:
                {
                    m_creature->clearUnitState(UNIT_STAT_ROOT);
                    EnterEvadeMode();
                    speechTimer = 3000;
                    break;
                }
                case 9:
                {
                    m_creature->MonsterSay(ELYSIUM_TEXT(190));
                    speechTimer = 5000;
                    break;
                }
                case 10:
                {
                    m_creature->MonsterTextEmote(ELYSIUM_TEXT(191), NULL);
                    Creature* tirionFordring = m_creature->SummonCreature(
                                                   NPC_TIRION_FORDRING,
                                                   2530.0f, -1934.9f, 94.18f, 0.0f,
                                                   TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000, true);
                    Creature* taelanFordring = m_creature->GetMap()->GetCreature(_fordringGuid);
                    if (!taelanFordring || !tirionFordring)
                    {
                        sLog.elysium("[q5944] Impossible d'invoquer %u (0x%x) ou de trouver le corps de %u (0x%x).",
                                        NPC_TIRION_FORDRING, tirionFordring, NPC_TAELAN_FORDRING, taelanFordring);
                        m_creature->DeleteLater();
                        return;
                    }
                    float x, y, z;
                    taelanFordring->GetContactPoint(tirionFordring, x, y, z);
                    tirionFordring->setFaction(taelanFordring->getFaction());
                    tirionFordring->AddAura(SPELL_TIRION_FORDRING_MOUNT);
                    tirionFordring->AI()->InformGuid(m_creature->GetObjectGuid(), NPC_ISILLIEN);
                    tirionFordring->AI()->InformGuid(_fordringGuid, NPC_TAELAN_FORDRING);
                    tirionFordring->GetMotionMaster()->MovePoint(1, x, y, z, MOVE_PATHFINDING);
                    break;
                }
                default:
                    ASSERT(false);
            }
        }
        else if (speechTimer)
            speechTimer -= uiDiff;

        if (speechStep == 5)
        {
            // Taelan Fordring est-il mort ?
            if (!m_creature->FindNearestCreature(NPC_TAELAN_FORDRING, 50.0f, true))
                speechTimer = 1;
            // Ou alors - triche infame - je suis en train de mourir. Mais cela ne doit pas se passer ainsi !
            if (m_creature->GetHealthPercent() < 10.0f)
                speechTimer = 1;
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (spellsTimer < uiDiff)
        {
            spellsTimer = urand(4000, 6000);
            uint32 spellId = 0;
            switch (urand(0, 2))
            {
                case 0:
                    spellId = SPELL_ISILLIEN_MANA_BURN;
                    break;
                case 1:
                    spellId = SPELL_ISILLIEN_MIND_BLAST;
                    break;
                case 2:
                    spellId = SPELL_ISILLIEN_MIND_FLAY;
                    break;
            }
            m_creature->CastSpell(m_creature->getVictim(), spellId, false);
            spellsTimer = 10000;
        }
        else
            spellsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* attacker, uint32& damage)
    {
        // Combat contre Taelan : les joueurs ne font aucun dégat.
        if (speechStep == 5 && attacker && attacker->IsPlayer())
            damage = 0;

        ScriptedAI::DamageTaken(attacker, damage);
    }
};

CreatureAI* GetAI_npc_isillien(Creature* pCreature)
{
    return new npc_isillienAI(pCreature);
}

// Tirion Fordring
struct npc_tirion_fordringAI : public ScriptedAI
{
    npc_tirion_fordringAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        speechStep = 0;
        speechTimer = 0;
        Reset();
    }

    ObjectGuid _fordringGuid;
    ObjectGuid _isillienGuid;
    uint32 speechStep;
    uint32 speechTimer;

    void Reset()
    {
        m_creature->SetWalk(false);
    }
    void InformGuid(ObjectGuid guid, uint32 type)
    {
        switch (type)
        {
            case NPC_ISILLIEN:
                _isillienGuid = guid;
                break;
            case NPC_TAELAN_FORDRING:
                _fordringGuid = guid;
                break;
        }
    }
    void MovementInform(uint32 movementType, uint32 moveId)
    {
        if (moveId == 1)
        {
            m_creature->SaveHomePosition();
            m_creature->SetWalk(true);
            speechStep = 0;
            speechTimer = 1;
        }
    }
    void KilledUnit(Unit* victim)
    {
    }
    void UpdateAI(uint32 uiDiff)
    {
        if (speechTimer && speechTimer <= uiDiff)
        {
            ++speechStep;
            switch (speechStep - 1)
            {
                case 0:
                {
                    m_creature->SetWalk(true);
                    m_creature->RemoveAurasDueToSpell(SPELL_TIRION_FORDRING_MOUNT);
                    m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                    speechTimer = 2000;
                    break;
                }
                case 1:
                {
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                    if (Creature* isillien = m_creature->GetMap()->GetCreature(_isillienGuid))
                        m_creature->SetFacingToObject(isillien);
                    m_creature->MonsterSay(ELYSIUM_TEXT(192));
                    speechTimer = 6000;
                    break;
                }
                case 2:
                {
                    if (Creature* isillien = m_creature->GetMap()->GetCreature(_isillienGuid))
                        isillien->MonsterSay(ELYSIUM_TEXT(193));
                    speechTimer = 6000;
                    break;
                }
                case 3:
                {
                    m_creature->MonsterSay(ELYSIUM_TEXT(194));
                    speechTimer = 4000;
                    break;
                }
                case 4:
                {
                    m_creature->MonsterSay(ELYSIUM_TEXT(195));
                    speechTimer = 3000;
                    break;
                }
                case 5:
                {
                    if (Creature* isillien = m_creature->GetMap()->GetCreature(_isillienGuid))
                    {
                        isillien->MonsterSay(ELYSIUM_TEXT(196));
                        isillien->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_UNK_16);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        isillien->AI()->AttackStart(m_creature);
                    }
                    else
                        sLog.elysium("[q5944] " __FILE__ ":%u Unable to find Isillien", __LINE__);
                    speechTimer = 0;
                    break;
                }
                case 6:
                {
                    EnterEvadeMode();
                    m_creature->SetWalk(true);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->MonsterSay(ELYSIUM_TEXT(197));
                    speechTimer = 10000;
                    break;
                }
                case 7:
                {
                    if (Creature* fordringSon = m_creature->GetMap()->GetCreature(_fordringGuid))
                        m_creature->SetFacingToObject(fordringSon);
                    m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                    m_creature->MonsterTextEmote(ELYSIUM_TEXT(198));
                    speechTimer = 6000;
                    break;
                }
                case 8:
                {
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                    m_creature->MonsterSay(ELYSIUM_TEXT(199));
                    speechTimer = 6000;
                    break;
                }
                case 9:
                {
                    if (Creature* fordringSon = m_creature->GetMap()->GetCreature(_fordringGuid))
                        m_creature->SetFacingToObject(fordringSon);
                    m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                    m_creature->MonsterTextEmote(ELYSIUM_TEXT(200));
                    speechTimer = 5000;
                    break;
                }
                case 10:
                {
                    m_creature->MonsterSay(ELYSIUM_TEXT(201));
                    speechTimer = 6000;
                    break;
                }
                case 11:
                {
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                    m_creature->MonsterSay(ELYSIUM_TEXT(202));
                    speechTimer = 8000;
                    break;
                }
                case 12:
                {
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                    m_creature->HandleEmote(TEXTEMOTE_PRAY);
                    m_creature->MonsterSay(ELYSIUM_TEXT(203));
                    speechTimer = 3000;
                    break;
                }
                case 13:
                {
                    // Quete terminee. Trouver le joueur pour la quete.
                    if (Creature* fordringSon = m_creature->GetMap()->GetCreature(_fordringGuid))
                        if (npc_taelan_fordringAI* ai = dynamic_cast<npc_taelan_fordringAI*>(fordringSon->AI()))
                            if (Player* escorter = ai->GetPlayerForEscort())
                                escorter->GroupEventHappens(QUEST_IN_DREAMS, fordringSon);

                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    speechTimer = 0;
                    break;
                }
                default:
                    ASSERT(false);
            }
        }
        else if (speechTimer)
            speechTimer -= uiDiff;

        // Isillien est-il mort ?
        if (speechStep == 6)
        {
            if (!m_creature->FindNearestCreature(NPC_ISILLIEN, 50.0f, true))
                speechTimer = 1;
        }

        if (!m_creature->HasAura(SPELL_AURA_DEVOTION))
            m_creature->CastSpell(m_creature, SPELL_AURA_DEVOTION, false);

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        DoMeleeAttackIfReady();
    }
    void AttackStart(Unit *target)
    {
        // Ne pas attaquer le daim infecte sur la route.
        if (target->GetEntry() != NPC_ISILLIEN)
            return;
        ScriptedAI::AttackStart(target);
    }
};

CreatureAI* GetAI_npc_tirion_fordring(Creature* pCreature)
{
    return new npc_tirion_fordringAI(pCreature);
}

/*######
## npc_HighProtectorLorik
######*/

enum
{
    NPC_HIGHPROTECTORLORIK   = 1846,
    SPELL_RETRIBUTIONAURA    = 8990,
    SPELL_ARCANEBLAST        = 10833,
    SPELL_DIVINESHIELD       = 13874,
    SPELL_HOLYLIGHT          = 15493,
    SPELL_SHIELDSLAM         = 15655
};

struct npc_highprotectorlorikAI : public ScriptedAI
{
    npc_highprotectorlorikAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiGlobalCooldown;
    uint32 m_uiDiff_Add;
    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiDivineShieldTimer;
    uint32 m_uiHolyLightTimer;
    uint32 m_uiShieldSlamTimer;

    void Reset()
    {
        m_uiGlobalCooldown          = 0;
        m_uiDiff_Add                = 0;
        m_uiArcaneBlastTimer        = 7000;
        m_uiDivineShieldTimer       = 2000;
        m_uiHolyLightTimer          = 2000;
        m_uiShieldSlamTimer         = 2000;
    }

    void UpdateAI(uint32 uiDiff)
    {
        if (!m_creature->HasAura(SPELL_RETRIBUTIONAURA))
            m_creature->CastSpell(m_creature, SPELL_RETRIBUTIONAURA, true);

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // GlobalCD non ecoule.
        if (m_uiGlobalCooldown > uiDiff)
            m_uiGlobalCooldown -= uiDiff;
        else
        {
            if (m_creature->IsNonMeleeSpellCasted(false))
                m_uiGlobalCooldown = 1;
            else
                m_uiGlobalCooldown = 0;
        }

        // DIVINESHIELD
        if (m_uiDivineShieldTimer < uiDiff)
        {
            if ((m_uiGlobalCooldown == 0) && (m_creature->GetHealthPercent() <= 15.0f))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DIVINESHIELD) == CAST_OK)
                {
                    m_uiDivineShieldTimer = 45000;
                    m_uiGlobalCooldown = 1000;
                }
            }
        }
        else
            m_uiDivineShieldTimer -= uiDiff;

        // ARCANE BLAST
        if (m_uiArcaneBlastTimer < uiDiff)
        {
            if (!m_uiGlobalCooldown)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANEBLAST) == CAST_OK)
                {
                    m_uiArcaneBlastTimer = urand(10000, 12000);
                    m_uiGlobalCooldown = 1000;
                }
            }
        }
        else
            m_uiArcaneBlastTimer -= uiDiff;

        // HOLYLIGHT
        if (m_uiHolyLightTimer < uiDiff)
        {
            if (!m_uiGlobalCooldown && m_creature->GetHealthPercent() <= 60.0f && m_creature->GetPower(POWER_MANA) > 700)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_HOLYLIGHT) == CAST_OK)
                {
                    m_uiHolyLightTimer = urand(2000, 6000);
                    m_uiGlobalCooldown = 1000;
                }
            }
        }
        else
            m_uiHolyLightTimer -= uiDiff;

        // SHIELDSLAM
        if (m_uiShieldSlamTimer < uiDiff)
        {
            if (!m_uiGlobalCooldown && m_creature->getVictim()->IsNonMeleeSpellCasted(false))
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHIELDSLAM) == CAST_OK)
                {
                    m_uiShieldSlamTimer = 9000;
                    m_uiGlobalCooldown = 1000;
                }
            }
        }
        else
            m_uiShieldSlamTimer -= uiDiff;

        DoMeleeAttackIfReady();

    }
};

CreatureAI* GetAI_npc_highprotectorlorik(Creature* pCreature)
{
    return new npc_highprotectorlorikAI(pCreature);
}


/*######
##
######*/

void AddSC_western_plaguelands()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npcs_dithers_and_arbington";
    newscript->pGossipHello = &GossipHello_npcs_dithers_and_arbington;
    newscript->pGossipSelect = &GossipSelect_npcs_dithers_and_arbington;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_myranda_the_hag";
    newscript->pGossipHello = &GossipHello_npc_myranda_the_hag;
    newscript->pGossipSelect = &GossipSelect_npc_myranda_the_hag;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_the_scourge_cauldron";
    newscript->GetAI = &GetAI_npc_the_scourge_cauldron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_andorhal_tower";
    newscript->GetAI = &GetAI_npc_andorhal_tower;
    newscript->RegisterSelf();

    // En reves
    newscript = new Script;
    newscript->Name = "npc_taelan_fordring";
    newscript->GetAI = &GetAI_npc_taelan_fordring;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_taelan_fordring;
    //newscript->pQuestComplete = &QuestComplete_npc_taelan_fordring;
    newscript->pQuestRewardedNPC = &QuestComplete_npc_taelan_fordring;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_isillien";
    newscript->GetAI = &GetAI_npc_isillien;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tirion_fordring_in_dreams";
    newscript->GetAI = &GetAI_npc_tirion_fordring;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_highprotectorlorik";
    newscript->GetAI = &GetAI_npc_highprotectorlorik;
    newscript->RegisterSelf();
}
