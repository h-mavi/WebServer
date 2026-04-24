#include "../hpp/Response.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"

class Client;

std::string    Response::saveDB()
{
    this->body = this->req->getBody();

    if (ft_find(this->db_post, this->body) != this->db_post.end() || this->body.empty()) { return ("HTTP/1.1 " + strStatusCode(417) + "\r\nConnection: close\r\n"); }
    this->db_post.push_back(this->body);

    std::map<std::string, LocationBlock>::const_iterator it = this->req->getSrvB().locations.find(this->req->getUri());
    char *curr = get_current_dir_name();
    if (it == this->req->getSrvB().locations.end())
        return "HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n\r\n" + sendError();

    std::string path = curr + it->second.upload_path;

    if (chdir(path.c_str()) == -1) { mkdir(path.c_str(), 0755); chdir(path.c_str()); }

    std::ofstream file;
    if (this->db_post.size() != 1) { file.open("database.txt", std::ios::out | std::ios::app); }
    else { file.open("database.txt", std::ios::out | std::ios::trunc); }
    file.write(this->body.c_str(), this->body.size());
    file.write("\n", 1);
    file.close();
    chdir(curr);
    free(curr);

    return ("HTTP/1.1 " + strStatusCode(this->stat) + "\r\nConnection: close\r\n");
}

std::string Response::RespPOST()
{
    std::string uri = this->req->getUri();
    if (checkExten(uri) == 1) { return (this->RespGETImg(uri)); }
    if (checkExten(uri) == 5)
    {
        std::string tmp = launchCGI(uri);
        if (tmp != "ERROR")
            return (tmp);
    }

    return (this->saveDB());
}