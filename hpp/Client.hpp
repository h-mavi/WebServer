#ifndef CLIENT_HPP
#define CLIENT_HPP

# include "Socket.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Response;

class Client
{
    private:
        int							fd;
		bool						is_upload;
		Socket						*sock = NULL;
		const Socket				*serv;
		Request						*req = NULL;
		Response					*resp = NULL;
		std::string					img_body, filename, uri, up_path;
		std::vector<std::string>	&db_post;
		std::vector<Session>        &cookies;

		int	HandleRequeUpload(std::string tmp_req, bool help);

	public :
		Client(int fd_a, const Socket *serv_a, const ServerBlock &tmp, std::vector<std::string> &a_db, std::vector<Session>& cook);
		~Client();
        int	HandleReque();
		int &GetFd() { return(this->fd); }
};

#endif