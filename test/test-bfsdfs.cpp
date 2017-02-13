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
  
  const int64 NNodes = 500;
  
  PGraph G = GenFull<PGraph>(NNodes);
  PNGraph GOut;
  int64 TreeSz, TreeDepth;
  
  GOut = GetBfsTree(G, 1, false, false);
  GetSubTreeSz(G, 1, false, false, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == 1);
  EXPECT_TRUE(GOut->GetEdges() == 0);
  EXPECT_TRUE(TreeSz == 1);
  EXPECT_TRUE(TreeDepth == 0);
  
  GOut = GetBfsTree(G, NNodes-1, true, true);
  GetSubTreeSz(G, 1, true, true, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);

  GOut = GetBfsTree(G, NNodes/2, true, false);
  GetSubTreeSz(G, 1, true, false, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);
  
  GOut = GetBfsTree(G, 1, false, true);
  GetSubTreeSz(G, 1, false, true, TreeSz, TreeDepth);
  EXPECT_TRUE(GOut->GetNodes() == NNodes);
  EXPECT_TRUE(GOut->GetEdges() == NNodes-1);
  EXPECT_TRUE(TreeSz == NNodes);
  EXPECT_TRUE(TreeDepth == 1);
  
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
  
}

// Test BFS functions on undirected graph that is not fully connected
TEST(BfsDfsTest, UndirectedRandom) {
  
  PUNGraph G;

  TStr FName = TStr::Fmt("%s/sample_bfsdfs_unpower.txt", DIRNAME);
  
  // For a benchmark, this is done once and compared visually to confirm values are correct
//  const int64 NNodes = 50;
//  G = GenRndPowerLaw(NNodes, 2.5);
//  SaveEdgeList(G, FName);
  
  G = LoadEdgeList<PUNGraph>(FName);
  TIntStr64H NodeLabelH;
  for (int64 i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlNeato, TStr::Fmt("%s/sample_bfsdfs_unpower.png", DIRNAME), "Sample bfsdfs Graph", NodeLabelH);
  
  TInt64V NIdV;
  int64 StartNId, Hop, Nodes;
  int64 IsDir = 0;
    
  StartNId = 1;
  Hop = 1;
  Nodes = GetNodesAtHop(G, StartNId, Hop, NIdV, IsDir);
    EXPECT_TRUE(Nodes == 1);
  EXPECT_TRUE(NIdV.Len() == 1);
  EXPECT_TRUE(NIdV[0] = 46);
  
  TIntPr64V HopCntV;
  Nodes = GetNodesAtHops(G, StartNId, HopCntV, IsDir);
    EXPECT_TRUE(Nodes == 8);
  EXPECT_TRUE(HopCntV.Len() == 8);
  
  int64 Length, SrcNId, DstNId;
  SrcNId = 1;
  DstNId = G->GetNodes() - 1;
  
  Length = GetShortPath(G, SrcNId, DstNId, IsDir);
  EXPECT_TRUE(Length == -1);
  
  SrcNId = 1;
  DstNId = 33;
  Length = GetShortPath(G, SrcNId, DstNId, IsDir);
  EXPECT_TRUE(Length == 7);

  TInt64H NIdToDistH;
  int64 MaxDist = 9;
  Length = GetShortPath(G, SrcNId, NIdToDistH, IsDir, MaxDist);
  EXPECT_TRUE(NIdToDistH[0] == 0);
  EXPECT_TRUE(NIdToDistH[1] == 1);
  EXPECT_TRUE(NIdToDistH[2] == 2);
  EXPECT_TRUE(NIdToDistH[3] == 3);
  EXPECT_TRUE(NIdToDistH[4] == 3);

  TInt::Rnd.PutSeed(0);

  int64 FullDiam;
  double EffDiam, AvgSPL;
  int64 NTestNodes = G->GetNodes() / 3 * 2;
  
  FullDiam = GetBfsFullDiam(G, NTestNodes, IsDir);
  EXPECT_TRUE(FullDiam <= 8 && FullDiam >= 5);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir);
  EXPECT_TRUE(EffDiam > 3.5 && EffDiam < 4.5);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir, EffDiam, FullDiam);
  EXPECT_TRUE(EffDiam > 3.5 && EffDiam < 4.5);
  EXPECT_TRUE(FullDiam > 5 && FullDiam < 8);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir, EffDiam, FullDiam, AvgSPL);
  EXPECT_TRUE(EffDiam > 3.5 && EffDiam < 4.5);
  EXPECT_TRUE(FullDiam > 5 && FullDiam < 8);
  EXPECT_TRUE(AvgSPL > 1.5 && AvgSPL < 5.0);

  TInt64V SubGraphNIdV;
  SubGraphNIdV.Add(0);
  SubGraphNIdV.Add(4);
  SubGraphNIdV.Add(31);
  SubGraphNIdV.Add(45);
  SubGraphNIdV.Add(18);
  SubGraphNIdV.Add(11);
  SubGraphNIdV.Add(11);
  SubGraphNIdV.Add(48);
  SubGraphNIdV.Add(34);
  SubGraphNIdV.Add(30);

  EffDiam = GetBfsEffDiam(G, NTestNodes, SubGraphNIdV, IsDir, EffDiam, FullDiam);
  EXPECT_TRUE(EffDiam > 2.7385 - EPSILON && EffDiam < 2.7385 + EPSILON);
  EXPECT_TRUE(FullDiam == 3);

}

// Test BFS functions on directed graph that is not fully connected
TEST(BfsDfsTest, DirectedRandom) {
  
  PNGraph G = TNGraph::New();
  
  TStr FName = TStr::Fmt("%s/sample_bfsdfs_ngraph.txt", DIRNAME);

// Create benchmark graph, initially visually to confirm values are correct
//  const int NNodes = 30;
//  G = GenRndGnm<PNGraph>(NNodes, NNodes*2);  
//  // Add some more random edges
//  for (int i = 0; i < 10; i++) {
//    TInt Src, Dst;
//    do {
//      Src = G->GetRndNId();
//      Dst = G->GetRndNId();
//    }
//    while (Src == Dst || G->IsEdge(Src, Dst));
//    printf("Adding edge %d, %d\n", Src.Val, Dst.Val);
//    G->AddEdge(Src, Dst);
//  }
//  // Add isolated component
//  G->AddNode(NNodes);
//  G->AddNode(NNodes+1);
//  G->AddNode(NNodes+2);
//  G->AddEdge(NNodes, NNodes+1);
//  G->AddEdge(NNodes+1, NNodes+2);
//  G->AddEdge(NNodes+2, NNodes+1);
//  printf("G->GetNodes() = %d, G->GetEdges() = %d\n", G->GetNodes(), G->GetEdges());
//  SaveEdgeList(G, FName);
  
  G = LoadEdgeList<PNGraph>(FName);
  TIntStr64H NodeLabelH;
  for (int64 i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlDot, TStr::Fmt("%s/sample_bfsdfs_ngraph.png", DIRNAME), "Sample BFS Graph", NodeLabelH);
  
  TInt64V NIdV;
  int64 StartNId, Hop, Nodes;
  
  //  for (int64 IsDir = 0; IsDir < 2; IsDir++) {
  int64 IsDir = 1;
  StartNId = 11;
  Hop = 1;
  Nodes = GetNodesAtHop(G, StartNId, Hop, NIdV, IsDir);
  EXPECT_TRUE(Nodes == 2);
  EXPECT_TRUE(NIdV.Len() == 2);
  EXPECT_TRUE(NIdV[0] = 20);
  EXPECT_TRUE(NIdV[1] = 25);
  
  TIntPr64V HopCntV;
  Nodes = GetNodesAtHops(G, StartNId, HopCntV, IsDir);
  EXPECT_TRUE(Nodes == 10);
  EXPECT_TRUE(HopCntV.Len() == 10);
  EXPECT_TRUE(HopCntV[0] == TInt64Pr(0, 1));
  EXPECT_TRUE(HopCntV[1] == TInt64Pr(1, 2));
  EXPECT_TRUE(HopCntV[2] == TInt64Pr(2, 3));
  EXPECT_TRUE(HopCntV[3] == TInt64Pr(3, 4));
  EXPECT_TRUE(HopCntV[4] == TInt64Pr(4, 5));
  EXPECT_TRUE(HopCntV[5] == TInt64Pr(5, 4));
  EXPECT_TRUE(HopCntV[6] == TInt64Pr(6, 1));
  EXPECT_TRUE(HopCntV[7] == TInt64Pr(7, 1));
  EXPECT_TRUE(HopCntV[8] == TInt64Pr(8, 3));
  EXPECT_TRUE(HopCntV[9] == TInt64Pr(9, 1));
  
  int64 Length, SrcNId, DstNId;
  SrcNId = 11;
  DstNId = G->GetNodes() - 1;
  
  Length = GetShortPath(G, SrcNId, DstNId, IsDir);
  EXPECT_TRUE(Length == 2);
  
  SrcNId = 11;
  DstNId = 27;
  Length = GetShortPath(G, SrcNId, DstNId, IsDir);
  EXPECT_TRUE(Length == 9);
  
  TInt64H NIdToDistH;
  int64 MaxDist = 9;
  Length = GetShortPath(G, SrcNId, NIdToDistH, IsDir, MaxDist);
  EXPECT_TRUE(NIdToDistH[0] == 0);
  EXPECT_TRUE(NIdToDistH[1] == 1);
  EXPECT_TRUE(NIdToDistH[2] == 1);
  EXPECT_TRUE(NIdToDistH[3] == 2);
  EXPECT_TRUE(NIdToDistH[4] == 2);
  
  TInt::Rnd.PutSeed(0);
  
  int64 FullDiam;
  double EffDiam, AvgSPL;
  int64 NTestNodes = G->GetNodes() / 2;
  
  FullDiam = GetBfsFullDiam(G, NTestNodes, IsDir);
  EXPECT_TRUE(FullDiam >= 7 && FullDiam <= 9);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir);
  EXPECT_TRUE(EffDiam > 4.0 && EffDiam < 6.0);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir, EffDiam, FullDiam);
  EXPECT_TRUE(EffDiam > 4.0 && EffDiam < 6.0);
  EXPECT_TRUE(FullDiam >= 7 && FullDiam <= 9);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, IsDir, EffDiam, FullDiam, AvgSPL);
  EXPECT_TRUE(EffDiam > 4.0 && EffDiam < 6.0);
  EXPECT_TRUE(FullDiam >= 7 && FullDiam <= 9);
  EXPECT_TRUE(AvgSPL > 2.75 && AvgSPL < 4.0);
  
  TInt64V SubGraphNIdV;
  SubGraphNIdV.Add(8);
  SubGraphNIdV.Add(29);
  SubGraphNIdV.Add(16);
  SubGraphNIdV.Add(0);
  SubGraphNIdV.Add(19);
  SubGraphNIdV.Add(17);
  SubGraphNIdV.Add(26);
  SubGraphNIdV.Add(14);
  SubGraphNIdV.Add(10);
  SubGraphNIdV.Add(24);
  SubGraphNIdV.Add(27);
  SubGraphNIdV.Add(2);
  SubGraphNIdV.Add(18);
  
  EffDiam = GetBfsEffDiam(G, NTestNodes, SubGraphNIdV, IsDir, EffDiam, FullDiam);
  EXPECT_TRUE(EffDiam > 4.0 && EffDiam < 6.0);
  EXPECT_TRUE(FullDiam == 9);
  
}

// Test BFS functions on full graphs of each type
TEST(BfsDfsTest, CompleteGraph) {
  
  mkdir(DIRNAME, S_IRWXU | S_IRWXG | S_IRWXO);
  
  TestFullBfsDfs<PUNGraph>();

  TestFullBfsDfs<PNGraph>();

  TestFullBfsDfs<PNEGraph>();
  
}
