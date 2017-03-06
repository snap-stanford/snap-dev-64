#include <gtest/gtest.h>

#include <iostream>
#include "Snap.h"

#define EPSILON 0.001
#define DIRNAME "bfsdfs"

class BfsDfsTest { };  // For gtest highlighting

using namespace TSnap;

// Test BFS functions on full graph
template <class PGraph>
void TestFullBfsDfs() {

  const int64 NNodes = 70000;

  PGraph G = GenFull<PGraph>(NNodes);
  PNGraph GOut;
  int64 TreeSz, TreeDepth;

  GOut = GetBfsTree(G, 1, false, false);
  GetSubTreeSz(G, 1, false, false, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == 1);
  EXPECT_TRUE(GOut->GetEdges() == 0);
  EXPECT_TRUE(TreeSz == 1);
  EXPECT_TRUE(TreeDepth == 0);
  std::cerr<<"GetBfsTree(G, 1, false, false) done\n";
  GOut = GetBfsTree(G, NNodes-1, true, true);
  GetSubTreeSz(G, 1, true, true, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);
  std::cerr<<"GetBfsTree(G, NNodes-1, true, true) done\n";

  GOut = GetBfsTree(G, NNodes/2, true, false);
  GetSubTreeSz(G, 1, true, false, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);
  std::cerr<<"GetBfsTree(G, NNodes/2, true, false) done\n";

  GOut = GetBfsTree(G, 1, false, true);
  GetSubTreeSz(G, 1, false, true, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);
  std::cerr<<"GetBfsTree(G, 1, false, true) done\n";
  TInt64V NIdV;
  int64 StartNId, Hop, Nodes;

  StartNId = 1;
  Hop = 1;
  Nodes = GetNodesAtHop(G, StartNId, Hop, NIdV, HasGraphFlag(typename PGraph::TObj, gfDirected));

  TIntPr64V HopCntV;
  Nodes = GetNodesAtHops(G, StartNId, HopCntV, HasGraphFlag(typename PGraph::TObj, gfDirected));

  int64 Length, SrcNId, DstNId;
  SrcNId = 1;
  DstNId = NNodes-1;

  Length = GetShortPath(G, SrcNId, DstNId, HasGraphFlag(typename PGraph::TObj, gfDirected));

  TInt64H NIdToDistH;
  int64 MaxDist = 9;
  Length = GetShortPath(G, SrcNId, NIdToDistH, HasGraphFlag(typename PGraph::TObj, gfDirected), MaxDist);
  EXPECT_TRUE(NIdToDistH[0] == 0);
  EXPECT_TRUE(NIdToDistH[1] == 1);
  EXPECT_TRUE(NIdToDistH[2] == 1);
  EXPECT_TRUE(NIdToDistH[3] == 1);
  EXPECT_TRUE(NIdToDistH[4] == 1);
  std::cerr<<"GetShortPath done\n";

  int64 FullDiam;
  double EffDiam, AvgDiam;
  int64 NTestNodes = 10;

  for (int64 IsDir = 0; IsDir < 2; IsDir++) {

    FullDiam = GetBfsFullDiam(G, NTestNodes, IsDir);
    EXPECT_TRUE(FullDiam == 1);

    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir);
    EXPECT_TRUE(EffDiam > 0.8 - EPSILON && EffDiam < 0.95 + EPSILON);

    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir, EffDiam, FullDiam);
    EXPECT_TRUE(EffDiam > 0.8 - EPSILON && EffDiam < 0.95 + EPSILON);
    EXPECT_TRUE(FullDiam == 1);

    EffDiam = GetBfsEffDiam (G, NTestNodes, IsDir, EffDiam, FullDiam, AvgDiam);
    EXPECT_TRUE(EffDiam > 0.8 - EPSILON && EffDiam < 0.95 + EPSILON);
    EXPECT_TRUE(FullDiam == 1);
    EXPECT_TRUE(AvgDiam > 0.9980 - EPSILON && AvgDiam < 0.9980 + EPSILON);

    TInt64V SubGraphNIdV;
    for (int64 i = 0; i < NTestNodes; i++) {
      SubGraphNIdV.Add(G->GetRndNId());
    }

    EffDiam = GetBfsEffDiam(G, NTestNodes, SubGraphNIdV, IsDir, EffDiam, FullDiam);
    EXPECT_TRUE(EffDiam > 0.8 - EPSILON && EffDiam < 0.95 + EPSILON);
    EXPECT_TRUE(FullDiam == 1);

  }
  std::cerr<<"GetBfsFullDiam done\n";


}


// Test BFS functions on full graphs of each type
TEST(BfsDfsTest, CompleteGraph) {

  mkdir(DIRNAME, S_IRWXU | S_IRWXG | S_IRWXO);
  std::cerr<<"PUNGraph\n";
  TestFullBfsDfs<PUNGraph>();
  std::cerr<<"PNGraph\n";

  TestFullBfsDfs<PNGraph>();
  std::cerr<<"PNEGraph\n";

  TestFullBfsDfs<PNEGraph>();

}
