#ifndef GET_ACTION_RESPONSE_HPP
#define GET_ACTION_RESPONSE_HPP

# include "DeleteActionResponse.hpp"
# include <string>
# include "Helper.hpp"


class GetActionResponse : public ReplyClient {

public:
    // Constructors & Destructor
    GetActionResponse();
    GetActionResponse(ServerConfiguration* serverBlock);
    virtual ~GetActionResponse();

    // Core methods
    void constructResponse(ClientRequest& request);
    void constructDefaultResponseWithBody(ClientRequest& request, const str& bodyContent);
    void constructConfigurationResponse(ClientRequest& request, const str& targetFilePath);
    void printResponse(); // useless
    void setStatusCode(StatusCode statusCode);

private:
    std::string _host;

    // Internal helpers
    void setHeaders();
    void setRawBody();
};

#endif // CLASS_GET_RESPONSE_HPP