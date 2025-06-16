#include "ReplyClient.hpp"
#include "ClientRequest.hpp"
#include "Helper.hpp"

str ReplyClient::getServerReply(void) {
    return (_raw_response);
}
