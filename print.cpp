#include "hpp/Server.hpp"

/// @param m The method to tranform into a string
/// @return A string based on the method m
static std::string methodToString(Methods m)
{
    switch (m)
    {
        case GET:    return "GET";
        case POST:   return "POST";
        case DELETE: return "DELETE";
        case OTHER: return "UNKNOWN";
    }
    return "UNKNOWN";
}

/// @brief Makes sure that the print is correct and cool
/// @param len lenght of char before the actual infos
/// @param srv_last is this ServerBlock the last one?
/// @param loc_last is this LocationBlock the last one?
static void print_exe(int len, bool srv_last, bool loc_last)
{
    for (int i = 0; i < len; ++i)
    {
        if ((!srv_last && i == 3) || (!loc_last && i == 5)) { std::cout << "│"; }
        std::cout << " ";
    }
}

/// @brief Prints the content of the LocationBlock struct 
void LocationBlock::print(bool srv_last, bool loc_last) const
{
    print_exe(7, srv_last, loc_last);
    std::cout << "├root: " << l_root << std::endl;

    print_exe(7, srv_last, loc_last);
    std::cout << "├index(" << l_index.size() << "):";
    for (std::vector<std::string>::const_iterator it = l_index.begin(); it != l_index.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << std::endl;

    print_exe(7, srv_last, loc_last);
    std::cout << "├upload_path: " << upload_path << std::endl;

    print_exe(7, srv_last, loc_last);
    std::cout << "├autoindex: ";
    if (l_autoind) { std::cout << "on" << std::endl; }
    else { std::cout << "off" << std::endl; }

    print_exe(7, srv_last, loc_last);
    std::cout << "├protected: ";
    if (prot) { std::cout << "on" << std::endl; }
    else { std::cout << "off" << std::endl; }

    print_exe(7, srv_last, loc_last);
    std::cout << "├return: ";
    if (!ret.empty())
        for (std::map<int, std::string>::const_iterator it = ret.begin(); it != ret.end(); ++it)
            std::cout << it->first << " " << it->second << "  ";
    std::cout << std::endl;

    print_exe(7, srv_last, loc_last);
    std::cout << "├cgi_extension: ";
    if (!cgi_extension.empty())
        for (std::map<std::string, std::string>::const_iterator it = cgi_extension.begin(); it != cgi_extension.end(); ++it)
            std::cout << it->first << " " << it->second << "  ";
    std::cout << std::endl;

    print_exe(7, srv_last, loc_last);
    std::cout << "└methods:";
    for (std::vector<Methods>::const_iterator it = meth.begin(); it != meth.end(); ++it)
        std::cout << ' ' << BLUE << methodToString(*it) << RESET;
}

/// @brief Prints the content of the ServerBlock struct 
void ServerBlock::print(bool last) const
{
    int i = 0;
    std::cout << "ServerBlock:" << std::endl;

    print_exe(5, last, true);
    std::cout << "├root: " << root << std::endl;

    print_exe(5, last, true);
    std::cout << "├listen(" << listen.size() << "):";
    for (std::vector<std::string>::const_iterator it = listen.begin(); it != listen.end(); ++it)
        std::cout << ' ' << YELLOW << *it << RESET;
    std::cout << std::endl;

    print_exe(5, last, true);
    std::cout << "├server_name(" << server_name.size() << "):";
    for (std::vector<std::string>::const_iterator it = server_name.begin(); it != server_name.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << std::endl;

    print_exe(5, last, true);
    std::cout << "├index(" << index.size() << "):";
    for (std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << std::endl;

    print_exe(5, last, true);
    std::cout << "├error_page(" << errorpages.size() << "):";
    for (std::map<int, std::string>::const_iterator it = errorpages.begin(); it != errorpages.end(); ++it)
    {
        if (i == 3) { std::cout << " ..."; break; }
        std::cout << ' ' << it->first << ' ' << it->second;
        i++;
    }
    std::cout << std::endl;

    print_exe(5, last, true);
    std::cout << "├autoindex: ";
    if (autoind) { std::cout << "on" << std::endl; }
    else { std::cout << "off" << std::endl; }

    print_exe(5, last, true);
    std::cout << "├auth_endpoint: ";
    if (!auth_endpoint.empty()) { std::cout << auth_endpoint << std::endl; }
    else { std::cout << std::endl; }

    print_exe(5, last, true);
    std::cout << "├global methods:";
    for (std::vector<Methods>::const_iterator it = globalmeth.begin(); it != globalmeth.end(); ++it)
        std::cout << ' ' << BLUE << methodToString(*it) << RESET;
    std::cout << std::endl;
    
    if (!locations.empty())
    {
        print_exe(5, last, true);
        std::cout << "├client_max_body_size: " << client_max_body_size << std::endl;
        int n = 1;
        for (std::map<std::string, LocationBlock>::const_iterator it = locations.begin(); it != locations.end(); ++it)
        {
            print_exe(5, last, true);
            if (n < locations.size()) { std::cout << "├LocationBlock:  " << it->first << std::endl; it->second.print(last, false); std::cout << std::endl; n++; }
            else { std::cout << "└LocationBlock:  " << it->first << std::endl; it->second.print(last, true); n++; }
        }
    }
    else
    {
        print_exe(5, last, true);
        std::cout << "└client_max_body_size: " << client_max_body_size << std::endl;
    }
}

/// @brief Prints the content of the ConfArgs class
void ConfArgs::print() const
{
    std::cout << " └ConfArgs:" << std::endl;
    std::cout << "   ├file: " << file << std::endl;

    for (std::vector<ServerBlock>::const_iterator it = srvrs.begin(); it != srvrs.end(); ++it)
    {
        if (it + 1 != srvrs.end()) { std::cout << "   ├"; it->print(false); }
        else { if (it != srvrs.begin()) { std::cout << std::endl; } std::cout << "   └"; it->print(true); }
    }
    std::cout << std::endl << "---------------------" << std::endl;
}

/// @brief Prints the content of the Server class
void Server::print() const
{
	std::cout << "Server:" << std::endl;
	if (this->opt) { this->opt->print(); }
	else { std::cout << "  (no configuration loaded)" << std::endl; }
}