#include "union_find.hpp"

#include <utility>

UnionFind::UnionFind(int n): n_(n), parent_(n), size_(n) {
    for (int i = 0; i < n_; i++) {
        parent_[i] = i;
        size_[i] = 1;
    }
}

int UnionFind::find(int u) {
    return parent_[u] == u ? u : parent_[u] = find(parent_[u]);
}

bool UnionFind::unite(int u, int v) {
    u = find(u);
    v = find(v);
    if (u == v) {
        return false;
    }
    if (size_[u] < size_[v]) {
        std::swap(u, v);
    }
    parent_[v] = u;
    size_[u] += size_[v];
    return true;
}

bool UnionFind::same(int u, int v) {
    return find(u) == find(v);
}
