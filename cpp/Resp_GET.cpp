#include "../hpp/Response.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"

class Client;

std::string Response::RespGETImg(std::string uri)
{
    std::vector<std::string> index;
    std::string file_names, root, response;
    const LocationBlock *tmp = this->req->getLoc();
    
    if (uri.find_first_of('/') != uri.find_last_of('/'))
        uri.erase(0, uri.find_last_of('/'));
    if (tmp && tmp->l_root.size()) { root = tmp->l_root; }
    else { root = this->req->getSrvB().root; }

    if (tmp && tmp->l_index.size()) { index = tmp->l_index; }
    else { index = this->req->getSrvB().index; }
        
    root.erase(0, 1);
    file_names = getFileNames(root);

    if (file_names.find(uri.c_str() + 1) == std::string::npos)
        return("HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n\r\n" + sendError());
    this->html.open(root + uri, std::ifstream::binary);
    if (!this->html.is_open())
        return "HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n\r\n" + sendError();
    
    this->html.seekg(0, std::ifstream::end);
    std::streamoff soff = this->html.tellg();
    this->html.seekg(0, std::ifstream::beg);
    if (soff < 0) { this->html.close(); return "HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n\r\n" + sendError(); }
    size_t imgSize = static_cast<size_t>(soff);

    this->body.resize(imgSize);

    std::streamsize bytesRead = this->html.read(&this->body[0], static_cast<std::streamsize>(imgSize)).gcount();
    if (bytesRead != imgSize) {
        this->html.close();
        return "HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n\r\n" + sendError();
    }

    response = "HTTP/1.1 " + strStatusCode(this->stat) + "\r\n"
        "Connection: close\r\n"
        "Content-Length: " + std::to_string(bytesRead) + "\r\n"
        "Content-Type: image/" + uri.substr(uri.find_last_of('.') + 1) + "\r\n"
        "\r\n";

    this->html.close();
    response.append(this->body.data(), this->body.size());

    size_t actualSize = response.size() - response.find("\r\n\r\n") - 4;
    if (actualSize != imgSize)
        return "HTTP/1.1 " + strStatusCode(413) + "\r\nConnection: close\r\n\r\n" + sendError();

    return (response);
}

std::string Response::dirListing(std::string uri, std::string files_name)
{
    int pipefds[2];
    if (pipe(pipefds) == -1) { return("HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n"); }
    
    std::string processed;

    std::ofstream filein;
    filein.open(".input", std::ios::out | std::ios::app);
    filein.write(uri.c_str(), uri.size());
    filein.write(" ", 1);
    filein.write(files_name.c_str(), files_name.size());
    filein.close();

    char *curr = get_current_dir_name();

    pid_t pid = fork();
    if (pid == -1) { return("HTTP/1.1 " + strStatusCode(500) + "\r\nConnection: close\r\n"); }

    if (pid == 0) // Processo figlio
    {   
        std::string route = curr;
        route += "/dirListing.php";
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

    return (processed);
}

std::string Response::RespGET()
{
    std::string uri = this->req->getUri();
    if (checkExten(uri) == 1) { return (this->RespGETImg(uri)); }
    if (checkExten(uri) == 5)
    {
        std::string tmp = launchCGI(uri);
        if (tmp != "ERROR")
            return (tmp);
        else { return("HTTP/1.1 " + strStatusCode(501) + "\r\nConnection: close\r\n\r\n" + strStatusCode(501)); }
    }

    size_t pos;
    std::vector<std::string> index;
    std::string file_names, root, response, key;
    const LocationBlock *tmp = this->req->getLoc();

    if (tmp && !tmp->l_root.empty()) { root = tmp->l_root.c_str(); }
    else { root = this->req->getSrvB().root; }

    if (tmp && !tmp->l_index.empty()) { index = tmp->l_index; }
    else { index = this->req->getSrvB().index; }

    if (uri.find_first_of('/') != uri.find_last_of('/') && uri.back() != '/')
        uri.erase(0, uri.find_last_of('/'));

    root.erase(0, 1);
    file_names = getFileNames(root);

    if (checkExten(uri) == -1)
    {
        if (uri != "/" && this->req->getSrvB().locations.find(uri) == this->req->getSrvB().locations.end())
            return("HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n\r\n" + sendError());
        for (int i = 0; i < index.size(); ++i)
            if (file_names.find(index[i]) != std::string::npos) { key = '/' + index[i]; break; }
        if (key.empty() && this->req->getLoc())
        {
            if (req->getLoc()->l_autoind == true && uri.back() == '/') { return (dirListing(uri, file_names)); }
            else if (req->getLoc()->l_autoind == false) { return("HTTP/1.1 " + strStatusCode(403) + "\r\nConnection: close\r\n\r\n" + sendError()); }
        }
    }
    else if (file_names.find(uri.c_str() + 1) == std::string::npos)
        return("HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n\r\n" + sendError());

    if (key.empty()) { pos = file_names.find(uri.c_str() + 1); }
    else { pos = file_names.find(key.c_str() + 1); }

    if (pos == std::string::npos)
        return("HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n\r\n" + sendError());
    size_t next = pos + key.size() - 1;
    if (next < file_names.size() && !std::isspace(static_cast<unsigned char>(file_names[next])))
        return("HTTP/1.1 " + strStatusCode(404) + "\r\nConnection: close\r\n\r\n" + sendError());

    if (key.empty()) { this->html.open(root + uri, std::ios::in); }
    else { this->html.open(root + key, std::ios::in); }

    std::getline(html, this->body, '\0');
    response = "HTTP/1.1 " + strStatusCode(this->stat) + "\r\n"
        "Connection: close\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Content-Type: text/html\r\n"
        "\r\n" + body;

    html.close();
    return (response);
}