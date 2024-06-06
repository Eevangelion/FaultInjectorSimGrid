#include "DeployManager.h"
XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(test);

std::shared_ptr<DeployManager> DeployManager::instance = nullptr;

DeployManager::DeployManager()
{
}

std::shared_ptr<DeployManager> DeployManager::getInstance()
{
    if (instance.get() == 0)
    {
        instance = std::shared_ptr<DeployManager>(new DeployManager());
    }
    return instance;
}

void DeployManager::deploy()
{
    std::ifstream fin("jobs.json");
    nlohmann::json json = nlohmann::json::parse(fin);
    for (auto &job : json["jobs"])
    {
        std::string topology = job["topologyType"];
        int processes = job["processes"];
        unsigned long long computationCost = job["computationCost"];
        unsigned long long communicationCost = job["communicationCost"];
        simgrid::s4u::NetZone* nz;
        for (auto &nnz : engine->get_netzone_root()->get_children()) {
            if (nnz->get_name() == topology) nz = nnz;
        }
        int numHosts = nz->get_all_hosts().size() - 1;

        std::shared_ptr<Job> j = std::shared_ptr<Job>(new Job(numHosts, processes, computationCost, communicationCost, topology));
        jobs.push_back(j);
    }
    XBT_INFO("Total jobs: %ld", jobs.size());
    fin.close();
    for (auto &job : jobs)
    {
        simgrid::s4u::NetZone* nz;
        for (auto &nnz : engine->get_netzone_root()->get_children()) {
            if (nnz->get_name() == job->getTopologyType()) nz = nnz;
        }
        for (auto &nnz : nz->get_children()) {
            XBT_INFO("%s", nnz->get_cname());
        }
        if (!nz) {
            XBT_INFO("Can't find net zone");
            return;
        }
        std::vector<simgrid::s4u::Host *> hosts = nz->get_all_hosts();
        std::sort(hosts.begin(), hosts.end(), [](simgrid::s4u::Host *a, simgrid::s4u::Host *b)
              {
            if (a->get_name().size() == b->get_name().size()) return a->get_name() < b->get_name();
            return a->get_name().size() < b->get_name().size(); });
        hosts.pop_back();
        for (int i = 0; i < hosts.size(); ++i)
        {
            std::shared_ptr<Worker> w = std::shared_ptr<Worker>(new Worker());
            std::shared_ptr<Task> t = std::shared_ptr<Task>(new Task(hosts[i]->get_name(), job, w));
            for (auto &m : job->getMessages()[i])
            {
                t->addMessage(hosts[m.process]->get_name(), job->getCompCost(), job->getCommCost());
            }
            w->setTask(t);
            job->addWorker(w);
            simgrid::s4u::Actor::create(hosts[i]->get_name(), hosts[i], *w.get());
        }
    }
    XBT_INFO("jobs created");
    XBT_INFO("actors created");
    std::shared_ptr<FaultInjector> faultInjector = FaultInjector::getInstance();

    faultInjector->inject(jobs);
    XBT_INFO("faults injected");
    for (auto & job : jobs) {
        simgrid::s4u::NetZone* nz;
        for (auto &nnz : engine->get_netzone_root()->get_children()) {
            if (nnz->get_name() == job->getTopologyType()) nz = nnz;
        }
        if (!nz) {
            XBT_INFO("Can't find net zone");
            return;
        }
        std::vector<simgrid::s4u::Host *> hosts = nz->get_all_hosts();
        std::sort(hosts.begin(), hosts.end(), [](simgrid::s4u::Host *a, simgrid::s4u::Host *b)
              {
            if (a->get_name().size() == b->get_name().size()) return a->get_name() < b->get_name();
            return a->get_name().size() < b->get_name().size(); });

        simgrid::s4u::Host* h_controller = hosts.back();
        if (job->getFaultAlgo() == 2) {
            controller = std::shared_ptr<Controller>(new Controller());

            controller->setJob(job);
            job->setController(controller);

            simgrid::s4u::Actor::create(h_controller->get_name(), h_controller, *controller.get());
            XBT_INFO("Controller created");
        }
    }
}

void DeployManager::setEngine(simgrid::s4u::Engine *e)
{
    this->engine = e;
}

void DeployManager::saveResultsMessageStatistic(std::string path)
{
    DeployManager::MessageStatistic messageStatistics;
    for (int i = 0; i < jobs.size(); ++i)
    {
        messageStatistics.sent += jobs[i]->getSentMessages();
        messageStatistics.received += jobs[i]->getReceivedMessages();
        messageStatistics.total += jobs[i]->getTotalMessages();
    }

    std::string file_name = path + "/total_msg_sent.csv";
    std::ofstream fout(file_name, std::ios::app);
    XBT_INFO("Messages sent: %d of %d", messageStatistics.sent, messageStatistics.total);
    fout << 100. * messageStatistics.sent / messageStatistics.total << '\n';
    fout.close();

    file_name = path + "/total_msg_received.csv";
    fout.open(file_name, std::ios::app);
    XBT_INFO("Messages received: %d of %d", messageStatistics.received, messageStatistics.total);
    fout << 100. * messageStatistics.received / messageStatistics.total << '\n';
    fout.close();
}
void DeployManager::saveResultsTotalActorSkipSend(std::string path)
{
    std::string file_name = path + "/total_actor_skip_send.csv";
    std::ofstream fout(file_name, std::ios::app);
    int skipSendPercentage = 0;
    for (int i = 0; i < jobs.size(); ++i)
    {
        skipSendPercentage += jobs[i]->getSendOmissionWorkersPercentage();
    }
    fout << skipSendPercentage << ',' << engine->get_clock() << '\n';
    fout.close();
}
void DeployManager::saveResultsTotalActorSkipReceive(std::string path)
{
    std::string file_name = path + "/total_actor_skip_receive.csv";
    std::ofstream fout(file_name, std::ios::app);
    int skipReceivePercentage = 0;
    for (int i = 0; i < jobs.size(); ++i)
    {
        skipReceivePercentage += jobs[i]->getReceiveOmissionWorkersPercentage();
    }
    fout << skipReceivePercentage << ',' << engine->get_clock() << '\n';
    fout.close();
}

void DeployManager::saveResultsTotalActorKilled(std::string path)
{
    std::string file_name = path + "/total_actor_kill.csv";
    std::ofstream fout(file_name, std::ios::app);
    int killedPercentage = 0;
    int rightGuessed = 0;
    int wrongGuessed = 0;
    double mean = 0;
    for (int i = 0; i < jobs.size(); ++i)
    {
        killedPercentage += jobs[i]->getKilledWorkersPercentage();
        rightGuessed += jobs[i]->getRightGuessesCount();
        wrongGuessed += jobs[i]->getWrongGuessesCount();
        mean += jobs[i]->getMeanRecoveryTime();
    }
    double rightGuessedPercentage;
    double wrongGuessedPercentage;
    if (rightGuessed + wrongGuessed == 0)
    {
        rightGuessedPercentage = wrongGuessedPercentage = 0;
    }
    else
    {
        rightGuessedPercentage = 100. * rightGuessed / (rightGuessed + wrongGuessed);
        wrongGuessedPercentage = 100. * wrongGuessed / (rightGuessed + wrongGuessed);
    }
    XBT_INFO("Right guesses: %d", rightGuessed);
    XBT_INFO("Wrong guesses: %d", wrongGuessed);
    XBT_INFO("Mean recovery time: %f", mean);
    fout << killedPercentage << ',' << rightGuessedPercentage << ',' << wrongGuessedPercentage << ',' << mean << ',' << engine->get_clock() << '\n';
    fout.close();
}

void DeployManager::saveResults(std::string path)
{
    saveResultsTotalActorKilled(path);
    saveResultsTotalActorSkipReceive(path);
    saveResultsTotalActorSkipSend(path);
    saveResultsMessageStatistic(path);
}