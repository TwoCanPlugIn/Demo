// Copyright(C) 2026 by Steven Adler
//
// This file is part of the Demo plugin for OpenCPN.
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
//

//
// Project: Demo Plugin
// Description: Demonstrate OpenCPN Plugin Chart API's - Quick & Dirty Quadtree
// Owner: twocanplugin@hotmail.com
// Date: 25/07/2026
// Version History: 
// 1.0 Initial Release

#include "demo_quadtree.h"

QuadTree::QuadTree(GeoBounds b) : boundary(b) {}

QuadTree::~QuadTree() {}

// Returns false only if the feature's bbox doesn't touch this node's
// boundary at all (e.g. rejected at the root because it's off-chart).
bool QuadTree::Insert(const FeatureReference& ref) {
    if (!boundary.Intersects(ref.boundingBox)) {
        return false;
    }

    if (!divided && items.size() >= CAPACITY) {
        Subdivide();
    }

    // Push the item down into whichever single child fully contains its
    // bbox, so a feature never has to be split or duplicated across
    // quadrants. If it straddles a boundary (fits in none of the four
    // children exactly), it stays in this node. 
    if (divided) {
        if (nw->boundary.ContainsBounds(ref.boundingBox)) {
            return nw->Insert(ref);
        }
        if (ne->boundary.ContainsBounds(ref.boundingBox)) {
            return ne->Insert(ref);
        }
        if (sw->boundary.ContainsBounds(ref.boundingBox)) {
            return sw->Insert(ref);
        }
        if (se->boundary.ContainsBounds(ref.boundingBox)) {
            return se->Insert(ref);
        }
    }

    items.push_back(ref);
    total++;
    return true;
}

void QuadTree::Subdivide() {
    // Only sub divide a node into quadrants when its capacity is exceeded
    double midX = (boundary.min.x + boundary.max.x) / 2.0;
    double midY = (boundary.min.y + boundary.max.y) / 2.0;

    nw = std::make_unique<QuadTree>(GeoBounds{ { boundary.min.x, boundary.min.y }, { midX, midY } });
    ne = std::make_unique<QuadTree>(GeoBounds{ { midX, boundary.min.y }, { boundary.max.x, midY } });
    sw = std::make_unique<QuadTree>(GeoBounds{ { boundary.min.x, midY }, { midX, boundary.max.y } });
    se = std::make_unique<QuadTree>(GeoBounds{ { midX, midY }, { boundary.max.x, boundary.max.y } });

    divided = true;
}

// Collects every feature whose bbox intersects range. This is a bbox
// test, so for line/polygon features it's only a coarse pre-filter 
// Perform exact geometry intersection on the results if precision is required
void QuadTree::Query(const GeoBounds& range, std::vector<FeatureReference>& found) const {
    if (!boundary.Intersects(range)) {
        return;
    }

    for (const auto& ref : items) {
        if (range.Intersects(ref.boundingBox)) {
            found.push_back(ref);
        }
    }

    if (divided) {
        nw->Query(range, found);
        ne->Query(range, found);
        sw->Query(range, found);
        se->Query(range, found);
    }
}

// Some crude stats
int QuadTree::Size() {
    return total;
}

// Debug Information
void QuadTree::Print(int level) const {
    wxLogDebug("Level %d Size %d ", level ,items.size());
    if (divided) {
        nw->Print(level + 1);
        ne->Print(level + 1);
        sw->Print(level + 1);
        se->Print(level + 1);
    }
}
