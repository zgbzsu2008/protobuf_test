#pragma once

#include <iostream>
#include <string>

#include <WinSock2.h>

#include <zlib.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace google::protobuf;

/*
 struct ProtobufTransportFormat
 {
   int32_t  len;
   int32_t  nameLen;
   char     typeName[nameLen];
   char     protobufData[len-nameLen-8];
   int32_t  checkSum; // adler32 of nameLen, typeName and protobufData
 }
*/

constexpr int kHeaderLen = sizeof(int32_t);

inline std::string encode(const Message& message)
{
  std::string result;
  result.resize(kHeaderLen);

  const std::string typeName = message.GetTypeName();
  int32_t nameLen = static_cast<int32_t>(typeName.size() + 1);
  int32_t be32 = ::htonl(nameLen);
  result.append(reinterpret_cast<char*>(&be32), sizeof(be32));  // result.append(std::to_string(be32)); // 2£¬int32_t
  result.append(typeName.c_str(), nameLen);                     // 3£¬char[]
  bool succeed = message.AppendToString(&result);               // 4£¬char[]
  if (succeed) {
    const char* begin = result.c_str() + kHeaderLen;
    int32 checkSum = adler32(1, reinterpret_cast<const Bytef*>(begin), result.size() - kHeaderLen);
    int32 be32 = ::htonl(checkSum);
    result.append(reinterpret_cast<char*>(&be32), sizeof(be32));  // 5£¬int32_t
    int32_t len = ::htonl(result.size() - kHeaderLen);
    std::copy(reinterpret_cast<char*>(&len), reinterpret_cast<char*>(&len) + sizeof(len),
              result.begin());  // 1£¬int32_t
  } else {
    result.clear();
  }
  return result;
}

inline Message* createMessageByTypeName(const std::string& typeName)
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

inline int32_t asInt32(const char* buf)
{
  int32_t be32 = 0;
  ::memcpy(&be32, buf, sizeof(be32));
  return ::ntohl(be32);
}

/*
 struct ProtobufTransportFormat
 {
   int32_t  nameLen;
   char     typeName[nameLen];
   char     protobufData[len-nameLen-8];
   int32_t  checkSum; // adler32 of nameLen, typeName and protobufData
 }
*/
inline Message* decode(const std::string& buf)
{
  Message* result = NULL;
  int32_t len = static_cast<int32>(buf.size());
  if (len >= 10) {
    int32_t expectedCheckSum = asInt32(buf.c_str() + buf.size() - kHeaderLen);
    const char* begin = buf.c_str();
    int32_t checkSum = adler32(1, reinterpret_cast<const Bytef*>(begin), len - kHeaderLen);
    if (checkSum == expectedCheckSum) {
      int32_t nameLen = asInt32(buf.c_str());
      if (nameLen >= 2 && nameLen <= len - 2 * kHeaderLen) {
        std::string typeName(buf.begin() + kHeaderLen, buf.begin() + kHeaderLen + nameLen - 1);
        Message* message = createMessageByTypeName(typeName);
        if (message) {
          const char* data = buf.c_str() + kHeaderLen + nameLen;
          int32_t dataLen = len - nameLen - 2 * kHeaderLen;
          if (message->ParseFromArray(data, dataLen)) {
            result = message;
          } else {
            delete message;
          }
        } else {
          // message
        }
      } else {
        // nameLen
      }
    } else {
      // checkSum != expectedCheckSum
    }
  } else {
    // len < 10
  }
  return result;
}
