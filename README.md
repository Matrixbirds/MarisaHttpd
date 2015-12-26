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

Then, 

`mkdir test`

`mv moe.so test`

`./marisa -m epoll -b 127.0.0.1 -p 8866 -d test`

Then, open `http://127.0.0.1:8866/moe` in your browser.


Result on my workstation:


<img src="https://lh3.googleusercontent.com/-QL58v679H1s/Vn63lgo1QJI/AAAAAAAAZVQ/bss7nZKvm-I/w1169-h1653/marisa-dp-demo.png"></img>


Note: The picture `dp_demo/34689497_p0.jpg` is illustrated by <a href="http://www.pixiv.net/member.php?id=292644">えふぇ</a>.


<h3>Notes</h3>

Important: You'd better recompile both the server (`marisa.c`) and your dynamic pages after making changes to `kirisame.c` and `kirisame.h`. Otherwise a segfault may occur (if you changed the data structure).


