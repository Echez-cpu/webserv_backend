#include "ClientSession.hpp"

void Connection::handleRequest(char buf[2000]) {
	int bytes_checked = 0;

	while (bytes_checked < 2000) {
		// Add a new Request if none exists or the last one is fully parsed
		if (requestResponseArray.empty() || requestResponseArray.back().first->isFullyParsed()) {
			Request* req = new Request;
			requestResponseArray.push_back(std::make_pair(req, (Response*)0));
		}

		Request* last_req = requestResponseArray.back().first;

		try {
			bytes_checked = last_req->parseRequest(buf, bytes_checked, getServerBlock());
			last_req->parse_status = "OK";
			last_req->setStatusCode(OK);
		} catch (std::exception& e) {
			bytes_checked = 2000;  // Stop processing buffer on parse failure
			handleRequestParseException(*last_req, e);
		}
	}
}



void Connection::handleRequestParseException(Request& req, std::exception& e) {
	if (dynamic_cast<Request::BadRequestException*>(&e)) {
		req.setStatusCode(BAD_REQUEST);
	}
	else if (dynamic_cast<Request::NotFoundException*>(&e)) {
		req.setStatusCode(NOT_FOUND);
	}
	else if (dynamic_cast<Request::HttpVersionNotSupportedException*>(&e)) {
		req.parse_status = "VersionMismatch";
		req.setStatusCode(HTTP_VERSION_NOT_SUPPORTED);
	}
	else if (dynamic_cast<Request::ContentTooLargeException*>(&e)) {
		req.parse_status = "ContentTooLarge";
		req.setStatusCode(CONTENT_TOO_LARGE);
	}
	else if (dynamic_cast<Request::UnauthorizedException*>(&e)) {
		req.parse_status = "Unauthorized";
		req.setStatusCode(UNAUTHORIZED);
	}
	else if (dynamic_cast<Request::RedirectException*>(&e)) {
		req.parse_status = "Redirect";
		req.setStatusCode(FOUND);
	}
}
