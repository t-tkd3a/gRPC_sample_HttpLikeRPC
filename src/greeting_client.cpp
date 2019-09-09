#include <vector>
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "http_like_rpc.grpc.pb.h"
#include "tools_rpc.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using HttpLikeRpc::HttpLike;
using HttpLikeRpc::HttpLikeObject;


class HttpLikeClient {
 public:
  HttpLikeClient(std::shared_ptr<Channel> channel)
      : stub_(HttpLike::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  bool Post(HttpLikeObject &request, HttpLikeObject &response) {

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->Post(&context, request, &response);

    // Act upon its status.
    if (status.ok()) {
      return true;
    } else {
      std::cerr << status.error_code() << ": " << status.error_message()
                << std::endl;
      return false;
    }
  }

 private:
  std::unique_ptr<HttpLike::Stub> stub_;
};


void SampleClientHellow() {
  std::cout << "--- SampleClientHellow() ---" << std::endl;

  HttpLikeObject request = {};
  HttpLikeObject response = {};
  HttpLikeClient client(grpc::CreateChannel(
    "127.0.0.1:55551", grpc::InsecureChannelCredentials()));


  request.set_message("hello");

  std::cout << "request :" << HttpLikeRpc::DumpToString(&request) << std::endl;
  if ( ! client.Post(request, response) ) {
    std::cerr << "Post failed." << std::endl;
    return;
  }
  std::cout << "Response:" << HttpLikeRpc::DumpToString(&response) << std::endl;

  // object内 の "map<string, string> map_arg_string" 内 から Key 名で 値を取得する例
  auto itr = response.map_arg_string().find("result");
  if (itr != response.map_arg_string().end()) {
    std::cout << "result:" << itr->second << std::endl;
  }

}


void SampleClientIp() {
  std::cout << "--- SampleClientIp() ---" << std::endl;

  HttpLikeObject request = {};
  HttpLikeObject response = {};
  HttpLikeClient client(grpc::CreateChannel(
    "127.0.0.1:55551", grpc::InsecureChannelCredentials()));

  request.set_message("ip");
  std::cout << "request :" << HttpLikeRpc::DumpToString(&request) << std::endl;
  if ( ! client.Post(request, response) ) {
    std::cerr << "Post failed." << std::endl;
    return;
  }
  std::cout << "Response:" << HttpLikeRpc::DumpToString(&response) << std::endl;

  // object内 の "map<string, string> map_arg_string" 内 から Key 名で 値を取得する例
  auto itr = response.map_arg_string().find("result");
  if (itr != response.map_arg_string().end()) {
    std::cout << "result:" << itr->second << std::endl;
  }
}


void SampleClientWithBin() {
  std::cout << "--- SampleClientWithBin() ---" << std::endl;

  std::vector<uint8_t> bin(256,0);
  for(int i=0; i<256; i++) {bin[i] = i;}

  HttpLikeObject request = {};
  HttpLikeObject response = {};
  HttpLikeClient client(grpc::CreateChannel(
    "127.0.0.1:55551", grpc::InsecureChannelCredentials()));

  // object内の "map<string, string> map_arg_bin" 内に バイナリデータを格納する例
  std::string tmp;
  tmp.resize(bin.size());
  memcpy((uint8_t *)&tmp[0], bin.data(), bin.size());
  request.mutable_map_arg_bin()->insert(
      HttpLikeRpc::MakeMapPair("data", tmp ));

  request.set_message("dumpData");
  std::cout << "request :" << HttpLikeRpc::DumpToString(&request) << std::endl;
  if ( ! client.Post(request, response) ) {
    std::cerr << "Post failed." << std::endl;
    return;
  }
  std::cout << "Response:" << HttpLikeRpc::DumpToString(&response) << std::endl;

}

int main(int argc, char** argv) {
  SampleClientHellow();
  SampleClientIp();
  SampleClientWithBin();
  SampleClientIp();
  return 0;
}
