#include <iostream>
#include <random>
#include <vector>
#include <thread>
#include <chrono>

#include <type_traits>
#include "growt/allocator/alignedallocator.hpp"
#include "growt/data-structures/table_config.hpp"
#include "growt/utils/hash/murmur2_hash.hpp"

using namespace std::chrono_literals;
using VId = std::uint64_t;
using DefaultHasherType = utils_tm::hash_tm::murmur2_hash;
using DefaultAllocatorType = ::growt::AlignedAllocator<>;

using GlobalVIdMap =
    typename ::growt::table_config<VId, VId, DefaultHasherType,
                                   DefaultAllocatorType>::table_type;

template <typename F>
inline void parallel_for(std::size_t begin, std::size_t end, F&& f) {
#pragma omp parallel
	{
#pragma omp for schedule(static)
		for (std::size_t i = begin; i < end; ++i) {
			f(i);
		}
#pragma omp barrier
#pragma omp single
		{
			std::cout << "finished loop" << std::endl;
		}
	}
	std::cout << "finished parallel" << std::endl;
}

int main() {
  const std::size_t n = 1ull << 20;
  std::cout << "n: " << n << std::endl;
  std::vector<std::pair<std::uint64_t, std::uint64_t>> data(n);
  for (std::size_t i = 1; i < n; ++i) {
    data[i].first = i;
    data[i].second = 5;
  }

  for (std::size_t iteration = 0; iteration < 200; ++iteration) {
    std::size_t num_unsucessful_finds = 0;
    std::size_t num_sucessful_finds = 0;

    const std::size_t map_size = data.size() * 1.2;
    GlobalVIdMap map{map_size};

    // fill table
    parallel_for(0, data.size(), [&](std::size_t i) {
      const auto key = data[i].first;
      const auto value = data[i].second;
      const auto [it, _] = map.insert(key + 1, value);
      if (it == map.end()) {
        std::cout << "error" << std::endl;
        std::abort();
      }
    });
    //sleep
    std::this_thread::sleep_for(500ms);

    // retrieve values and check
    for (std::size_t i = 0; i < n; ++i) {
      const auto key = data[i].first;
      const auto value = data[i].second;
      auto it = map.find(key + 1);
      const VId res = (it == map.end()) ? std::numeric_limits<VId>::max() : VId((*it).second);
      num_unsucessful_finds += (res != value);
      num_sucessful_finds += (res == value);
      if (res != value) {
        std::cout << "key: " << key << " expected value: " << value
                  << " found value: " << res << std::endl;
      }
    }
    std::cout << "iteration: " << iteration << std::endl;
    std::cout << "\tSucc Finds: " << num_sucessful_finds << std::endl;
    std::cout << "\tUnSucc Finds: " << num_unsucessful_finds << std::endl;
    std::cout << "\tTotal: " << (num_sucessful_finds + num_unsucessful_finds)
              << std::endl;
    if (num_unsucessful_finds) std::abort();
  }
}
