#ifndef _WORKER_H
#define _WORKER_H

#include <memory>
#include <variant>
#include <random>
#include <simgrid/s4u.hpp>
#include "Task.h"

class Task;

class Worker {
private:
    std::shared_ptr<Task> task;
    bool killed;
    bool skipReceive;
    bool skipSend;
    int sendCount;
    int receivedCount;
    bool doneSending;
    int lastSent;
    bool alive;
    double deadSince;
public:
    Worker();
    void operator()();
    void setTask(std::shared_ptr<Task>);
    void makeKilled();
    void makeSkipReceive();
    void makeSkipSend();
    void messageSent();
    void messageReceived();
    void done();
    bool isAlive();
    void makeDead();
    void makeAlive();
    bool isDone();
    double getDeadSince();
    int getSendCount();
    int getReceivedCount();
    int getLastSent();
    bool isKilled();
    bool isSkipReceive();
    bool isSkipSend();
    std::string getHostName();
protected:
};

#endif