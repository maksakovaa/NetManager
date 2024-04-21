#include "reqAPI.h"

ReqAPI::ReqAPI()
{
}

ReqAPI::~ReqAPI()
{
}

void ReqAPI::spellOut(char req[])
{
    strcpy(request, req);
    if (strncmp("AUTH", request, sizeof("AUTH") - 1) == 0)
    {
        authCheck();
    }
    
    if (strncmp("GET_USRBASE", request, sizeof("GET_USRBASE") - 1) == 0)
	{
		sendUsrBase();
	}
	else if (strncmp("GET_PER_MSGBASE", request, sizeof("GET_PER_MSGBASE") - 1) == 0)
	{
		sendPerMsgBase();
	}
	else if (strncmp("GET_ALL_MSGBASE", request, sizeof("GET_ALL_MSGBASE") - 1) == 0)
	{
		sendAllMsgBase();
	}
	else if (strncmp("REG_USER", request, sizeof("REG_USER") - 1) == 0)
	{
		regUser();
	}
	else if (strncmp("SND_MSG", request, sizeof("SND_MSG") - 1) == 0)
	{
		regMSG();
	}
	else if (strncmp("DEL_USER", request, sizeof("DEL_USER") - 1) == 0)
	{
		delUsr();
	}
	else if (strncmp("CHG_PWD", request, sizeof("CHG_PWD") - 1) == 0)
	{
		chgPwd();
	}
	else if (strncmp("SET_PMSG_STATUS", request, sizeof("SET_PMSG_STATUS") - 1) == 0)
	{
		setPMStatus();
	}
	else if (strncmp("SET_AMSG_STATUS", request, sizeof("SET_AMSG_STATUS") - 1) == 0)
	{
		setAMStatus();
	}
}

void ReqAPI::authCheck()
{

}
void ReqAPI::sendUsrBase()
{

}
void ReqAPI::sendPerMsgBase()
{

}
void ReqAPI::sendAllMsgBase()
{

}
void ReqAPI::regUser()
{

}
void ReqAPI::regMSG()
{

}
void ReqAPI::delUsr()
{

}
void ReqAPI::chgPwd()
{

}
void ReqAPI::setPMStatus()
{

}
void ReqAPI::setAMStatus()
{
    
}