#ifndef NINGER_ENTITY_H
#define NINGER_ENTITY_H

enum NingerEntityState :uint32
{
    NingerEntityState_None = 0,
    NingerEntityState_OffLine,
    NingerEntityState_Enter,
    NingerEntityState_CheckAccount,
    NingerEntityState_CreateAccount,
    NingerEntityState_CheckCharacter,
    NingerEntityState_CreateCharacter,    
    NingerEntityState_CheckLogin,
    NingerEntityState_DoLogin,
    NingerEntityState_Initialize,
    NingerEntityState_Online,
    NingerEntityState_Exit,
    NingerEntityState_CheckLogoff,
    NingerEntityState_DoLogoff,
};

class NingerEntity
{
public:
    NingerEntity(uint32 pmNingerID);
    void Update(uint32 pmDiff);

public:
    uint32 ninger_id;
    uint32 account_id;
    std::string account_name;
    uint32 character_id;
    uint32 target_level;
    int checkDelay;
    uint32 entityState;
};
#endif
