#ifndef DEMO_SHARED_LIB_REXSHAREDLIB_H
#define DEMO_SHARED_LIB_REXSHAREDLIB_H

#include <cstdint>
#include <iostream>
#include <cstdio>
#include <string>
#import <mach-o/dyld.h>
#include <fstream>

enum Return: uint32_t {
    Return_Failed = 0,
    Return_Ok,
    Return_Aborted,
    Return_Unavailable,
    Return_DLL_OutOfMemory = 100,
    Return_DLL_LoadFailed,
    Return_DLL_Outdated,
    Return_DLL_NotFound,
    Return_APIOutdated,
    Return_OutOfMemory,
    Return_FileCorrupted,
    Return_FileUnsupported,
    Return_FileEmpty,
    Return_BadOS,
    Return_DLL_NotLoaded = 200,
    Return_DLL_Loaded,
    Return_BadHandle,
    Return_BadSize,
    Return_BadArgument,
    Return_BadSlice,
    Return_BadSampleRate,
    Return_BufferTooSmall,
    Return_InPreviewMode,
    Return_NotInPreviewMode,
    Return_BadTempo,
    Return_Unknown = 666
};

enum CallbackReturn: uint32_t {
    CallbackReturn_Fail = 1,
    CallbackReturn_Ok
};

# pragma pack(push,4)

struct Info {
    uint32_t channels;
    uint32_t sampleRate;
    uint32_t slices;
    uint32_t tempo;
    uint32_t nativeTempo;
    uint32_t length;
    uint32_t unk1;
    uint32_t unk2;
    uint32_t bits;
};

struct Slice {
    uint32_t position;
    uint32_t length;
};

struct Creator {
    char name[256];
    char copyright[256];
    char url[256];
    char email[256];
    char description[256];
};

# pragma pack(pop)

extern "C" {
typedef unsigned long Handle;
typedef CallbackReturn (*fnCreateCallback)( long percent, void* data );
typedef uint32_t (*fnOpen)();
typedef void     (*fnClose)();
typedef Return   (*fnREXCreate)( Handle* handle, void* buffer, long size, fnCreateCallback callback, void* data );
typedef void     (*fnREXDelete)( Handle* handle );
typedef Return   (*fnREXGetInfo)( Handle handle, long infoSize, Info* info );
typedef Return   (*fnREXGetInfoFromBuffer)( long size, void* buffer, long infoSize, Info* info );
typedef Return   (*fnREXGetCreatorInfo)( Handle handle, long infoSize, Creator* info );
typedef Return   (*fnREXGetSliceInfo)( Handle handle, long index, long infoSize, Slice* info );
typedef Return   (*fnREXSetOutputSampleRate)( Handle handle, long sampleRate );
typedef Return   (*fnREXRenderSlice)( Handle handle, long index, long length, float* buffers[2] );
typedef Return   (*fnREXStartPreview)( Handle handle );
typedef Return   (*fnREXStopPreview)( Handle handle );
typedef Return   (*fnREXRenderPreviewBatch)( Handle handle, long length, float* buffers[2] );
typedef Return   (*fnREXSetPreviewTempo)( Handle handle, long tempo );
}

#define STRINGIFY(x) #x
#define LOAD_REX_FUNC(x,handle) \
    NSSymbol sym ## x ;             \
    sym ## x = NSLookupSymbolInModule(handle, STRINGIFY(_##x)); \
    if (sym##x == NULL) { \
        std::cerr << "Could not find symbol " << STRINGIFY(_##x) << "\n"; \
        exit(-2); \
    } \
    fn##x##1 = (fn ## x)NSAddressOfSymbol(sym##x);\

class REXNative {
private:
    std::string _bundleLocation;

    inline bool fileExists(const std::string& name) {
        if (FILE *file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        } else {
            return false;
        }
    }

public:
    fnOpen fnOpen1;         // Function pointer for fnOpen1
    fnClose fnClose1;
    fnREXCreate fnREXCreate1;
    fnREXGetCreatorInfo fnREXGetCreatorInfo1;
    fnREXGetInfo fnREXGetInfo1;
    fnREXGetSliceInfo fnREXGetSliceInfo1;
    fnREXRenderSlice fnREXRenderSlice1;

    REXNative(std::string bundleLocation) : _bundleLocation(bundleLocation) {

        if (!fileExists(_bundleLocation)) {
            std::cerr << " bundle file was not found. " << _bundleLocation << "\n";
            return;
        }

        NSObjectFileImage img; // Represents the bundle's object file
        NSModule handle;       // Handle to the loaded bundle
        NSSymbol sym;          // Represents a symbol in the bundle

        int rc = NSCreateObjectFileImageFromFile(_bundleLocation.c_str(), &img);
        if (rc != NSObjectFileImageSuccess) {
            std::cerr <<  "Could not load " << _bundleLocation << " - rc:" << rc << "\n" ;

            switch (rc) {
                case NSObjectFileImageFailure: {
                    std::cerr << " The operation was not successfully completed. When this result code is returned, an error message is printed to the standard error stream.";
                    return;
                }
                case NSObjectFileImageInappropriateFile: {
                    std::cerr << " The specified Mach-O file is not of a type this function can operate upon.";
                    return;
                }
                default:
                    return;
            }
        }

        handle = NSLinkModule(img, _bundleLocation.c_str(), FALSE);
        LOAD_REX_FUNC(Open, handle);
        LOAD_REX_FUNC(Close, handle);
        LOAD_REX_FUNC(REXCreate, handle);
        LOAD_REX_FUNC(REXGetInfo, handle);
        LOAD_REX_FUNC(REXGetCreatorInfo, handle);
        LOAD_REX_FUNC(REXGetSliceInfo, handle);
        LOAD_REX_FUNC(REXRenderSlice, handle);
    }

};


#endif //DEMO_SHARED_LIB_REXSHAREDLIB_H
