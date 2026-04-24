#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../Headers.hpp"
#include "ConfArgs.hpp"

class Request
{
    private :
        int bodylen;
        Methods meth;
        const ServerBlock &srvblock;
        std::string original, uri, version, body;
        std::map<std::string, std::string>  header;
        std::vector<Session>&               cookies;
        const LocationBlock*                loc;
        bool isUp;
    
    public :
        Request(std::string rawreq, const ServerBlock& srv, bool isUp, std::vector<Session>& cook);
        int ParsReq();
        int parseUri(std::string line);
        int parseHeader();
        int parseBody();
        int findLoc(std::map<std::string, LocationBlock>::const_iterator *it);

        const int getMeth() const { return(this->meth); }
        const std::string getUri() const { return(this->uri); }
        const std::string getBody() const { return(this->body); }
        const LocationBlock* getLoc() const { return(this->loc); }
        const ServerBlock& getSrvB() const { return(this->srvblock); }
        const std::vector<Session>& getCookie() const { return (this->cookies); }
        const std::map<std::string, std::string> getHeader() const { return(this->header); }

        void setCookie(Session ses) const { this->cookies.push_back(ses); }
};

#endif