#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include "headers/REXSharedLib.h"

#include <type_traits>

using namespace std;

void writeMetadataString(ofstream &myfile, const char *s);

void writeMetadataUInt32(ofstream &file, uint32_t value);

void usage() {
    cout << "usage:\n";
    cout << "\trex2aus librarypath inputfile outputfile\n";
    cout << "\t\tlibrarypath : path to REX Shared Library.bundle\n";
    cout << "\t\tinputfile : path to .rex or .rx2 input file \n";
    cout << "\t\toutputfile : path to .aus output file \n\n";
    cout << "\teg : rex2aus ~/rex/REX\\ Shared\\ Library.bundle kissbang.rx2 kissbang.aus\n";
}

CallbackReturn myCallback( long percent, void* data ) {
    std::cout << "myCallback(" << percent << ")\n";
    return CallbackReturn_Ok;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage();
        exit(-2);
    }

    int rc;                // Success or failure result value

    std::string bundlePath = string(argv[1]) + "/Contents/MacOS/REX Shared Library";
    std::string inputREXfile = argv[2];
    std::string outputAUSfile = argv[3];  // audio slice file extension .aus ?
    REXNative rex = REXNative(bundlePath);

    int open_result = rex.fnOpen1();

    std::ifstream file(inputREXfile, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::ofstream myfile;
    myfile.open (outputAUSfile);
    myfile << "SlicedAudio" ;

    std::cout << " filesize is: " << size << "\n";
    writeMetadataUInt32(myfile, size);

    Handle myHandle; //handle to loop opened by REX shared lib
    char data[size];
    if (file.read(data, size))
    {
        std::streamsize bytes = file.gcount();
        std::cout << "read myfile bytes " << bytes << "\n";
        /* worked! */
        Return rc = rex.fnREXCreate1(&myHandle, data, size, NULL, NULL) ;
        if (rc != Return_Ok) {
            std::cerr << "Failed to REXCreate \n " << rc;
            exit(-2);
        } else {
            std::cout << "REXCreate succeeded (handle:" << myHandle << ")\n";
        }
    } else {
        std::cerr << "Failed to read REX file " << rc << "\n";
        exit(-2);
    }


    Creator creator;
    rc = rex.fnREXGetCreatorInfo1( myHandle, sizeof( Creator ), &creator );
    if ( rc != Return_Ok ) {
        std::cerr << "fnREXGetCreatorInfo failed  to read Creator info " << rc << "\n";
    }
    writeMetadataString(myfile, creator.name);
    writeMetadataString(myfile, creator.copyright);
    writeMetadataString(myfile, creator.url);
    writeMetadataString(myfile, creator.email);
    writeMetadataString(myfile, creator.description);

    Info info;
    rc = rex.fnREXGetInfo1(myHandle, sizeof(Info), &info);
    if ( rc != Return_Ok ) {
        std::cerr << "REXGetInfo failed  to read info " << rc << "\n";
    }

    writeMetadataUInt32(myfile, info.channels);
    writeMetadataUInt32(myfile, info.sampleRate);
    writeMetadataUInt32(myfile, info.bits);
    writeMetadataUInt32(myfile, info.slices);
    writeMetadataUInt32(myfile, info.tempo);
    writeMetadataUInt32(myfile, info.nativeTempo);

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

void writeMetadataUInt32(ofstream &file, uint32_t value) {
    uint32_t b2[1];
    b2[0] = value;
    file.write((char*)b2, 4);
}

void writeMetadataString(ofstream &myfile, const char *s) {
    uint8_t b[1];
    b[0] = MIN(strlen(s), 255);
    myfile.write((char*)b, 1);

    printf("writeMetadataString(%d):%s\n", b[0], s);
    myfile.write(s, b[0]);
}
