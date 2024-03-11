// Copyright 2007-2019 The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#include "fuzz.h"

#include <cstdint>
#include <json/config.h>
#include <json/json.h>
#include <memory>
#include <string>
#include <google/protobuf/util/json_util.h>
#include "genfiles/proto.pb.h"
#include "libprotobuf-mutator/src/libfuzzer/libfuzzer_macro.h"

namespace Json {
class Exception;
}

void ParseFromJsonString(const std::string& json_string, CharReaderBuilder_Proto& proto) {
  google::protobuf::util::JsonStringToMessage(json_string, &proto);
}

// Function to convert ValueProto to Json::Value
void ConvertValueProto(const ValueProto& value_proto, Json::Value& value) {
  switch (value_proto.value_case()) {
    case ValueProto::kIntValue:
      value = static_cast<Json::Int>(value_proto.int_value().value());
      break;
    case ValueProto::kUintValue:
      value = static_cast<Json::UInt>(value_proto.uint_value().value());
      break;
    case ValueProto::kRealValue:
      value = value_proto.real_value();
      break;
    case ValueProto::kBoolValue:
      value = value_proto.bool_value();
      break;
    case ValueProto::kStringValue:
      value = value_proto.string_value();
      break;
    case ValueProto::kMapValue:
      for (const auto& pair : value_proto.map_value().values()) {
        Json::Value child_value;
        ConvertValueProto(pair.second, child_value);
        value[pair.first] = std::move(child_value);
      }
      break;
    case ValueProto::VALUE_NOT_SET:
      break;
  }

  // Set additional fields
//   value.setComment(value_proto.comments().array().values_size() > 0);
//   value.setOffsetStart(value_proto.start());
//   value.setOffsetLimit(value_proto.limit());
}

// Function to convert CharReaderBuilder_Proto to Json::CharReaderBuilder
void ConvertCharReaderBuilder(const CharReaderBuilder_Proto& proto, Json::CharReaderBuilder& builder) {
  ConvertValueProto(proto.settings(), builder.settings_);
}

DEFINE_BINARY_PROTO_FUZZER(const CharReaderBuilder_Proto& builder_proto) {
  Json::CharReaderBuilder builder;
  ConvertCharReaderBuilder(builder_proto, builder);

  std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;

  std::string json_string;
  google::protobuf::util::MessageToJsonString(builder_proto, &json_string);

  try {
    reader->parse(json_string.data(), json_string.data() + json_string.size(), &root, nullptr);
  } catch (Json::Exception const&) {
  }

  // Whether it succeeded or not doesn't matter.
  return;
}