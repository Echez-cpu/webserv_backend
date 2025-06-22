#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <set>
#include "ServerConfiguration.hpp"
// #include "utils.hpp"

// class ConfigParser {
// public:
//     ConfigParser() {} // Default constructor
//     ~ConfigParser() {} // Destructor

//     void validateConfig(const std::string& configFilePath); // Dummy declaration
//     void printConfig() const; // Dummy declaration
// };
class ConfigParser
{
    private:
        // Maximum size for client request bodies
        unsigned long _maxClientBodySize;

        // Listening port for the server
        // const char*	
        std::string _listeningServerPort;

        // Default error pages mapped by HTTP status codes
        // std::map<StatusCode, std::string>
        std::map<int, std::string> _defaultErrorPages;

        // Server blocks parsed from the configuration
        
    public:
        // Vector of pointers to _server_blocks objects
        std::vector<ServerConfiguration*> _serverBlocksFromConfig;

        // Constructors and Destructor
        ConfigParser();
        ConfigParser(const ConfigParser &other);
        ~ConfigParser();

        // Assignment Operator
        ConfigParser &operator=(const ConfigParser &other);

        // Configuration Parsing Methods
        bool validateConfig(const std::string &configFilePath);
        bool validateDirectives(const std::string &fileName);
        void validateLocationBlock(std::ifstream &fileStream, const std::vector<std::string> &tokens, ServerConfiguration *serverBlock);
        void checkForDuplicateServerBlocks();
        void displayConfiguration() const;

        // Accessors
        // std::vector<ServerConfiguration *> &getServerBlocks();
        const std::vector<ServerConfiguration *> &getServerBlocks() const;
        unsigned long getClientMaximumBodySize() const;


        //const char*
        const std::string &getListeningServerPort() const;

        // Exception Classes
        class FileNotFoundException : public std::exception {
        public:
            const char *what() const throw();
        };

        class InvalidDirectiveException : public std::exception {
        public:
            const char *what() const throw();
        };

        class MissingBracketsException : public std::exception {
        public:
            const char *what() const throw();
        };

        class DuplicateServerBlockException : public std::exception {
        public:
            const char *what() const throw();
        };

        class DirectiveDoesNotExistException : public std::exception {
        public:
            const char *what() const throw();
        };
};

#endif