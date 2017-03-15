#include "Anticheat.h"

AnticheatLibInterface* GetAnticheatLib()
{
    static AnticheatDefaultLib l;
    return &l;
}