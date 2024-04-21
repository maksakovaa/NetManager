#pragma once
#include "Logger.h"

class ReqAPI
{
private:
    char request[1024];
    void authCheck();
    void sendUsrBase();
    void sendPerMsgBase();
    void sendAllMsgBase();
    void regUser();
    void regMSG();
    void delUsr();
    void chgPwd();
    void setPMStatus();
    void setAMStatus();
public:
    ReqAPI();
    ~ReqAPI();
    void spellOut(char req[]);
};
