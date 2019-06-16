#include <iostream>
#include <stdio.h>
#include <string>
#import <mach-o/dyld.h>
#include <fstream>
#include "headers/REXSharedLib.h"

#include <type_traits>

using namespace std;

CallbackReturn myCallback( long percent, void* data ) {
    std::cout << "myCallback(" << percent << ")\n";
    return CallbackReturn_Ok;
}

// swap_endian: https://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c?answertab=votes#4956493
#include <climits>

template <typename T>
T swap_endian(T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}



int main( )
{
    int rc;                // Success or failure result value

    std::string bundlePath = "/Users/johnsmith/CLionProjects/demo-shared-lib/REXSharedLibrary.bundle/Contents/MacOS/REX Shared Library";
    std::string inputREXfile = "../kissbang.rx2";
    std::string outputAUSfile = "../kissbang.aus";  // audio slice file extension .aus ?
    REXNative rex = REXNative(bundlePath);

    /* Invoke the function and display the answer. */
    int open_result = rex.fnOpen1();
    printf("The result is... %d\n", open_result);

    std::ifstream file(inputREXfile, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::ofstream myfile;
    myfile.open (outputAUSfile);
    myfile << "NN\0" ;

    uint32_t bufu32[1];
    bufu32[0] = swap_endian(size);
    myfile.write((char*)bufu32, 4);

    Handle myHandle; //handle to loop opened by REX shared lib
    char data[size];
    if (file.read(data, size))
    {
        std::cout << "read myfile bytes " << size << "\n";
        /* worked! */
        Return rc = rex.fnREXCreate1(&myHandle, data, size, NULL, NULL) ;
        if (rc != Return_Ok) {
            std::cerr << "Failed to REXCreate \n " << rc;
        } else {
            std::cout << "REXCreate succeeded (handle:" << myHandle << ")\n";
        }
    } else {
        std::cerr << "Failed to read REX file " << rc << "\n";
    }


    Creator creator;
    rc = rex.fnREXGetCreatorInfo1( myHandle, sizeof( Creator ), &creator );
    if ( rc != Return_Ok ) {
        std::cerr << "fnREXGetCreatorInfo failed  to read Creator info " << rc << "\n";
    }
    myfile << "-";
    uint16_t bbb[1];
    bbb[0] = strlen(creator.name);
    myfile.write((char*)bbb, 2);
    std::cout << "name:" << creator.name << "\n";
    myfile << creator.name;
    myfile << strlen(creator.copyright);
    std::cout << "copyright:" << creator.copyright << "\n";
    myfile << creator.copyright;
    myfile << strlen(creator.url);
    std::cout << "url:" << creator.url << "\n";
    myfile << creator.url;
    myfile << strlen(creator.email);
    std::cout << "email:" << creator.email << "\n";
    myfile << creator.email;
    myfile << strlen(creator.description);
    std::cout << "description:" << creator.description << "\n";
    myfile << creator.description;


    Info info;
    rc = rex.fnREXGetInfo1(myHandle, sizeof(Info), &info);
    if ( rc != Return_Ok ) {
        std::cerr << "REXGetInfo failed  to read info " << rc << "\n";
    }

    std::cout << "channels:" << info.channels << "\n";
    std::cout << "sampleRate:" << info.sampleRate << "\n";
    std::cout << "bits:" << info.bits << "\n";
    std::cout << "slices:" << info.slices << "\n";
    std::cout << "tempo:" << info.tempo / 1000.0 << "\n";
    std::cout << "nativeTempo:" << info.nativeTempo / 1000.0 << "\n";




    for (long i =0; i<info.slices; i++) {
        Slice slice;
        rc = rex.fnREXGetSliceInfo1(myHandle, i, sizeof(Slice), &slice);
        if ( rc != Return_Ok ) {
            std::cerr << "REXGetSliceInfo1 failed  to read slice " << i << "\n";
        }
        std::cout << "  slice:" << i << "\n";
        std::cout << "      position:" << slice.position << "\n";
        std::cout << "      length:" << slice.length << "\n";


        float *buffers[2];
        buffers[0] = new float[slice.length];
        buffers[1] = new float[slice.length];
        rc = rex.fnREXRenderSlice1(myHandle, i, slice.length, buffers);
        if ( rc != Return_Ok ) {
            std::cerr << "REXRenderSlice failed  to render slice " << i << "\n";
        }

        for (long j=0; j<slice.length; j++) {
            //std::cout << "(" << (long)(buffers[0][j] * 0xFFFF) << ", " << (long)(buffers[1][j]*0xFFFF) << "),";
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

    rex.fnClose1();
    printf("closed \n");
    return 0;
}
