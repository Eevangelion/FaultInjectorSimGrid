#include "Task.h"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(test);
Task::Task(std::string hostName, std::shared_ptr<Job> job, std::shared_ptr<Worker> w) : hostName(hostName), job(job), worker(w)
{
}

void Task::sendMessage(Message *m)
{
    if (!worker->isSkipSend() || worker->getSendCount() % 4)
    {
        simgrid::s4u::Mailbox *mailbox = simgrid::s4u::Mailbox::by_name(m->targetHostName);
        Payload payload;
        payload.senderTime = simgrid::s4u::Engine::get_clock();
        payload.compCost = m->computationCost;
        std::variant<Payload, bool>* msg = new std::variant<Payload, bool>(payload);
        simgrid::s4u::CommPtr putComm = mailbox->put_async(msg, m->communicationCost);
        double start = simgrid::s4u::Engine::get_clock();
        double timeout;
        if (job->getTopologyType() == "GRID" || job->getTopologyType() == "THIN_TREE") { 
            timeout = 0.2;
        } else timeout = 0.1;
        if (job->getFaultAlgo() == 1) {
            timeout = 0.5;
        }
        try
        {
            putComm->wait_for(timeout);
        }
        catch (const simgrid::TimeoutException &e)
        {
            XBT_INFO("Failed to send message from %s to %s", simgrid::s4u::this_actor::get_host()->get_cname(), m->targetHostName.c_str());
            // connection was not established, but technically message was sent
            job->messageSent();
            worker->messageSent();
            switch (job->getFaultAlgo()) {
            case 0:
            {
                double waitTime = simgrid::s4u::Engine::get_clock() - start;
                break;
            }
            case 1:
            {
                if (job->resurrectActor(m->targetHostName, job->getWorkerByHost(m->targetHostName))) {
                    job->rightGuessed();
                    simgrid::s4u::ActorPtr a = simgrid::s4u::Host::by_name(m->targetHostName)->get_all_actors()[0];
                    mailbox->set_receiver(a);
                    a->resume();
                    job->getWorkerByHost(m->targetHostName)->makeAlive();
                }
                else job->wrongGuessed();
                job->addWaitTime(2);
                break;
            }
            }
            return;
        }
        double waitTime = simgrid::s4u::Engine::get_clock() - start;
        job->addWaitTime(waitTime);
        job->updateTimeout(waitTime);
        // XBT_INFO("Sugoma) %f", waitTime);
        job->messageSent();
    }
    worker->messageSent();
}

void Task::receiveSomething() {
    std::variant<Payload, bool> * data;
    simgrid::s4u::Mailbox *currentMailbox = simgrid::s4u::Mailbox::by_name(hostName);
    simgrid::s4u::CommPtr getComm = currentMailbox->get_async(&data);
    double timeout;
    if (job->getTopologyType() == "GRID" || job->getTopologyType() == "THIN_TREE") { 
        timeout = 0.2;
    } else timeout = 0.1;
    if (job->getFaultAlgo() == 1) {
        timeout = 0.5;
    }
    try {
        getComm->wait_for(timeout);
    } catch (const simgrid::TimeoutException &e)
    {
        XBT_INFO("Host %s is failed to got message", simgrid::s4u::this_actor::get_host()->get_cname());
        return;
    }
    if (std::holds_alternative<Payload>(*data)) {
        receiveMessage(std::get<Payload>(*data));
    } else {
        receiveHeartbeat(std::get<bool>(*data));
    }
}

void Task::receiveMessage(Payload data)
{
    if (!worker->isSkipReceive() || worker->getReceivedCount() % 4)
    {
        double commTime = simgrid::s4u::Engine::get_clock() - data.senderTime;
        job->addMessagesTime(simgrid::s4u::Engine::get_clock(), commTime, simgrid::s4u::this_actor::get_name());
        simgrid::s4u::this_actor::execute(data.compCost);
        job->messageReceived();
    }
    worker->messageReceived();
}

void Task::receiveHeartbeat(bool data) {
    if (data) {
        // XBT_INFO("Got it!");
    }
}
void Task::operator()()
{
    double liveTime = 1 + simgrid::s4u::this_actor::get_pid() / (10 + simgrid::s4u::this_actor::get_pid());
    double timeToDie = simgrid::s4u::Engine::get_clock() + 1.5;
    XBT_INFO("Worker %s is up!", hostName.c_str());
    simgrid::s4u::Mailbox *currentMailbox = simgrid::s4u::Mailbox::by_name(hostName);
    currentMailbox->set_receiver(simgrid::s4u::Actor::by_pid(simgrid::s4u::this_actor::get_pid()));    
    std::random_device rd; 
    std::mt19937 gen(rd());
    for (int i = 0; i < messages.size(); ++i)
    {
        Message *m = messages[i];
        if (worker->isKilled())
        {
            if (simgrid::s4u::Engine::get_clock() >= timeToDie) {
                XBT_INFO("Worker %s has died!", simgrid::s4u::this_actor::get_cname());
                currentMailbox->set_receiver(nullptr);
                worker->makeDead();
                simgrid::s4u::this_actor::suspend();
                worker->makeAlive();
                timeToDie = simgrid::s4u::Engine::get_clock() + 45678909876543;
            }
        }   
        // PUT
        sendMessage(m);
        // GET
        while (currentMailbox->ready()) receiveSomething();
    }
    worker->done();
    XBT_INFO("Worker done!");
    XBT_INFO("Workers dead: %d, workers done: %d", job->getDeadWorkersCount(), job->getDoneWorkersCount());
    while (!job->readyToFinish())
    {
        simgrid::s4u::this_actor::sleep_for(0.001);
        while (currentMailbox->ready()) receiveSomething();
    }
    while (currentMailbox->ready()) receiveSomething();
    currentMailbox->set_receiver(nullptr);
}

void Task::addMessage(std::string targetHostName, int computationCost, int communicationCost)
{
    messages.push_back(new Message{targetHostName, computationCost, communicationCost});
}

std::string Task::getHostName()
{
    return hostName;
}