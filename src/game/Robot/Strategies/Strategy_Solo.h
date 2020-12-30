#ifndef ROBOT_STRATEGY_SOLO_H
#define ROBOT_STRATEGY_SOLO_H

#include "Strategy_Base.h"

enum RobotSoloState :uint32
{
    RobotSoloState_None = 0,
    RobotSoloState_Wander,
    RobotSoloState_Battle,
    RobotSoloState_Rest,
    RobotSoloState_Wait,
    RobotSoloState_Stroll,
    RobotSoloState_Confuse,
};

class Strategy_Solo :public Strategy_Base
{
public:
    Strategy_Solo(Player* pmMe) :Strategy_Base(pmMe)
    {
        InitialStrategy();
    }
    void InitialStrategy();
    void Reset();
    void HandleChatCommand(Player* pmSender, std::string pmCMD);

    void Update(uint32 pmDiff) override;

public:

};
#endif
