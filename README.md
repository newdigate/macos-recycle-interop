# exporting recycle .REX audio loop files on macos

a minimum viable product to load and export recycle .REX and .RX2 audio and slice information
* dynamically load the REX Shared Library (available as download from propellerheads, see below...) 
* inspect .rx2 and .rex audio loop file info with slice info
* export to slices to .AUS bespoke audio file format

## usage
```
usage:
	rex2aus librarypath inputfile outputfile
		librarypath : path to REX Shared Library.bundle
		inputfile : path to .rex or .rx2 input file 
		outputfile : path to .aus output file 

	eg : rex2aus ~/rex/REX\ Shared\ Library.bundle kissbang.rx2 kissbang.aus
```

## build
``` bash
mkdir build
cd build
cmake ..
make
```

## credit
* based on rexlib from https://github.com/noorus/rexlib 
  * unfortunately this only works on windows 
  * it is refactored nicely. 
* Loading dynamic library from .bundle image in macos from stack-overflow
  * https://stackoverflow.com/questions/54003455/cant-link-to-so-file-on-mac-with-cmake?answertab=votes#tab-top

## dependencies
* REX-Shared-Library
  * library is loaded dynamically in run-time from .bundle disk image format
  * library is available as download from propellerheads
    * https://help.propellerheads.com/hc/en-us/articles/360002216393-REX-Shared-Library
