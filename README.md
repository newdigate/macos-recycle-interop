# Loading and saving recycle .REX audio loop files on macos

a rough macos c++ sketch/proof-of-concept 
* dynamically load the REX Shared Library (available as download from propellerheads, see below) 
* inspect .rx2 and .rex audio loop file info with slice info
* export to .raw audio file

## credit
* based on rexlib from https://github.com/noorus/rexlib 
  * unfortunately this only works on windows 
  * it is refactored nicely. 
* Loading dynamic library from .bundle image in macos from stack-overflow
  * https://stackoverflow.com/questions/54003455/cant-link-to-so-file-on-mac-with-cmake?answertab=votes#tab-top

## REX-Shared-Library
Macos bundle with shared recycle library from propellerheads
* https://help.propellerheads.com/hc/en-us/articles/360002216393-REX-Shared-Library
