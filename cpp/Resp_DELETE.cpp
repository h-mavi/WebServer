#include "../hpp/Response.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"

class Client;

std::string Response::deleteDB()
{
    this->body = this->req->getBody();

    if (ft_find(this->db_post, body) == this->db_post.end()) { return ("HTTP/1.1 " + strStatusCode(417) + "\r\nConnection: close\r\n"); }
    this->db_post.erase(ft_find(this->db_post, body));

    std::map<std::string, LocationBlock>::const_iterator it = this->req->getSrvB().locations.find(this->req->getUri());
    char *curr = get_current_dir_name();
    if (it == this->req->getSrvB().locations.end())
        return "HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n\r\n" + sendError();

    std::string path = curr + it->second.upload_path;
    if (chdir(path.c_str()) == -1) { return "HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n\r\n" + sendError(); }

    std::ofstream file;
    file.open("database.txt", std::ios::out | std::ios::trunc);
    for (std::vector<std::string>::iterator it = db_post.begin(); it != db_post.end(); it++) { file<<*it<<std::endl; }
    file.close();
    chdir(curr);
    free(curr);
    return ("HTTP/1.1 " + strStatusCode(this->stat) + "\r\nConnection: close\r\n");
}

std::string Response::RespDELETEImg(std::string uri)
{
    std::string path, filesname;
    if (uri.find_first_of('/') != uri.find_last_of('/'))
        uri.erase(0, uri.find_last_of('/'));

    path = this->req->getLoc()->upload_path;

    path.erase(0, 1);
    filesname = getFileNames(path);

    if (filesname.find(uri.c_str() + 1) == std::string::npos) { return ("HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n"); }

    char *curr = get_current_dir_name();
    std::string root = curr + ('/' + path), p_file = root + "/" + uri;
    chdir(root.c_str());

    remove(p_file.c_str());
    chdir(curr);
    free(curr);

    return ("HTTP/1.1 " + strStatusCode(this->stat) + "\r\nConnection: close\r\n");
}

std::string Response::RespDELETE()
{
    std::string uri = this->req->getUri();
    if (checkExten(uri) == 1) { return (this->RespDELETEImg(uri)); }
    if (checkExten(uri) == 5)
    {
        std::string tmp = launchCGI(uri);
        if (tmp != "ERROR")
            return (tmp);
    }

    return (this->deleteDB());
}