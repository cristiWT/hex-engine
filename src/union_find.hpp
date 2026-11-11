#pragma once

#include <vector>

class UnionFind {
public:
    explicit UnionFind(int n);

    int find(int u);
    bool unite(int u, int v);
    bool same(int u, int v);
private:
    int n_;
    std::vector<int> parent_;
    std::vector<int> size_;
};