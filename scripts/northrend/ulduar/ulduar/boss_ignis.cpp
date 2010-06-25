/*
 * Copyright (C) 2008 - 2009 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "ScriptedPch.h"
#include "ulduar.h"



enum Yells
{
    SAY_AGGRO                                   = -1603220,
    SAY_SLAY_1                                  = -1603221,
    SAY_SLAY_2                                  = -1603222,
    SAY_DEATH                                   = -1603223,
    SAY_SUMMON                                  = -1603224,
    SAY_SLAG_POT                                = -1603225,
    SAY_SCORCH_1                                = -1603226,
    SAY_SCORCH_2                                = -1603227,
    SAY_BERSERK                                 = -1603228,

};

enum
{
    ACHIEV_TIMED_START_EVENT                      = 20951,
	id_add										  = 33121,
	dome_spell									  = 62548,
	fragilite									  = 62382,
};

const Position position_add[20] =
{	// gauche de Ignis
	{630.434998, 337.246002, 360.885986, 3.001970},
	{630.383972, 329.584991, 360.885986, 3.001970},
	{630.383972, 321.584991, 360.885986, 3.001970},
	{630.383972, 313.584991, 360.885986, 3.001970},
	{630.383972, 305.584991, 360.885986, 3.001970},
	//droite de Ignis
	{543.315979, 337.467987, 360.885986, 0.157080},
	{543.315979, 329.467987, 360.885986, 0.157080},
	{543.315979, 321.467987, 360.885986, 0.157080},
	{543.315979, 313.467987, 360.885986, 0.157080},
	{543.315979, 305.467987, 360.885986, 0.157080},
	//fond à gauche
	{630.366028, 247.307007, 360.888000, 3.001970},
	{630.366028, 239.307007, 360.888000, 3.001970},
	{630.366028, 231.307007, 360.888000, 3.001970},
	{630.366028, 223.307007, 360.888000, 3.001970},
	{630.366028, 215.307007, 360.888000, 3.001970},
	//fond à droite
	{543.075989, 247.457993, 360.888000, 0.157080},
	{543.075989, 239.457993, 360.888000, 0.157080},
	{543.075989, 231.457993, 360.888000, 0.157080},
	{543.075989, 223.457993, 360.888000, 0.157080},
	{543.075989, 215.457993, 360.888000, 0.157080},
};



struct boss_ignis_AI : public BossAI
{
    boss_ignis_AI(Creature *pCreature) : BossAI(pCreature, TYPE_IGNIS) {}

    uint32 uiFlameJetsTimer;
    uint32 uiScorchTimer;
    uint32 uiSlagPotTimer;
	uint32 add_timer;
	int spell_brulure;
	int spell_flot;
	int spell_active_assemblage;
	int spell_marmite;
	Creature * add[20];

	
	


    void Reset()
    {
        _Reset();
        uiFlameJetsTimer = 32000;
        uiScorchTimer = 10000;
        uiSlagPotTimer = 20000;
		

        if (instance)
            instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
		
		if (getDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
		{
			spell_brulure = 63474;
			spell_flot = 63472;
			spell_marmite = 63477;
			spell_active_assemblage = 62488;
			add_timer = 40000;

		}
		else
		{
			spell_active_assemblage = 62488;
			spell_brulure =62546;
			spell_flot = 62680;
			spell_marmite = 62717;
			add_timer = 30000;
		}
		
		for (uint32 i = 0; i < 20; ++i)
		{
			add[i]= NULL;
		}


    }

	void decor()
	{
		

		for (uint32 i = 0; i < 20; ++i)
		{
			add[i] = me->SummonCreature(id_add, position_add[i].GetPositionX(), position_add[i].GetPositionY() , position_add[i].GetPositionZ(), position_add[i].GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000000);
			
			

		
		}	
	}

	void active_add()
	{
		Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);	
		int choix = urand(0,20);
		add[choix]->setFaction(16);
		

		BossAI::JustSummoned(add[choix]);
		
	}


		

    void EnterCombat(Unit* /*who*/)
    {
		decor(); //plante le décor (Pop les adds)
        DoScriptText(SAY_AGGRO,me);
        _EnterCombat();

        if (instance)
            instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
		
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(SAY_SLAY_1,SAY_SLAY_2), me);
    }

    void JustDied(Unit * /*victim*/)
    {
        DoScriptText(SAY_DEATH, me);
        _JustDied();
    }
	void SummonedCreatureDespawn(Creature*)
	
	{
		if(me->HasAura(64473))
			me->RemoveAura(64473); // Debuf le boss du 20% dégats
		
	}

    void MoveInLineOfSight(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (me->GetPositionY() < 150 || me->GetPositionX() < 450) // Not Blizzlike, anti-exploit to prevent players from pulling bosses to vehicles.
        {	
			
            me->RemoveAllAuras();
            me->DeleteThreatList();
            me->CombatStop(false);
            me->GetMotionMaster()->MoveTargetedHome();
        }

		if (add_timer <= diff)
		{
			active_add();
			add_timer = 40000;
			DoCast(me, 64473); // Buff 20% dégats Fonctionnel ?
		} else add_timer -= diff;



        if (uiFlameJetsTimer <= diff)
        {
			
            DoCast(spell_flot);
            uiFlameJetsTimer = 30000;
        } else uiFlameJetsTimer -= diff;

        if (uiScorchTimer <= diff)
        {
            DoScriptText(RAND(SAY_SCORCH_1,SAY_SCORCH_2), me);
            DoCast(spell_brulure);
            uiScorchTimer = 40000;
			Creature *torche = me->SummonCreature(6491,me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000 ); //l'id importe peu tant que le mob est amical (ici Id d'un ange)
			torche->SetName("Torche");
			torche->SetDisplayId(21514); // apparence humanoide (permet au visual du dome d'être correct)
			torche->AddAura(dome_spell, torche);
			
			
        } else uiScorchTimer -= diff;

        if (uiSlagPotTimer <= diff)
        {
            if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                DoScriptText(SAY_SLAG_POT, me);
                DoCast(pTarget, spell_marmite);
            }
            uiSlagPotTimer = 30000;
        } else uiSlagPotTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct add_ignis_AI : public BossAI // Add AI
{

	add_ignis_AI(Creature *pCreature) : BossAI(pCreature, 0) {};


	void reset()
	{



	}
	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if(me->GetPositionZ() <= 359.9) // correspond aux bassins
		{

			if (me->HasAura(62373))
			{
				me->RemoveAura(62373);
				me->AddAura(fragilite,me); // Fragilité
			}
		}
		DoMeleeAttackIfReady();




	}


	



};


CreatureAI* GetAI_boss_ignis(Creature* pCreature)
{
    return new boss_ignis_AI (pCreature);
}

CreatureAI* GetAI_add_ignis(Creature* pCreature)
{
    return new add_ignis_AI (pCreature);
}



void AddSC_boss_ignis()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ignis";
    newscript->GetAI = &GetAI_boss_ignis;
    newscript->RegisterSelf();
}

void AddSC_add_ignis()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "add_ignis";
    newscript->GetAI = &GetAI_add_ignis;
    newscript->RegisterSelf();
}

