#
# $Id: device.profile,v 1.8 2005/05/27 20:18:19 slava Exp $
#
# Series 60 SDK 3.0 Device Profile
#

device.name = Series 60 SDK 3.0
device.version = 3.0
device.executable = sdkw.exe
device.interface.cli = nokbos-devsimcli-1.0
device.instance = new, attach

#
device.load-types = \
    text/vnd.sun.j2me.app-descriptor,\
    application/java,\
    application/java-archive,\
    application/x-java-archive,\
    text/vnd.wap.wml,\
    text/vnd.wap.wmlsc,\
    text/vnd.wap.wmlscript,\
    text/html,\
    application/xhtml+xml,\
    application/vnd.wap.wmlc,\
    application/vnd.wap.wmlsc,\
    application/vnd.wap.wmlscriptc,\
    application/vnd.wap.mms-message,\
    application/vnd.wap.sic,\
    application/vnd.wap.slc,\
    application/vnd.wap.multipart.mixed \
    application/vnd.wap.multipart.related \
    image/gif,\
    image/png,\
    image/jpeg,\
    image/tiff,\
    image/bmp,\
    image/vnd.wap.wbmp,\
    audio/midi,\
    audio/x-midi,\
    audio/sp-midi,\
    audio/amr,\
    audio/amr-wb,\
    image/vnd.nok-wallpaper,\
    application/vnd.nokia.ringing-tone,\
    text/x-vCard,\
    text/x-vCalendar,\
    text/x-co-desc

#
device.browser.support = file, http-wap

#
device.mms.support = receive
device.mms.message-types = M-Retrieve.conf, M-Send.req 
device.mms.receive = cli
device.mms.max-message-size = 64628
device.mms.media-types = \
    image/gif,\
    image/jpeg,\
    image/tiff,\
    image/png,\
    image/bmp,\
    image/vnd.wap.wbmp,\
    image/vnd.nok-wallpaper,\
    application/vnd.nokia.ringing-tone,\
    application/vnd.wap.multipart.mixed,\
    audio/midi,\
    audio/sp-midi,\
    audio/x-midi,\
    audio/amr,\
    text/plain
