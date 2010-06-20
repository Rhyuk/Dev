
#include "ScriptedPch.h"
#include <cstring>

//This function is called when the player opens the gossip menubool
bool GossipHello_script_bot_giver(Player *pPlayer, Creature *pCreature)
{
    WorldSession *session = pPlayer->GetSession();
    uint8 count = 0;

    for(PlayerBotMap::const_iterator itr = session->GetPlayerBotsBegin(); itr != session->GetPlayerBotsEnd(); ++itr)
    {
        if(count == 0)
            pPlayer->ADD_GOSSIP_ITEM(0, "Abandon Your Player?", 6004, GOSSIP_ACTION_INFO_DEF + 100);

        ++count;
    }

    if(pPlayer->HaveBot())
    {
        pPlayer->ADD_GOSSIP_ITEM(0, "Abandon Your Minion?", 6001, GOSSIP_ACTION_INFO_DEF + 101);
    } else
        pPlayer->ADD_GOSSIP_ITEM(0, "Recruit a Minion", 6006, GOSSIP_ACTION_INFO_DEF + 2);

    if(count < pPlayer->GetMaxPlayerBot()) pPlayer->ADD_GOSSIP_ITEM(0, "Recruit a Player", 6002, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->PlayerTalkClass->SendGossipMenu(907, pCreature->GetGUID());
    return true;
}


void SendCreatePlayerBot(Player *pPlayer, Creature *pCreature, uint32 action)
{
    std::list<std::string> *names;
    names = pPlayer->GetCharacterList();
    if(names == NULL || names->empty())
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        return;
    }

    int8 x = action - GOSSIP_ACTION_INFO_DEF - 1;

    std::list<std::string>::iterator iter, next;
    for(iter = names->begin(); iter != names->end(); iter++)
    {
        if (x==0) pPlayer->CreatePlayerBot((*iter).c_str());
        else {
            if(x == 1)
            {
                pPlayer->CreatePlayerBot((*iter).c_str());
                break;
            }
            --x;
        }
    }

    pPlayer->CLOSE_GOSSIP_MENU();
} //end SendCreatePlayerBot

void SendCreatePlayerBotMenu(Player *pPlayer, Creature *pCreature, uint32 action)
{
    std::list<std::string> *names;
    names = pPlayer->GetCharacterList();
    if(names == NULL || names->empty())
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        return;
    }

    pPlayer->ADD_GOSSIP_ITEM(9, "ADD ALL" , 6003, GOSSIP_ACTION_INFO_DEF + 1);
    int8 x = 2;
    
    std::list<std::string>::iterator iter, next;
    for(iter = names->begin(); iter != names->end(); iter++)
    {
        //error_log("character : %s", (*iter).c_str());
        pPlayer->ADD_GOSSIP_ITEM(9, (*iter).c_str() , 6003, GOSSIP_ACTION_INFO_DEF + x);
        ++x;
    }
    pPlayer->SEND_GOSSIP_MENU(907, pCreature->GetGUID());
} //end SendCreatePlayerBotMenu

void SendRemovePlayerBot(Player *pPlayer, Creature *pCreature, uint32 action);
void SendRemovePlayerBotAll(Player *pPlayer, Creature *pCreature) {
    for (int8 x = 2; x<=10; x++ )
    {
        SendRemovePlayerBot (pPlayer, pCreature, GOSSIP_ACTION_INFO_DEF + 2);
    }
}

void SendRemovePlayerBot(Player *pPlayer, Creature *pCreature, uint32 action)
{
    int8 x = action - GOSSIP_ACTION_INFO_DEF - 1;

    if (x == 0) {
        SendRemovePlayerBotAll(pPlayer, pCreature);
        return;
    }

    WorldSession *session = pPlayer->GetSession();
    for(PlayerBotMap::const_iterator itr = session->GetPlayerBotsBegin(); itr != session->GetPlayerBotsEnd(); ++itr)
    {
        if(x == 1 && itr->second && itr->second->GetGroup())
        {
            Player *m_bot = itr->second;
            Group *m_group = m_bot->GetGroup();

            //removing bot from group
            if(m_group->IsMember(m_bot->GetGUID()))
            {
                //deleting bot from group
                if(m_group->RemoveMember(m_bot->GetGUID(), 99) < 1) // 99 means I'm a bot
                {
                    //no one left in group so deleting group
                    delete m_group;
                    //objmgr.RemoveGroup(m_group);
                }
            }
            session->LogoutPlayerBot(m_bot->GetGUID(), true);
            break;
        }
        --x;
    }
    pPlayer->CLOSE_GOSSIP_MENU();
} //end SendRemovePlayerBot

void SendRemovePlayerBotMenu(Player *pPlayer, Creature *pCreature, uint32 action)
{
    pPlayer->ADD_GOSSIP_ITEM(9, "REMOVE ALL", 6005, GOSSIP_ACTION_INFO_DEF + 1);

    uint8 x = 2;
    WorldSession *session = pPlayer->GetSession();
    for(PlayerBotMap::const_iterator itr = session->GetPlayerBotsBegin(); itr != session->GetPlayerBotsEnd(); ++itr)
    {
        Player *bot = itr->second;
        pPlayer->ADD_GOSSIP_ITEM(9, bot->GetName(), 6005, GOSSIP_ACTION_INFO_DEF + x);
        ++x;
    }
    pPlayer->SEND_GOSSIP_MENU(907, pCreature->GetGUID());
} //end SendRemovePlayerBotMenu

void SendCreateNPCBot(Player *pPlayer, Creature *pCreature, uint32 action)
{
    uint8 bot_class = 0;
    if(action == GOSSIP_ACTION_INFO_DEF + 101) //abandon bot
    {
        if(pPlayer->HaveBot())
            pPlayer->SetBotMustDie();
        pPlayer->CLOSE_GOSSIP_MENU();
        return;
    }
    else if(action == GOSSIP_ACTION_INFO_DEF + 1){ //playerbot
        pPlayer->CLOSE_GOSSIP_MENU();
        return;
    }
    else if(action == GOSSIP_ACTION_INFO_DEF + 2)
        bot_class = CLASS_WARRIOR;
    else if(action == GOSSIP_ACTION_INFO_DEF + 3)
        bot_class = CLASS_HUNTER;
    else if(action == GOSSIP_ACTION_INFO_DEF + 4)
        bot_class = CLASS_PALADIN;
    else if(action == GOSSIP_ACTION_INFO_DEF + 5)
        bot_class = CLASS_SHAMAN;
    else if(action == GOSSIP_ACTION_INFO_DEF + 6)
        bot_class = CLASS_ROGUE;
    else if(action == GOSSIP_ACTION_INFO_DEF + 7)
        bot_class = CLASS_DRUID;
    else if(action == GOSSIP_ACTION_INFO_DEF + 8)
        bot_class = CLASS_MAGE;
    else if(action == GOSSIP_ACTION_INFO_DEF + 9)
        bot_class = CLASS_PRIEST;
    else if(action == GOSSIP_ACTION_INFO_DEF + 10)
        bot_class = CLASS_WARLOCK;
    //else if(action == GOSSIP_ACTION_INFO_DEF + 11)
        //bot_class = CLASS_DEATH_KNIGHT;

    if(bot_class > 0)
    {
        //error_log("script_bot_giver.SendCreateNPCBot class = %u", bot_class);
        pPlayer->CreateNPCBot(bot_class);
    }
    //else
        //pCreature->Say("Invalid selection.", LANG_UNIVERSAL, NULL);
    pPlayer->CLOSE_GOSSIP_MENU();
    return;
}

void SendCreateNPCBotMenu(Player *pPlayer, Creature *pCreature, uint32 action)
{
    pPlayer->ADD_GOSSIP_ITEM(9, "Recruit a Warrior", 6001, GOSSIP_ACTION_INFO_DEF + 2);
    pPlayer->ADD_GOSSIP_ITEM(9, "Recruit a Hunter", 6001, GOSSIP_ACTION_INFO_DEF + 3);
    pPlayer->ADD_GOSSIP_ITEM(9, "Recruit a Paladin", 6001, GOSSIP_ACTION_INFO_DEF + 4);
    pPlayer->ADD_GOSSIP_ITEM(9, "Recruit a Shaman", 6001, GOSSIP_ACTION_INFO_DEF + 5);
    pPlayer->ADD_GOSSIP_ITEM(9, "Recruit a Rogue", 6001, GOSSIP_ACTION_INFO_DEF + 6);
    pPlayer->ADD_GOSSIP_ITEM(3, "Recruit a Druid", 6001, GOSSIP_ACTION_INFO_DEF + 7);
    pPlayer->ADD_GOSSIP_ITEM(3, "Recruit a Mage", 6001, GOSSIP_ACTION_INFO_DEF + 8);
    pPlayer->ADD_GOSSIP_ITEM(3, "Recruit a Priest", 6001, GOSSIP_ACTION_INFO_DEF + 9);
    pPlayer->ADD_GOSSIP_ITEM(3, "Recruit a Warlock", 6001, GOSSIP_ACTION_INFO_DEF + 10);
    //pPlayer->ADD_GOSSIP_ITEM(9, "Recruit a Death Knight", 1, GOSSIP_ACTION_INFO_DEF + 11);
    pPlayer->SEND_GOSSIP_MENU(907, pCreature->GetGUID());
} //end SendCreateNPCBotMenu


//This function is called when the player clicks an option on the gossip menu
bool GossipSelect_script_bot_giver(Player *pPlayer, Creature *pCreature, uint32 sender, uint32 action)
{
    switch(sender)
    {
        case 6006: SendCreateNPCBotMenu(pPlayer, pCreature, action); break;
        case 6001: SendCreateNPCBot(pPlayer, pCreature, action); break;
        case 6002: SendCreatePlayerBotMenu(pPlayer, pCreature, action); break;
        case 6003: SendCreatePlayerBot(pPlayer, pCreature, action); break;
        case 6004: SendRemovePlayerBotMenu(pPlayer, pCreature, action); break;
        case 6005: SendRemovePlayerBot(pPlayer, pCreature, action); break;
    }
    return true;
}
void AddSC_script_bot_giver()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "script_bot_giver";
    newscript->pGossipHello = &GossipHello_script_bot_giver;
    newscript->pGossipSelect = &GossipSelect_script_bot_giver;
    newscript->RegisterSelf();
}
