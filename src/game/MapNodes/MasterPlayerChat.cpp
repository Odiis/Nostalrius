#include "MasterPlayer.h"
#include "Chat.h"
#include "Language.h"
#include "World.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "Player.h"

// ######################## CHAT SYSTEM    ###########################
void MasterPlayer::UpdateSpeakTime()
{
    // ignore chat spam protection for GMs in any mode
    if (GetSession()->GetSecurity() > SEC_PLAYER)
        return;

    time_t current = time(NULL);
    if (m_speakTime > current)
    {
        uint32 max_count = sWorld.getConfig(CONFIG_UINT32_CHATFLOOD_MESSAGE_COUNT);
        if (!max_count)
            return;

        ++m_speakCount;
        if (m_speakCount >= max_count)
        {
            // prevent overwrite mute time, if message send just before mutes set, for example.
            time_t new_mute = current + sWorld.getConfig(CONFIG_UINT32_CHATFLOOD_MUTE_TIME);
            if (GetSession()->m_muteTime < new_mute)
                GetSession()->m_muteTime = new_mute;

            m_speakCount = 0;
        }
    }
    else
        m_speakCount = 0;

    m_speakTime = current + sWorld.getConfig(CONFIG_UINT32_CHATFLOOD_MESSAGE_DELAY);
}


void MasterPlayer::Whisper(const std::string& text, uint32 language, MasterPlayer* receiver)
{
    if (language != LANG_ADDON)                             // if not addon data
        language = LANG_UNIVERSAL;                          // whispers should always be readable

    // when player you are whispering to is dnd, he cannot receive your message, unless you are in gm mode
    if (!receiver->isDND() || isGameMaster())
    {
        WorldPacket data(SMSG_MESSAGECHAT, 100);
        Player::BuildPlayerChat(GetObjectGuid(), chatTag(), &data, CHAT_MSG_WHISPER, text, language);
        receiver->GetSession()->SendPacket(&data);

        // not send confirmation for addon messages
        if (language != LANG_ADDON)
        {
            data.Initialize(SMSG_MESSAGECHAT, 100);
            Player::BuildPlayerChat(receiver->GetObjectGuid(), receiver->chatTag(), &data, CHAT_MSG_WHISPER_INFORM, text, language);
            GetSession()->SendPacket(&data);
        }
        ALL_SESSION_SCRIPTS(receiver->GetSession(), OnWhispered(GetObjectGuid()));
    }
    else
    {
        // announce to player that player he is whispering to is dnd and cannot receive his message
        ChatHandler(GetSession()).PSendSysMessage(LANG_PLAYER_DND, receiver->GetName(), receiver->dndMsg.c_str());
    }

    if (!IsAcceptWhispers())
        AddAllowedWhisperer(receiver->GetObjectGuid());

    // announce to player that player he is whispering to is afk
    if (receiver->isAFK())
        ChatHandler(GetSession()).PSendSysMessage(LANG_PLAYER_AFK, receiver->GetName(), receiver->afkMsg.c_str());

    // if player whisper someone, auto turn of dnd to be able to receive an answer
    if (isDND() && !receiver->isGameMaster())
        ToggleDND();
}

void MasterPlayer::ToggleDND()
{
    if (m_chatTag == 2)
        m_chatTag = 0;
    else
        m_chatTag = 2;
}

void MasterPlayer::ToggleAFK()
{
    if (m_chatTag == 1)
        m_chatTag = 0;
    else
        m_chatTag = 1;
}

void MasterPlayer::JoinedChannel(Channel *c)
{
    m_channels.push_back(c);
}

void MasterPlayer::LeftChannel(Channel *c)
{
    m_channels.remove(c);
}

void MasterPlayer::CleanupChannels()
{
    while (!m_channels.empty())
    {
        Channel* ch = *m_channels.begin();
        m_channels.erase(m_channels.begin());               // remove from player's channel list
        ch->Leave(GetObjectGuid(), false);                  // not send to client, not remove from player's channel list
        if (ChannelMgr* cMgr = channelMgr(GetTeam()))
            cMgr->LeftChannel(ch->GetName());               // deleted channel if empty
    }
}
