#include "../hpp/Client.hpp"

Client::Client(int fd_a, const Socket* serv_a, const ServerBlock &tmp, std::vector<std::string>& a_db, std::vector<Session>& cook) : fd(fd_a), serv(serv_a), db_post(a_db), is_upload(false), cookies(cook)
{
    this->sock = new Socket(this->fd, tmp);
}

Client::~Client()
{
    if (resp) { delete resp; }
    if (sock) { delete sock; }
    if (req)  { delete req; }
    if (this->fd != -1) { close(this->fd); }
}

int    Client::HandleRequeUpload(std::string tmp_req, bool help)
{
    if (this->img_body.empty() && !this->req->getBody().empty())
    {
        this->img_body = this->req->getBody();
        if (this->req->getBody().substr(0, 24) == "------WebKitFormBoundary")
		{
            int start = img_body.find("filename=\"") + 10, end = img_body.find_first_of("\"", img_body.find("filename=\"") + 10);
            this->filename = img_body.substr(start, end - start);

			for (int i = 0; i < 4; ++i) { img_body.erase(0, img_body.find_first_of("\r\n\r\n") + 2); }
		}
    }
    else
        this->img_body.append(this->req->getBody());

    if (this->is_upload == false)
        this->is_upload = true;
    
    if (help) { delete this->req; }
    return 1;
}

int    Client::HandleReque()
{
    char buff[1892];
    memset(buff, 0, 1892);
    std::string tmp_req, response, body;
    std::ifstream html;
    static bool   help = false;

    ssize_t len = recv(this->fd, buff, sizeof(buff) -1, 0);
    if (!error_checker(len, -1, "ok", "function error, recv() failed") || \
            !error_checker(len, 0, "ok", "function error, recv() failed"))
        return 0;
        
    buff[len] = '\0';
    tmp_req.assign(buff, len);

    this->req = new Request(tmp_req, this->sock->GetsrvBlck(), is_upload, this->cookies);
    int stat = req->ParsReq();
    std::cout << "error code: " << stat << std::endl;

    if (((this->req->getMeth() == POST && tmp_req.find("boundary=") != std::string::npos) || help == true) && stat == 200)
    {
        if (this->uri.empty()) { this->uri = this->req->getUri(); this->up_path = this->req->getLoc()->upload_path; }
        if (help == false) { help = true; }
        else if (tmp_req.find("------WebKitFormBoundary") != std::string::npos && \
        tmp_req.find("------WebKitFormBoundary") != 0 && tmp_req.find(this->uri) == std::string::npos)
        {
            help = false;
            this->HandleRequeUpload(tmp_req, help);
            this->img_body = this->img_body.substr(0, img_body.find("------WebKitFormBoundary"));

            char *curr = get_current_dir_name();
            std::string path = curr + this->up_path;
            if (chdir(path.c_str()) == -1) { mkdir(path.c_str(), 0755); chdir(path.c_str()); }
            
            std::ofstream file(this->filename, std::ios::out | std::ios::trunc);
            file.write(this->img_body.c_str(), this->img_body.size());
            file.close();
            chdir(curr);
            free(curr);
        }

        if (help == true)
            {return this->HandleRequeUpload(tmp_req, help);}
    }
    
    this->resp = new Response(this->req, stat, this->db_post);

    response = this->resp->GenerateResp(is_upload);

    if (DEBUG_MODE && !this->db_post.empty())
    {
        std::cout<< std::endl << GREEN << "Database:" << RESET << std::endl;
        for (int i = 0; i != static_cast<int>(this->db_post.size()); ++i)
            std::cout<<"///////:"<<this->db_post[i]<<":///////"<<std::endl;
    }

    if (DEBUG_MODE)
    {
        std::cout << std::endl << GREEN << "Request:" << RESET << std::endl;
        if (this->img_body.empty()) { std::cout << tmp_req << std::endl; }
        else { std::cout << this->img_body << std::endl; }
    }

    if (DEBUG_MODE /* && checkExten(req->getUri()) != 1 */)
        std::cout << std::endl << GREEN << "Response:" << RESET << std::endl << response << std::endl;

    if (DEBUG_MODE)
    {
        std::cout << std::endl << GREEN << "Cookies:" << RESET << std::endl;
        for (std::vector<Session>::iterator it = this->cookies.begin(); it != this->cookies.end(); ++it)
            std::cout << "---" << it->id << std::endl;
    }

    ssize_t sd = send(this->fd, response.c_str(), static_cast<ssize_t>(response.size()), 0);
    error_checker(sd, -1, "ok", "Error on send()");
    error_checker(sd, 0, "ok", "Error on send()");
    return 0;
}