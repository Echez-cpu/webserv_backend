#include "ClientSession.hpp"

// remember the TODO and do them accordingly...

ClientSession::ClientSession() {}

ClientSession::~ClientSession() {}



void ClientSession::handleClientRequest(char buf[2000]) {
	int bytes_checked = 0;

	while (bytes_checked < 2000)
    {
		
        // Add a new Request if none exists or the last one is fully parsed
        // In HTTP/1.1, clients can send multiple requests back-to-back without closing the connection 
        // (pipelining or reusing the connection)
        // Multiple requests in a single buffer or connection are handled safely :)
		if (requestResponseArray.empty() || requestResponseArray.back().first->hasCompleteRequest())
        {
			ClientRequest* req = new ClientRequest;
			requestResponseArray.push_back(std::make_pair(req, (Response*)0));
		}

		ClientRequest* last_req = requestResponseArray.back().first;

		try 
        {
			bytes_checked = last_req->parseClientRequest(buf, bytes_checked, getServerConfig());
			last_req->parse_status = "OK";
			last_req->setStatusCode(OK);
		} catch (std::exception& e) 
        {
			bytes_checked = 2000;  // Stop processing buffer on parse failure
			handleRequestParseException(*last_req, e);
		}
	}
}



void ClientSession::handleRequestParseException(ClientRequest& req, std::exception& e) {
	if (dynamic_cast<ClientRequest::BadRequestException*>(&e)) {
		req.setStatusCode(BAD_REQUEST);
	}
	else if (dynamic_cast<ClientRequest::NotFoundException*>(&e)) {
		req.setStatusCode(NOT_FOUND);
	}
	else if (dynamic_cast<ClientRequest::HttpVersionNotSupportedException*>(&e)) {
		req.parse_status = "VersionMismatch";
		req.setStatusCode(HTTP_VERSION_NOT_SUPPORTED);
	}
	else if (dynamic_cast<ClientRequest::ContentTooLargeException*>(&e)) {
		req.parse_status = "ContentTooLarge";
		req.setStatusCode(PAYLOAD_TOO_LARGE);
	}
	else if (dynamic_cast<ClientRequest::UnauthorizedException*>(&e)) {
		req.parse_status = "Unauthorized";
		req.setStatusCode(UNAUTHORIZED);
	}
	else if (dynamic_cast<ClientRequest::RedirectException*>(&e)) {
		req.parse_status = "Redirect";
		req.setStatusCode(FOUND);
	}
}


str ClientSession::buildHttpResponseString(void)
{
    str reply;
    std::vector<std::pair<ClientRequest*, ReplyClient*> >::iterator it = requestResponseArray.begin();

    while (it != requestResponseArray.end())
    {
        ClientRequest* reqst = it->first;

        if (reqst->getStatusCode() != OK) {
            it->second = new NotOkResponse(); // Daniel class new FailActionResponse()

            str errorPath = getServerConfig()->fetchErrorPagePath(reqst->getStatusCode()); // Tell Daniel to change data type to Statuscode
            if (!errorPath.empty())
                it->second->constructConfigResponse(*reqst, errorPath); // Change name according to Daniel
            else
                it->second->constructDefaultResponseWithBody(*reqst, DefaultErrorPage(reqst->getStatusCode())); // change name based on Daniel...

            reply = it->second->getServerReply();

            delete reqst;
            delete it->second;
            it = requestResponseArray.erase(it);
            return reply;
        }
        
        else if (reqst->hasCompleteRequest()) 
        {
            try {
                if (reqst->get_Http_Method() == GET)
                    it->second = new GetActionResponse(getServerConfig()); // Gospel according to Daniel
                else if (reqst->get_Http_Method() == POST)
                    it->second = new PostActionResponse(getServerConfig());
                else if (reqst->get_Http_Method() == DELETE)
                    it->second = new DeleteActionResponse(getServerConfig());

                it->second->constructResponse(*reqst);
            } catch (std::exception& e) {
                it->second = new NotOkResponse(); // Daniel class new FailActionResponse()
                reqst->setStatusCode(INTERNAL_SERVER_ERROR);

                str errorPath = getServerConfig()->fetchErrorPagePath(reqst->getStatusCode()); // Daniel
                if (!errorPath.empty())
                    it->second->constructConfigResponse(*req, errorPath); // check with Daniel
                else
                    it->second->constructDefaultResponseWithBody(*reqst, DefaultErrorPage(reqst->getStatusCode())); // Daniel
            }

            reply = it->second->getServerReply();

            delete reqst;
            delete it->second;
            it = requestResponseArray.erase(it);
            return reply;
        }

        ++it;
    }

    return reply;
}


