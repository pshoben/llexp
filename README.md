# llexp
some experiments in low latency linux

## Examining rdtsc and rdtscp performance
* What is the overhead of taking a timestamp?
* What is the overhead of **rdtsc** instruction vs **rdtscp** instruction?
* What is the maximum usable resolution of **rdtsc** instruction vs **rdtscp** instruction across different cores?
* Does **rdtsc** instruction return the same value on different cores on the same socket?
* What is the expected difference or drift between cpu cores.

[Examining rdtsc and rdtscp performance](https://github.com/pshoben/llexp/wiki/Examining-rdtsc-and-rdtscp-performance)


## Benchmarking Low Latency Queues

* What is the fastest way to pass data between threads with lowest possible latency ?
* When is a spinlock faster than lockfree queue ?
* How fast is a simple std::queue synchronised using std::mutex implementation ?



[Benchmarking Low Latency Queues](https://github.com/pshoben/llexp/wiki/Benchmarking-Low-Latency-Queues)


