#include "primer/trie.h"
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  // throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  auto node = root_;
  for (auto c : key) {
    if (node->children_.find(c) == node->children_.end()) {
      return nullptr;
    }
    node = node->children_.at(c);
  }
  auto value_node = dynamic_cast<const TrieNodeWithValue<T> *>(node.get());
  if (value_node == nullptr) {
    return nullptr;
  }
  return value_node->value_.get();
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  std::shared_ptr<T> shared_value = std::make_shared<T>(std::move(value));
  std::vector<std::shared_ptr<const TrieNode>> node_stack;
  std::shared_ptr<const TrieNode> cur_node = root_;
  std::size_t key_size = key.size();

  // 1. store the same node in the stack
  std::size_t idx = 0;
  for (; idx < key_size && cur_node; idx++) {
    char ch = key[idx];
    node_stack.push_back(cur_node);
    cur_node = cur_node->children_.find(ch) == cur_node->children_.end() ? nullptr : cur_node->children_.at(ch);
  }

  // 2. create diff node
  std::shared_ptr<const TrieNodeWithValue<T>> leaf_node =
      cur_node ? std::make_shared<const TrieNodeWithValue<T>>(cur_node->children_, shared_value)
               : std::make_shared<const TrieNodeWithValue<T>>(shared_value);

  std::shared_ptr<const TrieNode> child_node = leaf_node;
  while (idx < key_size) {
    char ch = key[--key_size];
    std::map<char, std::shared_ptr<const TrieNode>> children{{ch, child_node}};
    cur_node = std::make_shared<const TrieNode>(children);
    child_node = cur_node;
  }

  // 3. copy same node
  cur_node = child_node;
  for (int i = static_cast<int>(node_stack.size()) - 1; i >= 0; i--) {
    cur_node = std::shared_ptr<const TrieNode>(node_stack[i]->Clone());
    const_cast<TrieNode *>(cur_node.get())->children_[key[i]] = child_node;
    child_node = cur_node;
  }

  return Trie(cur_node);
}

auto Trie::Remove(std::string_view key) const -> Trie {
  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  std::vector<std::shared_ptr<const TrieNode>> node_stack;
  std::shared_ptr<const TrieNode> cur_node = root_;
  std::size_t key_size = key.size();

  // 1. store the same node in the stack
  std::size_t idx = 0;
  for (; idx < key_size && cur_node; idx++) {
    char ch = key[idx];
    node_stack.push_back(cur_node);
    cur_node = cur_node->children_.find(ch) == cur_node->children_.end() ? nullptr : cur_node->children_.at(ch);
  }
  // not found key
  if (idx != key_size || cur_node == nullptr || !cur_node->is_value_node_) {
    return *this;
  }

  // 2. remove end node
  std::shared_ptr<const TrieNode> end_node =
      cur_node->children_.empty() ? nullptr : std::make_shared<const TrieNode>(cur_node->children_);

  // 3. copy same node
  std::shared_ptr<const TrieNode> child_node = end_node;
  for (int i = static_cast<int>(node_stack.size()) - 1; i >= 0; i--) {
    cur_node = std::shared_ptr<const TrieNode>(node_stack[i]->Clone());
    const_cast<TrieNode *>(cur_node.get())->children_[key[i]] = child_node;
    child_node = cur_node;
  }
  return Trie(cur_node);
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
