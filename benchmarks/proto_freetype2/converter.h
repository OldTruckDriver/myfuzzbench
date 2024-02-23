// convert.h
#ifndef CONVERT_H
#define CONVERT_H

#include <ft2build.h>
#include FT_FREETYPE_H // Include FreeType2 headers
#include "freetype2.pb.h" // Include the generated protobuf headers

// Function to convert a protobuf FT_Byte message to an unsigned char used by FreeType2
unsigned char ConvertFT_Byte(const FT_Byte& proto_byte);

// Function to convert a protobuf FT_Long message to an FT_Long used by FreeType2
FT_Long ConvertFT_Long(const FT_Long& proto_long);

#endif // CONVERT_H
