/*
 * Wireless Markup Language (WML) Document Type Definition.
 * 
 * Copyright Wireless Application Protocol Forum Ltd., 1998,1999.
 * All rights reserved.  
 * 
 * WML is an XML language. Typical usage:
 * <?xml version="1.0"?>
 * <!DOCTYPE wml PUBLIC "-//WAPFORUM//DTD WML 1.2//EN"
 * "http://www.wapforum.org/DTD/wml12.dtd">
 * <wml>
 * ...
 * </wml>
 * 
 * Terms and conditions of use are available from the Wireless Application 
 * Protocol Forum Ltd. web site at http://www.wapforum.org/docs/copyright.htm
 */
static char WML12DTD[] =
"<!ENTITY % length \"CDATA\"> \n"\
"<!ENTITY % vdata \"CDATA\"> \n"\
"<!ENTITY % HREF \"%vdata;\"> \n"\
"<!ENTITY % boolean \"(true|false)\">\n"\
"<!ENTITY % number \"NMTOKEN\"> \n"\
"<!ENTITY % coreattrs \"id ID #IMPLIED\n"\
"class CDATA #IMPLIED\">\n"\
"<!ENTITY % ContentType \"%vdata;\"> \n"\
"<!ENTITY % emph \"em | strong |b |i |u |big |small\">\n"\
"<!ENTITY % layout \"br\">\n"\
"<!ENTITY % text \"#PCDATA | %emph;\">\n"\
"<!ENTITY % flow \"%text; | %layout; | img | anchor |a |table\">\n"\
"<!ENTITY % task \"go | prev | noop | refresh\">\n"\
"<!ENTITY % navelmts \"do | onevent\">\n"\
"<!ELEMENT wml ( head?, template?, card+ )>\n"\
"<!ATTLIST wml\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ENTITY % cardev\n"\
"\"onenterforward %HREF; #IMPLIED\n"\
"onenterbackward %HREF; #IMPLIED\n"\
"ontimer %HREF; #IMPLIED\"\n"\
">\n"\
"<!ENTITY % fields \"%flow; | input | select | fieldset\">\n"\
"<!ELEMENT card (onevent*, timer?, (do | p | pre)*)>\n"\
"<!ATTLIST card\n"\
"title %vdata; #IMPLIED\n"\
"newcontext %boolean; \"false\"\n"\
"ordered %boolean; \"true\"\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%cardev;\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT do (%task;)>\n"\
"<!ATTLIST do\n"\
"type CDATA #REQUIRED\n"\
"label %vdata; #IMPLIED\n"\
"name NMTOKEN #IMPLIED\n"\
"optional %boolean; \"false\"\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT onevent (%task;)>\n"\
"<!ATTLIST onevent\n"\
"type CDATA #REQUIRED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT head ( access | meta )+>\n"\
"<!ATTLIST head\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT template (%navelmts;)*>\n"\
"<!ATTLIST template\n"\
"%cardev;\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT access EMPTY>\n"\
"<!ATTLIST access\n"\
"domain CDATA #IMPLIED\n"\
"path CDATA #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT meta EMPTY>\n"\
"<!ATTLIST meta\n"\
"http-equiv CDATA #IMPLIED\n"\
"name CDATA #IMPLIED\n"\
"forua %boolean; \"false\"\n"\
"content CDATA #REQUIRED\n"\
"scheme CDATA #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT go (postfield | setvar)*>\n"\
"<!ATTLIST go\n"\
"href %HREF; #REQUIRED\n"\
"sendreferer %boolean; \"false\"\n"\
"method (post|get) \"get\"\n"\
"enctype %ContentType; \"application/x-www-form-urlencoded\"\n"\
"accept-charset CDATA #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT prev (setvar)*>\n"\
"<!ATTLIST prev\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT refresh (setvar)*>\n"\
"<!ATTLIST refresh\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT noop EMPTY>\n"\
"<!ATTLIST noop\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT postfield EMPTY>\n"\
"<!ATTLIST postfield\n"\
"name %vdata; #REQUIRED\n"\
"value %vdata; #REQUIRED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT setvar EMPTY>\n"\
"<!ATTLIST setvar\n"\
"name %vdata; #REQUIRED\n"\
"value %vdata; #REQUIRED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT select (optgroup|option)+>\n"\
"<!ATTLIST select\n"\
"title %vdata; #IMPLIED\n"\
"name NMTOKEN #IMPLIED\n"\
"value %vdata; #IMPLIED\n"\
"iname NMTOKEN #IMPLIED\n"\
"ivalue %vdata; #IMPLIED\n"\
"multiple %boolean; \"false\"\n"\
"tabindex %number; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT optgroup (optgroup|option)+ >\n"\
"<!ATTLIST optgroup\n"\
"title %vdata; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT option (#PCDATA | onevent)*>\n"\
"<!ATTLIST option\n"\
"value %vdata; #IMPLIED\n"\
"title %vdata; #IMPLIED\n"\
"onpick %HREF; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT input EMPTY>\n"\
"<!ATTLIST input\n"\
"name NMTOKEN #REQUIRED\n"\
"type (text|password) \"text\"\n"\
"value %vdata; #IMPLIED\n"\
"format CDATA #IMPLIED\n"\
"emptyok %boolean; \"false\"\n"\
"size %number; #IMPLIED\n"\
"maxlength %number; #IMPLIED\n"\
"tabindex %number; #IMPLIED\n"\
"title %vdata; #IMPLIED\n"\
"accesskey %vdata; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT fieldset (%fields; | do)* >\n"\
"<!ATTLIST fieldset\n"\
"title %vdata; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT timer EMPTY>\n"\
"<!ATTLIST timer\n"\
"name NMTOKEN #IMPLIED\n"\
"value %vdata; #REQUIRED\n"\
"%coreattrs;\n"\
">\n"\
"<!ENTITY % IAlign \"(top|middle|bottom)\" >\n"\
"<!ELEMENT img EMPTY>\n"\
"<!ATTLIST img\n"\
"alt %vdata; #REQUIRED\n"\
"src %HREF; #REQUIRED\n"\
"localsrc %vdata; #IMPLIED\n"\
"vspace %length; \"0\"\n"\
"hspace %length; \"0\"\n"\
"align %IAlign; \"bottom\"\n"\
"height %length; #IMPLIED\n"\
"width %length; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT anchor ( #PCDATA | br | img | go | prev | refresh )*>\n"\
"<!ATTLIST anchor\n"\
"title %vdata; #IMPLIED\n"\
"accesskey %vdata; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT a ( #PCDATA | br | img )*>\n"\
"<!ATTLIST a\n"\
"href %HREF; #REQUIRED\n"\
"title %vdata; #IMPLIED\n"\
"accesskey %vdata; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT table (tr)+>\n"\
"<!ATTLIST table\n"\
"title %vdata; #IMPLIED\n"\
"align CDATA #IMPLIED\n"\
"columns %number; #REQUIRED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT tr (td)+>\n"\
"<!ATTLIST tr\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT td ( %text; | %layout; | img | anchor |a )*>\n"\
"<!ATTLIST td\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT em (%flow;)*>\n"\
"<!ATTLIST em\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT strong (%flow;)*>\n"\
"<!ATTLIST strong\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT b (%flow;)*>\n"\
"<!ATTLIST b\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT i (%flow;)*>\n"\
"<!ATTLIST i\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT u (%flow;)*>\n"\
"<!ATTLIST u\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT big (%flow;)*>\n"\
"<!ATTLIST big\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT small (%flow;)*>\n"\
"<!ATTLIST small\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ENTITY % TAlign \"(left|right|center)\">\n"\
"<!ENTITY % WrapMode \"(wrap|nowrap)\" >\n"\
"<!ELEMENT p (%fields; | do)*>\n"\
"<!ATTLIST p\n"\
"align %TAlign; \"left\"\n"\
"mode %WrapMode; #IMPLIED\n"\
"xml:lang NMTOKEN #IMPLIED\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT br EMPTY>\n"\
"<!ATTLIST br\n"\
"%coreattrs;\n"\
">\n"\
"<!ELEMENT pre (#PCDATA |a |br |i |b |em |strong | input | select )*>\n"\
"<!ATTLIST pre\n"\
"xml:space CDATA #FIXED \"preserve\"\n"\
"%coreattrs;\n"\
">\n"\
"<!ENTITY quot \"&#34;\"> \n"\
"<!ENTITY amp \"&#38;#38;\"> \n"\
"<!ENTITY apos \"&#39;\"> \n"\
"<!ENTITY lt \"&#38;#60;\"> \n"\
"<!ENTITY gt \"&#62;\"> \n"\
"<!ENTITY nbsp \"&#160;\"> \n"\
"<!ENTITY shy \"&#173;\"> \n";
