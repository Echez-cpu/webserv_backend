#pragma once

# include "UR_Locator.hpp"
# include "Helper.hpp"
#include "ConfigParser.hpp"
#include "ServerConfiguration.hpp"
#include <string>
#include "LocationBlock.hpp"

typedef std::string str;


class ClientRequest {

    public:
        str        parse_status; // what's up?

        ClientRequest();
		virtual ~ClientRequest();

        str                                 getResolvedResourcePath(void);
        str     					getRequestLine(void);
        str   						getBodyContent(void);
        str     					getRawHttpRequest(void);
        str   						getHeaderSection(void);
        Http_Method						get_Http_Method(void);
        UR_Locator&            					getUriObject(void);
        std::map<std::string, std::string>  getParsedHeaders(void);
		StatusCode							getStatusCode(void);
        void	                            setStatusCode(StatusCode status_code);
        

		

        int                 parseClientRequest(char buf[2000], int bytes, ServerConfiguration* sb); // check
        void                debugPrintRequest(void);
        bool                fullyParsedHeaders(void);
        bool                hasCompleteRequest(void);

        // Exceptions
       	class BadRequestException : public std::exception {
        public:
            const char * what () const throw();
		};
        class NotFoundException : public std::exception {
        public:
			const char * what () const throw();
		};
        class HttpVersionNotSupportedException : public std::exception {
        public:
			const char * what () const throw();
		};

		class PayloadTooLargeException : public std::exception {
			public:
				const char * what () const throw();
		};
 		class UnauthorizedException : public std::exception {
			public:
				const char * what () const throw();
		};
        class RedirectException : public std::exception {
			public:
				const char * what () const throw();
		};

    
        private:
        str			_unparsed_request;
	    str		    _raw_start_line;
	    str			_raw_headers; 
	    str			_raw_body;
        bool                _has_body;
        unsigned long       _body_length;
        int                 _body_bytes_read;
		StatusCode			_status_code;

        Http_Method                       _request_method;
        UR_Locator                                 _uri;
        std::map<str, str>  _headers;
        str                       _resource;

        void                extractRequestMetadata(ServerConfiguration* sb);
        void                extractHeadersFromRequest();
        void                decomposeURI(str uri);


};