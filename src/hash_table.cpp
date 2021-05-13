#include "hash_table.hpp"

#include <stdexcept>

namespace itis {

  int HashTable::hash(int key) const {
    return utils::hash(key, static_cast<int>(buckets_.size()));
  }

  HashTable::HashTable(int capacity, double load_factor) : load_factor_{load_factor} {
    if (capacity <= 0) {
      throw std::logic_error("hash table capacity must be greater than zero");
    }

    if (load_factor <= 0.0 || load_factor > 1.0) {
      throw std::logic_error("hash table load factor must be in range [0...1]");
    }

    buckets_.resize(capacity);
  }

  std::optional<std::string> HashTable::Search(int key) const {
    if (num_keys_ == 0) {
      return std::nullopt;
    }

    int index = hash(key);
    auto bucket = buckets_[index];
    if (bucket.empty()) {
      return std::nullopt;
    }

    for (auto pair : bucket){
      if (pair.first == key) {
        return pair.second;
      }
    }
    return std::nullopt;
  }

  void HashTable::Put(int key, const std::string &value) {
    num_keys_ += 1;
    int index = hash(key);
    for (std::pair<int, std::string>& pair : buckets_[index]){
      if (pair.first == key) {
        pair.second = value;
        return;
      }
    }

    std::pair<int, std::string> new_pair = std::make_pair(key, value);
    buckets_[index].push_back(new_pair);

    if (static_cast<double>(num_keys_) / buckets_.size() >= load_factor_) {
      auto new_size = buckets_.size() * kGrowthCoefficient;
      std::vector<Bucket> new_buckets = std::vector<Bucket>{};
      new_buckets.resize(new_size);

      for (auto bucket_in_buckets : buckets_) {
        for (auto pair : bucket_in_buckets) {
          int new_index = utils::hash(pair.first, new_size);
          new_buckets[new_index].push_back(pair);
        }
      }

      buckets_ = new_buckets;
    }
  }

  std::optional<std::string> HashTable::Remove(int key) {
    std::pair<int, std::string> pair_to_return;
    int index = hash(key);
    for (std::pair<int, std::string>& pair : buckets_[index]){
      if (pair.first == key){
        pair_to_return = std::make_pair(pair.first, pair.second);
        buckets_[index].remove(pair);
        return pair_to_return.second;
      }
    }
    return std::nullopt;
  }

  bool HashTable::ContainsKey(int key) const {
    // Note: uses Search(key) which is not initially implemented
    return Search(key).has_value();
  }

  bool HashTable::empty() const {
    return size() == 0;
  }

  int HashTable::size() const {
    return num_keys_;
  }

  int HashTable::capacity() const {
    return static_cast<int>(buckets_.size());
  }

  double HashTable::load_factor() const {
    return load_factor_;
  }

  std::unordered_set<int> HashTable::keys() const {
    std::unordered_set<int> keys(num_keys_);
    for (const auto &bucket : buckets_) {
      for (const auto &[key, _] : bucket) {
        keys.insert(key);
      }
    }
    return keys;
  }

  std::vector<std::string> HashTable::values() const {
    std::vector<std::string> values;
    for (const auto &bucket : buckets_) {
      for (const auto &[_, value] : bucket) {
        values.push_back(value);
      }
    }
    return values;
  }

} // namespace itis