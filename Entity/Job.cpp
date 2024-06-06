#include "Job.h"
XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(test);

Job::Job(int numHosts, int processes, int computationCost, int communicationCost, std::string topologyType)
    : numHosts(numHosts), processes(processes), computationCost(computationCost), communicationCost(communicationCost), topologyType(topologyType)
{
    messagesReceived = 0;
    messagesSent = 0;
    rightGuessedCrashedNodes = 0;
    wrongGuessedCrashedNodes = 0;
    timeout = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
    messages.resize(numHosts);
    for (int i = 0; i < numHosts; ++i)
    {
        for (int j = 0; j < numHosts; ++j)
        {
            if (i == j) continue;
            for (int k = 0; k < processes / numHosts; ++k)
                messages[i].push_back({j});
            if (i < processes % numHosts) {
                messages[i].push_back({j});
            }
        }
        std::shuffle(messages[i].begin(), messages[i].end(), gen);
    }
}

std::string Job::getTopologyType() {
    return topologyType;
}

void Job::setFaultAlgo(int fa)
{
    faultAlgo = fa;
}

int Job::getFaultAlgo()
{
    return faultAlgo;
}

void Job::rightGuessed() {
    ++rightGuessedCrashedNodes;
}

void Job::wrongGuessed() {
    ++wrongGuessedCrashedNodes;
}

std::vector<std::vector<Job::Message>> &Job::getMessages()
{
    return messages;
}

unsigned long long Job::getCommCost()
{
    return communicationCost;
}

unsigned long long Job::getCompCost()
{
    return computationCost;
}

double Job::getTotalTime()
{
    return time;
}

void Job::addMessagesTime(double t, double mt, std::string actorName)
{
    messagesTime.push_back({t, mt, actorName});
}

std::vector<Job::PrintedData> Job::getMessagesTime()
{
    return messagesTime;
}

void Job::addWorker(std::shared_ptr<Worker> a)
{
    workers.push_back(a);
}

std::vector<std::shared_ptr<Worker>> Job::getWorkers()
{
    return workers;
}

std::shared_ptr<Worker> Job::getWorkerByHost(std::string hostName)
{
    for (auto &w : workers)
        if (w->getHostName() == hostName)
        {
            return w;
        }
    return nullptr;
}

void Job::addWaitTime(double time) {
    waitTimes.push_back(time);
}

void Job::messageSent()
{
    ++messagesSent;
}
void Job::messageReceived()
{
    ++messagesReceived;
}

int Job::getSentMessages()
{
    return messagesSent;
}
int Job::getReceivedMessages()
{
    return messagesReceived;
}
int Job::getTotalMessages()
{
    int total = 0;
    for (auto & m : messages) {
        total += m.size();
    }
    return total;
}

int Job::getKilledWorkersPercentage()
{
    int total = workers.size();
    int killed = 0;
    for (auto &w : workers)
    {
        killed += w->isKilled();
    }
    return (int)std::ceil(killed * 100. / total);
}

int Job::getSendOmissionWorkersPercentage()
{
    int total = workers.size();
    int skipSend = 0;
    for (auto &w : workers)
    {
        skipSend += w->isSkipSend();
    }
    return (int)std::ceil(skipSend * 100. / total);
}

int Job::getReceiveOmissionWorkersPercentage()
{
    int total = workers.size();
    int skipReceive = 0;
    for (auto &w : workers)
    {
        skipReceive += w->isSkipReceive();
    }
    return (int)std::ceil(skipReceive * 100. / total);
}

void Job::setController(std::shared_ptr<Controller> c) {
    controller = c;
}

double Job::getMeanRecoveryTime() {
    if (recoveryTimes.size() == 0) return 0;
    return std::accumulate(recoveryTimes.begin(), recoveryTimes.end(), 0.0) / recoveryTimes.size();
}

bool Job::resurrectActor(std::string hostName, std::shared_ptr<Worker> w)
{
    if (w->isAlive()) {
        return false;
    }
    double recoverTime = simgrid::s4u::Engine::get_clock() - w->getDeadSince();
    recoveryTimes.push_back(recoverTime);
    XBT_INFO("Actor restarted");
    return true;
}

double Job::getTimeout()
{
    return timeout;
}

int Job::getDeadWorkersCount() {
    int res = 0;
    for (auto & w : workers) {
        res += !w->isAlive();
    }
    return res;
}
int Job::getDoneWorkersCount() {
    int res = 0;
    for (auto & w : workers) {
        res += w->isDone();
    }
    return res;
}

void Job::updateTimeout(double t) {
    timeout = (timeout + t) / 2.; 
}

bool Job::readyToFinish()
{
    return workers.size() == getDeadWorkersCount() + getDoneWorkersCount();
}

int Job::getRightGuessesCount() {
    return rightGuessedCrashedNodes;
}

int Job::getWrongGuessesCount() {
    return wrongGuessedCrashedNodes;
}