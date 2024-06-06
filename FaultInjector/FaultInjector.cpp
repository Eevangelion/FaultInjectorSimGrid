#include "FaultInjector.h"
XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(test);

std::shared_ptr<FaultInjector> FaultInjector::instance = nullptr;

FaultInjector::FaultInjector()
{
}

std::shared_ptr<FaultInjector> FaultInjector::getInstance()
{
    if (instance.get() == 0)
    {
        instance = std::shared_ptr<FaultInjector>(new FaultInjector());
    }
    return instance;
}

void FaultInjector::inject(std::vector<std::shared_ptr<Job>> &jobs)
{
    std::fstream fin("faults.json");
    nlohmann::json json = nlohmann::json::parse(fin);
    std::random_device rd;
    std::mt19937 gen(rd());
    srand(5003);
    for (auto &fault : json["faults"])
    {
        FaultType faultType = fault["faultType"];
        int job = fault["job"];
        int perc = fault["percentage"];
        int faultAlgo = fault["faultAlgo"];
        jobs[job]->setFaultAlgo(faultAlgo);
        XBT_INFO("Fault algo is setted");
        std::vector<int> toInject(jobs[job]->getWorkers().size());
        for (int i = 0; i < toInject.size(); ++i)
            toInject[i] = i;
        int injectedCnt = std::ceil(jobs[job]->getWorkers().size() * perc / 100.);
        int j = toInject.size() - injectedCnt;
        if (j == 0) ++j;
        for (int i = toInject.size() - 1; i >= j; --i)
        {
            // std::uniform_int_distribution<int> dis(0, i - 1);
            // std::swap(toInject[i], toInject[dis(gen)]);
            std::swap(toInject[i], toInject[rand() % i]);
        }
        XBT_INFO("Injected nodes selected");
        switch (faultType)
        {
        case CRASH_FAILURE:
        {
            for (int i = 0; i < jobs[job]->getWorkers().size() * perc / 100; ++i)
            {
                XBT_INFO("Injected crash failure into %s", jobs[job]->getWorkers()[toInject[toInject.size() - i - 1]]->getHostName().c_str());

                jobs[job]->getWorkers()[toInject[toInject.size() - i - 1]]->makeKilled();
            }
            break;
        }
        case RECEIVE_OMISSION:
        {
            for (int i = 0; i < jobs[job]->getWorkers().size() * perc / 100; ++i)
            {
                jobs[job]->getWorkers()[toInject[toInject.size() - i - 1]]->makeSkipReceive();
            }
            break;
        }
        case SEND_OMISSION:
        {
            for (int i = 0; i < jobs[job]->getWorkers().size() * perc / 100; ++i)
            {
                jobs[job]->getWorkers()[toInject[toInject.size() - i - 1]]->makeSkipSend();
            }
            break;
        }
        case TIMING_FAILURE:
        {
            for (int i = 0; i < jobs[job]->getWorkers().size() * perc / 100; ++i)
            {
                // jobs[job]->getWorkers()[toInject[toInject.size() - i - 1]]->makeKilled();
            }
            break;
        }
        }
        XBT_INFO("Injected fault type %d in job #%d", faultType, job);
    }
}