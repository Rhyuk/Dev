/*
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include "Chat.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Language.h"
#include "AccountMgr.h"
#include "SystemConfig.h"
#include "revision.h"
#include "Util.h"
#include "math.h"

bool ChatHandler::HandleHelpCommand(const char* args)
{
    char* cmd = strtok((char*)args, " ");
    if (!cmd)
    {
        ShowHelpForCommand(getCommandTable(), "help");
        ShowHelpForCommand(getCommandTable(), "");
    }
    else
    {
        if (!ShowHelpForCommand(getCommandTable(), cmd))
            SendSysMessage(LANG_NO_HELP_CMD);
    }

    return true;
}

bool ChatHandler::HandleCommandsCommand(const char* /*args*/)
{
    ShowHelpForCommand(getCommandTable(), "");
    return true;
}

bool ChatHandler::HandleAccountCommand(const char* /*args*/)
{
    AccountTypes gmlevel = m_session->GetSecurity();
    PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmlevel));
    return true;
}

bool ChatHandler::HandleStartCommand(const char* /*args*/)
{
		// Jail by WarHead edited by spgm
    if (m_session->GetPlayer()->m_jail_isjailed)
    {
        SendSysMessage(LANG_JAIL_DENIED);
        return true;
    }
    
    Player *chr = m_session->GetPlayer();

    if (chr->isInFlight())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    if (chr->isInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }

    if ((chr->isDead()) || (chr->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST)))
    {
    // if player is dead and stuck, send ghost to graveyard
    chr->RepopAtGraveyard();
    return true;
    }

    // cast spell Stuck
    chr->CastSpell(chr,8690,false);
    return true;
}

bool ChatHandler::HandleServerInfoCommand(const char* /*args*/)
{
    uint32 PlayersNum = sWorld.GetPlayerCount();
    uint32 MaxPlayersNum = sWorld.GetMaxPlayerCount();
    uint32 activeClientsNum = sWorld.GetActiveSessionCount();
    uint32 queuedClientsNum = sWorld.GetQueuedSessionCount();
    uint32 maxActiveClientsNum = sWorld.GetMaxActiveSessionCount();
    uint32 maxQueuedClientsNum = sWorld.GetMaxQueuedSessionCount();
    std::string uptime = secsToTimeString(sWorld.GetUptime());
    uint32 updateTime = sWorld.GetUpdateTime();

    PSendSysMessage(_FULLVERSION);
    //if (m_session)
    //    full = _FULLVERSION(REVISION_DATE,REVISION_TIME,"|cffffffff|Hurl:" REVISION_ID "|h" REVISION_ID "|h|r");
    //else
    //    full = _FULLVERSION(REVISION_DATE,REVISION_TIME,REVISION_ID);

    //SendSysMessage(full);
    //PSendSysMessage(LANG_USING_WORLD_DB,sWorld.GetDBVersion());
    //PSendSysMessage(LANG_USING_EVENT_AI,sWorld.GetCreatureEventAIVersion());
    PSendSysMessage(LANG_CONNECTED_PLAYERS, PlayersNum, MaxPlayersNum);
    PSendSysMessage(LANG_CONNECTED_USERS, activeClientsNum, maxActiveClientsNum, queuedClientsNum, maxQueuedClientsNum);
    PSendSysMessage(LANG_UPTIME, uptime.c_str());
    PSendSysMessage("Update time diff: %u.", updateTime);

    return true;
}

bool ChatHandler::HandleDismountCommand(const char* /*args*/)
{
    //If player is not mounted, so go out :)
    if (!m_session->GetPlayer()->IsMounted())
    {
        SendSysMessage(LANG_CHAR_NON_MOUNTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetPlayer()->isInFlight())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->Unmount();
    m_session->GetPlayer()->RemoveAurasByType(SPELL_AURA_MOUNTED);
    return true;
}

bool ChatHandler::HandleSaveCommand(const char* /*args*/)
{
    Player *player=m_session->GetPlayer();
    
		// Jail by WarHead edited by spgm
    if (player->m_jail_isjailed)
    {
        SendSysMessage(LANG_JAIL_DENIED);
        return true;
    }
    
    // save GM account without delay and output message (testing, etc)
    if (m_session->GetSecurity() > SEC_PLAYER)
    {
        player->SaveToDB();
        SendSysMessage(LANG_PLAYER_SAVED);
        return true;
    }

    // save or plan save after 20 sec (logout delay) if current next save time more this value and _not_ output any messages to prevent cheat planning
    uint32 save_interval = sWorld.getConfig(CONFIG_INTERVAL_SAVE);
    if ((save_interval == 0 || save_interval > 20*IN_MILISECONDS && player->GetSaveTimer() <= save_interval - 20*IN_MILISECONDS))
        player->SaveToDB();

    return true;
}

bool ChatHandler::HandleGMListIngameCommand(const char* /*args*/)
{
    bool first = true;

    ObjectAccessor::Guard guard(*HashMapHolder<Player>::GetLock());
    HashMapHolder<Player>::MapType &m = ObjectAccessor::Instance().GetPlayers();
    for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        AccountTypes itr_sec = itr->second->GetSession()->GetSecurity();
        if ((itr->second->isGameMaster() || (itr_sec > SEC_PLAYER && itr_sec <= sWorld.getConfig(CONFIG_GM_LEVEL_IN_GM_LIST))) &&
            (!m_session || itr->second->IsVisibleGloballyFor(m_session->GetPlayer())))
        {
            if (first)
            {
                SendSysMessage(LANG_GMS_ON_SRV);
                first = false;
            }

            SendSysMessage(GetNameLink(itr->second).c_str());
        }
    }

    if (first)
        SendSysMessage(LANG_GMS_NOT_LOGGED);

    return true;
}

bool ChatHandler::HandleAccountPasswordCommand(const char* args)
{
    if (!*args)
        return false;

    char *old_pass = strtok ((char*)args, " ");
    char *new_pass = strtok (NULL, " ");
    char *new_pass_c  = strtok (NULL, " ");

    if (!old_pass || !new_pass || !new_pass_c)
        return false;

    std::string password_old = old_pass;
    std::string password_new = new_pass;
    std::string password_new_c = new_pass_c;

    if (strcmp(new_pass, new_pass_c) != 0)
    {
        SendSysMessage (LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage (true);
        return false;
    }

    if (!accmgr.CheckPassword (m_session->GetAccountId(), password_old))
    {
        SendSysMessage (LANG_COMMAND_WRONGOLDPASSWORD);
        SetSentErrorMessage (true);
        return false;
    }

    AccountOpResult result = accmgr.ChangePassword(m_session->GetAccountId(), password_new);

    switch(result)
    {
        case AOR_OK:
            SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_PASS_TOO_LONG:
            SendSysMessage(LANG_PASSWORD_TOO_LONG);
            SetSentErrorMessage(true);
            return false;
        case AOR_NAME_NOT_EXIST:                            // not possible case, don't want get account name for output
        default:
            SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            SetSentErrorMessage(true);
            return false;
    }

    return true;
}

bool ChatHandler::HandleAccountAddonCommand(const char* args)
{
    if (!*args)
        return false;

    char *szExp = strtok((char*)args," ");

    uint32 account_id = m_session->GetAccountId();

    int expansion=atoi(szExp);                                    //get int anyway (0 if error)
    if (expansion < 0 || expansion > sWorld.getConfig(CONFIG_EXPANSION))
         return false;

    // No SQL injection
    LoginDatabase.PExecute("UPDATE account SET expansion = '%d' WHERE id = '%u'", expansion, account_id);
    PSendSysMessage(LANG_ACCOUNT_ADDON, expansion);
    return true;
}
	//Trinity Jail Edited by spgm
bool ChatHandler::HandleJailInfoCommand(const char* args)
{
    time_t localtime;
    localtime = time(NULL);
    Player *chr = m_session->GetPlayer();

    if (chr->m_jail_release > 0)
    {
        uint32 min_left = (uint32)floor(float(chr->m_jail_release - localtime) / 60);

        if (min_left <= 0)
        {
            chr->m_jail_release = 0;
            chr->_SaveJail();
            SendSysMessage(LANG_JAIL_NOTJAILED_INFO);
            return true;
        }
        else
        {
            if (min_left >= 60) PSendSysMessage(LANG_JAIL_JAILED_H_INFO, (uint32)floor(float(chr->m_jail_release - localtime) / 60 / 60));
            else PSendSysMessage(LANG_JAIL_JAILED_M_INFO, min_left);
            PSendSysMessage(LANG_JAIL_REASON, chr->m_jail_gmchar.c_str(), chr->m_jail_reason.c_str());

            return true;
        }
    }
    else
    {
        SendSysMessage(LANG_JAIL_NOTJAILED_INFO);
        return true;
    }
    return false;
}

bool ChatHandler::HandleAccountLockCommand(const char* args)
{
    if (!*args)
    {
        SendSysMessage(LANG_USE_BOL);
        return true;
    }

    std::string argstr = (char*)args;
    if (argstr == "on")
    {
        LoginDatabase.PExecute("UPDATE account SET locked = '1' WHERE id = '%d'",m_session->GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKLOCKED);
        return true;
    }

    if (argstr == "off")
    {
        LoginDatabase.PExecute("UPDATE account SET locked = '0' WHERE id = '%d'",m_session->GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKUNLOCKED);
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    return true;
}

//Playerbot mod
bool ChatHandler::HandlePlayerbotCommand(const char *args)
{
    if(!m_session)
    {
        PSendSysMessage("You may only add bots from an active session");
        SetSentErrorMessage(true);
        return false;
    }

    if(!*args)
    {
        PSendSysMessage("usage: add PLAYERNAME  or  remove PLAYERNAME");
        SetSentErrorMessage(true);
        return false;
    }

    char *cmd = strtok ((char*)args, " ");
    char *charname = strtok (NULL, " ");
    if(!cmd || !charname)
    {
        PSendSysMessage("usage: add PLAYERNAME  or  remove PLAYERNAME");
        SetSentErrorMessage(true);
        return false;
    }

    std::string cmdStr = cmd;
    std::string charnameStr = charname;
    uint64 guid;

   if (charnameStr.compare("all") != 0)
   {
       if (!normalizePlayerName(charnameStr))
           return false;

       guid = objmgr.GetPlayerGUIDByName(charnameStr.c_str());
       if (guid == 0 || (guid == m_session->GetPlayer()->GetGUID()))
       {
           SendSysMessage(LANG_PLAYER_NOT_FOUND);
           SetSentErrorMessage(true);
           return false;
       }

       uint32 accountId = objmgr.GetPlayerAccountIdByGUID(guid);
       if (accountId != m_session->GetAccountId())
       {
           PSendSysMessage("You may only add bots from the same account.");
           SetSentErrorMessage(true);
           return false;
       }
   }

    if (cmdStr.compare("add") == 0 || cmdStr.compare("login") == 0)
    {
        if (charnameStr.compare("all") == 0)
        {
            std::list<std::string> *names;
            names=m_session->GetPlayer()->GetCharacterList();
            std::list<std::string>::iterator iter,next;
            for (iter = names->begin(); iter != names->end(); iter++)
            {
                std::stringstream arg;
                arg << "add " << (*iter).c_str();
                HandlePlayerbotCommand(arg.str().c_str());
            }
            PSendSysMessage("Bots added successfully.");
            return true;
        }
        else
        {
            if(m_session->GetPlayerBot(guid) != NULL)
            {
                PSendSysMessage("Bot already exists in world.");
                SetSentErrorMessage(true);
                return false;
            }
            m_session->AddPlayerBot(guid);
        }

    }
    else if (cmdStr.compare("remove") == 0 || cmdStr.compare("logout") == 0)
    {
        if (charnameStr.compare("all") == 0)
        {
            std::list<std::string> *names = new std::list<std::string>;
            for (PlayerBotMap::const_iterator iter = m_session->GetPlayerBotsBegin(); iter != m_session->GetPlayerBotsEnd(); ++iter)
            {
                names->push_back(iter->second->GetName());
            }
            std::list<std::string>::iterator iter,next;
            for (iter = names->begin(); iter != names->end(); iter++)
            {
                std::stringstream arg;
                arg << "remove " << (*iter).c_str();
                HandlePlayerbotCommand(arg.str().c_str());
            }
            return true;
        }
        else
        {
            if (m_session->GetPlayerBot(guid) == NULL)
            {
                PSendSysMessage("Bot can not be removed because bot does not exst in world.");
                SetSentErrorMessage(true);
                return false;
            }
            m_session->LogoutPlayerBot(guid, true);
            PSendSysMessage("Bot removed successfully.");
            return true;
        }
    }
    return true;
}

bool ChatHandler::HandlePlayerbotMainTankCommand(const char *args)
{
    uint64 guid = 0;
    uint64 pGuid = 0;
    char *charname ;
    Group *group = m_session->GetPlayer()->GetGroup();
    
    if (group == NULL) {
        PSendSysMessage("Must be in a group to set a main tank.");
        SetSentErrorMessage(true);
        return false;
    }

    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT memberGuid FROM group_member WHERE memberFlags='%u' AND guid = '%u'",MEMBER_FLAG_MAINTANK, group->GetGUID());
    if(result)
    {    
        pGuid = MAKE_NEW_GUID(result->Fetch()->GetInt32(),0,HIGHGUID_PLAYER);
    }

    // if no arguments are passed in, just say who the current main tank is
    if(!*args) {

        if (pGuid>0) {
            Player *pPlayer = objmgr.GetPlayer(pGuid);

            if (pPlayer  && pPlayer->isAlive()){
                PSendSysMessage("Main tank is %s.", pPlayer->GetName());
                return true;
            }
        }    

        PSendSysMessage("Currently there is no main tank. ");
        return true;
    } else {
        charname = strtok ((char*)args, " ");
        std::string charnameStr = charname;
        guid = objmgr.GetPlayerGUIDByName(charnameStr.c_str());

        // clear if same player
        if (pGuid==guid) {
            group->SetMainTank(guid, false);
            PSendSysMessage("Main tank has been cleared. ");
            return true;
        }

        if (m_session->GetPlayer()->GetGroup()->IsMember(guid)) {
            group->SetMainTank(pGuid,false); // clear old one
            group->SetMainTank(guid, true);  // set new one
            Player *pPlayer = objmgr.GetPlayer(guid);
            if (pPlayer->IsInWorld())
                PSendSysMessage("Main tank is %s.", pPlayer->GetName());
            else
                PSendSysMessage("Player is not online.");
           
        } else {
            PSendSysMessage("Player is not in your group.");
        }

    }


    return true;
}


/// Display the 'Message of the day' for the realm
bool ChatHandler::HandleServerMotdCommand(const char* /*args*/)
{
    PSendSysMessage(LANG_MOTD_CURRENT, sWorld.GetMotd());
    return true;
}

