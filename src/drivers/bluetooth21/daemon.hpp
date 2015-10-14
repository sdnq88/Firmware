#pragma once

#include <cstdio>

namespace BT
{
namespace Daemon
{

namespace Main
{

extern const char
PROCESS_NAME[];

bool
is_running();

bool
has_started();

void
start(const char * argv[]);

void
report_status(FILE *);

void
request_stop();

enum class Maintenance
{
	FIRMWARE_VERSION,
	LOCAL_ADDRESS,
};

int
maintenance(const char ttyname[], Maintenance op);

}
// end of namespace Main

namespace Multiplexer
{

extern const char
PROCESS_NAME[];

bool
is_running();

bool
has_started();

bool
start(const char uart_dev_name[]);

void
join();

void
report_status(FILE *);

void
request_stop();

}
// end of namespace Multiplexer

namespace Service
{

extern const char
PROCESS_NAME[];

bool
is_running();

bool
has_started();

bool
start(const char mode[], const char address_no[]);

void
join();

void
report_status(FILE *);

void
request_stop();

bool
check_version_firmware();

bool
local_address();

}
// end of namespace Service

}
// end of namespace Daemon
}
// end of namespace BT
