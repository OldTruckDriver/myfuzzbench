#ifndef WOFF2PROTO_H
#define WOFF2PROTO_H

#include "woff2.pb.h"

void Point2Proto(const Point& input, PointProto& result);
void GlyphPoint2Proto(const GlyphPoint& input, GlyphPointProto& result);
void ContourVec2Proto(const ContourVec& input, ContourVecProto& result);
void Glyph2Proto(const Glyph& input, GlyphProto& result);
void Font2Proto(const Font& input, FontProto& result);
void FontCollection2Proto(const FontCollection& input, FontCollectionProto& result);

void Proto2Point(const PointProto& input, Point& result);
void Proto2GlyphPoint(const GlyphPointProto& input, GlyphPoint& result);
void Proto2ContourVec(const ContourVecProto& input, ContourVec& result);
void Proto2Glyph(const GlyphProto& input, Glyph& result);
void Proto2Font(const FontProto& input, Font& result);
void Proto2FontCollection(const FontCollectionProto& input, FontCollection& result);

#endif // WOFF2PROTO_H
