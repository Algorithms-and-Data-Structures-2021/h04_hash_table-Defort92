#pragma once

#include <list>
#include <optional>
#include <string>
#include <utility>  // pair
#include <vector>
#include <unordered_set>

namespace itis {

  namespace utils {
    inline int hash(int key, int table_size) {
      return key % table_size;
    }
  }  // namespace utils

  class HashTable final {
   public:
    // constants
    static constexpr auto kGrowthCoefficient = 2;
    static constexpr auto kDefaultLoadFactor = 0.75;

   private:
    // [(key1, value1), (key2, value2), ...]
    using Bucket = std::list<std::pair<int, std::string>>;

    // struct members
    int num_keys_{0};           // number of (unique) keys in the hash table
    const double load_factor_;  // ratio of "busy" buckets to the total number of buckets [0...1]

    std::vector<Bucket> buckets_;  // array of hash table buckets

    /**
     * Compute hash for a given key using modulo operator.
     * @param key - value of the key
     * @return hash code (or index)
    */
    int hash(int key) const;

   public:
    /**
     * Construct a hash table of a given capacity and constant load factor.
     * @param capacity - number of buckets in the hash table
     * @param load_factor - coefficient of the hash-table fullness
     */
    explicit HashTable(int capacity, double load_factor = kDefaultLoadFactor);

    /**
     * Search (lookup) for the key-value pair.
     * @param key - value of the key
     * @return found value or nothing
     */
    std::optional<std::string> Search(int key) const;

    /**
     * Puts a new or updates an existing key-value pair.
     * @param key - value of the key
     * @param value - data associated with the key
     */
    void Put(int key, const std::string &value);

    /**
     * Remove a key-value pair for the given key.
     * @param key - value of the key
     * @return removed value associated with the key
     */
    std::optional<std::string> Remove(int key);

    /**
     * Check if there is a key-value pair for the given key.
     * @param key - value of the key
     * @return true - if there is a pair with the key, false - otherwise
     */
    bool ContainsKey(int key) const;

    /**
     * @return true - there are no key-value pairs, false - otherwise
     */
    bool empty() const;

    /**
     * @return number of key-value pairs in the hash-table
     */
    int size() const;

    /**
     * @return number of buckets in the hash-table
     */
    int capacity() const;

    double load_factor() const;

    std::unordered_set<int> keys() const;

    std::vector<std::string> values() const;
  };

}  // namespace itis