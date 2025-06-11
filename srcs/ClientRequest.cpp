#include "ClientRequest.hpp"


bool     Request::fullyParsedHeaders() {
    if (_unparsed_request.find("\r\n\r\n") != str::npos)
        return (true);
    else
        return (false);
}



int Request::parseRequest(char* buf, int bytes_read, ServerBlock* sb) {
    int start = bytes_read;

    while (start < BUFF_SIZE) {
        char current_char = buf[start];

        if (!headersFullyParsed()) {
            _unparsed_request += current_char;

            if (_unparsed_request.find("\r\n\r\n") != std::string::npos) {
                // Complete headers received
                parseRequestStartLine(sb);
                parseRequestHeaders();
                printRequest();
            }

            ++bytes_read;
        } else if (_has_body) {
            _raw_body += current_char;
            ++_body_bytes_read;
            ++bytes_read;

            if (static_cast<unsigned long>(_body_bytes_read) == _body_length) {
                if (_body_length > sb->getClientMaxBodySize()) {
                    throw Request::ContentTooLargeException();
                }
                break; // Full body received
            }
        }

        ++start;
    }

    return bytes_read;
}



void Request::parseRequestStartLine(ServerConfiguration* sb) {
    std::vector<std::string> startline_split;
    std::string keys[] = {"GET", "POST", "DELETE"};

    // Extract raw start line (first line of HTTP request)
    std::size_t line_end = _unparsed_request.find("\r\n");
    _raw_start_line = _unparsed_request.substr(0, line_end);

    // Trim garbage values before the method keyword (GET, POST, DELETE)
    std::size_t pos = _raw_start_line.length();
    for (int i = 0; i < 3; ++i) {
        std::size_t keyPos = _raw_start_line.find(keys[i]);
        if (keyPos != std::string::npos && keyPos < pos)
            pos = keyPos;
    }
    if (pos != _raw_start_line.length())
        _raw_start_line = _raw_start_line.substr(pos);

    // Remove the processed start line from the unparsed request
    _unparsed_request = _unparsed_request.substr(line_end + 2);

    // Split the start line into method, URI, and HTTP version
    startline_split = ft_split(_raw_start_line, ' ');
    if (startline_split.size() != 3) 
    {
        throw BadRequestException();
    }

    // Parse HTTP method
    if (startline_split[0] == "GET")
        _request_method = GET;
    else if (startline_split[0] == "POST")
        _request_method = POST;
    else if (startline_split[0] == "DELETE")
        _request_method = DELETE;
    else
        throw BadRequestException();

    // Enforce limit_except checks
    if ((_request_method == GET && !sb->_allowGET) ||
        (_request_method == POST && !sb->_allowPOST) ||
        (_request_method == DELETE && !sb->_allowDELETE)) {
        throw UnauthorizedException();
    }

    // Parse and validate the URI
    parseURI(startline_split[1]);

    str path = remove_slashes(_uri.getPath());

    if (is_folder_in_tree(sb->getDocumentRoot(), path))
    {
        if (path == "images")
            throw RedirectException(); // i might have to remove this...

        LocationBlock* loc = locationExistsInBlock(sb->getRoutes(), path);
        if (!loc)
            throw UnauthorizedException();

        _resource = loc->getDocumentRoot() + "/" + path + "/" + loc->getDefaultIndex();
    }
    else if (startline_split[1] == "/") {
        _resource = sb->getDocumentRoot() + _uri.getPath() + sb->getDefaultIndex();
    }
    else if (hasFileExtension(_uri.getPath(), ".php")) {
        _resource = _uri.getPath();
    }
    else {
        _resource = sb->getDocumentRoot() + _uri.getPath();
    }

    if (!ft_is_resource_avail(_resource) && !hasFileExtension(_uri.getPath(), ".php"))
        throw NotFoundException();

    std::cout << "Requested Resource: " << _resource << " is available" << std::endl;

    // Validate HTTP version
    if (startline_split[2] != "HTTP/1.1")
        throw HttpVersionNotSupportedException();
}


void Request::parseURI(str uri) {
    std::size_t query_pos = uri.find('?'); 
    std::size_t hash_pos = uri.find('#'); // to store fragment...

    str path;

    // Extract path: if there's a '?', path is everything before it
    if (query_pos != str::npos) 
    {
        path = uri.substr(0, query_pos);
    } 
    else 
    {
        path = uri;
    }
    _uri.setPath(path); 

    // Extract query string if a '?' is present
    if (query_pos != str::npos) {
        str query; 

        if (hash_pos != str::npos && hash_pos > query_pos) {
            query = uri.substr(query_pos + 1, hash_pos - query_pos - 1);
        } else {
            query = uri.substr(query_pos + 1);
        }

        _uri.setQuery(query);      
    }

    // Throw 404 error
    if (!ft_is_resource_avail(_uri.getPath()) &&
        !hasFileExtension(_uri.getPath(), ".php")) {
        throw NotFoundException();
    }
}

LocationBlock* locationExistsInBlock(std::vector<LocationBlock*>& lbs, std::string url) {
    std::vector<LocationBlock*>::iterator it = lbs.begin();
    while (it != lbs.end()) 
    {
        if ((*it)->getPathUri() == url)
            return (*it);
        ++it;
    }
    return NULL;
}

const char * Request::RedirectException::what() const throw ()
{
    return ("Redirect");
}


const char * Request::HttpVersionNotSupportedException::what() const throw ()
{
    return ("This server only accepts HTTP/1.1 requests");
}
