/*
 * Service Loading (SL) Document Type Definition.
 * 
 * Copyright Wireless Application Protocol Forum Ltd., 1998,1999.
 * All rights reserved.  
 * 
 * SL is an XML language.  Typical usage:
 *    <?xml version="1.0"?>
 *    <!DOCTYPE si PUBLIC "-//WAPFORUM//DTD SL 1.0//EN"
 *                 "http://www.wapforum.org/DTD/sl.dtd">
 *    <sl>
 *    ...
 *    </sl>
 * 
 * Terms and conditions of use are available from the Wireless Application
 * Protocol Forum Ltd. web site at http://www.wapforum.org/docs/copyright.htm
 */
static char SL10DTD[] = \
"<!ENTITY % URI \"CDATA\"> \n"\
"<!ELEMENT sl EMPTY> \n"\
"<!ATTLIST sl \n"\
"href %URI; #REQUIRED \n"\
"action (execute-low|execute-high|cache) \"execute-low\" \n"\
">\n";
