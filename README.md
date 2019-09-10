# gRPC の使い方の学習のためのサンプルソース
## 概要
* gRPC の使用方法を調査して作成したサンプルプログラムです。
  * gRPC で提供されるサンプルプログラムを元にしています。

* gRPC の サンプルプログラムのチュートリアルから、自前アプリでgRPCを使用するまでの間で、調べた内容を中心に期しています。
  * 動作するサンプルプログラムから読み取る方が分かりやすいと考え、サンプルプログラムを例に↓の内容を示します。
	* CentOS7 に gRPC の環境を構築方法
	* gRPC 用を autotools で生成するプロジェクト での 使用例として↓を示します。
		* configure.ac に gRPC の必要ライブラリの検出
		* Makefile.am に protoc の実行
	* gRPC の RPC Object に Map の使用例として↓を示します。
		* RPC Ojject の Map への Key と Value の格納
		* RPC Ojject の Map への Key で検索し Value の取得
		* RPC Ojject の Map への 全要素 の 取得
	* gRPC の Server Client の実装例として↓を示します。
		* Server を Thread から動作
		* Server が使用する listen port の 番号を空いているものから自動で取得させる
		* Server サイドから Client の IPアドレスを取得
		* Server サイドの Thread数上限 と Timeout 値の設定 
	
# gRPC環境構築 for CentOS7
## rpm インストール
* centos7 では cbs.centos.org から rpm をダウンロードし、yum install するのが 最短手に思えています。
* ↓の 1連のコマンド grpc の環境構築が行えます。 (↓は2019/9/9時点のバージョン、後日ではバージョンが更新されてると予想します、cbs.centos.org で検索・確認を推奨します）
```
wget -P /dev/shm https://cbs.centos.org/kojifiles/packages/protobuf/3.6.1/4.el7/x86_64/protobuf-3.6.1-4.el7.x86_64.rpm &&
wget -P /dev/shm https://cbs.centos.org/kojifiles/packages/protobuf/3.6.1/4.el7/x86_64/protobuf-compiler-3.6.1-4.el7.x86_64.rpm &&
wget -P /dev/shm https://cbs.centos.org/kojifiles/packages/protobuf/3.6.1/4.el7/x86_64/protobuf-devel-3.6.1-4.el7.x86_64.rpm &&
wget -P /dev/shm https://cbs.centos.org/kojifiles/packages/grpc/1.20.1/1.el7/x86_64/grpc-1.20.1-1.el7.x86_64.rpm &&
wget -P /dev/shm https://cbs.centos.org/kojifiles/packages/grpc/1.20.1/1.el7/x86_64/grpc-devel-1.20.1-1.el7.x86_64.rpm &&
wget -P /dev/shm https://cbs.centos.org/kojifiles/packages/grpc/1.20.1/1.el7/x86_64/grpc-plugins-1.20.1-1.el7.x86_64.rpm &&
sudo yum install -y /dev/shm/protobuf-3.6.1-4.el7.x86_64.rpm \
/dev/shm/protobuf-compiler-3.6.1-4.el7.x86_64.rpm \
/dev/shm/protobuf-devel-3.6.1-4.el7.x86_64.rpm \
/dev/shm/grpc-1.20.1-1.el7.x86_64.rpm \
/dev/shm/grpc-devel-1.20.1-1.el7.x86_64.rpm \
/dev/shm/grpc-plugins-1.20.1-1.el7.x86_64.rpm
```

## サンプルソース の build と 実行方法
### build 方法
```
./autoghen && ./configure && make
```

### 実行方法
* コンソール画面を２面開きます
  * 1つめのコンソールで `./src/greeting_server ` を実行します。
  * 2つめのコンソールで `./src/greeting_client ` を実行します。
* 双方の画面に、通信内容が表示されます。


## autotools project に gRPC build 用の処理 の 追加
### ./configure.ac
* gRPC と gRPC++ が インストールされているか確認します。
	* gRPC と gRPC++ が見つからない時は その旨を表示しエラーとして終了します。
	* ↓が該当箇所です。
```
PKG_CHECK_MODULES([GRPC], [grpc >= 7.0.0],
                  [
                  AM_CONDITIONAL(HAVE_GRPC, true)
                  AC_SUBST(have_grpc, 1)
                  AC_SUBST(grpc_version, `pkg-config --modversion grpc`)
                  ],
                  [
                  AM_CONDITIONAL(HAVE_GRPC, false)
                  AC_SUBST(grpc_version, 0)
                  AC_MSG_ERROR([Can not find voice_recognizer. Please check whether grpc is installed. Maybe you want to set environment variable PKG_CONFIG_PATH.])
                  ])

PKG_CHECK_MODULES([GRPCPP], [grpc++ >= 1.20.0],
                  [
                  AM_CONDITIONAL(HAVE_GRPCPP, true)
                  AC_SUBST(have_grpcpp, 1)
                  AC_SUBST(grpcpp_version, `pkg-config --modversion grpc++`)
                  ],
                  [
                  AM_CONDITIONAL(HAVE_GRPCPP, false)
                  AC_SUBST(grpcpp_version, 0)
                  AC_MSG_ERROR([Can not find voice_recognizer. Please check whether grpc++ is installed. Maybe you want to set environment variable PKG_CONFIG_PATH.])
                  ])
```	
* gRPC と gRPC++ が見つかった時は バージョン情報を表示します。
	* 	↓ が該当箇所です
```
LIBRARY:

  gRPC              : $(if test "x$have_grpc" = "x1"; then echo yes; else echo no; fi)
     version        : $grpc_version

  gRPC++            : $(if test "x$have_grpcpp" = "x1"; then echo yes; else echo no; fi)
     version        : $grpcpp_version
```

### 前提: gRPC の .proho ファイルと、 protoc で C++向けファイルの生成について。


* gRPC では .proto ファイルに、サービス と RPC オブジェクトを定義します。
  * 例えば hoge.proto ファイルに サービス と RPC オブジェクトを記載したとして、C++ のソースコードへの変換は、↓の２コマンドで行います。
```
# hoge.proto の存在するディレクトリで実行する例
protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./hoge.proto
protoc -I=. --cpp_out=. hoge.proto

``` 
* ↑の2コマンドにより、↓の4ファイルが生成されます。
```
hoge.pb.h
hoge.bp.cc
hoge.grpc.pb.h
hoge.grpc.pb.cc
```
* ユーザプロジェクトではこの４ファイルを依存に組み込みます。
  * .h ファイルは gRPC を用いるソースコードファイルから include します
  * .cc ファイルは gRPC を用いるプログラムの build 対象に加える様に Makefile に記載します。

* protoc で生成される .pb.h .pb.cc は gRPC のバージョンに依存します。 
  * 異なるバージョンのgRPCが入った環境での build はエラーとなることが多い様です。
  * 各環境で protoc 変換を行うべきです。
  * ↑の変換を毎回 手動で行うのは忘れがちなので、Makefile.am で自動で行わせるようにします。

### ./src/Makefile.am
* protoc コマンドで .proto ファイル → C++用ファイル変換を行います。
  * ↓が該当箇所です
```.src/Makefile.am
# protoc を用いての *.proto から .grpc.pb.cc と .pb.cc の作成ルール
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) --cpp_out=. $<

```

* make clean 時に、.proto から生成したファイルの削除を行わせます。
  * ↓が該当箇所です
```./src/Makefile.am
# protoc での作成物の make clean 時の削除
MOSTLYCLEANFILES = \ 
 http_like_rpc.pb.cc\
 http_like_rpc.pb.h \
 http_like_rpc.grpc.pb.cc\
 http_like_rpc.grpc.pb.h 
```

# C++ から gRPC の 使用方法
## gRPC Object の Map の使用例

* .proto 内で ↓の RPC オブジェクトを定義しています。
```
message HttpLikeObject {
  uint32 code = 1;
  string message = 2;
  map<string, string> map_arg_string = 3;
  map<string, bytes> map_arg_bin = 4;
}
```
* ↑の map : map_arg_string に key と Value を格納する例が↓です。 

```./src/tools_rpc.h
using MakeMapPair =
        google::protobuf::MapPair<std::string, std::string>;

```
```./src/greeting_server.cpp
      // object内の "map<string, string> map_arg_string" に key と value を 格納する例
      response->mutable_map_arg_string()->insert(HttpLikeRpc::MakeMapPair("result","hello world."));
```

* map : map_arg_string から key で検索して value を取り出す例が↓です。 (std::map と 同様です)
```./src/greeting_client.cpp
  // object内 の "map<string, string> map_arg_string" 内 から Key 名で 値を取得する例
  auto itr = response.map_arg_string().find("result");
  if (itr != response.map_arg_string().end()) {
    std::cout << "result:" << itr->second << std::endl;
  }
```

* map : map_arg_string の内容を順次処理する例が↓です。(std::map と 同様です)
```./src/tools_rpc.h
  for(auto itr = obj->map_arg_string().begin(); itr != obj->map_arg_string().end(); ++itr) {
    retuen_stream << "'" << itr->first << "':'" << itr->second << "', " ;
  }

```

* map : map_arg_bin に バイナリーデータ (const uint8_t *p , size_t size) を格納する例が↓です。
```
  std::string tmp;
  tmp.resize(size);
  memcpy((uint8_t *)&tmp[0], p, size); // このコピーを省きたいが思い浮かばない
  request.mutable_map_arg_bin()->insert(
      HttpLikeRpc::MakeMapPair(HttpLikeRpc::kArgData, tmp ));
```

* map : map_arg_bin から key で検索して バイナリーデータ部にアクセスする例が↓です。
```
  auto itr = request->map_arg_bin().find(HttpLikeRpc::kArgData);
  if (itr != request->map_arg_string().end()) {
    for(int i=0; i<itr->second.size();i++){
      printf("%02X ", (uint8_t)itr->second[i]);
    }
    printf("\n");
  }

```

## gRPC の Server Client の実装例
* Server を Thread から動作
	* gRPC の Server を稼働させると該当Thread が Block されるため、専用のThreadを用意するのが良いようです。
	* 開始と終了の指示が行いやすい様に、↓のclass を用意しています。
```.src/greeting_server.cpp
class HttpLikeServer {
 public:
  HttpLikeServer() {}
  grpc::ServerBuilder builder_;
  std::unique_ptr<grpc::Server> server_ = {};

  // Server Thread 稼働開始：
  //  addr : Listen port に用いる IPアドレス
  //  port に 0 を指定した際には、Listen port 番号は 空いているものから自動で取得する。
  //  max_thread_num : Server が生成する上限Thread数。clientからの通信頻度によってはこの数を少ない数で稼働している時もある。
  //  timeout_msec : client との通信 timeout 単位は ミリ秒
  void Run(const char *addr, uint16_t port=0,
           int max_threads_num=1, uint32_t timeout_msec=2000);

  // Server Thread 終了指示
  void Shutdown();
  
  // Server の listen port 番号を戻す
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
```


* Server が使用する listen port の 番号の自動取得方法
``` .src/greeting_server.cpp
  // builder への指定で実施する
  // Listen on the given address without any authentication mechanism.
  builder_.AddListeningPort(address, grpc::InsecureServerCredentials(), &port_ );
    // 自動取得させたい場合には ↑の関数で
    //  address の文字列で "127.0.0.1:0" と port 番号に 0 を指定し、第3引数に戻り値用(Port番号)の変数を指定する。
```

* Server サイドの Thread数上限 と Timeout 値の設定 
``` .src/greeting_server.cpp
  // builder への指定で実施する
  builder_.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::NUM_CQS, 1);
  builder_.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MIN_POLLERS, 1); // Thread の最小数 = 1
  builder_.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MAX_POLLERS, max_threads_num); // Thread の最大数 
  builder_.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::CQ_TIMEOUT_MSEC, timeout_msec); // Timeout 時間の設定
```


* Server サイドから Client の IPアドレスを取得
``` .src/greeting_server.cpp
  // RPC の サービス関数の 引数から取得する
  Status Post(ServerContext* context, const HttpLikeObject* request,
                  HttpLikeObject* response) override {
    std::cout << "clientIP: " << context->peer() << std::endl;
	// 中略
  }

```

* Client 側の例
	* 通信の度にクライアントを生成している例です。
```
void SampleClientHellow(const char *server_address) {
  std::cout << "--- SampleClientHellow() ---" << std::endl;

  HttpLikeObject request = {};
  HttpLikeObject response = {};
  HttpLikeClient client(grpc::CreateChannel(
      server_address,grpc::InsecureChannelCredentials()));

  request.set_message(HttpLikeRpc::kCmdHello);

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
```


## 補足

* HttpLikeRPC
  * この例サンプルプログラムでは、 RPC オブジェクト は一つにして、その内容を HTTP 風に用いてます。
    * この使用例では、protobuf の 利点を削いでます。 
    * .proto ファイルに、サービス毎に定義し、その RPCオブジェクトも定義しておき、.proto ファイルのみで完結させておくのがセオリーです。
