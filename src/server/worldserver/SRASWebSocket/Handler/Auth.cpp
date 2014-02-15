#include "SRASConnection.h"
#include "AccountMgr.h"
#include "DatabaseEnv.h"

void SRASConnection::AuthChallenge(SRASPacket pkt)
{
    std::string account = pkt.next();
    std::string sha = pkt.next();

    uint32 accountId = sAccountMgr->GetId(account);

    if(!accountId) //Acount not found
    {
        sLog->outError(LOG_FILTER_SRAS, "Account not found %s", account.c_str());
        SRASPacket resp;
        resp.add("1"); //Opcode
        resp.add("1"); //Error : account does not exist
        SendPacket(resp.finalize());
        return;
    }

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CHECK_PASSWORD);
    stmt->setUInt32(0, accountId);
    stmt->setString(1, sha);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if(!result)
    {
        sLog->outError(LOG_FILTER_SRAS, "Wrong password for %s", account.c_str());
        SRASPacket resp;
        resp.add(1);
        resp.add(2); //Error : Wrong password
        SendPacket(resp.finalize());
        return;
    }

    uint32 security = sAccountMgr->GetSecurity(accountId);

    if(security < SEC_ADMINISTRATOR)
    {
        sLog->outError(LOG_FILTER_SRAS, "Not enought security for %s", account.c_str());
        SRASPacket resp;
        resp.add(1);
        resp.add(3); //Error : Not enough access
        SendPacket(resp.finalize());
        return;
    }

    sLog->outInfo(LOG_FILTER_SRAS, "%s successfully authenticated", account.c_str());
    SRASPacket resp;
    resp.add(1);
    resp.add(0); //Error : Succes :-)
    SendPacket(resp.finalize());

    QueryResult persoList = CharacterDatabase.PQuery("SELECT guid, name FROM characters WHERE account=%u", accountId);

    if(!persoList)
        return;

    SRASPacket charList;
    charList.add(CHAR_LIST);
    charList.add(persoList->GetRowCount());

    int i = 0;
    do
    {
        Field *perso = persoList->Fetch();
        std::string name = perso[1].GetString();
        uint32 guid = perso[0].GetUInt32();

        charList.add(guid);
        charList.add(name);
        if(i == 0)
            m_currentGuid = guid;
        i++;
    }
    while(persoList->NextRow());

    SendPacket(charList.finalize());

    m_user = account;
}

void SRASConnection::SetCurrentGuid(SRASPacket pkt)
{
    pkt.next();
    m_currentGuid = pkt.toInt();
}
