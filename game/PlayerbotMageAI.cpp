/*
Name : PlayerbotMageAI.cpp
Complete: maybe around 70%

Limitations:    - Talent build decision is made by key talent spells, which makes them viable only after level 50-ish.. Behaviour does not change though..
                - isFrozen() checks may not be accurate
                - Amplify/dampen magic, spellsteal, fire/frost ward, frostfire bolt spells are not used..
                - Mage would probably get aggro quite easily with this AI, need some extra work to lower threat..

Authors : SwaLLoweD
Version : 0.40
*/
#include "PlayerbotMageAI.h"

class PlayerbotAI;
PlayerbotMageAI::PlayerbotMageAI(Player *const master, Player *const bot, PlayerbotAI *const ai): PlayerbotClassAI(master, bot, ai){\
    foodDrinkSpamTimer = 0;
    LoadSpells();
}
PlayerbotMageAI::~PlayerbotMageAI(){}

void PlayerbotMageAI::LoadSpells() {
    PlayerbotAI *ai = GetAI();
    if (!ai) return;
    #pragma region SpellId Fill
    //arcane
    ARCANE_MISSILES = ai->getSpellIdExact("Arcane Missiles");
    ARCANE_EXPLOSION = ai->getSpellIdExact("Arcane Explosion");
    ARCANE_BLAST = ai->getSpellIdExact("Arcane Blast");
    ARCANE_BARRAGE = ai->getSpellIdExact("Arcane Barrage");
    

    //fire
    FIREBALL = ai->getSpellIdExact("Fireball");
    FROSTFIRE_BOLT = ai->getSpellIdExact("Frostfire Bolt");
    FIRE_BLAST = ai->getSpellIdExact("Fire Blast");
    FLAMESTRIKE = ai->getSpellIdExact("Flamestrike");
    BLAST_WAVE = ai->getSpellIdExact("Blastwave");
    SCORCH = ai->getSpellIdExact("Scorch");
    PYROBLAST = ai->getSpellIdExact("Pyroblast");
    LIVING_BOMB = ai->getSpellIdExact("Living Bomb");
    

    //cold
    FROSTBOLT = ai->getSpellIdExact("Frostbolt");
    FROST_NOVA = ai->getSpellIdExact("Frost Nova");
    ICE_LANCE = ai->getSpellIdExact("Ice Lance");
    BLIZZARD = ai->getSpellIdExact("Blizzard");
    CONE_OF_COLD = ai->getSpellIdExact("Cone of Cold");
    
    WATER_ELEMENTAL = ai->getSpellIdExact("Summon Water Elemental");
    

    // buffs
    FROST_ARMOR = ai->getSpellIdExact("Ice Armor");
    if (!FROST_ARMOR) FROST_ARMOR = ai->getSpellIdExact("Frost Armor");
    MAGE_ARMOR = ai->getSpellIdExact("Mage Armor");
    MOLTEN_ARMOR = ai->getSpellIdExact("Molten Armor");
    FIRE_WARD = ai->getSpellIdExact("Fire Ward");
    FROST_WARD = ai->getSpellIdExact("Frost Ward");
    MANA_SHIELD = ai->getSpellIdExact("Mana Shield");
    ICE_BARRIER = ai->getSpellIdExact("Ice Barrier");
    POM = ai->getSpellIdExact("Presence of Mind");
    FOCUS_MAGIC = ai->getSpellIdExact("Focus Magic");
    ARCANE_POWER = ai->getSpellIdExact("Arance Power");
    COMBUSTION = ai->getSpellIdExact("Combustion");
    ICY_VEINS = ai->getSpellIdExact("Icy Veins");
    
    ARCANE_INTELLECT = ai->getSpellIdExact("Arcane Intellect");
    ARCANE_BRILLIANCE = ai->getSpellIdExact("Arcane Brilliance");
    DALARAN_INTELLECT = ai->getSpellIdExact("Dalaran Intellect");
    DALARAN_BRILLIANCE = ai->getSpellIdExact("Dalaran Brilliance");
    DAMPEN_MAGIC = ai->getSpellIdExact("Dampen Magic");
    AMPLIFY_MAGIC = ai->getSpellIdExact("Amplify Magic");
    

    //CC
    POLYMORPH = ai->getSpellIdExact("Polymorph");
    DRAGONS_BREATH = ai->getSpellIdExact("Dragon's Breath");
    DEEP_FREEZE = ai->getSpellIdExact("Deep Freeze");
    
    
    //other
    CONJURE_REFRESHMENT = ai->getSpellIdExact("Conjure Refreshment");
    CONJURE_WATER = ai->getSpellIdExact("Conjure Water");
    CONJURE_FOOD = ai->getSpellIdExact("Conjure Food");
    CONJURE_MANA_GEM = ai->getSpellIdExact("Conjure Mana Gem");
    MIRROR_IMAGE = ai->getSpellIdExact("Mirror Image");
    BLINK = ai->getSpellIdExact("Blink");
    ICE_BLOCK = ai->getSpellIdExact("Ice Block");
    INVISIBILITY = ai->getSpellIdExact("Invisibility");
    EVOCATION = ai->getSpellIdExact("Evocation");
    REMOVE_CURSE = ai->getSpellIdExact("Remove Curse");
    COUNTER_SPELL = ai->getSpellIdExact("Counter Spell");
    SLOW = ai->getSpellIdExact("Slow");

    //Special
    P_BRAIN_FREEZE = 57761; //Brain Freeze proc
    P_FIRESTARTER = 54741; //Firestarter proc
    P_HOT_STREAK = 48108; //Hot Sreak proc
    P_ARCANE_BLAST = 36032; //Arcane blast proc

    SHOOT = ai->getSpellIdExact("Shoot");

    TALENT_ARCANE = SLOW;
    TALENT_FIRE = COMBUSTION;
    TALENT_FROST = ICE_BARRIER;

    uint8 talentCounter = 0;
    if (TALENT_ARCANE) talentCounter++;
    if (TALENT_FIRE) talentCounter++;
    if (TALENT_FROST) talentCounter++;
    if (talentCounter > 1) { TALENT_ARCANE = 0; TALENT_FIRE = 0; TALENT_FROST = 0; } //Unreliable Talent detection.
    #pragma endregion
}

void PlayerbotMageAI::DoNextCombatManeuver(Unit *pTarget)
{
    if (!pTarget || pTarget->isDead()) return;
    PlayerbotAI *ai = GetAI();
    if (!ai) return;
    Player *m_bot = GetPlayerBot();
    if (!m_bot || m_bot->isDead()) return;
    Unit *pVictim = pTarget->getVictim();
    Unit *m_tank = FindMainTankInRaid(GetMaster());
    if (!m_tank && m_bot->GetGroup() && GetMaster()->GetGroup() != m_bot->GetGroup()) { FindMainTankInRaid(m_bot); }
    if (!m_tank) { m_tank = m_bot; }
    uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();
    float pDist = m_bot->GetDistance(pTarget);
    uint8 pThreat = GetThreatPercent(pTarget);

    /* switch(ai->GetScenarioType())
    {
        case SCENARIO_DUEL:
            (LIVING_BOMB && !pTarget->HasAura(LIVING_BOMB, 0) && ai->CastSpell(LIVING_BOMB, pTarget));

            if(FIREBALL > 0) CastSpell(FIREBALL);
            return;
    }*/

    //------- Non Duel combat ----------


    #pragma region Choose Actions
    // Choose actions accoring to talents (MAGE is always ranged dps)
    m_role = BOT_ROLE_DPS_RANGED;

    // if i am under attack and if i am not tank or offtank: change target if needed
    if (m_tank->GetGUID() != m_bot->GetGUID() && isUnderAttack() ) 
    {
        // Keep hitting but reduce threat
        if (CastSpell(INVISIBILITY,m_bot)) { return; }
        else if (CastSpell(ICE_BLOCK,m_bot)) { return; }
        //else if (m_bot->getRace() == (uint8) RACE_NIGHTELF && CastSpell(R_SHADOWMELD,m_bot)) { return; }
        else //I cannot reduce threat so
        {
            if (pVictim && pVictim->GetGUID() == m_bot->GetGUID() && pDist <= 2) {  } // My target is almost up to me, no need to search
            else //Have to select nearest target
            {
                Unit *curAtt = GetNearestAttackerOf(m_bot);
                if (curAtt && curAtt->GetGUID() != pTarget->GetGUID())
                {
                    m_bot->SetSelection(curAtt->GetGUID());
                    //ai->AddLootGUID(curAtt->GetGUID());
                    DoNextCombatManeuver(curAtt); //Restart new update to get variables fixed..
                    return; 
                }
            }
            //my target is attacking me
        }
    }
    #pragma endregion

    TakePosition(pTarget);
    // If there's a cast stop
    if (m_bot->hasUnitState(UNIT_STAT_CASTING)) { return; }

    //Buff
    if (isUnderAttack()) { if ( CastSpell(FROST_ARMOR,m_bot)) { return; } }
    else if (MOLTEN_ARMOR) { if ( CastSpell(MOLTEN_ARMOR,m_bot)) { return; } }
    else if (CastSpell(MAGE_ARMOR,m_bot)) { return; }
    if (CastSpell(COMBUSTION,m_bot)) { } //nogcd
    if (DoSupportRaid(m_bot,30,0,0,0,1,1)) { return; }
    
    //Protect
    if (m_tank->GetGUID() != m_bot->GetGUID() && pVictim && pVictim->GetGUID() == m_bot->GetGUID() )
    {
        if (CastSpell(INVISIBILITY, m_bot)) { return; }
        if (ai->GetHealthPercent(*pTarget) > 50 && CastSpell(POLYMORPH)) { return; }
        //if (m_bot->getRace() == (uint8) RACE_NIGHTELF && isUnderAttack() && CastSpell(R_SHADOWMELD, m_bot)) { return; }
    }
    if (isUnderAttack() && pDist > 5 && CastSpell(FROST_NOVA, pTarget)) { return; }
    if (DEEP_FREEZE && pTarget->isFrozen() && CastSpell(DEEP_FREEZE,pTarget)) { return; }
    if (isUnderAttack() && CastSpell(DRAGONS_BREATH, pTarget)) { return; }
    if ((isUnderAttack() || ai->GetHealthPercent() < 75)  && ai->GetManaPercent() > 40 && CastSpell(MANA_SHIELD,m_bot)) { return; }
    if (m_bot->getRace() == (uint8) RACE_DWARF && ai->GetHealthPercent() < 75 && CastSpell(R_STONEFORM,m_bot)) { } //no gcd
    if (m_bot->getRace() == (uint8) RACE_DRAENEI && ai->GetHealthPercent() < 55 && CastSpell(R_GIFT_OF_NAARU,m_bot)) { return; } //no Gcd, but has cast
    if (m_bot->getRace() == (uint8) RACE_TAUREN && pDist < 8 && CastSpell(R_WAR_STOMP, pTarget)) { return; } //no gcd but is cast
    if ((ai->GetHealthPercent() < 65 || ai->GetManaPercent() < 5) && CastSpell(ICE_BLOCK,m_bot)) { return; }
    if (isUnderAttack() && CastSpell(ICE_BARRIER, pTarget)) { return; }
    if (ai->GetManaPercent() < 30 && CastSpell (EVOCATION, m_bot)) { return; }
    //Use Mana gem

    //Break spells
    if (m_bot->getRace() == (uint8) RACE_BLOODELF && pDist < 8 && (pTarget->IsNonMeleeSpellCasted(true) || ai->GetManaPercent() < 40) && CastSpell(R_ARCANE_TORRENT, pTarget)) { } //no gcd
    else if (pThreat < threatThreshold && pTarget->IsNonMeleeSpellCasted(true) && CastSpell(COUNTER_SPELL, pTarget)) { return; } //High threat

    //Catch
    if (pTarget->HasUnitMovementFlag(UNIT_FLAG_FLEEING))
    {
        if (CastSpell(FROST_NOVA,pTarget)) return;
        if (CastSpell(FROSTBOLT,pTarget)) return;
    }

    // If at threat limit, try to reduce threat
    if (pThreat > threatThreshold && m_tank->GetGUID() != m_bot->GetGUID() && !isUnderAttack())
    {
        if (m_tank->getVictim() && m_tank->getVictim()->GetGUID() != pTarget->GetGUID()) // I am attacking wrong target!!
        {
            m_bot->SetSelection(m_tank->getVictim()->GetGUID());
            return;
        }
        else  
        {
            if (CastSpell(INVISIBILITY,m_bot)) { return; } //Lets see if we can manage
            else if (m_bot->FindCurrentSpellBySpellId(SHOOT)) { m_bot->InterruptNonMeleeSpells( true, SHOOT ); return; } //Disable wand
            else { return; } //use no spells and wait threat to be reduced
        }
    }

    //Dps up
    if (ai->GetHealthPercent(*pTarget) > 95) { return; } // dont dps too early

    if (CastSpell(ICY_VEINS,m_bot)) {} //nogcd
    if (CastSpell(ARCANE_POWER,m_bot)) {} //nogcd
    if (CastSpell(POM,m_bot)) {} //nogcd
    if (m_bot->getRace() == (uint8) RACE_TROLL && CastSpell(R_BERSERKING,m_bot)) {} //no GCD
    if (m_bot->getRace() == (uint8) RACE_ORC && CastSpell(R_BLOOD_FURY,m_bot)) {} //no GCD
    if (CastSpell(FOCUS_MAGIC,m_tank)) { return; }
    if (CastSpell(MIRROR_IMAGE,m_bot)) { return; }
    if (CastSpell(WATER_ELEMENTAL,m_bot)) { return; }

    //AOE - support
    if (DEEP_FREEZE && pTarget->isFrozen() && CastSpell(DEEP_FREEZE,pTarget)) { return; }
    if (isUnderAttack(m_tank,4))
    {
        if (TALENT_FIRE)
        {
            if (CastSpell(FLAMESTRIKE,pTarget)) { return; } 
            if (CastSpell(BLAST_WAVE,pTarget)) { return; }
            if (CastSpell(LIVING_BOMB,pTarget)) { return; }
            if (CastSpell(DRAGONS_BREATH,pTarget)) { return; }
        }
        else
        { 
            if (CastSpell(BLIZZARD,pTarget)) { return; } 
            if (CastSpell(FROST_NOVA,pTarget)) { return; }
            if (CastSpell(CONE_OF_COLD,pTarget)) { return; }
        }
    }
    if (CastSpell(SLOW,pTarget)) { return; }
    

    //Main Dps
    if (TALENT_FIRE)
    {
        //Fire procs
        if (m_bot->HasAura(P_BRAIN_FREEZE) && CastSpell(FIREBALL,pTarget,1,1)) { return; }
        if (m_bot->HasAura(P_FIRESTARTER) && CastSpell(FLAMESTRIKE,pTarget,1,1)) { return; }
        if ((m_bot->HasAura(POM) || m_bot->HasAura(P_HOT_STREAK)) && CastSpell(PYROBLAST,pTarget,1,1)) { return; }
        if (m_bot->isMoving() && CastSpell(FIRE_BLAST,pTarget,1,1)) { return; }
        if (CastSpell(FIREBALL,pTarget,1,1)) { return; }

    }
    else if (TALENT_FROST)
    {
        if ((m_bot->HasAura(POM) || m_bot->HasAura(P_BRAIN_FREEZE)) && CastSpell(FROSTBOLT,pTarget,1,1)) { return; }
        if ((pTarget->isFrozen() || m_bot->isMoving()) && CastSpell(ICE_LANCE,pTarget)) { return; }
        if (CastSpell(FROSTBOLT,pTarget,1,1)) { return; }
    }
    else if (TALENT_ARCANE)
    {
        if (m_bot->HasAura(POM) && CastSpell(FIREBALL,pTarget,1,1)) { return; }
        if (ARCANE_BLAST)
        {
            Aura *abaura = m_bot->GetAura(P_ARCANE_BLAST);
            if (abaura && abaura->GetStackAmount() >= 4) { if (CastSpell(ARCANE_MISSILES,pTarget,1,1)) { return; } }
            else { if (CastSpell(ARCANE_BLAST,pTarget) ) { return; }}
        }
        if (m_bot->isMoving() && CastSpell(ARCANE_BARRAGE,pTarget)) { return; }
        if (CastSpell(ARCANE_MISSILES,pTarget)) { return; }
    }
    else
    {
        if (m_bot->HasAura(P_BRAIN_FREEZE) && CastSpell(FIREBALL,pTarget,1,1)) { return; }
        if (m_bot->HasAura(P_FIRESTARTER) && CastSpell(FLAMESTRIKE,pTarget,1,1)) { return; }
        if (m_bot->HasAura(P_HOT_STREAK) && CastSpell(PYROBLAST,pTarget,1,1)) { return; }
        if (m_bot->HasAura(POM) && (CastSpell(PYROBLAST,pTarget,1,1) || CastSpell(FIREBALL,pTarget,1,1) || CastSpell(FROSTBOLT,pTarget,1,1))) { return; }
        if (pTarget->isFrozen() && CastSpell(ICE_LANCE,pTarget)) { return; }
        if (m_bot->isMoving() && (CastSpell(FIRE_BLAST,pTarget,1,1) || CastSpell(ARCANE_BARRAGE,pTarget) || CastSpell(ICE_LANCE,pTarget))) { return; }
        if (CastSpell(FIREBALL,pTarget)) { return; }
        if (CastSpell(FROSTBOLT,pTarget)) { return; }
        if (CastSpell(ARCANE_MISSILES,pTarget)) { return; }
    }


    //Use mana stone??
     // drink potion
    if(ai->GetManaPercent() < 5 )
    {
        Item *pItem = ai->FindPotion();
        if(pItem != NULL)
        {
            if (pItem->GetSpell() && m_bot->HasSpellCooldown(pItem->GetSpell()) ) { return; } //pot is in cooldown
            ai->UseItem(*pItem);
        }
    }
} //end DoNextCombatManeuver

void PlayerbotMageAI::DoNonCombatActions()
{
    PlayerbotAI *ai = GetAI();
    Player *m_bot = GetPlayerBot();
    if (!m_bot || !ai || m_bot->isDead()) { return; }

    //If Casting or Eating/Drinking return
    if (m_bot->hasUnitState(UNIT_STAT_CASTING)) { return; }
    if (m_bot->getStandState() == UNIT_STAND_STATE_SIT) { return; }

    //buff and heal raid
    if (DoSupportRaid(m_bot,30,0,0,0,1,1)) { return; }

    //Own Buffs
    if (MOLTEN_ARMOR) { if ( CastSpell(MOLTEN_ARMOR,m_bot)) { return; } }
    else if (CastSpell(MAGE_ARMOR,m_bot)) { return; }
    if (CastSpell(COMBUSTION,m_bot)) { } //nogcd

    //conjure food & water
    Item *pItem = ai->FindDrink();
    if(pItem == NULL && ai->GetBaseManaPercent() >= 48)
    {
        if (CastSpell(CONJURE_REFRESHMENT, m_bot)) { return; }
        if (CastSpell(CONJURE_WATER, m_bot)) { return; }
        return;
    }
    pItem = ai->FindFood();
    if(pItem == NULL && ai->GetBaseManaPercent() >= 48)
    {
        if (CastSpell(CONJURE_REFRESHMENT, m_bot)) { return; }
        if (CastSpell(CONJURE_FOOD, m_bot)) { return; }
        return;
    }
    //Conjure mana gem??

    //mana/hp check
    //Don't bother with eating, if low on hp, just let it heal themself
    if (m_bot->getRace() == (uint8) RACE_UNDEAD_PLAYER && ai->GetHealthPercent() < 75 && CastSpell(R_CANNIBALIZE,m_bot)) { return; }
    if (ai->GetManaPercent() < 50 && CastSpell (EVOCATION, m_bot)) { return; }
    if (ai->GetManaPercent() < 50 || ai->GetHealthPercent() < 50) { ai->Feast(); }
} //end DoNonCombatActions


bool PlayerbotMageAI::BuffPlayer(Unit *target)
{
    if (!target || target->isDead()) return false;
    
    if (target->getClass() == CLASS_WARRIOR || target->getClass() == CLASS_DEATH_KNIGHT || target->getClass() == CLASS_ROGUE) return false;

    if (!HasAuraName(target, ARCANE_INTELLECT) && !HasAuraName(target, ARCANE_BRILLIANCE) && !HasAuraName(target, DALARAN_INTELLECT) && !HasAuraName(target, DALARAN_BRILLIANCE))
    {
        return CastSpell(ARCANE_INTELLECT, target);
    }
    return false;
}

//Cures the target
bool PlayerbotMageAI::CureTarget(Unit *target)
{
    Player *m_bot = GetPlayerBot();

    if(!target || target->isDead()) { return false; }
    if (castDispel(DISPEL_CURSE, target)) return true;
    return false;
}


