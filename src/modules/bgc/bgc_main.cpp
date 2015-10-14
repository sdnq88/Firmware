#include <nuttx/config.h>

#include <quick_log/quick_log.hpp>
#include <string.h>

#include "bgc.hpp"

extern "C" __EXPORT int bgc_main(int argc, char *argv[]);

int bgc_main(int argc, char *argv[]) {
    if ( argc == 2 && !strcmp(argv[1], "start") ) {
        if ( !BGC::BGC::Start_thread() ) {
            return 2;
        }
    } else if ( argc == 2 && !strcmp(argv[1], "stop") ) {
        if ( !BGC::BGC::Stop_thread() ) {
            return 3;
        }
    } else if ( argc == 2 && !strcmp(argv[1], "test") ) {
        if ( !BGC::BGC::Factory_check(false) ) {
            return 4;
        }
    } else if ( argc == 2 && !strcmp(argv[1], "test-quick") ) {
        if ( !BGC::BGC::Factory_check(true) ) {
            return 4;
        }
    } else {
        QLOG_literal("[BGC] bad usage of bgc");
        printf("usage: bgc [start|stop|test|test-quick]\n");
        return 1;
    }

    return 0;
}
