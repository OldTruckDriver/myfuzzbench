#include <stdint.h>

#include <json.h>

#include "proto.pb.h"
#include <json_tokener.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  JsonTokener_Proto tok_proto;
  if (!tok_proto.ParseFromArray(data, size)) {
    return 0;
  }

  // 创建 Mutator 对象
  protobuf_mutator::Mutator mutator;

  // 变异 JsonTokener_Proto 消息
  for (int i = 0; i < tok_proto._mutation_cycles(); ++i) {
    mutator.Mutate(&tok_proto, tok_proto._mutation_probability());
  }

  // 使用变异后的 JsonTokener_Proto 消息进行模糊测试
  json_tokener *tok = JsonTokenerConverter::ProtoToJsonTokener(tok_proto);
  if (!tok) {
    return 0;
  }

  const char *data1 = reinterpret_cast<const char *>(data);
  json_object *obj = json_tokener_parse_ex(tok, data1, size);
  json_object_put(obj);
  json_tokener_free(tok);

  return 0;
}
