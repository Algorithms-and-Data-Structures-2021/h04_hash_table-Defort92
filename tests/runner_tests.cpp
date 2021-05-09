#define CATCH_CONFIG_MAIN

// This file provides with unit tests runner

#include <catch2/catch.hpp>

#include <algorithm>  // transform
#include <numeric>    // iota
#include <string>     // to_string

#include "hash_table.hpp"

using namespace std;
using namespace itis;
using Catch::Equals;

SCENARIO("create hash table") {

  GIVEN("capacity in the valid range") {
    const int capacity = GENERATE(range(1, 10));
    const double load_factor = GENERATE(0.01, 0.5, 0.9, 0.99);

    WHEN("creating a hash table") {
      REQUIRE_NOTHROW(HashTable{capacity, load_factor});

      const auto hash_table = HashTable(capacity, load_factor);

      THEN("hash table size should be equal to zero") {
        CHECK(hash_table.empty());
        CHECK(hash_table.capacity() == capacity);
      }
    }
  }

  AND_GIVEN("capacity in the invalid range") {
    const int capacity = GENERATE(range(-10, 1));
    const double load_factor = GENERATE(0.0, 1.01, -0.01);

    WHEN("creating a hash table") {

      THEN("an exception must be thrown") {
        REQUIRE_THROWS_AS(HashTable{capacity}, std::logic_error);
      }
    }
  }
}

SCENARIO("put keys and values") {

  GIVEN("empty hash table") {
    const int capacity = GENERATE(range(1, 10));
    auto hash_table = HashTable(capacity, 1);

    WHEN("putting elements with different keys within loading factor") {
      auto keys = std::vector<int>(capacity - 1);
      auto values = std::vector<std::string>();

      std::iota(std::begin(keys), std::end(keys), 0);  // [0, ... capacity - 1]
      values.reserve(keys.size());

      std::transform(keys.cbegin(), keys.cend(), std::back_inserter(values),
                     [](int key) { return std::to_string(key); });  // ["0", ... "capacity - 1"]

      for (int index = 0; index < static_cast<int>(keys.size()); index++) {
        hash_table.Put(keys[index], values[index]);
      }

      THEN("hash table should not be empty") {
        CHECK(hash_table.size() == keys.size());
        CHECK(hash_table.capacity() == capacity);
      }

      AND_THEN("hash table keys should be correct") {
        for (const auto& key : keys) {
          CHECK(hash_table.ContainsKey(key));
        }
      }

      AND_THEN("hash table values should be correct") {
        CHECK_THAT(values, Equals(hash_table.values()));
      }
    }

    AND_WHEN("putting elements with different keys exceeding loading factor") {
      const auto expected_num_keys = static_cast<int>(capacity * HashTable::kGrowthCoefficient) - 1;
      auto keys = std::vector<int>(expected_num_keys);
      auto values = std::vector<std::string>();

      std::iota(std::begin(keys), std::end(keys), 0);
      values.reserve(keys.size());

      std::transform(keys.cbegin(), keys.cend(), std::back_inserter(values),
                     [](int key) { return std::to_string(key); });

      for (int index = 0; index < static_cast<int>(keys.size()); index++) {
        hash_table.Put(keys[index], values[index]);
      }

      CAPTURE(keys.size());

      THEN("hash table should not be empty") {
        CHECK(hash_table.size() == expected_num_keys);
        CHECK(hash_table.capacity() == static_cast<int>(capacity * HashTable::kGrowthCoefficient));
      }

      AND_THEN("hash table keys should be correct") {
        for (const auto& key : keys) {
          CHECK(hash_table.ContainsKey(key));
        }
      }

      AND_THEN("hash table values should be correct") {
        CHECK_THAT(values, Equals(hash_table.values()));
      }
    }
  }

  AND_GIVEN("non-empty hash table") {
    const int expected_num_keys = GENERATE(range(1, 10));
    const int expected_capacity = GENERATE_COPY(expected_num_keys * 2);

    auto hash_table = HashTable(expected_capacity, 0.9);

    REQUIRE(hash_table.capacity() == expected_capacity);

    auto keys = std::vector<int>(expected_num_keys);
    auto values = std::vector<std::string>();

    std::iota(std::begin(keys), std::end(keys), 0);
    values.reserve(keys.size());

    std::transform(keys.cbegin(), keys.cend(), std::back_inserter(values), [](int key) { return std::to_string(key); });

    for (int index = 0; index < static_cast<int>(keys.size()); index++) {
      hash_table.Put(keys[index], values[index]);
    }

    WHEN("putting an existing key with a different value") {
      const int key_index = GENERATE_COPY(take(10, random(0, expected_num_keys - 1)));
      const std::string value = "reference";

      hash_table.Put(key_index, value);
      values[key_index] = value;

      THEN("value should change") {
        CHECK_THAT(hash_table.values(), Equals(values));
      }
    }
  }
}

SCENARIO("search by key") {
  GIVEN("non-empty hash table") {
    const auto capacity = GENERATE(range(2, 10));
    auto hash_table = HashTable(capacity);

    auto keys = std::vector<int>(capacity - 1);
    auto values = std::vector<std::string>();

    std::iota(std::begin(keys), std::end(keys), 0);
    values.reserve(keys.size());

    std::transform(keys.cbegin(), keys.cend(), std::back_inserter(values), [](int key) { return std::to_string(key); });

    for (int index = 0; index < static_cast<int>(keys.size()); index++) {
      hash_table.Put(keys[index], values[index]);
    }

    REQUIRE(hash_table.size() == keys.size());
    REQUIRE_THAT(hash_table.values(), Equals(values));

    WHEN("searching for existing key-value pairs") {
      const int search_key = GENERATE_COPY(take(100, random(0, static_cast<int>(keys.size() - 1))));

      THEN("they should be found") {
        REQUIRE(hash_table.Search(search_key));
        CHECK(hash_table.Search(search_key).value() == values[search_key]);
      }
    }
  }
}

SCENARIO("remove by key") {
  GIVEN("non-empty hash-table") {
    const auto capacity = GENERATE(range(2, 10));
    auto hash_table = HashTable(capacity, 1);

    auto keys = std::vector<int>(capacity - 1);
    auto values = std::vector<std::string>();

    std::iota(std::begin(keys), std::end(keys), 0);
    values.reserve(keys.size());

    std::transform(keys.cbegin(), keys.cend(), std::back_inserter(values), [](int key) { return std::to_string(key); });

    for (int index = 0; index < static_cast<int>(keys.size()); index++) {
      hash_table.Put(keys[index], values[index]);
    }

    REQUIRE(hash_table.size() == keys.size());
    REQUIRE_THAT(hash_table.values(), Equals(values));

    WHEN("removing by an existing key") {
      const int remove_key = GENERATE_COPY(range(0, capacity - 1));

      auto removed = hash_table.Remove(remove_key);

      THEN("key should be removed") {
        CHECK_FALSE(hash_table.ContainsKey(remove_key));
        CHECK(removed.has_value());
        CHECK(removed.value() == values[remove_key]);
      }
    }

    AND_WHEN("removing by a non-existing key") {
      const int remove_key = GENERATE_COPY(range(capacity, capacity + 10));

      auto removed = hash_table.Remove(remove_key);

      THEN("no key should be removed") {
        CHECK_FALSE(removed.has_value());
        CHECK(hash_table.size() == keys.size());
      }
    }
  }
}
