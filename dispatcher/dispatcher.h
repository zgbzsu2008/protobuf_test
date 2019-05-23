#pragma once

#include <functional>
#include <iostream>
#include <memory>

#include <google/protobuf/message.h>

using namespace google::protobuf;

class Callback
{
 public:
  Callback() {}
  virtual ~Callback() {}
  virtual void onMessage(Message* message) const = 0;

 private:
  Callback(const Callback&);
  Callback& operator=(const Callback&);
};

template <typename T>
class CallbackT : public Callback
{
 public:
  using ProtobufMessageCallback = std::function<void(T* message)>;
  CallbackT(const ProtobufMessageCallback& callback) : callback_(callback) {}

  virtual void onMessage(Message* message) const
  {
    T* t = static_cast<T*>(message);
    assert(t != NULL);
    callback_(t);
  }

 private:
  ProtobufMessageCallback callback_;
};

inline void discardProtobufMessage(Message* message)
{
  std::cout << "Discarding " << message->GetTypeName() << std::endl;
}

class ProtobufDispatcher
{
 public:
  ProtobufDispatcher() : defaultCallback_(discardProtobufMessage) {}

  template <typename T>
  void registerMessageCallback(const typename CallbackT<T>::ProtobufMessageCallback& callback)
  {
    std::shared_ptr<CallbackT<T>> pd(new CallbackT<T>(callback));
    callbacks_[T::descriptor()] = pd;
  }

  void onMessage(Message* message) const
  {
    CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
    if (it != callbacks_.end()) {
      it->second->onMessage(message);
    } else {
      defaultCallback_(message);
    }
  }

  using CallbackMap = std::map<const Descriptor*, std::shared_ptr<Callback>>;
  CallbackMap callbacks_;
  std::function<void(Message* message)> defaultCallback_;
};
