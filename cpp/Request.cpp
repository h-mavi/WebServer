# include "../hpp/Request.hpp"


Request::Request(std::string rawreq, const ServerBlock& srv, bool isUp, std::vector<Session>& cook) : srvblock(srv), cookies(cook)
{
	original = rawreq;
	this->isUp = isUp;

	// std::cout<<"xxxxxxxx raw = " << uri <<"xxxxxxxx"<<std::endl;

	if (this->isUp == true) { this->meth = OTHER; body = rawreq; return; }
	
	if (rawreq.substr(0, rawreq.find_first_of(' ')) == "GET") 		  {meth = GET;}
	else if (rawreq.substr(0, rawreq.find_first_of(' ')) == "POST")   {meth = POST;}
	else if (rawreq.substr(0, rawreq.find_first_of(' ')) == "DELETE") {meth = DELETE;}
	else {meth = OTHER;}
	
	rawreq.erase(0, rawreq.find_first_of('/'));
	uri = rawreq.substr(0, rawreq.find_first_of(' '));

	rawreq.erase(0, rawreq.find_first_of('H'));
	version = rawreq.substr(0, rawreq.find_first_of("\r\n"));
	rawreq.erase(0, rawreq.find_first_of("\r\n\r\n") + 2);
	while (rawreq.size() && rawreq.substr(0, 2) != "\r\n")
	{
		std::string key = rawreq.substr(0, rawreq.find_first_of(':'));
		if (key == "Host" || key == "Content-Length" || key == "Transfer-Encoding" || key == "Cookie")
			header[key] = rawreq.substr(rawreq.find_first_of(' ') + 1, rawreq.find_first_of("\r\n") - rawreq.find_first_of(' '));
		rawreq.erase(0, rawreq.find_first_of("\r\n") + 2);
	}
	if (rawreq.find('\r') != std::string::npos)
	{
		rawreq.erase(0, 2);
		body = rawreq;
	}
	bodylen = 0;
}

int Request::parseUri(std::string line)
{
	if (this->uri[0] != '/' || this->uri.find("../") != std::string::npos || line.find(' ') != std::string::npos || line.find('\t') != std::string::npos)
			return false;
	for (int i = 0; i != static_cast<int>(line.size()); i++)
	{
		if (!isprint(line[i]))
				return false;
	}
	return true;
}

static int hasAlpha(std::string line)
{
	for (int i = 0; i != static_cast<int>(line.size()); i++)
	{
		if (!isdigit(line[i])) { return false; }
	}
	return true;
}

int Request::parseHeader()
{
	if (header.find("Host") == header.end()) { return false;}
	// if (ft_find(getSrvB().listen, header.find("Host")->second) == getSrvB().listen.end() && \
	// 	ft_find(getSrvB().server_name, header.find("Host")->second) == getSrvB().server_name.end() && \
	// 	header.find("Host")->second != "localhost:8080") { return false; }
	if (this->getMeth() != GET)
	{
		if (header.find("Content-Length") != header.end() && header.find("Transfer-Encoding") != header.end()) { return false; }
		if (header.find("Content-Length") != header.end())
		{
			if (hasAlpha(header.find("Content-Length")->second)) { return false; }
				bodylen = std::atoi(header.find("Content-Length")->second.c_str());
		}
	}
	return true;
}

int Request::parseBody()
{
	if (body.find("Content-Type: image") != std::string::npos)	 { return true;  }
	if (body.empty() && bodylen != 0) 		 					 { return false; }
	if (body.size() > MAX_REQUEST_SIZE / 10) 					 { return false; }
	if (body.size() != bodylen) 								 { return false; }
	if (this->srvblock.client_max_body_size < bodylen) 			 { return false; }
	if (body.size() > this->srvblock.client_max_body_size)		 { return false; }
	if (bodylen > 0 && static_cast<int>(body.size()) != bodylen) { return false; }

	return true;
}

static std::string cutUri(std::string uri)
{
	if (uri.find_last_of('/') != 0) { return uri.substr(0, uri.find_first_of('/')); }

	return uri.substr(0, uri.size());
}

int Request::findLoc(std::map<std::string, LocationBlock>::const_iterator *it)
{
	if (this->uri.find_first_of('/') != uri.find_last_of('/'))
	{
		std::string tmp = this->uri;
		tmp.erase(uri.find_last_of('/') + 1, uri.size() - uri.find_last_of('/'));
		*it = srvblock.locations.find(tmp);
	}
	else
		*it = srvblock.locations.find(uri);
	if (*it == srvblock.locations.end())
	{
		*it = srvblock.locations.find("/");
		if (*it == srvblock.locations.end())
		{
			if (srvblock.locations.empty())
			{
				this->loc = NULL;
				return true;
			}
			*it = srvblock.locations.begin();
		}
	}
	return false;
}

int Request::ParsReq()
{
	std::string line = original.substr(0, original.find_first_of("\r\n"));
	
	if (static_cast<int>(this->original.size()) == MAX_REQUEST_SIZE) { return 413; }

	if (srvblock.globalmeth.size())
	{
		int j;
		for (j = 0; j != srvblock.globalmeth.size(); j++)
			if (srvblock.globalmeth[j] == this->meth) { break; }
		if (isUp == true && this->meth == OTHER) { return 200; }
		else if (j == static_cast<int>(srvblock.globalmeth.size())) { return 405; }
	}

	if (isUp == true) { return 200; }
	
	if (line.size() == 8192)	{ return 414; }
	if (meth == OTHER) 			{ return 405; }
	if (version != "HTTP/1.1") 	{ return 505; }
	if (!parseUri(this->uri)) 	{ return 400; }
	for (size_t i = uri.find("%20"); i != std::string::npos;)
	{
		uri.replace(i, 3, " ");
		i = uri.find("%20");
	}
	
	if (!parseHeader()) { return 400;}
	if (header.find("Transfer-Encoding") != header.end())
	{
		if (header.find("Transfer-Encoding")->second != "chunked") { return 501; }
		bodylen = -1;
	}
	
	if (!body.empty() && meth == GET) 						 { return 405; }
	else if (!body.empty())			  { if (!parseBody()) 	 { return 413; }}
	
	std::map<std::string, LocationBlock>::const_iterator it;
	if (findLoc(&it)) { return 200; }
	this->loc = &it->second;
	std::map<int, std::string>::const_iterator jt = this->loc->ret.begin();
	bool ret = false;
	if (!this->loc->ret.empty()) 
	{
		if (checkExten(jt->second) == -1)
		{
			this->loc = &srvblock.locations.find(jt->second)->second;
			uri.erase(0, uri.find_first_of('/', 1));
			uri = jt->second + uri;
		}
		else
		{
			ret = true;
			this->uri = jt->second;
			if (findLoc(&it)) { return 200; }
			this->loc = &it->second;
		}
	}

	if (this->loc->prot == true && this->header.find("Cookie") == this->header.end()) { return 401; }
	else if (this->loc->prot == true)
	{
		std::vector<Session>::const_iterator it;

		std::string str = this->header.find("Cookie")->second;
		std::cout << "++++++++:"<<str.substr(str.find("=") + 1, str.size() - str.find("="))<<"++++++++"<<std::endl;
		for (it = this->cookies.begin(); it != this->cookies.end(); ++it)
    	{
			std::cout << "++++++++:"<<it->id<<"++++++++"<<std::endl;
    	    if (it->id == std::atol(str.substr(str.find("=") + 1, str.size() - str.find("=")).c_str())) { break ;}
    	}
		if (it == this->cookies.end()) { return 401; }
	}

	int i, j;
	for (i = 0; i != static_cast<int>(it->second.meth.size()); i++)
		if (it->second.meth[i] == this->meth) { break; }
	if (srvblock.globalmeth.size())
	{
		for (j = 0; j != srvblock.globalmeth.size(); j++)
			if (srvblock.globalmeth[j] == this->meth) { break; }
		if (j == static_cast<int>(srvblock.globalmeth.size())) { return 405; }
	}
	if (i == static_cast<int>(it->second.meth.size())) { return 405; }
	if (ret) { return jt->first; }
	return 200;
}
