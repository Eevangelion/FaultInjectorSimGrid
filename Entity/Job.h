#ifndef _JOB_H
#define _JOB_H

#include <simgrid/s4u.hpp>
#include <vector>
#include <string>
#include "Worker.h"
#include "../Controller/Controller.h"

class Worker;
class Controller;
class Job
{
private:
    struct Message
    {
        int process;
    };

    struct PrintedData
    {
        double time;
        double messageTime;
        std::string receiverName;
    };

    int id;
    int numHosts;
    int killed;
    unsigned long long computationCost;
    unsigned long long communicationCost;
    std::vector<std::vector<Message>> messages;
    std::vector<PrintedData> messagesTime;
    std::string topologyType;
    std::vector<std::shared_ptr<Worker>> workers;
    std::vector<double> waitTimes;
    std::vector<double> recoveryTimes;
    std::shared_ptr<Controller> controller;
    double time;
    int messagesSent;
    int messagesReceived;
    int faultAlgo;
    double timeout;
    int processes;
    double meanTimeout;
    double minTimeout;
    double maxTimeout;
    int rightGuessedCrashedNodes;
    int wrongGuessedCrashedNodes;

public:

    Job(int, int, int, int, std::string);
    void setFaultAlgo(int);
    void setController(std::shared_ptr<Controller>);
    int getFaultAlgo();
    double getTimeout();
    void updateTimeout(double);
    std::string getTopologyType();
    std::vector<std::vector<Message>> &getMessages();
    unsigned long long getCommCost();
    unsigned long long getCompCost();
    double getTotalTime();
    void addMessagesTime(double, double, std::string);
    std::vector<PrintedData> getMessagesTime();
    void addWorker(std::shared_ptr<Worker>);
    std::vector<std::shared_ptr<Worker>> getWorkers();
    void messageSent();
    void messageReceived();
    double getMeanRecoveryTime();
    void rightGuessed();
    void wrongGuessed();
    int getSentMessages();
    int getReceivedMessages();
    int getTotalMessages();
    int getKilledWorkersPercentage();
    int getSendOmissionWorkersPercentage();
    int getReceiveOmissionWorkersPercentage();
    int getRightGuessesCount();
    int getWrongGuessesCount();
    int getDeadWorkersCount();
    int getDoneWorkersCount();
    void addWaitTime(double);
    std::shared_ptr<Worker> getWorkerByHost(std::string);
    bool resurrectActor(std::string, std::shared_ptr<Worker>);
    bool readyToFinish();

protected:
};

#endif