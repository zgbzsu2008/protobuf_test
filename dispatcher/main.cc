#include <iostream>
#include <string>

#include "dispatcher.h"
#include "query.pb.h"

class ProtobufExceptionClean
{
 public:
  ~ProtobufExceptionClean() { ShutdownProtobufLibrary(); }
};

void onQuery(muduo::Query* query) { std::cout << "onQuery: " << query->GetTypeName() << std::endl; }

void onAnswer(muduo::Answer* answer) { std::cout << "onAnswer: " << answer->GetTypeName() << std::endl; }

int main(int argc, char* argv[])
{
  try {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    ProtobufExceptionClean clean;

    ProtobufDispatcher dispatcher;
    dispatcher.registerMessageCallback<muduo::Query>(onQuery);
    dispatcher.registerMessageCallback<muduo::Answer>(onAnswer);

    muduo::Query q;
    muduo::Answer a;
    muduo::Empty e;

    dispatcher.onMessage(&q);
    dispatcher.onMessage(&a);
    dispatcher.onMessage(&e);

  } catch (const FatalException& e) {
    std::cout << e.filename() << " " << e.line() << " " << e.what() << std::endl;
    return 1;
  }

  return 0;
}