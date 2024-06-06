#ifndef _TASK_H
#define _TASK_H
#include <simgrid/s4u.hpp>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <random>
#include <xbt/log.h>
#include "Job.h"
#include "Worker.h"

class Job;
class Worker;
class Task
{
private:
    std::string hostName;
    std::shared_ptr<Job> job;
    std::shared_ptr<Worker> worker;
    struct Message
    {
        std::string targetHostName;
        int computationCost;
        int communicationCost;
    };

    std::vector<Message *> messages;

public:
    struct Payload
    {
        double senderTime;
        double compCost;
    };
    Task(std::string, std::shared_ptr<Job>, std::shared_ptr<Worker>);
    void operator()();
    void addMessage(std::string, int, int);
    void sendMessage(Message *);
    void receiveSomething();
    void receiveMessage(Payload);
    void receiveHeartbeat(bool);
    std::string getHostName();

protected:
};

#endif