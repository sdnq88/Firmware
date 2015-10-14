#pragma once

namespace BT
{
namespace Globals
{
namespace Service
{

extern bool pairing_on;
extern bool drop_all_connections_flag;
extern bool reset_module_flag;

void turn_pairing_on();
void turn_pairing_off();
void toggle_pairing();
bool get_pairing_status();
void drop_all_connections();
void drop_all_connections_done();
void reset_module();
void reset_module_done();

}
// end of namespace Service
}
// end of namespace Globals
}
// end of namespace BT
