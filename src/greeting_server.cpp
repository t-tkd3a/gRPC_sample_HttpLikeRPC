#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "http_like_rpc.grpc.pb.h"
#include "tools_rpc.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using HttpLikeRpc::HttpLike;
using HttpLikeRpc::HttpLikeObject;

// Logic and data behind the server's behavior.
class HttpLikeServiceImpl final : public HttpLike::Service {
public :
  HttpLikeServiceImpl() {}
  virtual ~HttpLikeServiceImpl() {}
  Status Post(ServerContext* context, const HttpLikeObject* request,
                  HttpLikeObject* response) override {

    std::cout << "--- HttpLikeServiceImpl::Post() ----- " << std::endl;
    std::cout << "clientIP: " << context->peer() << std::endl;
    std::cout << "Recuest: " << HttpLikeRpc::DumpToString( request ) << std::endl;

    std::cout << "client_metadata: [";
    for (auto& itr:context->client_metadata()) {
      std::cout << "'" << itr.first << "':'" << itr.second << "', " << std::endl;
    }
    std::cout << "]" << std::endl;

    std::cout << "GetPeerIdentityPropertyName: ";
    std::cout << context->auth_context()->GetPeerIdentityPropertyName() << std::endl;

    if (request->message() == "hello") {
      response->set_code(200);
      response->set_message("200 OK.");
      // object内の "map<string, string> map_arg_string" に key と value を 格納する例
      response->mutable_map_arg_string()->insert(HttpLikeRpc::MakeMapPair("result","hello world."));
      return Status::OK;
    }

    if (request->message() == "ip") {
      response->set_code(200);
      response->set_message("200 OK.");
      response->mutable_map_arg_string()->insert(HttpLikeRpc::MakeMapPair("result", context->peer()));
      return Status::OK;
    }

    // object内 の "map<string, string> map_arg_bin" 内 から Key 名で 値を取得する例
    if (request->message() == "dumpData") {
      auto itr = request->map_arg_bin().find("data");
      if (itr != request->map_arg_string().end()) {
        for(int i=0; i<itr->second.size();i++){
          printf("%02X ", (uint8_t)itr->second[i]);
        }
        printf("\n");
      }
      response->set_code(200);
      response->set_message("200 OK.");
      return Status::OK;
    }

    response->set_message("404 Not found.");
    response->set_code(404);

    return Status::OK;
  }
};

//#######|#########|#########|#########|#########|#########|#########|#########|
class HttpLikeServer {
 public:
  HttpLikeServer() {}
  grpc::ServerBuilder builder_;
  std::unique_ptr<grpc::Server> server_ = {};

  void Run(const char *addr, uint16_t port=0,
           int max_threads_num=1, uint32_t timeout_msec=2000);
  void Shutdown();
  int Port() {return port_;}

 private:
  HttpLikeServiceImpl service_impl_ = {};
  std::thread thread_ = {};
  int port_ = 0;

 //インスタンスのコピー禁止
 private:
  HttpLikeServer(const HttpLikeServer &) = delete;
  HttpLikeServer& operator=(const HttpLikeServer &) = delete;
};


void HttpLikeServerThreadRun( HttpLikeServer *server ) {
  // server が shutdown されるまで blocking します。
  // この呼び出しが戻るには、他の Thread から shutdown する必要があることに注意してください。
  server->server_->Wait();
}

// port が 0 の場合には 空いている port番号を自動取得する
void HttpLikeServer::Run(const char *addr, uint16_t port,
                         int max_threads_num, uint32_t timeout_msec) {
  std::string address(addr);
  address += ":";
  address += std::to_string(port);

  // Listen on the given address without any authentication mechanism.
  builder_.AddListeningPort(address, grpc::InsecureServerCredentials(), &port_ );

  builder_.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::NUM_CQS, 1);
  builder_.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MIN_POLLERS, 1);
  builder_.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MAX_POLLERS, max_threads_num);
  builder_.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::CQ_TIMEOUT_MSEC, timeout_msec);

  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder_.RegisterService(&service_impl_);

  // Finally assemble the server.
  server_ = std::unique_ptr<grpc::Server> (builder_.BuildAndStart());

  thread_ = std::thread(HttpLikeServerThreadRun, this);
}


void HttpLikeServer::Shutdown() {
  if(server_) {
    server_->Shutdown();
  }

  if(thread_.joinable()) {
    thread_.join();
  }
}

//#######|#########|#########|#########|#########|#########|#########|#########|

int main(int argc, char** argv) {
  HttpLikeServer http_like_server;
  http_like_server.Run("0.0.0.0",55551);

  sleep(120);

  http_like_server.Shutdown();
  std::cout << "Server shutdown." << std::endl;
  return 0;
}
