#ifndef SERVER_HPP
#define SERVER_HPP

# include "../Headers.hpp"
# include "ConfArgs.hpp"
# include "Client.hpp"
# include "Socket.hpp"

class Client;

enum State
{
    SRV_INIT,  //il server e' stato creato
    SRV_READY, //il server e' pronto all'avvio
    SRV_RUN,   //il server e' attivo 
    SRV_STOP   //il server e' statto fermato
};

class Server
{
    private :
        State                    status;
        int                      epollfd;
        ConfArgs*                opt;
        std::map<int, Socket*>   sock; //key value -> server fd
        std::map<int, Client*>   clie; //key value -> client fd
        std::vector<std::string> db_post;
        std::vector<Session>     cookies;

        void	HandleNewCon(int fd_cli, int fd_s);
        void    HandleDisco(int fd_cl);
    
    public :
        Server()  : status(SRV_INIT), epollfd(-1), opt(NULL) {}
        ~Server();

        void SetOpt(std::string conf_file) { this->opt = new ConfArgs(conf_file); }

        void InitWebSrv();
        void RunWebSrv();
        void StopWebSrv() { this->status = SRV_STOP; }

        const std::map<int, Socket*>& getSock() const { return (this->sock); }
        const std::map<int, Client*>& getClie() const { return (this->clie); }
        const int getEpoolFd() const { return(this->epollfd); }

        void print() const;
};
    
#endif