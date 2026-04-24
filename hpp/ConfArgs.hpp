#ifndef CONFARGS_HPP
#define CONFARGS_HPP

# include "../Headers.hpp"

struct LocationBlock
{
    std::vector<Methods>                meth;
    std::vector<std::string>            l_index;
    std::map<std::string, std::string>  cgi_extension;
    std::map<int, std::string>          ret;
    std::string                         l_root, upload_path;
    int                                 l_autoind = -1, prot = -1;
    
    void print(bool srv_last, bool loc_last) const;
    LocationBlock& operator=(const LocationBlock& original);
};

struct ServerBlock
{
    std::string                             root, auth_endpoint;
    std::vector<Methods>                    globalmeth;
    std::vector<std::string>                listen, server_name, index;
    std::map<std::string, LocationBlock>    locations;
    std::map<int, std::string>              errorpages;
    int                                     autoind = -1;
    unsigned int                            client_max_body_size = MAX_REQUEST_SIZE;   
    void print(bool last) const;
};


class ConfArgs
{
    private :
        std::string                file;
        std::vector<ServerBlock>   srvrs;

        LocationBlock conf_LocationBlock(std::ifstream &file, std::string& line);
        ServerBlock   conf_ServerBlock(std::ifstream &file, std::string& line);
        bool          checknames(void);

    public:
        ConfArgs() : file("") {}
        ConfArgs(std::string conf_file);

        void    print() const;

        const std::vector<ServerBlock> &GetSrv() { return (this->srvrs); }
};

#endif