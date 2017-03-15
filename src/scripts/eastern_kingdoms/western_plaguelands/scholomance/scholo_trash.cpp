#include "scriptPCH.h"

enum
{
    SPELL_DARK_PLAGUE       = 18270,
    SPELL_EXPLOSION         = 17689,
    SPELL_FEINT_DEATH       = 19822,
    SPELL_FULL_HEALTH       = 23329
};

struct npc_unstable_corpseAI : public ScriptedAI
{
    npc_unstable_corpseAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 _darkPlagueTimer;
    bool   _exploded;

    void Reset()
    {
        _darkPlagueTimer = 5;
        _exploded        = false;
    }

    void JustDied(Unit* pKiller)
    {
    }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        float lifePct = ((float(m_creature->GetHealth()) - float(damage)) / float(m_creature->GetMaxHealth()));
        if (!_exploded && lifePct < 0.05f)
        {
            _exploded = true;
            m_creature->CastSpell(m_creature, SPELL_EXPLOSION, false);
        }
        if (_exploded)
            damage = m_creature->GetHealth() - 1;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (_exploded)
        {
            m_creature->DoKillUnit();
            return;
        }

        if (_darkPlagueTimer < diff)
        {
            _darkPlagueTimer = urand(4000, 11000);
            m_creature->CastSpell(m_creature->getVictim(), 18270, false);
        }
        else
            _darkPlagueTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_unstable_corpse(Creature* pCreature)
{
    return new npc_unstable_corpseAI(pCreature);
}



struct npc_reanimated_corpseAI : public ScriptedAI
{
    npc_reanimated_corpseAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 _darkPlagueTimer;
    uint32 _phase;

    void Reset()
    {
        _darkPlagueTimer = 5;
        _phase = 1;
    }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if ((damage >= m_creature->GetHealth()) && (_phase != 3))
        {
            damage = m_creature->GetHealth() - 1;
            if (_phase == 1)
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_FEINT_DEATH, false);
                _phase = 2;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (_phase == 2 && !(m_creature->HasAura(SPELL_FEINT_DEATH)))
        {
            _phase = 3;
            m_creature->CastSpell(m_creature, SPELL_FULL_HEALTH, false);
        }

        if (_phase == 2)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (_darkPlagueTimer < diff)
        {
            _darkPlagueTimer = urand(4000, 11000);
            m_creature->CastSpell(m_creature->getVictim(), SPELL_DARK_PLAGUE, false);
        }
        else
            _darkPlagueTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_reanimated_corpse(Creature* pCreature)
{
    return new npc_reanimated_corpseAI(pCreature);
}

void AddSC_scholo_trash()
{
    Script* script;

    script = new Script;
    script->Name = "npc_unstable_corpse";
    script->GetAI = &GetAI_npc_unstable_corpse;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_reanimated_corpse";
    script->GetAI = &GetAI_npc_reanimated_corpse;
    script->RegisterSelf();
}
