/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pokpalae <pokpalae@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 21:28:41 by pokpalae          #+#    #+#             */
/*   Updated: 2025/06/24 20:11:50 by pokpalae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    
    
    
    Server webServer(&configHandler);

    
        webServer.launchServer();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Server terminated unexpectedly: " << error.what() << std::endl;
    }

    return 0;
}

