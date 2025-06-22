#include "../includes/PostActionResponse.hpp"

PostActionResponse::PostActionResponse() {}

PostActionResponse::PostActionResponse(ServerConfiguration* serverBlock)
    : _rootDirectory(serverBlock->getDocumentRoot()), _host(serverBlock->getHostname()) {}

PostActionResponse::~PostActionResponse() {}



std::string trimNonNumerical(const std::string& input) {
    std::string output;
    output.reserve(input.length()); // expandable

    std::string::const_iterator it = input.begin();
    while (it != input.end()) {
        if (std::isdigit(*it))
            output += *it;
        ++it;
    }

    return output;
}


void parseMultipartFormData(std::ifstream& tmpFile) {
    std::string line, separator, name, value;
    std::string picture, description, price, phone;

    std::getline(tmpFile, separator);
    while (std::getline(tmpFile, line)) {
        if (line.find("Content-Disposition: form-data; name=") != std::string::npos) {
            name = line.substr(line.find("name=") + 6);
            name = name.substr(0, name.find("\""));

            std::getline(tmpFile, line); // Skip content-type or blank line
            if (name != "picture") std::getline(tmpFile, line); // value line

            value = line.substr(0, line.length() - 1);

            if (name == "description") description = value;
            else if (name == "price") price = value;
            else if (name == "phone") phone = value;
        } else if (name == "picture" && line.find("Content-Type: image/jpeg") != std::string::npos) {
            std::getline(tmpFile, line); // Skip empty line
            while (std::getline(tmpFile, line)) {
                if (line.find(trimNonNumerical(separator)) == std::string::npos) {
                    picture += line + '\n';
                } else {
                    break;
                }
            }
        }
    }

    std::ofstream dataFile("tmp/data-entry.txt");
    if (dataFile.is_open()) {
        dataFile << "description=" << description << "&price=" << price << "&phone=" << phone;
        dataFile.close();
    }

    if (!picture.empty()) {
        std::ofstream pictureFile("tmp/pic-entry.jpeg", std::ios::binary);
        if (pictureFile.is_open()) {
            pictureFile.write(picture.c_str(), picture.size());
            pictureFile.close();
        }
    }
}

void moveAndRenameFile(const std::string& from, const std::string& to) {
    if (std::rename(from.c_str(), to.c_str()) != 0) {
        std::perror(("Error renaming " + from + " to " + to).c_str());
    }
}

 // anonymous namespace

void PostActionResponse::executePostResponse(ClientRequest& request) {
    const std::string dataFilePath = "tmp/data-entry.txt";
    const std::string idFilePath = "tmp/id_file";
    const std::string picTempPath = "tmp/pic-entry.jpeg";

    {
        std::ofstream tmpFile(dataFilePath);
        if (tmpFile) {
            tmpFile << request.getBodyContent();
        }
    }

    {
        std::ifstream inputFile(dataFilePath);
        if (inputFile) {
            parseMultipartFormData(inputFile);
        }
    }

    std::string command = "php cgi-bin/add-entry.php " + dataFilePath + " > " + idFilePath;
    int status = std::system(command.c_str());
    std::cout << "[PostResponse] PHP command: " << command << "\n";
    std::cout << "[PostResponse] Exit status: " << status << "\n";

    std::ifstream pictureFile(picTempPath);
    if (pictureFile) {
        std::ifstream idFile(idFilePath);
        std::string id((std::istreambuf_iterator<char>(idFile)), std::istreambuf_iterator<char>());
        std::string finalImagePath = _rootDirectory + "/images/" + id + ".jpeg";

        std::cout << "[PostResponse] Moving image to: " << finalImagePath << "\n";
        moveAndRenameFile(picTempPath, finalImagePath);
    }

    std::remove(dataFilePath.c_str());
    std::remove("tmp/post-entry.txt");
    std::remove(idFilePath.c_str());
}

void PostActionResponse::executePostDeleteResponse(ClientRequest& request) {
    const std::string deleteFilePath = "tmp/delete-entry.txt";

    {
        std::ofstream tmpFile(deleteFilePath);
        if (tmpFile) {
            tmpFile << request.getBodyContent();
        }
    }

    std::string command = "php cgi-bin/delete-entry.php " + deleteFilePath;
    std::system(command.c_str());

    std::string id = request.getBodyContent().substr(request.getBodyContent().find("&") + 4);
    std::string removePath = _rootDirectory + "/images/" + id + ".jpeg";

    std::cout << "[PostResponse] Deleting image: " << removePath << "\n";
    std::remove(removePath.c_str());
    std::remove(deleteFilePath.c_str());
}

void PostActionResponse::setLocationHeader() {
    _headers["Location"] = "/";
}

void PostActionResponse::setRefreshHeader(double sec) {
    std::ostringstream ss;
    ss << sec;
    _headers["Refresh"] = ss.str() + ";url=/";
}

void PostActionResponse::setHeaders() {
    addDateHeader();
    addConnectionHeader("close");
    setLocationHeader();
    setHostHeader(_host.c_str());
    addCacheControlHeader("no-cache");
}

void PostActionResponse::constructResponse(ClientRequest& request) {
    std::string path = (request.getUriObject()).getPath();

    initializeResourceFromRequest(request);
    _raw_status_line = Http_version_ + " 302 Found\r\n";

    if (path.find("add-entry.php") != std::string::npos)
        executePostResponse(request);
    else
        executePostDeleteResponse(request);

    setHeaders();
    composeHeaderString();
    getServerReply();
}

void PostActionResponse::constructDefaultResponseWithBody(ClientRequest& request, const str& bodyContent) {
    (void)request;
    (void)bodyContent;
}

void PostActionResponse::constructConfigurationResponse(ClientRequest& request, const str& targetFilePath) {
    (void)request;
    (void)targetFilePath;
}

void PostActionResponse::printResponse() {
    std::cout << "PostResponse:\n"
              << _raw_status_line << _raw_headers << _raw_body << std::endl;
}

// void PostActionResponse::setStatusCode(StatusCode statusCode) {
//     setHttpResponseStatusCode(statusCode); // it's useless Daniel
// }