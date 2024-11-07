/*

  This is the starting point for your Unit. Edit this directly and implement the
  missing methods!

*/

#include <simple_sub.h>

using namespace unit::simple_sub;

#include <chrono>
#include <iostream>
#include <thread>

class StatsCounter {
public:
  void OnMessage(const std::string &msg) {
    std::lock_guard<std::mutex> lock(mutex_);

    try {
      const int msg_num = std::stoi(msg);
      const int gap = msg_num - last_msg_num_ - 1;
      if (gap != 0) {
        std::cout << "Missed " << gap << " messages." << std::endl;
        missed_ += gap;
      }
      last_msg_num_ = msg_num;

    } catch (const std::invalid_argument &e) {
      std::cerr << "Invalid input: not a valid integer." << std::endl;
    } catch (const std::out_of_range &e) {
      std::cerr << "Invalid input: number out of range." << std::endl;
    }

    received_++;
  }

  void ShowStats() {
    std::lock_guard<std::mutex> lock(mutex_);
    const int period = received_ > 0 ? int(1000000.0 / received_) : 0;
    std::cout << "Stats: received=" << received_ << " <=> " << period
              << "us    missed=" << missed_ << std::endl;
    received_ = 0;
    missed_ = 0;
  }

private:
  std::mutex mutex_;
  int received_ = 0;
  int missed_ = 0;
  int last_msg_num_ = -1;
};

static StatsCounter stats_counter;

std::thread *stats_thread = nullptr;

void handle_things(const std::string &msg) {
  if (stats_thread == nullptr) {
    stats_thread = new std::thread([]() {
      while (true) {
        stats_counter.ShowStats();
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    });
  }

  stats_counter.OnMessage(msg);
}

OnChatter::Output simple_sub::OnChatter(const OnChatter::Input &input) {
  // Logger().info("OnChatter: {}", input.chatter->message());
  handle_things(input.chatter->message());
  return OnChatter::Output();
}
