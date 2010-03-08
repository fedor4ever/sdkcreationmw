This directory contains sources for the Header Codec which used to be
a part of WPS client protocol stack. This code has been removed from 
the WPS client source tree, but toolkit's NMB simulator still needs it.

Note that the codec's public header files (hc_api.h and files included 
from it) still live in WPS source tree, in the top level include directory.
That means that you still need WPS source code (at least the include
directory) to compile this code.
