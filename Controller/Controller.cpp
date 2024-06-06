#include "Controller.h"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(test);


Controller::Controller()
{
}


void Controller::startSendingHeartbeats() {
    for (auto w : job->getWorkers()) {
        if (w->isDone() && !w->isAlive()) {
            XBT_INFO("Impossible happened");
        }
        if (w->isDone()) continue;
        simgrid::s4u::Mailbox* mailbox = simgrid::s4u::Mailbox::by_name(w->getHostName());
        std::string* response;
        // XBT_INFO("Try to send %s", hostName->c_str());
        std::variant<Task::Payload, bool>* msg = new std::variant<Task::Payload, bool>(true);
        try {
            mailbox->put_async(msg, sizeof(void*))->wait_for(0.01);
        } catch (simgrid::TimeoutException&) {
            // seems that this actor is dead
            // XBT_INFO("Failed to check %s", w->getHostName().c_str());
            if (job->resurrectActor(w->getHostName(), w)) {
                job->rightGuessed();
                simgrid::s4u::ActorPtr a = simgrid::s4u::Host::by_name(w->getHostName())->get_all_actors()[0];
                mailbox->set_receiver(a);
                a->resume();
                w->makeAlive();
            }
            else job->wrongGuessed();
            continue;
        }
        // XBT_INFO("Success");
    }
}

void Controller::setJob(std::shared_ptr<Job> j) {
    job = j;
}

void Controller::operator()() {
    hostName = new std::string(simgrid::s4u::this_actor::get_host()->get_name());
    XBT_INFO("I've been there!, Host name is %s", hostName->c_str());
    while (!job->readyToFinish()) {
        startSendingHeartbeats();
        simgrid::s4u::this_actor::sleep_for(0.001);
    }
}