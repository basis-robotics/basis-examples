#include <simple_sub.h>

using namespace unit::simple_sub;

OnChatter::Output simple_sub::OnChatter(const OnChatter::Input &input) {
  Logger().info("OnChatter: {}", input.chatter->message());
  return OnChatter::Output();
}
/*
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
std::thread stats_thread([this]() {
  while (true) {
    Logger().info("Stats: {}", stats());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
});
*/