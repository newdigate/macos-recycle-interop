#include <iostream>
#include <stdio.h>
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


CallbackReturn myCallback( long percent, void* data ) {
    std::cout << "myCallback(" << percent << ")\n";
    return CallbackReturn_Ok;
}


inline bool exists_test1 (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

int main( )
{
    int rc;                // Success or failure result value

    NSObjectFileImage img; // Represents the bundle's object file
    NSModule handle;       // Handle to the loaded bundle
    NSSymbol sym;          // Represents a symbol in the bundle

    fnOpen fnOpen1;         // Function pointer for fnOpen1
    fnClose fnClose1;
    fnREXCreate fnREXCreate1;
    fnREXGetCreatorInfo fnREXGetCreatorInfo1;
    fnREXGetInfo fnREXGetInfo1;
    fnREXGetSliceInfo fnREXGetSliceInfo1;
    fnREXRenderSlice fnREXRenderSlice1;

    /* Get an object file for the bundle. */
    std::string bundlePath = "/Users/johnsmith/CLionProjects/demo-shared-lib/REXSharedLibrary.bundle/Contents/MacOS/REX Shared Library";

    if (!exists_test1(bundlePath)) {
        std::cerr << " bundle file was not found. \n";
        return (-2);
    } else {
        std::cout << " bundle file does exist. \n";
        std::cout.flush();
    }

    std::cout << "attempting to load bundle: " << bundlePath << "\n";
    std::cout.flush();

    rc = NSCreateObjectFileImageFromFile(bundlePath.c_str(), &img);
    if (rc != NSObjectFileImageSuccess) {
        std::cerr <<  "Could not load" << bundlePath << " - rc:" << rc << "\n" ;

        switch (rc) {
            case NSObjectFileImageFailure: {
                std::cerr << " The operation was not successfully completed. When this result code is returned, an error message is printed to the standard error stream.";
                break;
            }
            case NSObjectFileImageInappropriateFile: {
                std::cerr << " The specified Mach-O file is not of a type this function can operate upon.";
                break;
            }
            default:
                break;
        }
        exit(-1);
    }

    /* Get a handle for the bundle. */
    handle = NSLinkModule(img, bundlePath.c_str(), FALSE);

    /* Look up the fnOpen1 function. */
    sym = NSLookupSymbolInModule(handle, "_Open");
    if (sym == NULL)
    {
        fprintf(stderr, "Could not find symbol: _Open.\n");
        exit(-2);
    }

    /* Get the address of the function. */
    fnOpen1 = (fnOpen)NSAddressOfSymbol(sym);

    /* Look up the fnOpen1 function. */
    sym = NSLookupSymbolInModule(handle, "_Close");
    if (sym == NULL)
    {
        fprintf(stderr, "Could not find symbol: _Close.\n");
        exit(-2);
    }

    /* Get the address of the function. */
    fnClose1 = (fnClose)NSAddressOfSymbol(sym);

    /* Look up the fnOpen1 function. */
    sym = NSLookupSymbolInModule(handle, "_REXCreate");
    if (sym == NULL)
    {
        fprintf(stderr, "Could not find symbol: _REXCreate.\n");
        exit(-2);
    }

    /* Get the address of the function. */
    fnREXCreate1 = (fnREXCreate)NSAddressOfSymbol(sym);



    /* Look up the fnOpen1 function. */
    sym = NSLookupSymbolInModule(handle, "_REXGetCreatorInfo");
    if (sym == NULL)
    {
        fprintf(stderr, "Could not find symbol: _REXGetCreatorInfo.\n");
        exit(-2);
    }

    /* Get the address of the function. */
    fnREXGetCreatorInfo1 = (fnREXGetCreatorInfo)NSAddressOfSymbol(sym);



    /* Look up the fnOpen1 function. */
    sym = NSLookupSymbolInModule(handle, "_REXGetInfo");
    if (sym == NULL)
    {
        fprintf(stderr, "Could not find symbol: _REXGetInfo.\n");
        exit(-2);
    }

    /* Get the address of the function. */
    fnREXGetInfo1 = (fnREXGetInfo)NSAddressOfSymbol(sym);

    /* Look up the fnOpen1 function. */
    sym = NSLookupSymbolInModule(handle, "_REXGetSliceInfo");
    if (sym == NULL)
    {
        fprintf(stderr, "Could not find symbol: _REXGetSliceInfo.\n");
        exit(-2);
    }

    /* Get the address of the function. */
    fnREXGetSliceInfo1 = (fnREXGetSliceInfo)NSAddressOfSymbol(sym);

    /* Look up the fnOpen1 function. */
    sym = NSLookupSymbolInModule(handle, "_REXRenderSlice");
    if (sym == NULL)
    {
        fprintf(stderr, "Could not find symbol: _REXRenderSlice.\n");
        exit(-2);
    }

    /* Get the address of the function. */
    fnREXRenderSlice1 = (fnREXRenderSlice)NSAddressOfSymbol(sym);


    /* Invoke the function and display the answer. */
    int open_result = fnOpen1( );
    printf("The result is... %d\n", open_result);

    Handle myHandle;

    std::ifstream file("../kissbang.rx2", std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char data[size];
    if (file.read(data, size))
    {
        std::cout << "read myfile bytes " << size << "\n";
        /* worked! */
        Return rc = fnREXCreate1(&myHandle, data, size, NULL, NULL) ;
        if (rc != Return_Ok) {
            std::cerr << "Failed to REXCreate \n " << rc;
        } else {
            std::cout << "REXCreate succeeded (handle:" << myHandle << ")\n";
        }
    } else {
        std::cerr << "Failed to read REX file " << rc << "\n";
    }


    Creator creator;
    rc = fnREXGetCreatorInfo1( myHandle, sizeof( Creator ), &creator );
    if ( rc != Return_Ok ) {
        std::cerr << "fnREXGetCreatorInfo failed  to read Creator info " << rc << "\n";
    }

    std::cout << "name:" << creator.name << "\n";
    std::cout << "copyright:" << creator.copyright << "\n";
    std::cout << "url:" << creator.url << "\n";
    std::cout << "email:" << creator.email << "\n";
    std::cout << "description:" << creator.description << "\n";


    Info info;
    rc = fnREXGetInfo1(myHandle, sizeof(Info), &info);
    if ( rc != Return_Ok ) {
        std::cerr << "REXGetInfo failed  to read info " << rc << "\n";
    }

    std::cout << "channels:" << info.channels << "\n";
    std::cout << "sampleRate:" << info.sampleRate << "\n";
    std::cout << "bits:" << info.bits << "\n";
    std::cout << "slices:" << info.slices << "\n";
    std::cout << "tempo:" << info.tempo / 1000.0 << "\n";
    std::cout << "nativeTempo:" << info.nativeTempo / 1000.0 << "\n";

    std::ofstream myfile;
    myfile.open ("example.raw");


    for (long i =0; i<info.slices; i++) {
        Slice slice;
        rc = fnREXGetSliceInfo1(myHandle, i, sizeof(Slice), &slice);
        if ( rc != Return_Ok ) {
            std::cerr << "REXGetSliceInfo1 failed  to read slice " << i << "\n";
        }
        std::cout << "  slice:" << i << "\n";
        std::cout << "      position:" << slice.position << "\n";
        std::cout << "      length:" << slice.length << "\n";


        float *buffers[2];
        buffers[0] = new float[slice.length];
        buffers[1] = new float[slice.length];
        rc = fnREXRenderSlice1(myHandle, i, slice.length, buffers);
        if ( rc != Return_Ok ) {
            std::cerr << "REXRenderSlice failed  to render slice " << i << "\n";
        }

        for (long j=0; j<slice.length; j++) {
            std::cout << "(" << (long)(buffers[0][j] * 0xFFFF) << ", " << (long)(buffers[1][j]*0xFFFF) << "),";
            int16_t b[2];
            b[0] = (int32_t)(buffers[0][j] * 0x7FFF);
            b[1] = (int32_t)(buffers[1][j] * 0x7FFF);
            myfile.write((char*)b, 4);
        }
        std::cout << "\n";

        delete [] buffers[0];
        delete [] buffers[1];
    }
    myfile.close();

    fnClose1();
    printf("closed \n");
    return 0;
}
