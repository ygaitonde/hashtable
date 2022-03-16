#ifndef HASHTABLE_H
#define HASHTABLE_H

// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <cassert> // useful for debugging!
#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>

// A bucket's status tells you whether it's empty, occupied,
// or contains a deleted element.
enum class Status : uint8_t { Empty, Occupied, Deleted };

template <typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
  // used by the autograder; do not change/remove.
  friend class Verifier;

public:
  // A bucket has a status, a key, and a value.
  struct Bucket {
    // Do not modify Bucket.
    Status status = Status::Empty;
    K key;
    V val;
  };

  HashTable() {
    // TODO: a default constructor (possibly empty).

    // You can use the following to avoid implementing rehash_and_grow().
    // However, you will only pass the AG test cases ending in _C.
    // To pass the rest, start with at most 20 buckets and implement
    // rehash_and_grow().
    //    buckets.resize(10000);
    for (int i = 0; i < 20; i++) {
      buckets.push_back(Bucket());
    }
  }

  size_t size() const { return num_elements; }

  // returns a reference to the value in the bucket with the key, if it
  // already exists. Otherwise, insert it with a default value, and return
  // a reference to the resulting bucket.
  V &operator[](const K &key) {
    size_t idx = get_hash(key);
    switch (buckets[idx].status) {
    case Status::Empty:
      insert(key, V());
      return buckets[idx].val;
      break;
    case Status::Deleted:
      for (size_t i = 1; i < buckets.size(); i++) {
        size_t probe_idx = (idx + i) % buckets.size();
        auto &bucket = buckets[probe_idx];
        if (bucket.status == Status::Empty) {
          // insert into the original spot
          insert(key, V());
          return buckets[idx].val;
        }
        if (bucket.status == Status::Occupied && bucket.key == key) {
          return bucket.val;
        }
      }
      break;
    case Status::Occupied:
      if (buckets[idx].key == key) {
        return buckets[idx].val;
      }
      int first_deleted = -1;
      for (size_t i = 1; i < buckets.size(); i++) {
        size_t probe_idx = (idx + i) % buckets.size();
        auto &bucket = buckets[probe_idx];
        if (bucket.status == Status::Empty) {
          insert(key, V());
          if (first_deleted == -1) {
            return buckets[probe_idx].val;
          } else {
            return buckets[first_deleted].val;
          }
        }
        if (bucket.status == Status::Deleted && first_deleted == -1) {
          first_deleted = probe_idx;
        }
        if (bucket.key == key && bucket.status == Status::Occupied) {
          return bucket.val;
        }
      }
      break;
    }
  }

  // insert returns whether inserted successfully
  // (if the key already exists in the table, do nothing and return false).
  bool insert(const K &key, const V &val) {
    size_t idx = get_hash(key);
    switch (buckets[idx].status) {
    case Status::Empty:
      buckets[idx].key = key;
      buckets[idx].val = val;
      buckets[idx].status = Status::Occupied;
      num_elements++;
      if (!buckets.empty() && (static_cast<double>(num_elements) /
                               static_cast<double>(buckets.size())) > 0.5) {
        rehash_and_grow();
      }
      return true;
    case Status::Deleted:
      for (size_t i = 1; i < buckets.size(); i++) {
        size_t probe_idx = (idx + i) % buckets.size();
        auto &bucket = buckets[probe_idx];
        if (bucket.status == Status::Empty) {
          break;
        }
        if (bucket.key == key && bucket.status == Status::Occupied) {
          return false;
        }
      }
      buckets[idx].key = key;
      buckets[idx].val = val;
      buckets[idx].status = Status::Occupied;
      num_elements++;
      if (!buckets.empty() &&
          (static_cast<double>(num_elements) / buckets.size()) > 0.5) {
        rehash_and_grow();
      }
      return true;
    case Status::Occupied:
      if (buckets[idx].key == key) {
        return false;
      }
      int first_deleted = -1;
      for (size_t i = 1; i < buckets.size(); i++) {
        size_t probe_idx = (idx + i) % buckets.size();
        auto &bucket = buckets[probe_idx];
        if (bucket.status == Status::Empty) {
          if (first_deleted == -1) {
            buckets[probe_idx].key = key;
            buckets[probe_idx].val = val;
            buckets[probe_idx].status = Status::Occupied;
          } else {
            buckets[first_deleted].key = key;
            buckets[first_deleted].val = val;
            buckets[first_deleted].status = Status::Occupied;
          }
          num_elements++;
          if (!buckets.empty() &&
              (static_cast<double>(num_elements) / buckets.size()) > 0.5) {
            rehash_and_grow();
          }
          return true;
        }
        if (bucket.status == Status::Deleted && first_deleted == -1) {
          first_deleted = probe_idx;
        }
        if (bucket.status == Status::Occupied && bucket.key == key) {
          return false;
        }
      }
    }
    return true;
  }

  // erase returns the number of items remove (0 or 1)
  size_t erase(const K &key) {
    size_t idx = get_hash(key);
    switch (buckets[idx].status) {
    case Status::Empty:
      return 0;
    case Status::Deleted:
      for (size_t i = 1; i < buckets.size(); i++) {
        size_t probe_idx = (idx + i) % buckets.size();
        auto &bucket = buckets[probe_idx];
        if (bucket.status == Status::Empty) {
          return 0;
        }
        if (bucket.key == key && bucket.status == Status::Occupied) {
          bucket.status = Status::Deleted;
          num_elements--;
          return 1;
        }
      }
      break;
    case Status::Occupied:
      if (buckets[idx].key == key) {
        buckets[idx].status = Status::Deleted;
        num_elements--;
        return 1;
      }
      for (size_t i = 1; i < buckets.size(); i++) {
        size_t probe_idx = (idx + i) % buckets.size();
        auto &bucket = buckets[probe_idx];
        if (bucket.status == Status::Empty) {
          return 0;
        }
        if (bucket.key == key && bucket.status == Status::Occupied) {
          bucket.status = Status::Deleted;
          num_elements--;
          return 1;
        }
      }
    }
  }

private:
  size_t num_elements = 0;
  size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
  std::vector<Bucket> buckets;

  void rehash_and_grow() {
    num_elements = 0;
    std::vector<Bucket> buckets_copy;
    buckets_copy.resize(buckets.size() * 2);
    std::swap(buckets, buckets_copy);
    for (auto &i : buckets_copy) {
      insert(i.key, i.val);
    }
  }

  size_t get_hash(const K &key) {
    Hasher hash;
    return hash(key) % buckets.size();
  }
};

#endif // HASHTABLE_H