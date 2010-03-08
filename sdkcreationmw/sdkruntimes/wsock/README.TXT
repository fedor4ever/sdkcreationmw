WSOCK - replaces Series 60 TCP/IP stack with  Windows sockets
=============================================================

WSOCK is  a  protocol driver for Series 60 emulator that uses
Windows sockets to  access the  network.

Note that wsock cannot coexist with the built-in TCP/IP stack
because it uses  (must use)  the same address family,  socket
types and protocol ids. Therefore, in order to use wsock, you
must remove ip.tcpip.esk file from the emulator's filesystem.
The location of the  ip.tcpip.esk  depends on the  version of 
the platform. Check comments in group/bld.inf file.

When the protocol loads,  it  creates an  access point called
"Winsock". Select it when prompted for access point.
