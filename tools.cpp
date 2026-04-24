# include "Headers.hpp"

std::string trimLine(std::string &line)
{
	const std::string white_chars = " \t\n\r\f\v";
	std::string result;

	int start = 0, end = line.size() - 1;
	while (white_chars.find(line[start]) != std::string::npos)
		start++;

	while (end >= 0 && white_chars.find(line[end]) != std::string::npos)
		end--;

	for (int i = start; i <= end; i++)
		result.push_back(line[i]);
	return (result);
}

std::string take_value(std::string orr)
{
    std::string str;

    for(int i = 0; orr[i] != ';'; ++i)
        str.insert(i, 1, orr.c_str()[i]);
    return (str);
}

void    take_values(const std::string line, std::vector<std::string> &vec)
{
    std::string tmp = (line.c_str() + line.find_first_of(' '));
    int len = 0;

    for(int i = 0; tmp[i] != ';'; ++i)
    {
        if (tmp[i] != 32 && tmp[i - 1] == 32)
        {
            len = i;
            while (tmp[len] != 32 && tmp[len] != ';')
                len++;
			len -= i;
            vec.push_back(tmp.substr(i, len));
        }
    }
}

std::string GetPortOrIp(std::string listen, std::string wanted)
{
	if (wanted == "port")
	{
		if (listen.find(':') == std::string::npos && listen.find('.') == std::string::npos)
			return (listen);
		else if (listen.find(':') != std::string::npos && listen.find('.') != std::string::npos)
			return (listen.c_str() + listen.find_last_of(':') + 1);
		else if (listen.find(':') == std::string::npos && listen.find('.') != std::string::npos)
			return ("80");
	}
	else if (wanted == "ip")
	{
		if (listen.find(':') == std::string::npos && listen.find('.') == std::string::npos)
            return ("127.0.0.1"); //0.0.0.0
		else if (listen.find(':') == std::string::npos && listen.find('.') != std::string::npos)
			return (listen);
		else if (listen.find(':') != std::string::npos && listen.find('.') != std::string::npos)
        {
            std::string tmp = listen;
            tmp.resize(listen.find_last_of(':'));
			return (tmp);
        }
	}
	return (NULL);
}

void	AddSocketToEpoll(int epollfd, int sockfd, uint32_t flags)
{
	epoll_event evt;

	evt.events = flags;
	evt.data.fd = sockfd;

	if (error_checker(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &evt), -1, "FATAL", \
						"function error, epoll_ctl() failed while trying to add a fd")) {}	
}

void	ModifySocketToEpoll(int epollfd, int sockfd, uint32_t flags)
{
	epoll_event evt;
	
	evt.events = flags;
	evt.data.fd = sockfd;

	if (error_checker(epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &evt), -1, "FATAL", \
						"function error, epoll_ctl() failed() while trying to modify a fd")) {}
}

void	RemoveSocketToEpoll(int epollfd, int sockfd)
{
	if (error_checker(epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL), -1, "FATAL", \
						"function error, epoll_ctl() failed while trying to delete a fd")) {}	
}

int error_checker(int val, int fail, std::string fat, std::string err)
{
	if (val == fail)
	{
		if (fat == "FATAL")
			throw PersonalExe(err.c_str());
		else
			return (std::cerr << RED << "Minor Error" << RESET << " -> " << err << std::endl, 0);
	}
	return (1);
}

int checkExten(std::string uri)
{
	std::string ext;
	try { ext = uri.substr(uri.find_last_of('.')); }
	catch(const std::exception& e) { return (-1); }

	ext = uri.substr(uri.find_last_of('.'));

    if (ext == ".css" || ext == ".csv" || ext == ".htm" || ext == ".html" || ext == ".ics" || ext == ".js" || \
            ext == ".md" || ext == ".mjs" || ext == ".txt")
        return (0); //text
    else if (ext == ".apng" || ext == ".avif" || ext == ".bmp" || ext == ".gif" || ext == ".ico" || ext == ".jpeg" || \
                ext == ".jpg" || ext == ".png" || ext == ".svg" || ext == ".tif" || ext == ".tiff" || ext == ".webp")
        return (1); //images
    else if (ext == ".avi" || ext == ".mp4" || ext == ".mpeg" || ext == ".ogv" || ext == ".ts" || ext == ".webm" || \
                ext == ".3gp" || ext == ".3g2")
        return (2); //video
    else if (ext == ".aac" || ext == ".mid" || ext == ".midi" || ext == ".mp3" || ext == ".oga" || ext == ".opus" || \
                ext == ".wav" || ext == ".weba" || ext == ".3gp" || ext == ".3g2")
        return (3); //audio
    else if (ext == ".otf" || ext == ".ttf" || ext == ".woff" || ext == ".woff2")
        return (4); //font
    else if (ext == ".abw" || ext == ".arc" || ext == ".azw" || ext == ".bin" || ext == ".bz" || ext == ".bz2" || \
                ext == ".cda" || ext == ".csh" || ext == ".doc" || ext == ".docx" || ext == ".eot" || ext == ".epub" || \
                ext == ".gz" || ext == ".jar" || ext == ".json" || ext == ".jsonld" || ext == ".mpkg" || ext == ".odp" || \
                ext == ".ods" || ext == ".odt" || ext == ".ogx" || ext == ".pdf" || ext == ".php" || ext == ".ppt" || \
                ext == ".pptx" || ext == ".rar" || ext == ".rtf" || ext == ".sh" || ext == ".tar" || ext == ".vsd" || \
                ext == ".webmanifest" || ext == ".xhtml" || ext == ".xls" || ext == ".xlsx" || ext == ".xml" || \
                ext == ".xul" || ext == ".zip" || ext == ".7z" || ext == ".py")
        return (5); //application
    return(-1);
}