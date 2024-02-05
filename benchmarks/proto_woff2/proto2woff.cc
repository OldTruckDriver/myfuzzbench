#include "woff2_dec.h"
#include "woff2.pb.h"
#include <fstream>

void Proto2Point(const PointProto& input, Point& result) {
    result.set_x(input.x());
    result.set_y(input.y());
    result.set_on_curve(input.on_curve());
}

void Proto2GlyphPoint(const GlyphPointProto& input, GlyphPoint& result) {
    result.set_x(input.x());
    result.set_y(input.y());
    result.set_on_curve(input.on_curve());
}

void Proto2ContourVec(const ContourVecProto& input, ContourVec& result) {
    for (const auto& pointProto : input.points()) {
        GlyphPoint point;
        Proto2GlyphPoint(pointProto, point);
        *result.add_points() = point;
    }
}

void Proto2Glyph(const GlyphProto& input, Glyph& result) {
    result.set_x_min(input.x_min());
    result.set_x_max(input.x_max());
    result.set_y_min(input.y_min());
    result.set_y_max(input.y_max());
    result.set_instructions_size(input.instructions_size());
    result.set_instructions_data(input.instructions_data());
    result.set_overlap_simple_flag_set(input.overlap_simple_flag_set());
    ContourVecProto2ContourVec(input.contours(), result.mutable_contours());
    result.set_composite_data(input.composite_data());
    result.set_composite_data_size(input.composite_data_size());
    result.set_have_instructions(input.have_instructions());
}

void Proto2Font(const FontProto& input, Font& result) {
    result.set_flavor(input.flavor());
    result.set_num_tables(input.num_tables());
    for (const auto& table : input.tables()) {
        (*result.mutable_tables())[table.first] = table.second;
    }
}

void Proto2FontCollection(const FontCollectionProto& input, FontCollection& result) {
    result.set_flavor(input.flavor());
    result.set_header_version(input.header_version());
    for (const auto& table : input.tables()) {
        (*result.mutable_tables())[table.first] = table.second;
    }
    for (const auto& fontProto : input.fonts()) {
        Font font;
        Proto2Font(fontProto, font);
        *result.add_fonts() = font;
    }
}

// Implement conversion functions for other message types...

// int main() {
//     // Read Protocol Buffer file into FontCollectionProto
//     FontCollectionProto fontCollectionProto;
//     std::ifstream input("input_file.pb", std::ios::binary);
//     fontCollectionProto.ParseFromIstream(&input);

//     // Convert Protocol Buffer message to FontCollection
//     FontCollection fontCollection;
//     Proto2FontCollection(fontCollectionProto, fontCollection);

//     // Write FontCollection to WOFF2 file
//     //...

//     return 0;
// }
