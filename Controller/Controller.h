#ifndef _CONTROLLER_H
#define _CONTROLLER_H
#include <simgrid/s4u.hpp>
#include <memory>
#include "../Entity/Job.h"

class Job;

class Controller {
private:
    std::string* hostName;
    std::shared_ptr<Job> job;
public:
    Controller();
    void startSendingHeartbeats();
    void operator()();
    void setJob(std::shared_ptr<Job>);
protected:
};

#endif