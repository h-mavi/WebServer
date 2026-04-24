#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <map>
#include <cctype>
#include <string>
#include <vector>
#include <poll.h>
#include <sstream>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <stdexcept>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define RESET "\033[0m"
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define B_RED "\033[1;31m"
#define B_BLUE "\033[1;34m"
#define B_GREEN "\033[1;32m"
#define B_YELLOW "\033[1;33m"

#define MAX_REQUEST_SIZE 10485760

#define DEBUG_MODE 0
#define PROTOCOL   0
#define BACKLOGS   100
#define FAMILY     AF_INET;
#define SOCK_TYPE  SOCK_STREAM

#define REQUEST_FLAGS  EPOLLIN | EPOLLRDHUP | EPOLLERR
#define RESPONSE_FLAGS EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT

enum Methods
{
    GET,
    POST,
    DELETE,
    OTHER
};

class PersonalExe : public std::exception
{
    protected :
        std::string msg;
    public :
        PersonalExe(const char* msg) : msg(msg) {}
        virtual ~PersonalExe() throw() {}
        virtual const char* what() const throw()
        {
            return (msg.c_str());
        }
};

template <typename T>
typename std::vector<T>::iterator ft_find(std::vector<T> &vec, const T &value)
{
    typename std::vector<T>::iterator it;

    for (it = vec.begin(); it != vec.end(); ++it)
    {
        if (*it == value) { return it; }
    }
    return vec.end();
}

struct Session
{
    unsigned long   id;
    std::string     path;
};

std::string trimLine(std::string &line);
std::string take_value(std::string orr);
void        take_values(const std::string line, std::vector<std::string> &vec);
std::string GetPortOrIp(std::string listen, std::string wanted);
void        AddSocketToEpoll(int epollfd, int sockfd, uint32_t flags);
void        ModifySocketToEpoll(int epollfd, int sockfd, uint32_t flags);
void        RemoveSocketToEpoll(int epollfd, int sockfd);
int         error_checker(int val, int fail, std::string fat, std::string err);
int         checkExten(std::string uri);

#endif