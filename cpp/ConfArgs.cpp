# include "../hpp/ConfArgs.hpp"

//----------------Struct Configuration----------------

LocationBlock& LocationBlock::operator=(const LocationBlock& original)
{
    if (this != &original)
    {
        this->ret = original.ret;
        this->meth = original.meth;
        this->prot = original.prot;
        this->l_root = original.l_root;
        this->l_index = original.l_index;
        this->l_autoind = original.l_autoind;
        this->upload_path = original.upload_path;
        this->cgi_extension = original.cgi_extension;
    }
    return (*this);
}

LocationBlock ConfArgs::conf_LocationBlock(std::ifstream &file, std::string& line)
{
    LocationBlock location;
    std::getline(file, line);
    while(line.find('}') == std::string::npos)
    {
        line = trimLine(line);
        if (line.empty() || line[0] == '#') { std::getline(file, line); continue; }

        if (line.find("root") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            location.l_root = take_value((line.c_str() + (line.find_last_of(' ') + 1)));

        else if (line.find("return") != std::string::npos && line.c_str()[line.size() - 1] == ';')
        {
            std::string value = (line.c_str() + line.find_last_of(' ') + 1);
            value.pop_back();
            location.ret[std::stoi(line.substr(line.find_first_of(' ') + 1, 3))] = value;
        }

        else if (line.find("autoindex") != std::string::npos && line.c_str()[line.size() - 1] == ';')
        {
            if (line.find("on;") != std::string::npos) { location.l_autoind = true; }
            else if (line.find("off;") != std::string::npos) { location.l_autoind = false; }
            else { throw(PersonalExe("Invalid configuration file, invalid parameter")); }
        }

        else if (line.find("protected") != std::string::npos && line.c_str()[line.size() - 1] == ';')
        {
            if (line.find("on;") != std::string::npos) { location.prot = true; }
            else if (line.find("off;") != std::string::npos) { location.prot = false; }
            else { throw(PersonalExe("Invalid configuration file, invalid parameter")); }
        }
            
        else if (line.find("index") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            take_values(line, location.l_index);

        else if (line.find("upload_path") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            location.upload_path = take_value((line.c_str() + (line.find_last_of(' ') + 1)));

        else if (line.find("cgi_extension") != std::string::npos && line.c_str()[line.size() - 1] == ';')
        {
            size_t space = line.find_first_of(' ');
            std::string tmp = (line.c_str() + line.find_last_of(' ') + 1);
            std::string key = line.substr(space + 1, line.find(' ', space + 1) - space - 1);
            tmp.pop_back();
            location.cgi_extension[key] = tmp;
        }

        else if (line.find("allow_methods") != std::string::npos && line.c_str()[line.size() - 1] == ';')
        {
            if (line.find("POST") != std::string::npos) { location.meth.push_back(POST); }
            if (line.find("GET") != std::string::npos) { location.meth.push_back(GET); }
            if (line.find("DELETE") != std::string::npos) { location.meth.push_back(DELETE); }
        }

        else if (line.c_str()[line.size() - 1] != ';') { throw(PersonalExe("Invalid configuration file, there's a ';' missing")); }

        else { throw(PersonalExe("Invalid configuration file, invalid parameter")); }

        std::getline(file, line);
    }

    if (!location.upload_path.size()) { location.upload_path = "var/www/upload"; }

    return (location);
}


ServerBlock ConfArgs::conf_ServerBlock(std::ifstream &file, std::string& line)
{
    ServerBlock server;
    int l = 0;
    std::getline(file, line);
    while(line.find('}') == std::string::npos)
    {
        line = trimLine(line);
        if (line.empty() || line[0] == '#') { std::getline(file, line); continue; }

        if (line.find("listen") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            server.listen.push_back(take_value((line.c_str() + (line.find_last_of(' ') + 1))));

        else if (line.find("server_name") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            take_values(line, server.server_name);
            
        else if (line.find("autoindex") != std::string::npos && line.c_str()[line.size() - 1] == ';')
        {
            if (line.find("on;") != std::string::npos) { server.autoind = true; }
            else if (line.find("off;") != std::string::npos) { server.autoind = false; }
            else { throw(PersonalExe("Invalid configuration file, invalid parameter")); }
        }

        else if (line.find("index") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            take_values(line, server.index);

        else if (line.find("auth_endpoint") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            server.auth_endpoint = take_value((line.c_str() + (line.find_last_of(' ') + 1)));

        else if (line.find("root") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            server.root = take_value((line.c_str() + (line.find_last_of(' ') + 1)));
            
        else if (line.find("client_max_body_size") != std::string::npos && line.c_str()[line.size() - 1] == ';')
            server.client_max_body_size = atoi(line.c_str() + line.find_first_of(' ') + 1);

        else if (line.find("error_page") != std::string::npos && line.c_str()[line.size() - 1] == ';')
        {
            std::string tmp = (line.c_str() + line.find_last_of(' ') + 1);
            tmp.pop_back();
            server.errorpages[atoi(line.c_str() + line.find_first_of(' ') + 1)] = tmp;
        }

        else if (line.find("allow_methods") != std::string::npos && line.c_str()[line.size() - 1] == ';')
        {
            if (line.find("POST") != std::string::npos) { server.globalmeth.push_back(POST); }
            if (line.find("GET") != std::string::npos) { server.globalmeth.push_back(GET); }
            if (line.find("DELETE") != std::string::npos) { server.globalmeth.push_back(DELETE); }
        }

        else if (line.find("location") != std::string::npos && \
        line.find('{', line.find("location") + 8) != std::string::npos)
        {
            std::string tmp = line;
            if (tmp.substr(tmp.find_first_of(' ') + 1, (tmp.find_last_of(' ') - tmp.find_first_of(' ') - 1)).back() != '/')
               throw(PersonalExe("Invalid configuration file, invalid location name")); 
            server.locations[tmp.substr(tmp.find_first_of(' ') + 1, (tmp.find_last_of(' ') - tmp.find_first_of(' ') - 1))] = conf_LocationBlock(file, line);
        }

        else if (line.c_str()[line.size() - 1] != ';') { throw(PersonalExe("Invalid configuration file, there's a ';' missing")); }

        else { throw(PersonalExe("Invalid configuration file, invalid parameter")); }

        std::getline(file, line);
    }

    if (!server.listen.size()) { server.listen.push_back("127.0.0.1:80"); }  //0.0.0.0:80

    if (!server.server_name.size()) { server.server_name.push_back("localhost"); }

    if (!server.index.size()) { server.index.push_back("index.html"); }

    if (!server.root.size()) { server.root = "/var/www/html"; }

    if (!server.errorpages.size()) { server.errorpages[0] = "/www/html/errorpage.html"; }

    if (server.autoind == -1) { server.autoind = false; }

    for (std::map<std::string, LocationBlock>::iterator it = server.locations.begin(); it != server.locations.end(); ++it)
    {
        if (it->second.prot == -1) { it->second.prot = false; }
        if (it->second.l_autoind == -1) { it->second.l_autoind = server.autoind; }
        if (!it->second.l_root.size()) { it->second.l_root = server.root; }
        if (!it->second.l_index.size()) { it->second.l_index = server.index; }
        if (!it->second.upload_path.size()) { it->second.upload_path = it->second.l_root; }
        if (!it->second.meth.size() && server.globalmeth.size()) { it->second.meth = server.globalmeth; }
        else if (!it->second.meth.size())
        {
            it->second.meth.push_back(POST);
            it->second.meth.push_back(GET);
            it->second.meth.push_back(DELETE);
        }
    }

    return (server);
}

bool ConfArgs::checknames(void)
{
    for (std::vector<ServerBlock>::iterator y = this->srvrs.begin(); y != this->srvrs.end(); ++y)
        for (std::vector<ServerBlock>::iterator x = y + 1; x != this->srvrs.end(); ++x)
            for (unsigned long i = 0; i < (*y).server_name.size(); ++i)
                for (unsigned long z = 0; z < (*x).server_name.size(); ++z)
                    if ((*y).server_name[i] == (*x).server_name[z])
                        return (false);
    return (true);
}

//----------------Constructor----------------

ConfArgs::ConfArgs(std::string conf_file)
{
    if (conf_file.find(".conf", conf_file.size() - 5) != conf_file.size() - 5)
        throw PersonalExe("invalid configuration file, file is not .conf");

    std::string line;
    this->file = conf_file;
    std::ifstream openfile(this->file.c_str());
    if (!openfile.is_open()) { throw PersonalExe("invalid configuration file, it coulnd't be open"); }

    while (std::getline(openfile, line))
    {
        line = trimLine(line);
        if (line.empty() || line[0] == '#')
            continue;
        if (line.find("server") != std::string::npos && \
        line.find('{', line.find("server") + 6) != std::string::npos)
            this->srvrs.push_back(conf_ServerBlock(openfile, line));
        else if (line[0] == '}') { break; }
        else { throw(PersonalExe("Invalid configuration file, invalid parameter")); }
    }

    if (!this->srvrs.size()) { throw PersonalExe("invalid configuration file, no server blocks in configuration file"); }

    if (!checknames()) { throw PersonalExe("invalid configuration file, 2 or more servers have the same servername"); }

    openfile.close();
}