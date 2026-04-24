#include "../hpp/Response.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"

class Client;

int lastStatus = 200;

//////////////////////////////////////////utils//////////////////////////////////////////

std::string strStatusCode(int stat)
{
    lastStatus = stat;
    switch (stat)
    {
        case 200: return ("200 OK");
        case 301: return ("301 Moved Permanently");
        case 400: return ("400 Bad Request");
        case 401: return ("401 Unauthorized");
        case 403: return ("403 Forbidden");
        case 404: return ("404 Not Found");
        case 405: return ("405 Method Not Allowed");
        case 413: return ("413 Payload Too Large");
        case 414: return ("414 URI Too Long");
        case 417: return ("417 Expectation Failed");
        case 501: return ("501 Not Implemented");
        case 505: return ("505 HTTP Version Not Supported");
    }
    return ("500 Internal Server Error");
}

std::string statusText(int stat)
{    
    std::string full = strStatusCode(stat);
    std::size_t pos = full.find(' '); 
    if (pos == std::string::npos) return full;
    return full.substr(pos + 1);
}

std::string getFileNames(const std::string root)
{
    DIR* dir;
    struct dirent* entry;
    std::string file_names;
    std::vector<struct dirent*> strc_dir;

    dir = opendir(root.c_str());
    if (dir == NULL)
        return ("ERROR");
    while ((entry = readdir(dir)) != NULL) { strc_dir.push_back(entry); }

    for (size_t i = 0; i < strc_dir.size(); ++i)
    {
        if (!file_names.size()) { file_names = strc_dir[i]->d_name; }
        else { file_names += std::string(" ") + strc_dir[i]->d_name; }
    }
    closedir(dir);
    return (file_names);
}

std::string Response::sendError()
{
    std::ifstream html;
    std::string body, path = "." + this->req->getSrvB().root;

    if (this->req->getSrvB().errorpages.find(lastStatus) != this->req->getSrvB().errorpages.end())
        path += this->req->getSrvB().errorpages.find(lastStatus)->second;

    html.open(path, std::ios::in);

    if (!html.is_open()) { return (""); }

    std::getline(html, body, '\0');
    html.close();
    return (body);
}

std::string Response::launchCGI(std::string uri)
{
    std::vector<std::string> ext;
    std::string path, filesname, send_bd = this->req->getBody();

    if (uri.find_first_of('/') != uri.find_last_of('/'))
        uri.erase(0, uri.find_last_of('/'));

    if (!this->req->getLoc() || this->req->getLoc()->cgi_extension.empty()) { return("ERROR"); }

    for (std::map<std::string, std::string>::const_iterator it = this->req->getLoc()->cgi_extension.begin(); it != this->req->getLoc()->cgi_extension.end(); ++it)
        ext.push_back(it->first);

    for (std::vector<std::string>::const_iterator it = ext.begin(); it != ext.end(); ++it)
        if (uri.find(*it) != std::string::npos)
            path = this->req->getLoc()->cgi_extension.find(*it)->second;

    if (path.empty()) { return("ERROR"); }
    
    path.erase(0, 1);
    uri.erase(0, 1);
    
    filesname = getFileNames(path);

    if (filesname == "ERROR") { return("HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n"); }

    if (filesname.find(uri) == std::string::npos) { return("HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n"); }

    size_t n = send_bd.find_first_of("\r\n");
	while (n != std::string::npos)
	{
        send_bd.replace(n, 2, "\n");
		n = send_bd.find_first_of("\r\n");
	}
    
    // Creiamo una pipe per la comunicazione tra padre e figlio
    int pipefds[2];
    if (pipe(pipefds) == -1) { return("HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n"); }
    
    std::string processed;

    std::ofstream filein;
    filein.open(".input", std::ios::out | std::ios::app);
    filein.write(this->req->getBody().c_str(), this->req->getBody().size());
    filein.close();

    char *curr = get_current_dir_name();

    pid_t pid = fork();
    if (pid == -1) { return("HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n"); }

    if (pid == 0) // Processo figlio
    {
        std::string child_path;
        if (this->req->getMeth() == POST) { child_path = curr + this->req->getLoc()->upload_path; }
        else { child_path = curr + this->req->getLoc()->l_root; }
        
        if (chdir(child_path.c_str()) == -1) { mkdir(child_path.c_str(), 0755); chdir(child_path.c_str()); }
        
        std::string route = curr + ('/' + path + '/') + uri;
        close(pipefds[0]);
        if (dup2(pipefds[1], STDOUT_FILENO) == -1) { close(pipefds[1]); _exit(1); }
        close(pipefds[1]);

        for (std::map<int, Socket *>::const_iterator it = big_srv->getSock().begin(); it != big_srv->getSock().end(); ++it)
            close(it->second->GetFd());

        for (std::map<int, Client *>::const_iterator it = big_srv->getClie().begin(); it != big_srv->getClie().end(); ++it)
            close(it->second->GetFd());

        close(big_srv->getEpoolFd());

        execl(route.c_str(), route.c_str(), (char *)NULL);
        free(curr);
        _exit(1);
    }
    else // Processo padre
    {
        // Parent: close write end and read child's stdout from read end
        close(pipefds[1]);

        char buffer[4096];
        ssize_t nread;
        while ((nread = read(pipefds[0], buffer, sizeof(buffer))) > 0) {
            processed.append(buffer, static_cast<size_t>(nread));
        }
        close(pipefds[0]);

        int status = 0;
        waitpid(pid, &status, 0);
    }

    std::string inp = static_cast<std::string>(curr) + "/.input";
    remove(inp.c_str());
    free(curr);

    std::string response = "HTTP/1.1 " + strStatusCode(this->stat) + "\r\n"
        "Connection: close\r\n"
        "Content-Length: " + std::to_string(processed.size()) + "\r\n"
        "Content-Type: application/json\r\n"
        "\r\n" + processed;
    return response;
}

std::string Response::cookieGen()
{
    Session ses;

    srand((unsigned)time(NULL));
	ses.id =  rand();
    for (std::vector<Session>::const_iterator it = this->req->getCookie().begin(); it != this->req->getCookie().end(); ++it)
    {
        if (it->id != ses.id) { break; }
        ses.id = rand();
    }
    ses.path = "/";

    this->req->setCookie(ses);

    std::cout << GREEN << "New backed cookie!!" << RESET << " ID=" << ses.id << std::endl;

    return ("Set-Cookie: session_id=" + std::to_string(ses.id) + "; "
            "Path=" + ses.path + "; HttpOnly\r\n");
}

std::string Response::GenerateResp(bool is_upload)
{
    if (this->stat != 200 && (this->stat > 400 || this->stat < 300))
    {
        if (this->req->getMeth() == GET) { return("HTTP/1.1 " + strStatusCode(stat) + "\r\nConnection: close\r\n\r\n" + sendError()); }
        else { return("HTTP/1.1 " + strStatusCode(stat) + "\r\nConnection: close\r\n\r\n" + strStatusCode(stat)); }
    }
    if (is_upload) { return("HTTP/1.1 " + strStatusCode(stat) + "\r\nConnection: close\r\n\r\n"); }

    if (this->req->getUri() == this->req->getSrvB().auth_endpoint)
    {
        this->cookie_ses = cookieGen();
        std::string resp = "HTTP/1.1 " + strStatusCode(200) + "\r\n"
            "Connection: close\r\n"
            "Content-Length: 0\r\n" + this->cookie_ses +
            "Content-Type: text/plain\r\n"
            "\r\n";
        return resp;
    }

    if (this->req->getMeth() == GET)    { return (this->RespGET()); }
    if (this->req->getMeth() == POST)   { return (this->RespPOST()); }
    if (this->req->getMeth() == DELETE) { return (this->RespDELETE()); }
    return ("HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n\r\n" + sendError());
}