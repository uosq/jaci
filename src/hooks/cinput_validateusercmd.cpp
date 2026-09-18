#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../logging/log.hpp"
#include "../abstract/ihookmanager.hpp"

INIT_HOOK(CInput_ValidateUserCmd, void, (void* input, void* cmd, int sequence_number), "client.so", "55 48 89 E5 41 56 41 89 D6 41 55 49 89 FD 41 54 4C 8D 65 DC")
{

}