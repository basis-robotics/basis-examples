


In the [Simple publisher & subscriber](README.md) project we set up two units: one publisher and one subscriber, sending and receiving a "Hello, world!" message on the topic `/chatter`.

Once the code is running, it is useful to look at the message traffic: topics, publishers, schemas, and print messages on a topic. Basis includes a Command Line Interface (CLI) tool that can do all these things.

Let's first look at what topics exist while our publisher/subscriber units are running. Using Basis' CLI, we request the list of topics:

```
$ basis topic ls
topics:
  /log [protobuf:foxglove.Log] (1 publisher)
  /chatter [protobuf:StringMessage] (1 publisher)
```

We recognize the `/chatter` topic we created for this project. There's also `/log` for the log of all units.

To retrieve a summary of information about the `/chatter` topic, use the following command:
```
$ basis topic info /chatter
topic: /chatter
type: protobuf:StringMessage

id: ef1159476af615b620011ac42022f9f
  endpoints: 
    net_tcp: 40341
    inproc: 2541
```

We can also get the schema for the `protobuf:StringMessage` type used by `/chatter`:

```
$ basis schema print protobuf:StringMessage
protobuf:StringMessage
file {
  name: "simple_pub_sub.proto"
  message_type {
    name: "StringMessage"
    field {
      name: "message"
      number: 1
      label: LABEL_OPTIONAL
      type: TYPE_STRING
    }
  }
  syntax: "proto3"
}
```

Finally, we can print new messages published on a topic. For `/chatter`, let's show the next three messages in `JSON` fornat:
```
$ basis topic print /chatter -n 3 --json
{"message":"Hello, world!"}
{"message":"Hello, world!"}
{"message":"Hello, world!"}
```

Without the `-n 3` option, it would continuously print messages on the topic.

For the `/log` topic, let's use a plain text output:
```
$ basis topic print /log -n 3       
timestamp {
  seconds: 3279
  nanos: 612674074
}
level: INFO
message: "[2024-07-21 12:49:41.025] [/simple_pub] [info] [simple_pub.cpp:7] PublishAt1Hz\n"
name: "/simple_pub"
file: "/basis/demos/simple_pub_sub/src/simple_pub.cpp"
line: 7

timestamp {
  seconds: 3279
  nanos: 612759490
}
level: INFO
message: "[2024-07-21 12:49:41.025] [/simple_sub] [info] [simple_sub.cpp:6] OnChatter: Hello, world!\n"
name: "/simple_sub"
file: "/basis/demos/simple_pub_sub/src/simple_sub.cpp"
line: 6

timestamp {
  seconds: 3280
  nanos: 612365658
}
level: INFO
message: "[2024-07-21 12:49:42.025] [/simple_pub] [info] [simple_pub.cpp:7] PublishAt1Hz\n"
name: "/simple_pub"
file: "/basis/demos/simple_pub_sub/src/simple_pub.cpp"
line: 7

timestamp {
  seconds: 3280
  nanos: 612579699
}
level: INFO
message: "[2024-07-21 12:49:42.025] [/simple_sub] [info] [simple_sub.cpp:6] OnChatter: Hello, world!\n"
name: "/simple_sub"
file: "/basis/demos/simple_pub_sub/src/simple_sub.cpp"
line: 6

```

