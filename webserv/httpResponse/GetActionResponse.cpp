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


#include "../includes/GetActionResponse.hpp"

#include <fstream>
#include <cstdio>
#include <string>
#include <iterator>


// Constructors
GetActionResponse::GetActionResponse() {}

GetActionResponse::GetActionResponse(ServerConfiguration* serverBlock) {
    _host = serverBlock->getHostname();
}

GetActionResponse::~GetActionResponse() {}

static std::string executePhpScript(std::string res) {
    
std::string command;
    std::string result;

    command = "php " + res;
    FILE *in = popen(command.c_str(), "r");

    if (in) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), in)) {
            result += buffer;
        }
        pclose(in);
    }
    return (result);


}

// Loads the content of a file or processes a PHP script if applicable
void GetActionResponse::setRawBody() {
    std::ifstream fileStream(_resource.c_str());
    std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

    if (hasFileExtension(_resource, ".html") || hasFileExtension(_resource, ".php")) 
    {
        _raw_body = executePhpScript(_resource);
    } 
    
    else {
        _raw_body = fileContent;
    }

    addContentLengthToHeaders();
}

// Sets the required HTTP headers for the response
void GetActionResponse::setHeaders() {
    addDateHeader();
    addConnectionHeader("keep-alive");
    addContentTypeHeader();
    addRetryAfterHeader(2); // Optional retry header
    setHostHeader(_host.c_str());
}

// Builds a complete HTTP response based on the GET request
void GetActionResponse::constructResponse(ClientRequest& request) {
    initializeResourceFromRequest(request);
    _raw_status_line = Http_version_ + " 200 OK" + "\r\n";

    setHeaders();
    setRawBody();
    composeHeaderString();
    setServerReply();
}

// Placeholder for default error response with custom body
void GetActionResponse::constructDefaultResponseWithBody(ClientRequest& request, const str& bodyContent) {
    (void)request;
    (void)bodyContent;
}

// Placeholder for serving a configured response from a file path
void GetActionResponse::constructConfigurationResponse(ClientRequest& request, const str& targetFilePath) {
    (void)request;
    (void)targetFilePath;
}



// static std::string executePhpScript(const std::string& scriptPath) {
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         perror("pipe");
//         return "";
//     }

//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("fork");
//         return "";
//     }

//     if (pid == 0) {
//         close(pipefd[0]); 
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]);

//         char* args[] = { (char*)"php", const_cast<char*>(scriptPath.c_str()), NULL };
//         execve("/usr/bin/php", args, NULL); 
//         perror("execve"); // 
//         _exit(1);
//     }

//     close(pipefd[1]);

//     char buffer[1024];
//     std::string output;
//     ssize_t bytesRead;
//     while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
//         buffer[bytesRead] = '\0';
//         output += buffer;
//     }

//     close(pipefd[0]);
//     waitpid(pid, NULL, 0);

//     return output;
// }


// std::string Php(std::string scriptPath) {
    
//     std::string command = "php " + scriptPath;
//     std::string output;

//     FILE* pipe = popen(command.c_str(), "r");
//     if (pipe) {
//         char buffer[1024];
//         while (fgets(buffer, sizeof(buffer), pipe)) {
//             output += buffer;
//         }
//         pclose(pipe);
//     }
//     return output;
// }

