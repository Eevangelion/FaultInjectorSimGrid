#include <simgrid/s4u.hpp>
#include <fstream>
#include <variant>
#include <string>
#include "FaultInjector/FaultInjector.h"
#include "DeployManager/DeployManager.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(test, "Messages specific for this example");
int main(int argc, char **argv)
{
    const std::string visualizationDirPath = "./visualization";
    simgrid::s4u::Engine e(&argc, argv);
    xbt_assert(argc == 2, "Usage: %s platform_file\n", argv[0]);

    e.load_platform(argv[1]);

    std::shared_ptr<DeployManager> deployManager = DeployManager::getInstance();
    std::shared_ptr<FaultInjector> faultInjector = FaultInjector::getInstance();

    deployManager->setEngine(&e);

    deployManager->deploy();

    e.run();
    deployManager->saveResults(visualizationDirPath);

    XBT_INFO("Results of simulation are saved");

    return 0;
}
