#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../Headers.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Client.hpp"

class Server;
class Client;

extern Server   *big_srv;

class Response
{
    private :
        int             stat;
        std::ifstream   html;
        std::string     body, cookie_ses;
        const Request*  req;
        std::vector<std::string>& db_post;

        std::string sendError();
        std::string RespGETErrJson();
        std::string RespGETImgJson();
        std::string RespGETImg(std::string uri);
        std::string RespGET();
        std::string RespPOST();
        std::string RespDELETEImg(std::string uri);
        std::string RespDELETE();
        std::string saveDB();
        std::string deleteDB();
        std::string launchCGI(std::string uri);
        std::string dirListing(std::string uri, std::string files_name);
        std::string cookieGen();


    public :
        Response(const Request* a_req, int a_stat, std::vector<std::string>& a_db) : req(a_req), stat(a_stat), db_post(a_db) {}
        std::string GenerateResp(bool is_upload);
};

std::string strStatusCode(int stat);
std::string statusText(int stat);
std::string getFileNames(const std::string root);

#endif