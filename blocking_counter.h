#ifndef TENSORFLOW_LITE_EXPERIMENTAL_RUY_BLOCKING_COUNTER_H_
#define TENSORFLOW_LITE_EXPERIMENTAL_RUY_BLOCKING_COUNTER_H_

#include <atomic>
#include <cstdint>

namespace ruy {

// A BlockingCounter lets one thread to wait for N events to occur.
// This is how the master thread waits for all the worker threads
// to have finished working.
// The waiting is done using a naive spinlock waiting for the atomic
// count_ to hit the value 0. This is acceptable because in our usage
// pattern, BlockingCounter is used only to synchronize threads after
// short-lived tasks (performing parts of the same GEMM). It is not used
// for synchronizing longer waits (resuming work on the next GEMM).
class BlockingCounter {
 public:
  BlockingCounter() : count_(0) {}

  // Sets/resets the counter; initial_count is the number of
  // decrementing events that the Wait() call will be waiting for.
  void Reset(std::size_t initial_count);

  // Decrements the counter; if the counter hits zero, signals
  // the threads that were waiting for that, and returns true.
  // Otherwise (if the decremented count is still nonzero),
  // returns false.
  bool DecrementCount();

  // Waits for the N other threads (N having been set by Reset())
  // to hit the BlockingCounter.
  void Wait();

 private:
  std::atomic<std::size_t> count_;
};

}  // namespace ruy

#endif  // TENSORFLOW_LITE_EXPERIMENTAL_RUY_BLOCKING_COUNTER_H_