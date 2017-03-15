#include "scriptPCH.h"
#include "dire_maul.h"


enum
{
    SPELL_BERSERKER_CHARGE = 22886,
    SPELL_KING_DEATH       = 23318,
    SPELL_MORTAL_STRIKE    = 15708,
    SPELL_SUNDER_ARMOR     = 15572,
    SPELL_WAR_STOMP        = 16727,
    POSITION_KING_RESET_X  = 835,
    POSITION_KING_RESET_Y  = 488,
    NPC_CHO_RUSH_THE_OBS   = 14324,
    RESET_TIMER            = 20000,
};

/******************/
struct KingGordokAI : public ScriptedAI
{
    KingGordokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiSunderArmor_Timer;
    uint32 m_uiMortalStrike_Timer;
    uint32 m_uiWarStomp_Timer;
    uint32 m_uiBerserkerCharge_Timer;
    uint32 m_uiReset_Timer;

    void Reset()
    {
        m_uiSunderArmor_Timer     = urand(3000,5000);
        m_uiMortalStrike_Timer    = urand(5000,7000);
        m_uiWarStomp_Timer        = urand(12000,15000);
        m_uiBerserkerCharge_Timer = 0;
        m_uiReset_Timer           = RESET_TIMER;
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();
    }

    void EnterEvadeMode()
    {
        ScriptedAI::EnterEvadeMode();
        if (Creature* choRush = m_creature->FindNearestCreature(NPC_CHO_RUSH_THE_OBS, 200.0f))
            if (choRush->isAlive() && choRush->isInCombat())
                choRush->AI()->EnterEvadeMode();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        Creature* choRush = m_creature->FindNearestCreature(NPC_CHO_RUSH_THE_OBS, 200.0f);
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (choRush && choRush->getVictim() && !m_creature->isInCombat())
                AttackStart(choRush->getVictim());
            return;
        }

        if (choRush && choRush->isAlive() && !choRush->isInCombat())
        {
            EnterEvadeMode();
            return;
        }

        /** Anti kitting system */
        if (m_uiReset_Timer > uiDiff)
            m_uiReset_Timer -= uiDiff;

        /** It's not possible for King Gordok not to use these skills, prevent hacking/kitting */
        if (m_uiReset_Timer < uiDiff)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            EnterEvadeMode();
            return;
        }

        if (m_uiBerserkerCharge_Timer < uiDiff)
        {
            Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (target)
                if (DoCastSpellIfCan(target, SPELL_BERSERKER_CHARGE) == CAST_OK)
                    m_uiBerserkerCharge_Timer = urand(12000, 15000);
        }
        else
            m_uiBerserkerCharge_Timer -= uiDiff;

        if (m_uiSunderArmor_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR) == CAST_OK)
            {
                m_uiSunderArmor_Timer = urand(7000, 12000);
                m_uiReset_Timer           = RESET_TIMER;
            }
        }
        else
            m_uiSunderArmor_Timer -= uiDiff;

        if (m_uiMortalStrike_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE) == CAST_OK)
            {
                m_uiMortalStrike_Timer = urand(9000, 14000);
                m_uiReset_Timer        = RESET_TIMER;
            }
        }
        else
            m_uiMortalStrike_Timer -= uiDiff;

        if (m_uiWarStomp_Timer < uiDiff)
        {
            /** War stomp is only effective at 5 yards. Margin of 5 additional yards in case of movement */
            if (m_creature->GetDistance(m_creature->getVictim()) < 7.0f)
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAR_STOMP) == CAST_OK)
                    m_uiWarStomp_Timer = urand(17000, 24000);
        }
        else
            m_uiWarStomp_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_king_gordok(Creature* pCreature)
{
    return new KingGordokAI(pCreature);
}

void AddSC_npc_king_gordok()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "npc_king_gordok";
    pNewScript->GetAI          = &GetAI_king_gordok;
    pNewScript->RegisterSelf();
}
