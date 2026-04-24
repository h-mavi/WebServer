# include "../hpp/Socket.hpp"
# include "../hpp/ConfArgs.hpp"

Socket::Socket(int fd_a, int port_a, std::string ip_a, const std::vector<ServerBlock> &conf_srv, int i) : fd(fd_a), port(port_a), ip(ip_a), srvblck(conf_srv[i])
{
    try
    {
        this->address.sin_family = AF_INET;
        this->address.sin_port = htons(this->port);
        std::cout << YELLOW << "ip:port" << RESET << " = " << this->ip.c_str() << ":" << this->port << std::endl;
        this->address.sin_addr.s_addr = inet_addr(this->ip.c_str());
        if (error_checker(fcntl(this->fd, F_SETFL, O_NONBLOCK), -1, "FATAL", "function error, fcntl() failed")) {}
        int opt = 1;
        if (error_checker(setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)), -1, "FATAL", "function error, setsockopt() failed")) {}
        if (error_checker(bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address)), -1, "FATAL", "function error, bind() failed")) {}
        if (error_checker(listen(this->fd, BACKLOGS), -1, "FATAL", "function error, listen() failed")) {}
    }
    catch (const PersonalExe& e)
    {
        std::cerr << RED << "Minor Error" << RESET << " -> " << e.what() << std::endl;
        this->fd = -1;
    }
}

Socket::Socket(int fd_a, const ServerBlock &srvb) : fd(fd_a), srvblck(srvb)
{
    socklen_t cli_len = sizeof(this->address);
    try
    {
        if (error_checker(getsockname(this->fd, (struct sockaddr *)&this->address, &cli_len), -1, "FATAL", "function error, getsockname() failed")) {}
        this->port = ntohs(this->address.sin_port);
        this->ip = ntohl(this->address.sin_addr.s_addr);
    }
    catch (const PersonalExe& e) { std::cerr << RED << "Minor Error" << RESET << " -> " << e.what() << std::endl; }
}