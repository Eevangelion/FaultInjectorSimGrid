#ifndef _FI
#define _FI
#include <memory>
#include <fstream>
#include <simgrid/s4u.hpp>
#include <random>
#include "../Utils/json.hpp"
#include "../Entity/Worker.h"

class FaultInjector
{
private:
    enum FaultType
    {
        CRASH_FAILURE,
        RECEIVE_OMISSION,
        SEND_OMISSION,
        TIMING_FAILURE
    };
    FaultInjector();
    static std::shared_ptr<FaultInjector> instance;

public:
    FaultInjector(FaultInjector &) = delete;
    void operator=(FaultInjector &) = delete;
    static std::shared_ptr<FaultInjector> getInstance();
    void inject(std::vector<std::shared_ptr<Job>> &);
protected:
};

#endif
