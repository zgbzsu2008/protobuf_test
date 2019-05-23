#include <iostream>
#include <string>
#include <google/protobuf/util/time_util.h>

#include "query.pb.h"

using namespace google::protobuf;

class ProtobufExceptionClean
{
 public:
  ~ProtobufExceptionClean() { ShutdownProtobufLibrary(); }
};

// 根据typename创建Message对象
Message* createMessageByTypeName(const std::string& typeName)
{
  Message* message = NULL;
  const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
  if (descriptor) {
    const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (prototype) {
      message = prototype->New();
    }
  }
  return message;
}

int main(int argc, char* argv[])
{
  try {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    ProtobufExceptionClean clean;

    // descriptor描述符
    using T = muduo::Query;
    std::string typeName = T::descriptor()->full_name();  // muduo.Query

    /*
    // 1，通过.proto文件获取file描述符 再通过typeName查找descriptor
    const FileDescriptor* fileDescriptor = DescriptorPool::generated_pool()->FindFileByName("query.proto");
    for (int i = 0; i < fileDescriptor->message_type_count(); ++i) {
      auto message_type = fileDescriptor->message_type(i);
    }
    const Descriptor* descriptor = fileDescriptor->FindMessageTypeByName("muduo.Query");
    */

    // 1，通过typeName查找descriptor
    const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    assert(descriptor == T::descriptor());

    // 2，通过descriptor获得prototype
    const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
    assert(prototype == &T::default_instance());

    // 3，通过prototype创建
    T* new_obj = static_cast<T*>(prototype->New());
    assert(typeid(*new_obj) == typeid(T::default_instance()));
    delete new_obj;

    T* query = static_cast<T*>(createMessageByTypeName("muduo.Query"));
    query->set_id(1);
    query->set_question("question");
    query->set_questioner("questioner");
    query->PrintDebugString();

  } catch (const FatalException& e) {
    std::cout << e.filename() << " " << e.line() << " " << e.message() << std::endl;
  }

  return 0;
}