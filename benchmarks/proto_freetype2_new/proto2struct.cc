#include <ft2build.h>
#include FT_FREETYPE_H // Include FreeType2
#include "genfiles/freetype2.pb.h" // Include your protobuf definitions

// Assuming FT_Byte in FreeType2 is equivalent to unsigned char
std::vector<unsigned char> ConvertFTByteProtoToVector(const FT_Byte_Proto& proto) {
    std::vector<unsigned char> result;

    for (const uint32_t& ft_byte : proto.ft_byte()) {
      // Assuming ft_byte is within the range of unsigned char
      result.push_back(static_cast<unsigned char>(ft_byte));
    }

    return result;
  }

// Assuming FT_Long in FreeType2 is equivalent to a signed long or similar type
FT_Long ConvertFT_Long(const FT_Long_Proto& proto_long) {
    return static_cast<FT_Long>(proto_long.ft_long());
}
