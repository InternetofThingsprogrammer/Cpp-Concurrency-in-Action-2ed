#include <atomic>
#include <thread>

struct barrier {
  std::atomic<unsigned> count;       // 需要同步的线程数
  std::atomic<unsigned> spaces;      // 剩余未到达 barrier 的线程数
  std::atomic<unsigned> generation;  // 所有线程到达 barrier 的总次数
  barrier(unsigned n) : count(n), spaces(n), generation(0) {}
  void wait() {
    const unsigned gen = generation.load();
    if (!--spaces) {          // 递减
      spaces = count.load();  // 递减后为 0 则重置 spaces 为 count
      ++generation;
    } else {
      while (generation.load() == gen) {
        std::this_thread::yield();
      }
    }
  }
  void done_waiting() {  // 供最后一个线程调用
    --count;
    if (!--spaces) {
      spaces = count.load();
      ++generation;
    }
  }
};