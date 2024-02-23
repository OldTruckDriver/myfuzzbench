#include <ft2build.h>
#include FT_FREETYPE_H // Include FreeType2
#include "your_protobuf_definitions.pb.h" // Include your protobuf definitions

// Assuming FT_Byte in FreeType2 is equivalent to unsigned char
unsigned char ConvertFT_Byte(const FT_Byte_Proto& proto_byte) {
    return static_cast<unsigned char>(proto_byte.ft_byte());
}

// Assuming FT_Long in FreeType2 is equivalent to a signed long or similar type
FT_Long ConvertFT_Long(const FT_Long_Proto& proto_long) {
    return static_cast<FT_Long>(proto_long.ft_long());
}
