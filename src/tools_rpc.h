#ifndef TOOLS_RPC_H_INCLUDE_GUARD
#define TOOLS_RPC_H_INCLUDE_GUARD
//#######|#########|#########|#########|#########|#########|#########|#########|

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <grpcpp/grpcpp.h>

#include "http_like_rpc.pb.h"
#include "http_like_rpc.grpc.pb.h"

namespace HttpLikeRpc {

using MakeMapPair =
        google::protobuf::MapPair<std::string, std::string>;

std::string DumpToString(const HttpLikeObject* obj) {

  std::ostringstream retuen_stream;
  // object内 の "uin32 code" と "string message" の 値の取得例
  retuen_stream << "TOP[" << obj->code() << ":`" << obj->message() << "`],";

  // object内 の "map<string, string> map_arg_string" 内 の 要素取得例
  retuen_stream << "STR[";
  for(auto itr = obj->map_arg_string().begin(); itr != obj->map_arg_string().end(); ++itr) {
    retuen_stream << "'" << itr->first << "':'" << itr->second << "', " ;
  }

  // object内 の "map<string, bytes> map_arg_bin" 内 の 要素取得例
  retuen_stream << "],BIN[";
  for(auto itr = obj->map_arg_bin().begin(); itr != obj->map_arg_bin().end(); ++itr) {
    retuen_stream << "'" << itr->first << "':" << itr->second.size() << "(B), " ;
  }
  retuen_stream << "]";
  return retuen_stream.str();
}

}// namespace HttpLikeRpc

//#######|#########|#########|#########|#########|#########|#########|#########|
#endif // TOOLS_RPC_H_INCLUDE_GUARD
