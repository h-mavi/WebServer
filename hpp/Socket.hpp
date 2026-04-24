#ifndef SOCKET_HPP
#define SOCKET_HPP

# include "../Headers.hpp"

struct ServerBlock;


class Socket
{
    private:
        int                fd;
        int                port;
        std::string        ip;
        struct sockaddr_in address;
        const ServerBlock& srvblck;
    
    public :
        Socket( int fd_a, int port_a, std::string ip_a, const std::vector<ServerBlock> &conf_srv, int i );
        Socket( int fd_a, const ServerBlock &srvb );
        int &GetFd() { return(this->fd); }
        const ServerBlock &GetsrvBlck() { return(this->srvblck); }
};

#endif