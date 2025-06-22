#include "includes/Helper.hpp"
#include "includes/Server.hpp"
#include "includes/ConfigParser.hpp"

// int main(int argc, char **argv) {
//     (void)argc;
//     (void)argv;
//     std::cout << "Webserv starting..." << std::endl;
//     return 0;
// }


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: Missing configuration file.\n";
        std::cerr << "Usage: ./webserv [configuration file path]" << std::endl;
        return 1;
    }

    ConfigParser configHandler;

    try
    {
        std::string configFilePath = argv[1];

        configHandler.validateConfig(configFilePath);

    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: Unable to load configuration file - " << error.what() << std::endl;
        return -1;
    }
    try
    {
       configHandler.displayConfiguration();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    Server webServer(&configHandler);

    try
    {
        webServer.launchServer();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Server terminated unexpectedly: " << error.what() << std::endl;
    }

    return 0;
}

