#ifndef NINGER_AWARENESS_SCRIPT_SHAMAN_H
#define NINGER_AWARENESS_SCRIPT_SHAMAN_H

#include "Script_Base.h"

enum ShamanEarthTotemType :uint32
{
    ShamanEarthTotemType_EarthbindTotem = 0,
    ShamanEarthTotemType_StoneskinTotem,
    ShamanEarthTotemType_StoneclawTotem,
    ShamanEarthTotemType_StrengthOfEarthTotem
};

class Script_Shaman :public Script_Base
{
public:
    Script_Shaman(Player* pmMe);

    uint32 earthTotemType;
};
#endif
