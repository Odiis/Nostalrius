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
SDName: Winterspring
SD%Complete: 90
SDComment: Quest support: 5126 (Loraxs' tale missing proper gossip items text). Vendor Rivern Frostwind. Obtain Cache of Mau'ari
SDCategory: Winterspring
EndScriptData */

/* ContentData
npc_lorax
npc_rivern_frostwind
npc_witch_doctor_mauari
EndContentData */

#include "scriptPCH.h"

/*######
## npc_lorax
######*/

bool GossipHello_npc_lorax(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(5126) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Talk to me", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_lorax(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What do you do here?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(3759, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I can help you", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(3760, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What deal?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(3761, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Then what happened?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(3762, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "He is not safe, i'll make sure of that.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(3763, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(5126);
            break;
    }
    return true;
}

/*######
## npc_rivern_frostwind
######*/

bool GossipHello_npc_rivern_frostwind(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isVendor() && pPlayer->GetReputationRank(589) >= REP_FRIENDLY)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_rivern_frostwind(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*######
## npc_witch_doctor_mauari
######*/

bool GossipHello_npc_witch_doctor_mauari(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestRewardStatus(975))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I'd like you to make me a new Cache of Mau'ari please.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(3377, pCreature->GetGUID());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(3375, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_witch_doctor_mauari(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, 16351, false);
    }

    return true;
}

// Elysium - quete épique chasseur

enum
{
    DEMONIC_FRENZY          = 23257,
    DEMONIC_DOOM            = 23298,
    STINGING_TRAUMA         = 23299,
    DESESPOIR_IDIOT         = 23503,
    FOOL_PLIGHT             = 23504,

    HUNTER_QUEST_DESPAWN_TIMER    = 2400000, //Shall despawn after 40 minutes if turned into a demon
    HUNTER_QUEST_COMBAT_TIMER     = 1200000, //Combat shall not last more than 20 minutes

    NPC_CLEANER             = 14503,
    NPC_ARTORIUS            = 14531,
    NPC_ARTORIUS_EVIL       = 14535
};

struct mob_ArtoriusAI : public ScriptedAI
{
    mob_ArtoriusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        EventBegin = false;
        EventEnd = true;
        BoredOrInterfere = false;
        CombatTimer = 10000;
        _combatDespawnTimer = HUNTER_QUEST_COMBAT_TIMER;
        _despawnTimer       = HUNTER_QUEST_DESPAWN_TIMER;

        Reset();
    }

    bool isEngaged;
    bool EventBegin;
    bool EventEnd;
    bool BoredOrInterfere;
    uint32 CombatTimer;
    uint32 FrenezyTimer;
    uint32 DespawnTimer;
    uint64 PlayerGuid;
    uint32 CheckBug_Timer;
    uint32 CleanerTimer;
    uint32 _despawnTimer;
    uint32 _combatDespawnTimer;

    void Reset()
    {
        isEngaged = false;
        //EventBegin = false;
        FrenezyTimer = 15000;
        CheckBug_Timer = 0;
        CleanerTimer = 0;
        PlayerGuid = 0;

        if (BoredOrInterfere || EventEnd)
        {
            EventBegin = false;
            EventEnd = false;
            BoredOrInterfere = false;
            CombatTimer = 10000;
            m_creature->SetEntry(NPC_ARTORIUS);
            m_creature->UpdateEntry(NPC_ARTORIUS);
            m_creature->SetHomePosition(7909.71f, -4598.67f, 710.008f, 0.606013f);
            m_creature->NearTeleportTo(7909.71f, -4598.67f, 710.008f, 0.606013f);
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
            m_creature->GetMotionMaster()->MoveWaypoint();
            _despawnTimer     = HUNTER_QUEST_DESPAWN_TIMER;
            m_creature->SaveToDB();
        }
        else if (EventBegin)
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->SetDefaultMovementType(IDLE_MOTION_TYPE);
            m_creature->GetMotionMaster()->MoveIdle();
             if (_despawnTimer < 1000)
             {
                 m_creature->SetRespawnDelay(10000);
                 m_creature->DisappearAndDie();
                 EventEnd = true;
             }
        }
    }

    void Aggro(Unit* pWho)
    {
         /** Combat shall not last more than 20 minutes */
         if(!isEngaged)
         {
             isEngaged     = true;
             _combatDespawnTimer = HUNTER_QUEST_DESPAWN_TIMER;
         }
    }

    void JustDied(Unit* pKiller)
    {
        //m_creature->SetHomePosition(7909.71f, -4598.67f, 710.01f, 0.91f);
        //m_creature->SaveToDB();
        if (!BoredOrInterfere)
            EventEnd = true;
    }

    void JustReachedHome()
    {
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == 13555 || pSpell->Id == 25295) // Morsure de serpent rang 8 et 9
        {
            m_creature->CastSpell(m_creature, STINGING_TRAUMA, true);
            if (m_creature->HasAura(DEMONIC_FRENZY))
                m_creature->RemoveAurasDueToSpell(DEMONIC_FRENZY);
        }
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (EventBegin == false)
            m_creature->CastSpell(done_by, DESESPOIR_IDIOT, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (EventBegin == false)
            return;
         else
         {
             if (_despawnTimer <= uiDiff)
             {
                 if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                 {
                     m_creature->SetRespawnDelay(10000);
                     m_creature->DisappearAndDie();
                     EventEnd = true;
                 }
            }
             else
                 _despawnTimer -= uiDiff;
          }

        if (CombatTimer <= uiDiff && CombatTimer != 0)
        {
            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(PlayerGuid))
            {
                CombatTimer = 0;
                m_creature->SetEntry(NPC_ARTORIUS_EVIL);
                m_creature->UpdateEntry(NPC_ARTORIUS_EVIL);
                //m_creature->AddThreat(pPlayer);
            }
            else
            {
                m_creature->MonsterYell("Joueur introuvable, reset.", 0);
                Reset();
            }
        }
        else if (CombatTimer != 0)
        {
            CombatTimer -= uiDiff;
            return;
        }


        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_creature->GetHealthPercent() < 100.0f)
            {
                // En cas de bug pathfinding
                CheckBug_Timer += uiDiff;
                if (CheckBug_Timer > 5000)
                {
                    EnterEvadeMode();
                    m_creature->CombatStop();
                    m_creature->SetHealth(m_creature->GetMaxHealth());
                }
            }
            return;
        }
        else
            CheckBug_Timer = 0;

          /** If combat last for too long, force creature respawn */
          if (_combatDespawnTimer < uiDiff)
          {
              if(isEngaged)
              {
                  BoredOrInterfere = true;
                  m_creature->SetRespawnDelay(10000);
                  m_creature->DisappearAndDie();
              }
          }
          else
              _combatDespawnTimer -= uiDiff;

        if (m_creature->getThreatManager().getThreatList().size() > 1 && !m_creature->FindNearestCreature(NPC_CLEANER, 150.0f))
        {
            if (Creature* Crea = m_creature->SummonCreature(NPC_CLEANER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000))
            {
                Crea->AddThreatsOf(m_creature);
                //Crea->MonsterYell("Vous osez gener le combat de cette creature ? La bataille doit etre menee seule ! Vous allez tous payer pour cette intervention !", 0);
                Crea->MonsterYell(ELYSIUM_TEXT(206), 0);

                //m_creature->MonsterSay("Seul un idiot resterait dans la bataille. Adieu, trouillard !", 0);
                m_creature->MonsterSay(ELYSIUM_TEXT(207), 0);
                //m_creature->SetRespawnDelay(25200); // Ustaag : prochain respawn fix a 7h 25200
                m_creature->CastSpellOnNearestVictim(FOOL_PLIGHT, 0.0f, 20.0f, false);
                BoredOrInterfere = true;
            }
        }

        if (Creature* Crea = m_creature->FindNearestCreature(NPC_CLEANER, 50.0f))
        {
            CleanerTimer += uiDiff;
            if (CleanerTimer > 2000)
                m_creature->DisappearAndDie();
            return;
        }
        else
            CleanerTimer = 0;

        if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 30.0f) && !m_creature->getVictim()->HasAura(DEMONIC_DOOM))
        {
            if (m_creature->HasAura(DEMONIC_FRENZY))
            {
                int demonicDoomCustom = 87; // dégâts finaux du spell en fonction du lvl du caster.. fix dégats initiaux pour coller a 350 degats max
                m_creature->CastCustomSpell(m_creature->getVictim(), DEMONIC_DOOM, &demonicDoomCustom, NULL, NULL, false);
            }
            else
                DoCastSpellIfCan(m_creature->getVictim(), DEMONIC_DOOM, CAST_AURA_NOT_PRESENT);
        }

        if (!m_creature->HasAura(13555) && !m_creature->HasAura(25295)) // Morsure de serpent rang 8 et 9
        {
            if (FrenezyTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, DEMONIC_FRENZY, CAST_AURA_NOT_PRESENT) == CAST_OK)
                    FrenezyTimer = 30000;
            }
            else
                FrenezyTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_mob_Artorius(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(7636) == QUEST_STATUS_INCOMPLETE && pPlayer->getClass() == CLASS_HUNTER)
        pPlayer->ADD_GOSSIP_ITEM(0, ELYSIUM_TEXT(208), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        //pPlayer->ADD_GOSSIP_ITEM(0, "Affronte moi, demon !", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(7045, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_mob_Artorius(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;

    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
        {
            if (mob_ArtoriusAI* pArtoriusEventAI = dynamic_cast<mob_ArtoriusAI*>(pCreature->AI()))
            {
                pArtoriusEventAI->EventBegin = true;
                pArtoriusEventAI->PlayerGuid = pPlayer->GetGUID();
                //pCreature->MonsterSay("Comme vous voudrez, Chasseur.", 0, 0);
                pCreature->MonsterSay(ELYSIUM_TEXT(209),0,0);
                pCreature->SetHomePosition(pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), pCreature->GetOrientation());
                pCreature->GetMotionMaster()->Clear(false);
                pCreature->SetDefaultMovementType(IDLE_MOTION_TYPE);
                pCreature->GetMotionMaster()->MoveIdle();
                pCreature->SaveToDB();
            }
            pPlayer->CLOSE_GOSSIP_MENU();
        }
        break;
    }
    return true;
}

CreatureAI* GetAI_mob_Artorius(Creature* pCreature)
{
    return new mob_ArtoriusAI(pCreature);
}

/*####
# npc_ranshalla (10300)
# Gardians of the altar (4901)
####*/
//Alita---

enum
{
    RANSHALLA_BEGIN         = -1780149,// "Remember, I need your help to properly channel.I will ask you to aid me sevral times on our path, so please be ready."
    RANSHALLA_BLUE          = -1780150,// "This blue light... It's strange. What do you think it means?"
    RANSHALLA_TORCH_ONE     = -1780151,// "Ok, let's get going!"
    RANSHALLA_TORCH_TWO     = -1780152,// "We must continue now."
    RANSHALLA_TORCH_THREE   = -1780153,// "Let's find the next one."
    RANSHALLA_ALTAR_ONE     = -1780154,// "It is time for the final step; we must activate the altar."
    RANSHALLA_ALTAR_TWO     = -1780155,// "I will read the word carved into the stone, and you will find a way to light it."

    RANSHALLA_ALTAR_ACTIVE  = -1780157,// "The altar is glowing! We have done it!"
    RANSHALLA_EVENT_1       = -1780158,// "What is happening? Look!"
    LEFT_PRIESTESS_1        = -1780159,// "It has been many years..."
    RIGHT_PRIESTESS_1       = -1780160,// "Who has disturbed the altar of the goddess?"
    RANSHALLA_EVENT_2       = -1780161,// "Please, priestesses, forgive us for our intrusion. We do not wish any harm here."
    RANSHALLA_EVENT_3       = -1780162,// "We only wish to know why the wildkin guard this area..."
    LEFT_PRIESTESS_2        = -1780163,// "Enu thora'serador. This is a sacred place."
    LEFT_PRIESTESS_3        = -1780164,// "We will show you..."
    RIGHT_PRIESTESS_2       = -1780165,// "Look above you; thara dormil dorah..."
    RIGHT_PRIESTESS_3       = -1780166,// "This gem once allowed direct communication with Elune, herself."

    RIGHT_PRIESTESS_4       = -1780167,// "Through the gem, Elune channeled her infinite wisdom..."
    RIGHT_PRIESTESS_5       = -1780168,// "Realizing that the gem needed to be protected, we turned to the goddess herself."
    RIGHT_PRIESTESS_6       = -1780169,// "Soon after, we began to have visions of a creature... A creature with the feathers of an owl, but the will and might of a bear..."
    LEFT_PRIESTESS_4        = -1780170,// "It was on that day that the wildkin were given to us. Fierce guardians, the goddess assigned the wildkin to protect all of her sacred places."
    VOICE_OF_ELUNE          = -1780171,// "Anu'dorini Talah, Ru shallora enudoril."
    LEFT_PRIESTESS_5        = -1780172,// "But now, many years later, the wildkin have grown more feral, and without the guidance of the goddess, they are confused..."
    LEFT_PRIESTESS_6        = -1780173,// "Without a purpose, they wander... But many find their way back to the sacred areas that they once were sworn to protect."
    LEFT_PRIESTESS_7        = -1780174,// "Wildkin are inherently magical; this power was bestowed upon them by the goddess."
    RIGHT_PRIESTESS_7       = -1780175,// "Know that wherever you might find them in the world, they are protecting something of importance, as they were entrusted to do so long ago."
    RIGHT_PRIESTESS_8       = -1780176,// "Please, remember what we have shown you..."
    LEFT_PRIESTESS_8        = -1780178,// "Farewell."
    RANSHALLA_EVENT_4       = -1780179,// "Thank you for you help, $N. I wish you well in your adventures."
    RANSHALLA_EVENT_5       = -1780180,// "I want to stay here and reflect on what we have seen. Please see Erelas and tell him what we have learned."

    RANSHALLA_LIGHT_TORCH   = -1780181,// "This is the place. Let's light it."
    RANSHALLA_LIGHT_ALTAR   = -1780182,// "This is the place. Let's light it."
    RANSHALLA_CHANTING      = -1780183// "Ranshalla begins chanting a strange spell."
};

struct Locations
{
    float x, y, z;
};
//pretresse gauche
static Locations xyzp1[] =
{
    {5496.097168, -4919.254395, 850.485291},  //0: point de pop
    {5499.214844, -4920.547852, 849.447754},
    {5511.166992, -4920.576172, 846.266663},   //2: à coté de l'altar
    {5515.273926, -4921.795898, 845.202515}
};
//pretresse droite
static Locations xyzp2[] =
{
    {5514.291992, -4898.066406, 847.032471},
    {5515.654297, -4900.294922, 846.531982},
    {5517.178223, -4912.854492, 845.877625},
    {5519.874023, -4919.447754, 844.709473}
};
//chouettard
static Locations xyzm[] =
{
    {5497.744629, -4901.567871, 850.968018},
    {5511.966797, -4915.375000, 846.856689}
};



struct npc_ranshallaAI : public npc_escortAI
{
    npc_ranshallaAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 timer;
    bool wpInvoqueAtteint; //pour démarrer le timer de texte.
    bool pretressesInvoque; //pour ne pas en invoquer une infinité ^^'
    bool pretressesRepartent;

    uint64 guidPriestess1;
    uint64 guidPriestess2;
    uint64 guidMoonkin;
    uint64 guidVoice;

    void WaitingForTorch()
    {
        if (GameObject* pGo = m_creature->FindNearestGameObject(177417, 30.000000))
        {
            SetEscortPaused(1); //STATE_ESCORT_PAUSED
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);

            //if(DoCastSpellIfCan(m_creature, 18955)) //Ranshalla's Torch Trap: 18955  DoCastSpellIfCan(Unit*, uint32, uint32, ObjectGuid) //incant sort DoCastSpellIfCan(m_creature->getVictim(), TRAIT_FOUDRE
            //{
            //    SetEscortPaused(0);
            //}
        }

    }
    void WaitingForAltar()
    {
        if (GameObject* pGo = m_creature->FindNearestGameObject(177404, 30.000000))
        {
            SetEscortPaused(1); //STATE_ESCORT_PAUSED
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);

            //if(DoCastSpellIfCan(m_creature, 18993)) //Ranshalla's Altar Trap: 18955
            //{
            //   SetEscortPaused(0);
            //}
        }

    }
    void PopLights()
    {

// GameObject* WorldObject::SummonGameObject(uint32 entry, float x, float y, float z, float ang, float rotation0, float rotation1, float rotation2, float rotation3, uint32 respawnTime)
        m_creature->SummonGameObject(177415, 5514.490234, -4917.569824, 846, 0, -0.240, 0, 0, 0, 120);//1,-0.239//1.1,-0.240 trop.//lumière centrale
        m_creature->SummonGameObject(177415, 5517.699219, -4923.396484, 845, 0, 0, 0, 0, 0, 120);//devant.
        m_creature->SummonGameObject(177415, 5535.837891, -4914.595215, 841, 0, -0.1, 0.3, 0, 0, 120);//devant à coté rocher
        m_creature->SummonGameObject(177415, 5507.489258, -4921.728516, 847, 0, 0, 0, 0, 0, 120);//gauche
        m_creature->SummonGameObject(177415, 5506.629883, -4908.958496, 849, 0, 0, 0, 0, 0, 120);//derrière
        m_creature->SummonGameObject(177415, 5500.777344, -4920.617676, 849, 0, -0.1, -0.2, 0, 0, 120);//poutres gauche
        m_creature->SummonGameObject(177415, 5516.331055, -4902.605469, 847, 0, -0.1, -0.3, 0, 0, 120);//poutres droite
        //m_creature->SummonGameObject(177415, 5515.588379, -4921.833496, 848, 0, -0.237, 0, 0, 0, 20000);// 00300 horizontal NS//00-0.700 horiz SN
        //00070 vertical très fin//00.7000 horizOE//0-0.235000 casi vertical OE, -0.239 disparait, -0.237 ~vertical
        //

    }

    void PopGem()
    {
        m_creature->SummonGameObject(177414, 5514.490234, -4917.569824, 852, 0, 0, 0, 0, 0, 90); //gemme
        m_creature->SummonGameObject(177486, 5514.490234, -4917.569824, 845, 0, 0, 0, 0, 0, 90); //lumière bleue
        Creature* pVoice = m_creature->SummonCreature(12152, 5514.890137, -4918.169922, 845.538025, 5.3, TEMPSUMMON_TIMED_DESPAWN, 90000); //voix d'elune
        guidVoice = pVoice->GetGUID();
    }

    bool PopPrietesses()
    {
        bool invoked = false;
        //Creature* SummonCreature(uint32 id, float x, float y, float z, float ang,TempSummonType spwtype = TEMPSUMMON_DEAD_DESPAWN,uint32 despwtime = 25000, bool asActiveObject = false);
        Creature* pPriestess1 = m_creature->SummonCreature(12116, 5496.097168, -4919.254395, 850.485291, 6.01, TEMPSUMMON_TIMED_DESPAWN, 240000); //dépop manuel à 2min normalement
        Creature* pPriestess2 = m_creature->SummonCreature(12116, 5514.291992, -4898.066406, 847.032471, 5.3, TEMPSUMMON_TIMED_DESPAWN, 240000);
        //pPriestess1 = m_creature->SummonCreature(12116, cop1[0].xyz[0],cop1[0].xyz[1], cop1[0].xyz[2], 6.01, TEMPSUMMON_TIMED_DESPAWN, 30000);

        if (pPriestess1 && pPriestess2)
        {
            guidPriestess1 = pPriestess1->GetGUID();
            guidPriestess2 = pPriestess2->GetGUID();

            if (guidPriestess1 && guidPriestess2)
            {
                invoked = true;
                pPriestess1->SetWalk(true);
                pPriestess2->SetWalk(true);
                pPriestess1->GetMotionMaster()->MovePoint(1, 5499.214844, -4920.547852, 849.447754);
                pPriestess2->GetMotionMaster()->MovePoint(1, 5515.654297, -4900.294922, 846.531982);
                //m_creature->MonsterSay("The priestesses have been invoked.");//test
            }
            else
                m_creature->MonsterSay("Navré, les prêtresses n'en font qu'a leur tête...");
        }
        else
        {
            wpInvoqueAtteint = 0;
            pretressesInvoque = 0;
            m_creature->MonsterSay("Navré, les pretresses ne veulent pas pop...");
        }

        return invoked;
    }

    void PopMoonkin()
    {
        Creature* pMoonkin = m_creature->SummonCreature(12140, xyzm[0].x, xyzm[0].y, xyzm[0].z, 6.01, TEMPSUMMON_TIMED_DESPAWN, 240000);
        pMoonkin->SetWalk(true);
        pMoonkin->GetMotionMaster()->MovePoint(1, xyzm[1].x, xyzm[1].y, xyzm[1].z);
        guidMoonkin = pMoonkin->GetGUID();
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId)
    {
        if (!(uiType != POINT_MOTION_TYPE || pSummoned->GetEntry() != 12116))
        {

            int id = 1;
            int maxid = 3;
            //m_creature->MonsterSay("Dans SummonedMovementInform");//test

            if (pretressesRepartent == 1)
            {
                if (uiPointId > 0 && uiPointId < 4)
                {
                    if (pSummoned->GetGUID() == guidPriestess1)
                        pSummoned->GetMotionMaster()->MovePoint(uiPointId - 1, xyzp1[uiPointId - 1].x, xyzp1[uiPointId - 1].y, xyzp1[uiPointId - 1].z);
                    else if (pSummoned->GetGUID() == guidPriestess2)
                        pSummoned->GetMotionMaster()->MovePoint(uiPointId - 1, xyzp2[uiPointId - 1].x, xyzp2[uiPointId - 1].y, xyzp2[uiPointId - 1].z);
                }
            }
            else
            {
                if (uiPointId == 1)
                {
                    if (pSummoned->GetGUID() == guidPriestess1)
                        pSummoned->GetMotionMaster()->MovePoint(uiPointId + 1, xyzp1[uiPointId + 1].x, xyzp1[uiPointId + 1].y, xyzp1[uiPointId + 1].z);
                    else if (pSummoned->GetGUID() == guidPriestess2)
                        pSummoned->GetMotionMaster()->MovePoint(uiPointId + 1, xyzp2[uiPointId + 1].x, xyzp2[uiPointId + 1].y, xyzp2[uiPointId + 1].z);
                }
            }
        }
    }

    void WaypointReached(uint32 i)
    {
        //altar = 49182;
        /*
        if(i<35)
            m_creature->SetWalk(false); // for tests :D
        else
            m_creature->SetWalk(true);//otherwise the event ends early.
        */
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch (i)
        {
            case 0:
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                if (pretressesRepartent != 0 ||  wpInvoqueAtteint != 0 || guidPriestess1 != 0 || guidPriestess2 != 0 ||  guidMoonkin != 0 ||  guidVoice != 0 || pretressesInvoque != 0)
                    m_creature->MonsterSay("WTF values have not been reset properly !");
                DoScriptText(RANSHALLA_BEGIN, m_creature, pPlayer);
                break;
            case 2:
                DoScriptText(RANSHALLA_BLUE, m_creature);
                break;
            case 7:
                DoScriptText(RANSHALLA_LIGHT_TORCH, m_creature);
                WaitingForTorch();
                break;
            case 13:
                DoScriptText(RANSHALLA_LIGHT_TORCH, m_creature);
                WaitingForTorch();
                break;
            case 18:
                DoScriptText(RANSHALLA_LIGHT_TORCH, m_creature);
                WaitingForTorch();
                break;
            case 22:
                DoScriptText(RANSHALLA_LIGHT_TORCH, m_creature);
                WaitingForTorch();
                break;
            case 28:
                DoScriptText(RANSHALLA_LIGHT_TORCH, m_creature);
                WaitingForTorch();
                break;
            case 31:
                DoScriptText(RANSHALLA_ALTAR_ONE, m_creature);
                break;
            case 32:
                DoScriptText(RANSHALLA_ALTAR_TWO, m_creature);
                break;
            case 33:
                DoScriptText(RANSHALLA_LIGHT_ALTAR, m_creature);
                m_creature->SetFacingTo(2.336624);
                WaitingForAltar();
                break;
            case 35:
                m_creature->SetFacingTo(5.557543);
                timer = 120000; //eum :tousse:
                wpInvoqueAtteint = 1;
                DoScriptText(RANSHALLA_EVENT_1, m_creature);
                PopLights();
                break;
            case 36:
                DoScriptText(RANSHALLA_EVENT_2, m_creature);
                break;
            case 37:
                DoScriptText(RANSHALLA_EVENT_3, m_creature);
                break;
            case 39:
                m_creature->SetFacingTo(2.336624);
                break;
            case 40:
                DoScriptText(RANSHALLA_EVENT_4, m_creature, pPlayer);
                break;
            case 41:
                DoScriptText(RANSHALLA_EVENT_5, m_creature);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                pPlayer->GroupEventHappens(4901, m_creature);  //QUEST_GUARDIANS_OF_THE_ALTAR
                break;
        }
    }

// pretresses sont pop pile 2MIN
    void UpdateAI(const uint32 uiDiff)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (wpInvoqueAtteint == 1)
            {
                if (pretressesInvoque == 0)
                {
                    //PopPrietesses(pPriestess1, pPriestess2);//1=left 2=right
                    //1=left 2=right
                    pretressesInvoque = PopPrietesses();
                    //m_creature->MonsterSay("tried to invoque priestesses.");//test
                }
                else
                {
                    if (timer < uiDiff)
                    {

                        //DoScriptText(LEFT_PRIESTESS_1, m_creature);//test OK
                        //DoScriptText(LEFT_PRIESTESS_1, pPriestess1);//test OK
                        if (Creature* pPriestess1 = m_creature->GetMap()->GetCreature(guidPriestess1))
                            pPriestess1->AddObjectToRemoveList();
                        if (Creature* pPriestess2 = m_creature->GetMap()->GetCreature(guidPriestess2))
                            pPriestess2->AddObjectToRemoveList();
                        wpInvoqueAtteint = 0;
                        pretressesInvoque = 0;
                        pretressesRepartent = 0;
                    }
                    else
                    {
                        //if(guidPriestess1!=0 && guidPriestess2 !=0)
                        //    m_creature->MonsterSay("guids pretresses non nulls.");//test
                        Creature* pPriestess1 = m_creature->GetMap()->GetCreature(guidPriestess1);
                        Creature* pPriestess2 = m_creature->GetMap()->GetCreature(guidPriestess2);
                        //if (Priestess1 = m_creature->GetMap()->GetCreature(guidPriestess1) && Creature* Priestess2 = m_creature->GetMap()->GetCreature(guidPriestess2))
                        if (pPriestess1  && pPriestess2)
                        {
                            if (pPriestess1->GetEntry() == 12116 && pPriestess2->GetEntry() == 12116)
                            {
                                if (timer < 115000 + uiDiff && timer >= 115000) //car sinon bah on sait ^^ pas très efficace tout ca...
                                    DoScriptText(RIGHT_PRIESTESS_1, pPriestess2);//13.20
                                else if (timer < 114000 + uiDiff && timer >= 114000) //13.21
                                    DoScriptText(LEFT_PRIESTESS_1, pPriestess1);  // 120 115 114 110 107(13.28) 102 98(13.37) 88 80(13.55)
                                else if (timer < 104000 + uiDiff && timer >= 104000) //13.25 // 75 71(14.04) 66 46(14.29) 37 34(14.41) 26 22 |17 17
                                    DoScriptText(LEFT_PRIESTESS_2, pPriestess1); // 15(15.01) |5 //ancien11000
                                else if (timer < 100000 + uiDiff && timer >= 100000) //13.28   //ancien107000
                                    DoScriptText(LEFT_PRIESTESS_3, pPriestess1);
                                else if (timer < 93000 + uiDiff && timer >= 93000)
                                    PopGem();
                                else if (timer < 90000 + uiDiff && timer >= 90000) //ancien88000
                                    DoScriptText(RIGHT_PRIESTESS_2, pPriestess2);
                                else if (timer < 83000 + uiDiff && timer >= 83000)
                                    pPriestess2->GetMotionMaster()->MovePoint(3, xyzp2[3].x, xyzp2[3].y, xyzp2[3].z);
                                else if (timer < 80000 + uiDiff && timer >= 80000)
                                    DoScriptText(RIGHT_PRIESTESS_3, pPriestess2);
                                else if (timer < 75000 + uiDiff && timer >= 75000)
                                    DoScriptText(RIGHT_PRIESTESS_4, pPriestess2);
                                else if (timer < 71000 + uiDiff && timer >= 71000)
                                    DoScriptText(RIGHT_PRIESTESS_5, pPriestess2);
                                else if (timer < 66000 + uiDiff && timer >= 66000)
                                    DoScriptText(RIGHT_PRIESTESS_6, pPriestess2);
                                else if (timer < 57000 + uiDiff && timer >= 57000)
                                    PopMoonkin();
                                else if (timer < 46000 + uiDiff && timer >= 46000)
                                {
                                    DoScriptText(LEFT_PRIESTESS_4, pPriestess1);
                                    if (Creature* pVoice = m_creature->GetMap()->GetCreature(guidVoice))
                                        DoScriptText(VOICE_OF_ELUNE, pVoice);//voice of elune
                                }
                                else if (timer < 40000 + uiDiff && timer >= 40000)
                                    pPriestess1->GetMotionMaster()->MovePoint(3, xyzp1[3].x, xyzp1[3].y, xyzp1[3].z);
                                else if (timer < 37000 + uiDiff && timer >= 37000)
                                    DoScriptText(LEFT_PRIESTESS_5, pPriestess1);
                                else if (timer < 34000 + uiDiff && timer >= 34000)
                                    DoScriptText(LEFT_PRIESTESS_6, pPriestess1);
                                else if (timer < 26000 + uiDiff && timer >= 26000)
                                    DoScriptText(LEFT_PRIESTESS_7, pPriestess1);
                                else if (timer < 22000 + uiDiff && timer >= 22000)
                                    DoScriptText(RIGHT_PRIESTESS_7, pPriestess2);
                                else if (timer < 20000 + uiDiff && timer >= 20000)
                                {
                                    if (Creature* pMoonkin = m_creature->GetMap()->GetCreature(guidMoonkin))
                                        pMoonkin->GetMotionMaster()->MovePoint(0, xyzm[0].x, xyzm[0].y, xyzm[0].z);
                                }
                                else if (timer < 17000 + uiDiff && timer >= 17000)
                                {
                                    DoScriptText(RIGHT_PRIESTESS_8, pPriestess2);
                                    pretressesRepartent = 1;
                                    pPriestess1->GetMotionMaster()->MovePoint(2, xyzp1[2].x, xyzp1[2].y, xyzp1[2].z);
                                    pPriestess2->GetMotionMaster()->MovePoint(2, xyzp2[2].x, xyzp2[2].y, xyzp2[2].z);
                                }
                                else if (timer < 15000 + uiDiff && timer >= 15000)
                                    DoScriptText(LEFT_PRIESTESS_8, pPriestess1);
                                else if (timer < 10000 + uiDiff && timer >= 10000)
                                {
                                    if (Creature* pMoonkin = m_creature->GetMap()->GetCreature(guidMoonkin))
                                        pMoonkin->AddObjectToRemoveList();
                                }
                            }
                        }
                        timer -= uiDiff;
                    }
                }
            }
        }
        npc_escortAI::UpdateAI(uiDiff);
    }

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            wpInvoqueAtteint = 0;
            pretressesInvoque = 0;
            //m_creature->setFaction(FACTION_ESCORT_A_NEUTRAL_PASSIVE);
            guidPriestess1 = 0;
            guidPriestess2 = 0;
            guidMoonkin = 0;
            guidVoice = 0;
            pretressesRepartent = 0;
        }
    }
};

bool QuestAccept_npc_ranshalla(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == 4901)  //QUEST_GUARDIANS_OF_THE_ALTAR
    {
        if (npc_ranshallaAI* pEscortAI = dynamic_cast<npc_ranshallaAI*>(pCreature->AI()))
        {
            //void npc_escortAI::Start(bool bIsActiveAttacker, bool bRun, uint64 uiPlayerGUID, const Quest* pQuest, bool bInstantRespawn, bool bCanLoopPath)
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_ranshalla(Creature* pCreature)
{
    return new npc_ranshallaAI(pCreature);
}


bool GOHello_go_fire_of_elune(Player* pPlayer, GameObject* pGo)
{
    pGo->UseDoorOrButton(12, false); // YEAH YEAH YEAH (false = alume, true = alume pas. dans les 2cas la torche n'est pas cliqueable pendant 12sec! :D)
    //if (pPlayer->GetQuestStatus(4901) == QUEST_STATUS_COMPLETE)// devrait etre QUEST_STATUS_INCOMPLETE mais avec l'auto complete -_-.
    //{// retiré car pas reclikable, dommage de faire planter toute la quete pour ca non?

    if (Creature* pCreature = pGo->FindNearestCreature(10300, 30.000000, true))
    {
        switch (urand(0, 2))
        {
            case 0:
                DoScriptText(RANSHALLA_TORCH_ONE, pCreature);
                break;
            case 1:
                DoScriptText(RANSHALLA_TORCH_TWO, pCreature);
                break;
            case 2:
                DoScriptText(RANSHALLA_TORCH_THREE, pCreature);
                break;
        }

        if (npc_ranshallaAI* pEscortAI = dynamic_cast<npc_ranshallaAI*>(pCreature->AI()))
            pEscortAI->SetEscortPaused(0);
    }
    //}
    return true;
}
bool GOHello_go_altar_of_elune(Player* pPlayer, GameObject* pGo)
{
    pGo->UseDoorOrButton(130, false); // YEAH YEAH YEAH (false = alume, true = alume pas. dans les 2cas la torche n'est pas cliqueable pendant 12sec! :D)
    // if (pPlayer->GetQuestStatus(4901) == QUEST_STATUS_COMPLETE)// devrait etre QUEST_STATUS_INCOMPLETE mais avec l'auto complete -_-.
    // {//retiré car pas reclikable, dommage de faire planter toute la quete pour ca(un conard qui passe non?

    if (Creature* pCreature = pGo->FindNearestCreature(10300, 30.000000, true))
    {
        DoScriptText(RANSHALLA_ALTAR_ACTIVE, pCreature);

        if (npc_ranshallaAI* pEscortAI = dynamic_cast<npc_ranshallaAI*>(pCreature->AI()))
            pEscortAI->SetEscortPaused(0);
    }
    // }
    return true;
}
//---

enum
{
    AREA_UNGORO                 = 541,
    AREA_TANARIS                = 976,
    AREA_WINTERSPRING           = 2255,

    NPC_SPRINKLE                = 7583,
    NPC_QUIXXIL                 = 10977,
    NPC_LEGACKI                 = 10978,

    SPELL_AUTO_FEAR             = 31365,
};

struct npc_umi_yetiAI : public ScriptedAI
{
    enum TargetType
    {
        TARGET_QUIXXIL = 0,
        TARGET_LEGACKI,
        TARGET_SPRINKLE
    };

    npc_umi_yetiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool        inited;
    bool        reached;
    uint32      phase;
    uint32      m_timer;
    ObjectGuid  m_targetGuid;
    TargetType  m_targetType;

    void Reset() override
    {
        inited = false;
        reached = false;
        phase = 0;
        m_timer = 0;
    }

    void MoveInLineOfSight(Unit *) override
    {
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (inited)
        {
            if (m_timer <= uiDiff)
            {
                if (Creature *pCreature = m_creature->GetMap()->GetCreature(m_targetGuid))
                {
                    if (!reached)
                    {
                        if (!(m_creature->GetDistance2d(pCreature) < 5.0f))
                            return;
                        reached = true;
                    }
                    switch (phase)
                    {
                        case 0:
                            m_creature->MonsterTextEmote("Umi's Mechanical Yeti marches around, roaring and making a ruckus.", 0);
                            m_creature->MonsterSay("RAAAAAAAR!", 0);
                            pCreature->CastSpell(pCreature, SPELL_AUTO_FEAR, true);
                            break;
                        case 1:
                            switch (m_targetType)
                            {
                                case TARGET_QUIXXIL:
                                    pCreature->MonsterSay("Oh!!! Get that thing away from me!", 0);
                                    pCreature->MonsterTextEmote("Quixxil runs quickly away from Umi's Mechanical yeti.", 0);
                                    break;
                                case TARGET_LEGACKI:
                                    pCreature->MonsterTextEmote("Legacki jumps in fright!", 0);
                                    pCreature->MonsterSay("Ahhhhh!!!", 0);
                                    break;
                                case TARGET_SPRINKLE:
                                    pCreature->MonsterTextEmote("Sprinkle jumps in fright!", 0);
                                    pCreature->MonsterSay("Ahhhhh!!! What is that thing?!", 0);
                                    break;
                            }
                            break;
                        case 2:
                            switch (m_targetType)
                            {
                                case TARGET_QUIXXIL:
                                    pCreature->MonsterSay("Why do you chase me, Mechanical yeti?! WHY?!", 0);
                                    break;
                                case TARGET_LEGACKI:
                                    pCreature->MonsterSay(",You big meanie! Who put you up to this?", 0);
                                    break;
                                case TARGET_SPRINKLE:
                                    pCreature->MonsterSay("Umi sent you, didn't she? She told me to expect a surprise, but I never thought that this is what she meant! ", 0);
                                    break;
                            }
                            break;
                        case 3:
                            switch (m_targetType)
                            {
                                case TARGET_QUIXXIL:
                                    m_creature->MonsterTextEmote("the mechanical yeti winds down.", 0);
                                    break;
                                case TARGET_LEGACKI:
                                    pCreature->MonsterSay("It was Umi, wasn't it?! She's always playing jokes on me, and now she's got you in on it too!", 0);
                                    break;
                            }
                            break;
                        case 4:
                            switch (m_targetType)
                            {
                                case TARGET_QUIXXIL:
                                    pCreature->MonsterTextEmote("Quixxil looks relieved.", 0);
                                    pCreature->MonsterSay("I'm jumpy as it is... and people insist on scaring me... Next time, though, I'll be ready!", 0);
                                    break;
                                case TARGET_LEGACKI:
                                    pCreature->MonsterTextEmote("Legacki sighs.", 0);
                                    break;
                            }
                        case 5:
                            pCreature->RemoveAurasDueToSpell(SPELL_AUTO_FEAR);
                            pCreature->GetMotionMaster()->MoveTargetedHome();
                            break;
                    }
                    phase++;
                    m_timer = 2500;
                }
            }
            else
                m_timer -= uiDiff;
            return;
        }
        inited = true;
        switch (m_creature->GetAreaId())
        {
            case AREA_UNGORO:
                if (Creature* pCreature = GetClosestCreatureWithEntry(m_creature, NPC_QUIXXIL, 15.0f))
                {
                    m_targetType = TARGET_QUIXXIL;
                    m_targetGuid = pCreature->GetObjectGuid();
                    m_creature->GetMotionMaster()->MoveFollow(pCreature, 0.6f, M_PI);
                }
                break;
            case AREA_TANARIS:
                if (Creature* pCreature = GetClosestCreatureWithEntry(m_creature, NPC_SPRINKLE, 15.0f))
                {
                    m_targetType = TARGET_LEGACKI;
                    m_targetGuid = pCreature->GetObjectGuid();
                    m_creature->GetMotionMaster()->MoveFollow(pCreature, 0.6f, M_PI);
                }
                break;
            case AREA_WINTERSPRING:
                if (Creature* pCreature = GetClosestCreatureWithEntry(m_creature, NPC_LEGACKI, 15.0f))
                {
                    m_targetType = TARGET_SPRINKLE;
                    m_targetGuid = pCreature->GetObjectGuid();
                    m_creature->GetMotionMaster()->MoveFollow(pCreature, 0.6f, M_PI);
                }
                break;
        }
    }
};

CreatureAI* GetAI_npc_umi_yeti(Creature* pCreature)
{
    return new npc_umi_yetiAI(pCreature);
}

enum
{
    QUEST_ASCENSION_H2      =   6602,
    QUEST_ASCENSION_A2      =   6502,
    SPELL_CREATE_AMULET     =   22207, // [Summon Drakefire Amulet DND]
    ITEM_DRAKEFIRE_AMULET   =   16309,
};

bool GossipHello_npc_haleh(Player* player, Creature* creature)
{
    if (!player->HasItemCount(ITEM_DRAKEFIRE_AMULET, 1, true))
        if (player->GetQuestRewardStatus(QUEST_ASCENSION_H2) || player->GetQuestRewardStatus(QUEST_ASCENSION_A2))
            creature->CastSpell(player, SPELL_CREATE_AMULET, false);
    return false;
}


void AddSC_winterspring()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_lorax";
    newscript->pGossipHello =  &GossipHello_npc_lorax;
    newscript->pGossipSelect = &GossipSelect_npc_lorax;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_rivern_frostwind";
    newscript->pGossipHello =  &GossipHello_npc_rivern_frostwind;
    newscript->pGossipSelect = &GossipSelect_npc_rivern_frostwind;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_witch_doctor_mauari";
    newscript->pGossipHello =  &GossipHello_npc_witch_doctor_mauari;
    newscript->pGossipSelect = &GossipSelect_npc_witch_doctor_mauari;
    newscript->RegisterSelf();

    // Elysium

    newscript = new Script;
    newscript->Name = "mob_Artorius";
    newscript->GetAI = &GetAI_mob_Artorius;
    newscript->pGossipHello = &GossipHello_mob_Artorius;
    newscript->pGossipSelect = &GossipSelect_mob_Artorius;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_umi_yeti";
    newscript->GetAI = &GetAI_npc_umi_yeti;
    newscript->RegisterSelf();

    //Alita---
    newscript = new Script;
    newscript->Name = "npc_ranshalla";
    newscript->GetAI = &GetAI_npc_ranshalla;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_ranshalla;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_fire_of_elune";
    newscript->pGOHello = &GOHello_go_fire_of_elune;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_altar_of_elune";
    newscript->pGOHello = &GOHello_go_altar_of_elune;
    newscript->RegisterSelf();
    //---

    newscript = new Script;
    newscript->Name = "npc_haleh";
    newscript->pGossipHello = &GossipHello_npc_haleh;
    newscript->RegisterSelf();
}
