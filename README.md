# WebServer

A lightweight HTTP/1.1 web server written in C++98, inspired by nginx. Built as part of the 42 School curriculum, it handles multiple virtual servers, CGI execution, file uploads, cookie sessions, and more — all through a single non-blocking event loop powered by `epoll`.

---

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [For Developers](#for-developers)
  - [Requirements](#requirements)
  - [Build](#build)
  - [Configuration File](#configuration-file)
  - [Running the Server](#running-the-server)
  - [Memory Check](#memory-check)
- [For Users](#for-users)
  - [Accessing the Server](#accessing-the-server)
  - [Available Pages](#available-pages)
  - [HTTP Methods](#http-methods)
  - [Login & Sessions](#login--sessions)

---

## Features

- **Multiple virtual servers** on the same IP/port, resolved by `server_name`
- **Location-based routing** with per-location overrides
- **HTTP methods**: GET, POST, DELETE
- **CGI support**: PHP (`.php`) and Python (`.py`) scripts
- **File uploads** with configurable upload directories
- **Directory listing** (autoindex)
- **Custom error pages** per HTTP status code
- **HTTP redirects** via `return` directive
- **Cookie-based session management**
- **Protected locations** requiring authentication
- **Configurable `client_max_body_size`**
- Non-blocking I/O with `epoll`
- Graceful shutdown on `SIGINT` (`Ctrl+C`)

---

## Project Structure

```
.
├── main.cpp              # Entry point
├── Makefile
├── Headers.hpp           # Shared includes, macros, enums
├── tools.cpp             # Utility functions
├── print.cpp             # Debug/print utilities
├── CGI/                  # CGI helper scripts
├── conf/                 # Example configuration files
│   ├── server.conf
│   └── test.conf
├── cpp/                  # Source files
│   ├── Server.cpp        # Server lifecycle (init, run, stop)
│   ├── Socket.cpp        # TCP socket management
│   ├── ConfArgs.cpp      # Configuration file parser
│   ├── Client.cpp        # Client connection handling
│   ├── Request.cpp       # HTTP request parsing
│   ├── Resp_GET.cpp      # GET response logic
│   ├── Resp_POST.cpp     # POST response logic
│   ├── Resp_DELETE.cpp   # DELETE response logic
│   └── Resp_tools.cpp    # Shared response utilities
├── hpp/                  # Header files
└── www/                  # Web root
    ├── html/             # Static HTML pages & error pages
    ├── photos/           # Static images
    └── cgi-bin/          # CGI scripts (PHP, Python)
```

---

## For Developers

### Requirements

- **Linux** (epoll is Linux-specific)
- **g++** or **clang++** with C++98 support
- **PHP CLI** (for `.php` CGI scripts)
- **Python 3** (for `.py` CGI scripts)

### Build

```bash
make          # compile → produces ./webserv
make clean    # remove object files
make fclean   # remove object files and binary
make re       # full rebuild
```

### Configuration File

The server is configured via a text file (similar to nginx). Pass its path as the only argument at startup. See [`conf/test.conf`](conf/test.conf) for a complete example.

#### Server block directives

| Directive              | Description                                             |
|------------------------|---------------------------------------------------------|
| `listen`               | `<ip:port>` or `<port>` — can appear multiple times    |
| `server_name`          | Virtual host name(s), space-separated                   |
| `root`                 | Document root path                                      |
| `index`                | Default index file(s)                                   |
| `error_page <code>`    | Custom error page per HTTP status code                  |
| `allow_methods`        | Allowed HTTP methods (GET, POST, DELETE)                |
| `autoindex`            | `on` / `off` — directory listing                        |
| `client_max_body_size` | Max request body size in bytes (default: 10 MB)         |
| `auth_endpoint`        | Path of the login page used for session auth            |

#### Location block directives

| Directive        | Description                                                         |
|------------------|---------------------------------------------------------------------|
| `root`           | Override root for this location                                     |
| `index`          | Override index file(s)                                              |
| `allow_methods`  | Restrict methods for this location                                  |
| `autoindex`      | `on` / `off`                                                        |
| `upload_path`    | Directory where uploaded files are saved                            |
| `cgi_extension`  | `<.ext> <cgi-bin-path>` — enable CGI for that extension             |
| `return`         | `<code> <url>` — HTTP redirect                                      |
| `protected`      | `on` / `off` — require a valid session cookie to access             |

#### Minimal configuration example

```nginx
server {
    listen 127.0.0.1:8080;
    server_name mysite.com;
    root /www/html;
    index index.html;
    allow_methods GET, POST, DELETE;

    location /upload/ {
        root /www/upload;
        upload_path /www/upload;
    }

    location /api/ {
        cgi_extension .php /www/cgi-bin;
    }
}
```

### Running the Server

```bash
./webserv conf/test.conf
```

The server prints the resolved configuration, then starts listening. Press `Ctrl+C` to stop gracefully.

### Memory Check

A Valgrind target is included:

```bash
make val
# equivalent to:
valgrind --leak-check=full --track-fds=all --show-leak-kinds=all \
         --trace-children=yes --track-origins=yes ./webserv conf/test.conf
```

---

## For Users

### Accessing the Server

Start the server with the default configuration and open a browser:

```
http://127.0.0.1:8080
```

> Replace `127.0.0.1:8080` with whatever `listen` address is in the configuration file you started the server with.

### Available Pages

| URL                  | Description                                      |
|----------------------|--------------------------------------------------|
| `/`                  | Home page — welcome screen                       |
| `/post_txt.html`     | Submit a text message (POST via CGI)             |
| `/post_img.html`     | Upload an image (POST — multipart/form-data)     |
| `/random_num.html`   | Generate a random number via Python CGI          |
| `/login.html`        | Create a cookie session                          |

### HTTP Methods

- **GET** — retrieve pages, images, or directory listings.
- **POST** — submit forms, upload files, or trigger CGI scripts.
- **DELETE** — delete a previously uploaded file by sending a `DELETE` request to its URL.

### Login & Sessions

Navigating to `/login.html` lets you authenticate and receive a session cookie. Pages marked as `protected on` in the configuration will redirect unauthenticated visitors to the login page.
