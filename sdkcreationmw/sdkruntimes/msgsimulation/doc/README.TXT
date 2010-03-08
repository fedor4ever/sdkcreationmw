
MessageRelay
============

MessageRelay is an application to handle file based
messaging. It is able to read SMS messages from a file
and write SMS messages to a file.

MessageRelay also relays messages to WMAPI. WMAPI can
register listeners to specific SMS ports, and when a
messag to such a port is received, it is sent to WMA.

MessageRelay is divided to MsgRelay, MsgSession and
MessageRelayApp.

MsgRelay
--------

MsgRelay is the server part of the relay. It 
relays messages from and to WMA and MCE Inbox.
Implemented in MsgRelay.dll

MsgSession
----------

MsgSession contains a client to connect to MsgRelay.
With it, applications are able to send and receive
SMS messages via specific ports.
Implemented in MsgSession.dll

MessageRelayApp
-----------

MsgRelayApp implements the actual writing and
reading of the files. It polls the inbox directory
for new messages, and routes them to MsgRelay for
further processing.
Implemented in MessageRelay.app


Building
========

MsgRelay must be built first.