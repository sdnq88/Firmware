#include "svc_globals.hpp"

namespace BT
{
namespace Globals
{
namespace Service
{

bool pairing_on = false;
bool drop_all_connections_flag = false;
bool reset_module_flag = false;

void
turn_pairing_on(){
    pairing_on = true;
}

void
turn_pairing_off(){
    pairing_on = false;
}

void
toggle_pairing(){
    pairing_on = !pairing_on;
}

bool
get_pairing_status(){
    return pairing_on;
}

void
drop_all_connections(){
    drop_all_connections_flag = true;
}

void
drop_all_connections_done(){
    drop_all_connections_flag = false;
}

void
reset_module(){
    reset_module_flag = true;
}

void
reset_module_done(){
    reset_module_flag = false;
}

}
// end of namespace Service
}
// end of namespace Globals
}
// end of namespace BT
