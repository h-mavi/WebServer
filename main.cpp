# include "hpp/Server.hpp"

Server *big_srv;

void sighand(int sig)
{
    std::cout << "\b\b  \b\b";
    if (sig == SIGINT) { big_srv->StopWebSrv(); }
}

int main(int arc, char *arv[])
{
    if (arc != 2)
        return (1);

    Server srv;
    std::string file = arv[1];
        
    big_srv = &srv;
    signal(SIGINT, sighand);

    try { ConfArgs test(file); }
    catch(const PersonalExe& e) { std::cerr << B_RED << "ERROR" << RESET << " -> " << e.what() << std::endl; return 1; }

    srv.SetOpt(file);
    srv.print();

    try { srv.InitWebSrv(); srv.RunWebSrv(); }
    catch(const std::exception& e) { std::cerr << B_RED << "ERROR" << RESET << " -> " << e.what() << std::endl; return 1; }

}