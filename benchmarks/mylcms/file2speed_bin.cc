#include "lcms2_plugin.h" 
#include "converter.h" 
#include <fstream> 
#include <iostream> 
#include <google/protobuf/text_format.h>

int main(int argc, char *argv[])
{
    const char *inputFileName = argv[1];
    const char *outputFileName = argv[2];
    _cmsICCPROFILE *hInputProfile = (_cmsICCPROFILE *)cmsOpenProfileFromFile(inputFileName, "r");
    if (hInputProfile == NULL)
    {
        std::cout << "Open failed! Not convert to proto..." << std::endl;
        cmsCloseProfile(hInputProfile);
        return 0;
    } // cmsSaveProfileToFile cmsICCPROFILEProto result; CmsICCPROFILE2Proto(hInputProfile, &result);
    std::ofstream outputStream(outputFileName, std::ios::out);
    result.SerializeToOstream(&outputStream);
    outputStream.close();
    cmsCloseProfile(hInputProfile);
    return 0;
}