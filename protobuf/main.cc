#include <ctime>
#include <iostream>
#include <string>

#include <google/protobuf/util/time_util.h>

#include "addressbook.pb.h"

using google::protobuf::util::TimeUtil;

class ProtobufExceptionClean
{
 public:
  ~ProtobufExceptionClean() { google::protobuf::ShutdownProtobufLibrary(); }
};

int main(int argc, char* argv[])
{
  try {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    ProtobufExceptionClean clean;

    test::AddressBook book;
    // person
    auto person = book.add_people();
    person->set_id(35);
    person->set_name("zgb");
    person->set_email("zgbzsu2008@163.com");

    // phone
    auto phone = person->add_phones();
    phone->set_number("13800000000");
    phone->set_type(test::Person::HOME);

    // last_updated
    *person->mutable_last_updated() = TimeUtil::SecondsToTimestamp(std::time(NULL));

    std::string buf;
    if (!book.SerializeToString(&buf)) {  // 序列化
      std::cout << "SerializeToString failed." << std::endl;
      return 1;
    }
    book.PrintDebugString();

    std::cout << "/*---\n" << buf << "\n---*/\n";

    test::AddressBook book2;
    if (!book2.ParsePartialFromString(buf)) {  // 反序列化
      std::cout << "ParsePartialFromString failed." << std::endl;
      return 1;
    }
    book2.PrintDebugString();

  } catch (const google::protobuf::FatalException& e) {
    std::cout << e.filename() << " " << e.line() << " " << e.message() << std::endl;
  }

  return 0;
}