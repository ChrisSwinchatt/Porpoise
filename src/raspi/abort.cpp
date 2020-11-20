#include <porpoise/abort.hpp>
#include <porpoise/io/logging.hpp>

namespace porpoise
{
    extern "C" void abort()
    {
        PORPOISE_LOG_ERROR(
            "*** Aborted ***\n\n"
            "Abort was called - there should some error messages above this one explaining why.\n\n"
            "Debugging information will be displayed and then the computer will hang."
        );

        // TODO log debug info.

        while (true)
            ;
    }
}
