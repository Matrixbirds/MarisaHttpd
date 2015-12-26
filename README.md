# MarisaHttpd

This is a standalone HTTP server with dynamic webpages support.


The dynamic pages are like PHP, but they are not text-based scripts, they are pre-compiled dynamic library files written in C or C++ (.so for *nix and .dll for Windows). The syntax is similiar to PHP and very easy to use.

Unlike many webservers, there are no worker processes (everything in one process with several threads). Plus epoll (only available in Linux) socket handling, the performance will be insane.

<h3>How to compile</h3>
The following libraries are required:

`libmicrohttpd`

`libmagic`

Run:


`gcc marisa.c minihakkero.c magicshop.c kirisame.c -lmicrohttpd -ldl -lpthread -lmagic -o marisa`

<h3>Example</h3>

See `dp_demo/moe.c`.

Compile with:

`gcc -fpic -shared dp_demo/moe.c kirisame.c -I. -o  moe.so`


