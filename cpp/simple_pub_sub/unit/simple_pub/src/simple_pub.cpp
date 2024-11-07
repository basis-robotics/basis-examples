/*

  This is the starting point for your Unit. Edit this directly and implement the missing methods!

*/

#include <simple_pub.h>

using namespace unit::simple_pub;

int count_ = 0;

PublishAt1Hz::Output simple_pub::PublishAt1Hz(const PublishAt1Hz::Input& input) {
  // Logger().info("PublishAt1Hz");
  PublishAt1Hz::Output output;
  std::shared_ptr<StringMessage> msg{std::make_shared<StringMessage>()};
  // msg->set_message(std::string("Hello, world!"));
  msg->set_message(std::to_string(count_++));
  output.chatter = msg;
  return output;
}
