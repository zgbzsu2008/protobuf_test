# protobuf_test
learn protobuf

#### 下载编译zlib

#### 下载编译protobuf 

#### protoc addressbook.proto
protoc -I=C:\protobuf\include -I=. --cpp_out=. addressbook.proto

#### 编译错误
```
// 加个()避免min宏-错误
(std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min)()
```

#### protoc query.proto
protoc -I=C:\protobuf\include -I=. --cpp_out=. query.proto