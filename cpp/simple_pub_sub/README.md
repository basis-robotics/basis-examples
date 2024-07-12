# Simple publisher & subscriber

This tutorial will guide you through the steps to create a simple demo using a publisher and a subscriber. The publisher will send a text message every second, and the subscriber will print the message it receives. We'll use Basis’ code generator to create the skeleton code and boilerplate logic. While using the code generator is optional, it simplifies the implementation.

In Basis, publishers and subscribers are called units. A unit has a number of inputs and outputs. An input listens to a topic defined by a name, while an output publishes messages on a topic. Before delivering inputs to the unit logic, a synchronizer determines when they should be delivered. For instance, the synchronizer can ensure that two input messages are present and received within a given time window before being delivered. In this tutorial, we'll use a simple pass-through synchronizer. 

A process may contain one or more units. Here, the publisher and the subscriber will each operate in their own process, showing how different processes can communicate with each other using Basis’ built-in TCP transport layer. Basis also supports in-process communication, which is much more efficient but will be the subject of another tutorial.

We'll use Protocol Buffers for message serialization. Basis also supports ROS natively and allows for the addition of other serialization methods, including custom ones.

Since Basis uses clang++ 18 for compiling C++ code and CMake for building, we'll use them in this tutorial as well.

## Foundation
The first step is to create the foundation of our project. This includes creating the Protocol Buffers schema, the Basis units description, and the related CMake files. The initial layout of the folders and files will look like this:

```
.
|-- CMakeLists.txt
|-- proto
|   |-- CMakeLists.txt
|   `-- simple_pub_sub.proto
|-- simple_pub.unit.yaml
`-- simple_sub.unit.yaml
```

### Protocol Buffers
The first file to add is the Protocol Buffer message schema. Starting from the root of the project folder, we create the file `proto/simple_pub_sub.proto`:

```proto
syntax= "proto3";

message StringMessage {
  string message = 1;
}
```
This defines the `StringMessage` message, which contains a single string property called `message`. Next, we need a CMake file to build this schema. We assume that you’re familiar with CMake. This file is located at `proto/CMakeLists.txt`:

```CMake
project(simple_pub_sub_proto)
include(FindProtobuf)
find_package(Protobuf REQUIRED)


add_library(simple_pub_sub_proto simple_pub_sub.proto)
target_link_libraries(simple_pub_sub_proto basis::plugins::serialization::protobuf)


set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
make_directory(${GENERATED_DIR})
target_include_directories(simple_pub_sub_proto SYSTEM PUBLIC "$<BUILD_INTERFACE:${GENERATED_DIR}>")


protobuf_generate(
 LANGUAGE cpp
 TARGET simple_pub_sub_proto
  PROTOC_OUT_DIR "${GENERATED_DIR}"
 )
```

The name of this project is `simple_pub_sub_proto`. The output will be transient and located in the build folder, meaning they will not be pushed to the git repository. The path is defined by the variable `GENERATED_DIR`. The line `target_link_libraries(...)` links Basis’ Protocol Buffers serialization plugin.

### Units description
To declare the units, we will now create two YAML files. The first file describes the publisher, which has no external input and instead relies on a "rate subscriber" to produce its output at regular intervals. In this example, the synchronizer is set to 'all' input, making it a pass-through. The publisher's output will be sent on the `/chatter` topic, using the `StringMessage` type we defined earlier. The second file describes the subscriber.

The publisher unit description `simple_pub.unit.yaml`:
```yaml
threading_model:
  single
cpp_includes:
  - simple_pub_sub.pb.h
handlers:
  PublishAt1Hz:
    sync:
      type: all
      rate: 1
    outputs:
      /chatter:
        type: protobuf:StringMessage
```

The subscriber has one input, listening to the `/chatter` topic, and no output. Synchronization is also set to 'all'.

The subscriber unit description `simple_sub.unit.yaml`:
```yaml
threading_model:
  single
cpp_includes:
  - simple_pub_sub.pb.h
handlers:
  OnChatter:
    sync:
      type: all
    inputs:
      /chatter:
        type: protobuf:StringMessage
```

### First build
We are now ready to set up the boilerplate code using Basis’ code generator. We will create a single CMake file for both the publisher and the subscriber. Note that in a real project, each would likely have its own CMake project file. 

`CMakeLists.txt`:

```CMake
cmake_minimum_required(VERSION 3.28)

project(SimplePubSub)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(/basis/ /basis/build)
add_subdirectory(proto)

generate_unit(simple_pub)
generate_unit(simple_sub)

target_link_libraries(unit_simple_pub simple_pub_sub_proto)
target_link_libraries(unit_simple_sub simple_pub_sub_proto)
```

We added the Basis code with `add_subdirectory(/basis/ /basis/build)` because we are running in a Docker container with the code mounted at `/basis`. The generated units are named `unit_simple_pub` and `unit_simple_sub`.

To build, we run the following command line instructions:
```bash
mkdir build
cd build
cmake ..
make -j
```

The last step will fail, which is intended. We’ll soon see why and how to fix it. The following errors will appear:
```
[..]/src/simple_pub.cpp:13:19: error: static assertion failed: Implement me
   13 |     static_assert(false, "Implement me");
      |                   ^~~~~
[..]/src/simple_sub.cpp:13:19: error: static assertion failed: Implement me
   13 |     static_assert(false, "Implement me");
      |                   ^~~~~
1 error generated.
```

Since it’s the first build, the code generator added four files to our folder structure: a header and a source file for both the publisher and the subscriber. They are shown in bold here:

```
.
|-- CMakeLists.txt
|-- include
|   |-- simple_pub.h
|   `-- simple_sub.h
|-- proto
|   |-- CMakeLists.txt
|   `-- simple_pub_sub.proto
|-- simple_pub.unit.yaml
|-- simple_sub.unit.yaml
|-- src
    |-- simple_pub.cpp
    `-- simple_sub.cpp
```

### Implementation and running
The build failed because the implementation of the publisher and the subscriber is missing. The generator created a placeholder with a static assert. For instance, the publisher code in `src/simple_pub.cpp` is:

```c++
PublishAt1Hz::Output
simple_pub::PublishAt1Hz(const PublishAt1Hz::Input &input) {
    static_assert(false, "Implement me");
}
```

We can now implement our logic in the publisher and the subscriber.

The publisher `src/simple_pub.cpp`:

```c++
#include <simple_pub.h>

using namespace unit::simple_pub;

PublishAt1Hz::Output
simple_pub::PublishAt1Hz(const PublishAt1Hz::Input &input) {
  spdlog::info("simple_pub::PublishAt1Hz");
  PublishAt1Hz::Output output;
  std::shared_ptr<StringMessage> msg{std::make_shared<StringMessage>()};
  msg->set_message(std::string("Hello, world!"));
  output.chatter = msg;
  return output;
}
```

The method returns a `PublishAt1Hz::Output` struct that was produced by the code generator. It holds all the output topics of this unit and other metadata:

```c++
struct Output {

    // /chatter
    std::shared_ptr<const StringMessage> chatter;


[... metadata such as timestamp...]
};
```

The subscriber `src/simple_sub.cpp`:

```c++
#include <simple_sub.h>

using namespace unit::simple_sub;

OnChatter::Output simple_sub::OnChatter(const OnChatter::Input &input) {
  spdlog::info("simple_sub::OnChatter: {}", input.chatter->message());
  return OnChatter::Output();
}
```

## Run individual processes

We can now build our code and run it. As before, we build with:

```bash
make -j
```

To run this tutorial code, we start three processes: the coordinator, the publisher, and the subscriber. The coordinator handles all the communications between units. It can be part of your code, but a default coordinator is provided as part of Basis. We use the default coordinator here.


```log
$ /basis/build/cpp/core/coordinator/coordinator
[2024-07-04 21:24:01.609] [info] Adding schema protobuf:StringMessage
[2024-07-04 21:24:11.608] [error] Client connection disconnect after 0 bytes
[2024-07-04 21:24:44.607] [error] Client connection disconnect after 0 bytes
```

```log
$ ./simple_pub 
[2024-07-04 21:24:34.467] [info] simple_pub::PublishAt1Hz
[2024-07-04 21:24:35.467] [info] simple_pub::PublishAt1Hz
[2024-07-04 21:24:36.467] [info] simple_pub::PublishAt1Hz
[2024-07-04 21:24:37.467] [info] simple_pub::PublishAt1Hz
[2024-07-04 21:24:38.467] [info] simple_pub::PublishAt1Hz
[2024-07-04 21:24:39.467] [info] simple_pub::PublishAt1Hz
[2024-07-04 21:24:40.467] [info] simple_pub::PublishAt1Hz
[2024-07-04 21:24:41.467] [info] simple_pub::PublishAt1Hz
[2024-07-04 21:24:42.467] [info] simple_pub::PublishAt1Hz
^C
```

```log
$ ./simple_sub
[2024-07-04 21:24:36.468] [info] simple_sub::OnChatter: Hello, world!
[2024-07-04 21:24:37.468] [info] simple_sub::OnChatter: Hello, world!
[2024-07-04 21:24:38.468] [info] simple_sub::OnChatter: Hello, world!
[2024-07-04 21:24:39.468] [info] simple_sub::OnChatter: Hello, world!
[2024-07-04 21:24:40.468] [info] simple_sub::OnChatter: Hello, world!
[2024-07-04 21:24:41.468] [info] simple_sub::OnChatter: Hello, world!
[2024-07-04 21:24:42.468] [info] simple_sub::OnChatter: Hello, world!
[2024-07-04 21:24:43.071] [error] Disconnecting from topic /chatter
^C
```

## Run with the Basis Launcher

Another way to run our code is to use the Basis launcher. The launcher requires a configuration file to specify how the units will be started and the parameters. Here we will look at how we can use the configuration file to run the publisher and subscriber either as two processes or as two threads within one process.

Two processes with one thread each (`launch_two_process.yaml`):

```yaml
processes:
  demo_pub:
    units:
      simple_pub: {}
  simple_sub:
    units:
      simple_sub: {}
```

Launching:
```sh
basis launch launch_two_process.yaml
```


Two threads within one process (`launch_single_process.yaml`):
```yaml
processes:
  demo:
    units:
      simple_pub: {}
      simple_sub: {}
```

Launching:
```sh
basis launch launch_single_process.yaml
```
