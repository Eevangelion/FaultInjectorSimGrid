#ifndef _DEPLOY_MANAGER_H
#define _DEPLOY_MANAGER_H
#include <simgrid/s4u.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "../Utils/json.hpp"
#include "../Entity/Job.h"
#include "../Entity/Task.h"
#include "../Entity/Worker.h"
#include "../FaultInjector/FaultInjector.h"
#include "../Controller/Controller.h"

class Job;
class Task;

class DeployManager
{
private:
    DeployManager();
    static std::shared_ptr<DeployManager> instance;
    simgrid::s4u::Engine *engine;
    std::shared_ptr<FaultInjector> faultInjector;
    std::shared_ptr<Controller> controller;
    std::vector<std::shared_ptr<Job>> jobs;
    std::vector<std::shared_ptr<Task>> tasks;
    struct MessageStatistic
    {
        int sent = 0;
        int received = 0;
        int total = 0;
    };

public:
    DeployManager(DeployManager &) = delete;
    void operator=(DeployManager &) = delete;
    static std::shared_ptr<DeployManager> getInstance();
    void deploy();
    void setEngine(simgrid::s4u::Engine *);
    void saveResultsMessageStatistic(std::string);
    void saveResultsTotalActorSkipSend(std::string);
    void saveResultsTotalActorSkipReceive(std::string);
    void saveResultsTotalActorKilled(std::string);
    void saveResults(std::string);

protected:
};

#endif