#ifndef LOCATION_BLOCK_HPP
#define LOCATION_BLOCK_HPP

#include <string>
#include <vector>

class LocationBlock {
private:
    std::string     _pathUri;               // URL path for location
    std::string     _documentRoot;          // File system path serving files
    std::string     _defaultIndex;          // Default index page
    unsigned long   _maxClientBodySize;     // Max body size in bytes

    // Method restrictions
    bool            _allowGet;
    bool            _allowPost;
    bool            _allowDelete;

public:
    // Constructors & Destructor
    LocationBlock();
    LocationBlock(const LocationBlock& other);
    ~LocationBlock();

    // Assignment operator
    LocationBlock& operator=(const LocationBlock& other);

    // Accessors (Getters)
    std::string     getPathUri() const;
    std::string     getDocumentRoot() const;
    std::string     getDefaultIndex() const;
    unsigned long   getMaxClientBodySize() const;
    bool            isGetAllowed() const;
    bool            isPostAllowed() const;
    bool            isDeleteAllowed() const;

    // Mutators (Setters)
    void setPathUri(const std::string& uri);
    void setDocumentRoot(const std::string& rootPath);
    void setDefaultIndex(const std::string& indexFile);
    void setMaxClientBodySize(unsigned long maxSize);
    void configureAllowedMethods(const std::vector<std::string>& methods);
};

#endif 