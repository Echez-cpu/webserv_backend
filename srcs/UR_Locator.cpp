#include "UR_Locator.hpp"

UR_Locator::UR_Locator() {

}

UR_Locator::~UR_Locator() {

}


str&    URI::getPath(void) {
    return (_path);
}   

str&    URI::getQuery(void) {
    return (_query);
}   

void    URI::setPath(str path) {
    this->_path = path; 
} 

void    URI::setQuery(str query) {
    this->_query = query;
}  
