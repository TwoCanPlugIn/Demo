// Copyright(C) 2026 by Steven Adler
//
// This file is part of Demo plugin for OpenCPN.
//
// Demo plugin for OpenCPN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Demo plugin for OpenCPN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Demo plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.

#ifndef DEMO_QUADTREE_H
#define DEMO_QUADTREE_H

#include "demo_chartfeature.h"

#include <vector>
#include <memory>
#include <algorithm>

#include <wx/log.h>

constexpr int CAPACITY = 1024;


// Lightweight handle stored in the tree: the feature's bounding box plus
// its index into whatever std::vector<GeoFeature> owns the real data.
struct FeatureReference {
    GeoBounds boundingBox;
    size_t index = 0;
};

// Compute the bounding box of a feature's geometry from its raw parts
// (works for Point/LineString/MultiLineString/Polygon uniformly, since
// they're all just vector<vector<GeoPoint>> under the hood).
GeoBounds ComputeBounds(const std::vector<std::vector<GeoPoint>>& parts);

class QuadTree {

public:
    QuadTree(GeoBounds b);
    ~QuadTree();
    bool Insert(const FeatureReference& ref);
    void Query(const GeoBounds& range, std::vector<FeatureReference>& found) const;
    void Print(int level = 0) const;
    int Size();
protected:

private:
    std::vector<FeatureReference> items;
    std::unique_ptr<QuadTree> nw;
    std::unique_ptr<QuadTree> ne;
    std::unique_ptr<QuadTree> sw;
    std::unique_ptr<QuadTree> se;
    GeoBounds boundary;
    bool divided = false;
    void Subdivide();
    int total = 0;
};

#endif
