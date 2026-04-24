# include "../hpp/Server.hpp"

Server::~Server()
{
	if (this->opt != NULL) { delete this->opt; }
	if (this->epollfd != -1) {  std::cout << RED << "closing " << RESET << this->epollfd << " epollfd" << std::endl; close(this->epollfd); }
	if (!sock.empty())
	{
		for (std::map<int, Socket*>::iterator iter = this->sock.begin(); iter != sock.end(); ++iter)
		{
			std::cout << RED << "closing " << RESET << sock[iter->first]->GetFd() << " server fd" << std::endl;
			close(sock[iter->first]->GetFd());
			delete this->sock[iter->first];
		}
		for (std::map<int, Client*>::iterator iter = this->clie.begin(); iter != clie.end(); ++iter)
		{
			std::cout << RED << "closing " << RESET << clie[iter->first]->GetFd() << " client fd" << std::endl;
			close(clie[iter->first]->GetFd());
			delete this->clie[iter->first];
		}
	}
	close(0);
	close(1);
	close(2);
}

static bool IsServ(int fd, const std::map<int, Socket*> sock)
{
	if (sock.find(fd) == sock.end())
		return (false);
	return (true);
}

void	Server::HandleNewCon(int fd_cli, int fd_s)
{
	this->clie[fd_cli] = new Client(fd_cli, this->sock[fd_s], this->sock[fd_s]->GetsrvBlck(), this->db_post, this->cookies);
	AddSocketToEpoll(this->epollfd, fd_cli, RESPONSE_FLAGS);
}

void Server::HandleDisco(int fd_cl)
{
	RemoveSocketToEpoll(this->epollfd, fd_cl);
	std::cout << RED << "closing " << RESET << fd_cl << " client fd" << std::endl;
	close(fd_cl);
	std::map<int, Client*>::iterator iter = this->clie.find(fd_cl);

	if (iter != this->clie.end())
	{
		delete iter->second;
		this->clie.erase(iter->first);
	}
}

void Server::InitWebSrv()
{
	this->epollfd = epoll_create(O_CLOEXEC);
	if (this->epollfd == -1) { throw PersonalExe("function error, epoll_create() failed"); }

	const std::vector<ServerBlock> &tmp = this->opt->GetSrv();
	int ns = 0;

	for (unsigned long i = 0; i < tmp.size(); ++i)
	{
		for (unsigned long x = 0; x < tmp[i].listen.size(); ++x)
		{
			int tmpfd = socket(AF_INET, SOCK_STREAM, 0);
			if (tmpfd < 0)
				throw PersonalExe("function error, socket() failed");
			
			this->sock[tmpfd] = new Socket(tmpfd, std::atoi(GetPortOrIp(tmp[i].listen[x], "port").c_str()), GetPortOrIp(tmp[i].listen[x], "ip"), tmp, i);
			if (this->sock[tmpfd]->GetFd() == -1)
			{
				std::cout << RED << "Closing " << RESET << tmpfd << " server fd" << std::endl;
				close(tmpfd);
				delete this->sock[tmpfd];
				this->sock.erase(tmpfd);
			}
			else
				AddSocketToEpoll(this->epollfd, tmpfd, REQUEST_FLAGS);
		}
	}
	std::cout << "---------------------" << std::endl;
	this->status = SRV_READY;
}

static void EventToString(uint32_t m)
{
	if (m & EPOLLONESHOT) { std::cout<< "EPOLLONESHOT "; }
	if (m & EPOLLRDHUP)   { std::cout<< "EPOLLRDHUP "; }
	if (m & EPOLLERR)     { std::cout<< "EPOLLERR "; }
	if (m & EPOLLHUP)     { std::cout<< "EPOLLHUP "; }
	if (m & EPOLLPRI)     { std::cout<< "EPOLLPRI "; }
	if (m & EPOLLOUT)     { std::cout<< "EPOLLOUT "; }
	if (m & EPOLLIN)      { std::cout<< "EPOLLIN "; }
	if (m & EPOLLET)      { std::cout<< "EPOLLET "; }
}


void Server::RunWebSrv()
{
	if (this->status != SRV_READY) { throw PersonalExe("program error, server tried to start but wasn't ready"); }

	this->status = SRV_RUN;
	epoll_event events[BACKLOGS];

	while (this->status == SRV_RUN)
	{
		int fds = epoll_wait(this->epollfd, events, BACKLOGS, 500); //fsd == numeri di fd pronti per eventi I/O
		if (!error_checker(fds, -1, "ok", "function error, epoll_wait() failed"))
			break;
		for (int i = 0; i < fds; ++i)
		{
			EventToString(events[i].events);
			std::cout << "on fd " << events[i].data.fd << std::endl;
			if (events[i].events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
			{
				if (!IsServ(events[i].data.fd, this->sock) && this->clie.find(events[i].data.fd) != this->clie.end())
				{
					std::cout << "Disconection" << std::endl;
					HandleDisco(events[i].data.fd);
					continue;
				}
			}
			else if (events[i].events & EPOLLIN)
			{
				if (IsServ(events[i].data.fd, this->sock))
				{
					struct sockaddr_in cli_addr;
    				socklen_t cli_len = sizeof(cli_addr);
					int fd_cl = accept(events[i].data.fd, (struct sockaddr *) &cli_addr, &cli_len);
					if (!error_checker(fd_cl, -1, "ok", "function error, accept() failed for a client"))
						continue ;

					if (this->clie.find(fd_cl) == this->clie.end())
					{
						std::cout << "New client! Creating new object" << std::endl;
						HandleNewCon(fd_cl, events[i].data.fd); //allora l'fd dato da events[i].data.fd è l'fd del server!!						
					}
					
				}
				else if (!IsServ(events[i].data.fd, this->sock) && this->clie.find(events[i].data.fd) != this->clie.end())
				{
					std::cout << "Same client (2), new request" << std::endl;
					if (!this->clie[events[i].data.fd]->HandleReque())
						HandleDisco(events[i].data.fd);
					std::cout << "---------------------" << std::endl;
				}
			}
			else if (events[i].events & EPOLLOUT) //qua secondo me dovremo fare anche un controllo sull'fd per assicurarci che è registrato
			{
				std::cout << "Same client (1), new request" << std::endl;
				if (!this->clie[events[i].data.fd]->HandleReque())
					HandleDisco(events[i].data.fd);
				std::cout << "---------------------" << std::endl;
			}
		}
	}
}