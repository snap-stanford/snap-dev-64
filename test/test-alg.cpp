#include <gtest/gtest.h>
#include "Snap.h"

void CheckVectors(const TIntPr64V& Expected, const TIntPr64V& Actual);
void CheckVectors(const TFltPr64V& Expected, const TFltPr64V& Actual);
void CheckVectors(const TInt64Set& Expected, const TInt64Set& Actual);
void CheckVectors(const TInt64V& Expected, const TInt64V& Actual);
void CheckGraphs(const TIntPr64V& Expected, const PUNGraph& Actual);
void CheckGraphs(const TIntPr64V& Expected, const PNGraph& Actual);
void CheckGraphs(const TIntPr64V& Expected, const PNEGraph& Actual);
void CheckNotInSet(int64 Key, const TInt64Set& Keys);
void AddValues(TInt64V& Vector, int64 NumValues, int64 Value);
void AddConsecutiveValues(TInt64V& Vector, int64 Start, int64 End);
void GetKeys(TInt64V& Keys, const TIntPr64V& Map);

/////////////////////////////////////////////////
// Test Algorithms
/////////////////////////////////////////////////

class GraphTest : public ::testing::Test {
 protected:

  PUNGraph TUNLoopGraph;
  PNGraph TNLoopGraph;
  PNEGraph TNELoopGraph;
  PUNGraph TUNReverseTree;
  PNGraph TNReverseTree;
  PNEGraph TNEReverseTree;
  PUNGraph TUNComplicatedGraph;
  PNGraph TNComplicatedGraph;
  PNEGraph TNEComplicatedGraph;
  PUNGraph TUNComplicatedGraphWithLoop;
  PNGraph TNComplicatedGraphWithLoop;
  PNEGraph TNEComplicatedGraphWithLoop;

  virtual void SetUp() {
    TUNLoopGraph = GenerateLoopGraph(TUNGraph::New());
    TNLoopGraph = GenerateLoopGraph(TNGraph::New());
    TNELoopGraph = GenerateLoopGraph(TNEGraph::New());
    TUNReverseTree = GenerateReverseTree(TUNGraph::New());
    TNReverseTree = GenerateReverseTree(TNGraph::New());
    TNEReverseTree = GenerateReverseTree(TNEGraph::New());
    TUNComplicatedGraph = GenerateComplicatedGraph(TUNGraph::New());
    TNComplicatedGraph = GenerateComplicatedGraph(TNGraph::New());
    TNEComplicatedGraph = GenerateComplicatedGraph(TNEGraph::New());
    TUNComplicatedGraphWithLoop =
        GenerateComplicatedGraphWithLoop(TUNGraph::New());
    TNComplicatedGraphWithLoop =
        GenerateComplicatedGraphWithLoop(TNGraph::New());
    TNEComplicatedGraphWithLoop =
        GenerateComplicatedGraphWithLoop(TNEGraph::New());
  }

  template<class PGraph>
  void GenerateNodes(const PGraph& Graph, int64 NumNodes = 5) {
    for (int64 i = 0; i < NumNodes; i++) {
      Graph->AddNode(i);
    }
  }

  // Returns the following graph:
  // 0 -> 1 -> 2 -> 3 -> 4
  //  \-----------------/ (two edges from 4 to 0 and one edge from 0 to 4)
  template<class PGraph>
  PGraph GenerateLoopGraph(const PGraph& Graph) {
    GenerateNodes(Graph);
    Graph->AddEdge(0, 1);
    Graph->AddEdge(1, 2);
    Graph->AddEdge(2, 3);
    Graph->AddEdge(3, 4);
    Graph->AddEdge(4, 0);
    Graph->AddEdge(4, 0);
    Graph->AddEdge(0, 4);
    return Graph;
  }

  // Returns the following graph:
  // 0 -> 1 -> 3
  //   \> 2 -> 4
  template<class PGraph>
  PGraph GenerateReverseTree(const PGraph& Graph) {
    GenerateNodes(Graph);
    Graph->AddEdge(0, 1);
    Graph->AddEdge(0, 2);
    Graph->AddEdge(1, 3);
    Graph->AddEdge(2, 4);
    return Graph;
  }

  // Returns a complicated graph with
  // no node 3 and two edges from node 1 to node 4
  template<class PGraph>
  PGraph GenerateComplicatedGraph(const PGraph& Graph) {
    GenerateNodes(Graph);
    Graph->AddEdge(0, 1);
    Graph->AddEdge(0, 2);
    Graph->AddEdge(1, 2);
    Graph->AddEdge(2, 1);
    Graph->AddEdge(2, 4);
    Graph->AddEdge(1, 4);
    Graph->AddEdge(1, 4);
    return Graph;
  }

  // Returns a complicated graph with
  // no node 3, a self-loop on node 0, and two edges from node 1 to node 4
  template<class PGraph>
  PGraph GenerateComplicatedGraphWithLoop(const PGraph& Graph) {
    GenerateComplicatedGraph(Graph);
    Graph->AddEdge(0, 0);
    return Graph;
  }
};

class TreeTest : public ::testing::Test {
 protected:

  PNGraph SingleNode;
  PNGraph Tree;
  PNGraph Forest;
  PNGraph Circle;

  virtual void SetUp() {
    SingleNode = GetSingleNode();
    Tree = GetTree();
    Forest = GetForest();
    Circle = GetCircle();
  }

  // Returns a graph with a single node
  PNGraph GetSingleNode() {
    PNGraph G = TNGraph::New();
    G->AddNode(1);
    return G;
  }


  // Returns a binary tree of 14 nodes
  PNGraph GetTree() {
    PNGraph G = TNGraph::New();
    for (int i = 1; i < 15; i++) {
      G->AddNode(i);
    }
    for (int i = 2; i < 15; i++) {
      G->AddEdge(i, i/2);
    }
    return G;
  }

  // Returns a binary tree minus one edge
  PNGraph GetForest() {
    PNGraph G = GetTree();
    G->DelEdge(4,2);
    return G;
  }

  // Returns a binary tree plus one edge
  PNGraph GetCircle() {
    PNGraph G = GetTree();
    G->AddEdge(7,2);
    return G;
  }
};

/////
// Node Degrees
////

// Testing CntInDegNodes
TEST_F(GraphTest, CntInDegNodesTest) {

  // LoopGraph
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNLoopGraph, 0));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNLoopGraph, 1));
  EXPECT_EQ(5, TSnap::CntInDegNodes(TUNLoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNLoopGraph, 4));

  EXPECT_EQ(0, TSnap::CntInDegNodes(TNLoopGraph, 0));
  EXPECT_EQ(4, TSnap::CntInDegNodes(TNLoopGraph, 1));
  EXPECT_EQ(1, TSnap::CntInDegNodes(TNLoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNLoopGraph, 4));

  EXPECT_EQ(0, TSnap::CntInDegNodes(TNELoopGraph, 0));
  EXPECT_EQ(3, TSnap::CntInDegNodes(TNELoopGraph, 1));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TNELoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNELoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNELoopGraph, 4));

  // ReverseTree

  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNReverseTree, 0));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TUNReverseTree, 1));
  EXPECT_EQ(3, TSnap::CntInDegNodes(TUNReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNReverseTree, 4));

  EXPECT_EQ(1, TSnap::CntInDegNodes(TNReverseTree, 0));
  EXPECT_EQ(4, TSnap::CntInDegNodes(TNReverseTree, 1));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNReverseTree, 4));

  EXPECT_EQ(1, TSnap::CntInDegNodes(TNEReverseTree, 0));
  EXPECT_EQ(4, TSnap::CntInDegNodes(TNEReverseTree, 1));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEReverseTree, 4));

  // ComplicatedGraph

  EXPECT_EQ(1, TSnap::CntInDegNodes(TUNComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNComplicatedGraph, 1));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TUNComplicatedGraph, 2));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TUNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TUNComplicatedGraph, 4));

  EXPECT_EQ(2, TSnap::CntInDegNodes(TNComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNComplicatedGraph, 1));
  EXPECT_EQ(3, TSnap::CntInDegNodes(TNComplicatedGraph, 2));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNComplicatedGraph, 4));

  EXPECT_EQ(2, TSnap::CntInDegNodes(TNEComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEComplicatedGraph, 1));
  EXPECT_EQ(2, TSnap::CntInDegNodes(TNEComplicatedGraph, 2));
  EXPECT_EQ(1, TSnap::CntInDegNodes(TNEComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntInDegNodes(TNEComplicatedGraph, 4));
}

// Testing CntOutDegNodes
TEST_F(GraphTest, CntOutDegNodesTest) {

  // LoopGraph

  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNLoopGraph, 0));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNLoopGraph, 1));
  EXPECT_EQ(5, TSnap::CntOutDegNodes(TUNLoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNLoopGraph, 4));

  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNLoopGraph, 0));
  EXPECT_EQ(4, TSnap::CntOutDegNodes(TNLoopGraph, 1));
  EXPECT_EQ(1, TSnap::CntOutDegNodes(TNLoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNLoopGraph, 4));

  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNELoopGraph, 0));
  EXPECT_EQ(3, TSnap::CntOutDegNodes(TNELoopGraph, 1));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNELoopGraph, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNELoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNELoopGraph, 4));

  // ReverseTree

  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNReverseTree, 0));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TUNReverseTree, 1));
  EXPECT_EQ(3, TSnap::CntOutDegNodes(TUNReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNReverseTree, 4));

  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNReverseTree, 0));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNReverseTree, 1));
  EXPECT_EQ(1, TSnap::CntOutDegNodes(TNReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNReverseTree, 4));

  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNEReverseTree, 0));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNEReverseTree, 1));
  EXPECT_EQ(1, TSnap::CntOutDegNodes(TNEReverseTree, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNEReverseTree, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNEReverseTree, 4));

  // ComplicatedGraph

  EXPECT_EQ(1, TSnap::CntOutDegNodes(TUNComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNComplicatedGraph, 1));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TUNComplicatedGraph, 2));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TUNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TUNComplicatedGraph, 4));

  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNComplicatedGraph, 1));
  EXPECT_EQ(3, TSnap::CntOutDegNodes(TNComplicatedGraph, 2));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNComplicatedGraph, 4));

  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNEComplicatedGraph, 0));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNEComplicatedGraph, 1));
  EXPECT_EQ(2, TSnap::CntOutDegNodes(TNEComplicatedGraph, 2));
  EXPECT_EQ(1, TSnap::CntOutDegNodes(TNEComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntOutDegNodes(TNEComplicatedGraph, 4));
}

// Testing CntDegNodes
TEST_F(GraphTest, CntDegNodesTest) {

  // LoopGraph

  EXPECT_EQ(5, TSnap::CntDegNodes(TUNLoopGraph, 2));
  EXPECT_EQ(3, TSnap::CntDegNodes(TNLoopGraph, 2));
  EXPECT_EQ(3, TSnap::CntDegNodes(TNELoopGraph, 2));

  EXPECT_EQ(0, TSnap::CntDegNodes(TUNLoopGraph, 3));
  EXPECT_EQ(2, TSnap::CntDegNodes(TNLoopGraph, 3));
  EXPECT_EQ(0, TSnap::CntDegNodes(TNELoopGraph, 3));

  // ReverseTree

  EXPECT_EQ(3, TSnap::CntDegNodes(TUNReverseTree, 2));
  EXPECT_EQ(3, TSnap::CntDegNodes(TNReverseTree, 2));
  EXPECT_EQ(3, TSnap::CntDegNodes(TNEReverseTree, 2));

  EXPECT_EQ(2, TSnap::CntDegNodes(TUNReverseTree, 1));
  EXPECT_EQ(2, TSnap::CntDegNodes(TNReverseTree, 1));
  EXPECT_EQ(2, TSnap::CntDegNodes(TNEReverseTree, 1));

  // ComplicatedGraph

  EXPECT_EQ(2, TSnap::CntDegNodes(TUNComplicatedGraph, 3));
  EXPECT_EQ(0, TSnap::CntDegNodes(TNComplicatedGraph, 3));
  EXPECT_EQ(1, TSnap::CntDegNodes(TNEComplicatedGraph, 3));

  EXPECT_EQ(0, TSnap::CntDegNodes(TUNComplicatedGraph, 4));
  EXPECT_EQ(2, TSnap::CntDegNodes(TNComplicatedGraph, 4));
  EXPECT_EQ(1, TSnap::CntDegNodes(TNEComplicatedGraph, 4));
}

// Testing CntNonZNodes
TEST_F(GraphTest, CntNonZNodesTest) {
  EXPECT_EQ(5, TSnap::CntNonZNodes(TUNLoopGraph));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNLoopGraph));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNELoopGraph));

  EXPECT_EQ(5, TSnap::CntNonZNodes(TUNReverseTree));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNReverseTree));
  EXPECT_EQ(5, TSnap::CntNonZNodes(TNEReverseTree));

  EXPECT_EQ(4, TSnap::CntNonZNodes(TUNComplicatedGraph));
  EXPECT_EQ(4, TSnap::CntNonZNodes(TNComplicatedGraph));
  EXPECT_EQ(4, TSnap::CntNonZNodes(TNEComplicatedGraph));
}

// Testing CntEdgesToSet
TEST_F(GraphTest, CntEdgesToSetTest) {
  TInt64Set NodeKeysV;
  NodeKeysV.AddKey(0);
  NodeKeysV.AddKey(1);
  NodeKeysV.AddKey(2);
  NodeKeysV.AddKey(3);
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TUNLoopGraph, 4, NodeKeysV));
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TNLoopGraph, 4, NodeKeysV));
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TNELoopGraph, 4, NodeKeysV));

  EXPECT_EQ(1, TSnap::CntEdgesToSet(TUNReverseTree, 4, NodeKeysV));
  EXPECT_EQ(1, TSnap::CntEdgesToSet(TNReverseTree, 4, NodeKeysV));
  EXPECT_EQ(1, TSnap::CntEdgesToSet(TNEReverseTree, 4, NodeKeysV));

  EXPECT_EQ(2, TSnap::CntEdgesToSet(TUNComplicatedGraph, 4, NodeKeysV));
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TNComplicatedGraph, 4, NodeKeysV));
  EXPECT_EQ(2, TSnap::CntEdgesToSet(TNEComplicatedGraph, 4, NodeKeysV));
}

// Testing GetMxDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxDegNIdTest) {
  TInt64Set IncorrectKeysLoopGraph;
  TInt64Set IncorrectKeysReverseTree;
  TInt64Set IncorrectKeysComplicatedGraph;

  // LoopGraph

  CheckNotInSet(TSnap::GetMxDegNId(TUNLoopGraph), IncorrectKeysLoopGraph);

  IncorrectKeysLoopGraph.AddKey(1);
  IncorrectKeysLoopGraph.AddKey(2);
  IncorrectKeysLoopGraph.AddKey(3);

  CheckNotInSet(TSnap::GetMxDegNId(TNLoopGraph), IncorrectKeysLoopGraph);
  CheckNotInSet(TSnap::GetMxDegNId(TNELoopGraph), IncorrectKeysLoopGraph);

  // ReverseTree

  IncorrectKeysReverseTree.AddKey(3);
  IncorrectKeysReverseTree.AddKey(4);

  CheckNotInSet(TSnap::GetMxDegNId(TUNReverseTree), IncorrectKeysReverseTree);
  CheckNotInSet(TSnap::GetMxDegNId(TNReverseTree), IncorrectKeysReverseTree);
  CheckNotInSet(TSnap::GetMxDegNId(TNEReverseTree), IncorrectKeysReverseTree);

  // ComplicatedGraph

  IncorrectKeysComplicatedGraph.AddKey(3);
  IncorrectKeysComplicatedGraph.AddKey(4);

  CheckNotInSet(TSnap::GetMxDegNId(TUNComplicatedGraph),
                IncorrectKeysComplicatedGraph);
  CheckNotInSet(TSnap::GetMxDegNId(TNComplicatedGraph),
                IncorrectKeysComplicatedGraph);
  EXPECT_EQ(1, TSnap::GetMxDegNId(TNEComplicatedGraph));
}

// Testing GetMxInDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxInDegNIdTest) {
  TInt64Set IncorrectKeysLoopGraph;
  TInt64Set TUNIncorrectKeysReverseTree;
  TInt64Set TNIncorrectKeysReverseTree;
  TInt64Set IncorrectKeysComplicatedGraph;

  // OP RS 2014/06/12 commented out this test, needs more investigation
  return;

  // LoopGraph

  CheckNotInSet(TSnap::GetMxInDegNId(TUNLoopGraph), IncorrectKeysLoopGraph);
  EXPECT_EQ(4, TSnap::GetMxInDegNId(TNLoopGraph));

  IncorrectKeysLoopGraph.AddKey(1);
  IncorrectKeysLoopGraph.AddKey(2);
  IncorrectKeysLoopGraph.AddKey(3);

  CheckNotInSet(TSnap::GetMxInDegNId(TNELoopGraph), IncorrectKeysLoopGraph);

  // ReverseTree

  TUNIncorrectKeysReverseTree.AddKey(3);
  TUNIncorrectKeysReverseTree.AddKey(4);
  TNIncorrectKeysReverseTree.AddKey(0);

  CheckNotInSet(TSnap::GetMxInDegNId(TUNReverseTree),
                TUNIncorrectKeysReverseTree);
  CheckNotInSet(TSnap::GetMxInDegNId(TNReverseTree),
                TNIncorrectKeysReverseTree);
  CheckNotInSet(TSnap::GetMxInDegNId(TNEReverseTree),
                TNIncorrectKeysReverseTree);

  // ComplicatedGraph

  IncorrectKeysComplicatedGraph.AddKey(3);
  IncorrectKeysComplicatedGraph.AddKey(4);

  CheckNotInSet(TSnap::GetMxInDegNId(TUNComplicatedGraph),
                IncorrectKeysComplicatedGraph);

  IncorrectKeysComplicatedGraph.AddKey(0);

  CheckNotInSet(TSnap::GetMxInDegNId(TNComplicatedGraph),
                IncorrectKeysComplicatedGraph);
  EXPECT_EQ(4, TSnap::GetMxInDegNId(TNEComplicatedGraph));
}

// Testing GetMxOutDegNId (check node IDs that do not have max degree)
TEST_F(GraphTest, GetMxOutDegNIdTest) {
  TInt64Set IncorrectKeysLoopGraph;
  TInt64Set IncorrectKeysReverseTree;
  TInt64Set IncorrectKeysComplicatedGraph;

  // LoopGraph

  CheckNotInSet(TSnap::GetMxOutDegNId(TUNLoopGraph), IncorrectKeysLoopGraph);
  EXPECT_EQ(0, TSnap::GetMxOutDegNId(TNLoopGraph));

  IncorrectKeysLoopGraph.AddKey(1);
  IncorrectKeysLoopGraph.AddKey(2);
  IncorrectKeysLoopGraph.AddKey(3);

  CheckNotInSet(TSnap::GetMxOutDegNId(TNELoopGraph), IncorrectKeysLoopGraph);

  // ReverseTree

  IncorrectKeysReverseTree.AddKey(3);
  IncorrectKeysReverseTree.AddKey(4);

  CheckNotInSet(TSnap::GetMxOutDegNId(TUNReverseTree),
                IncorrectKeysReverseTree);
  EXPECT_EQ(0, TSnap::GetMxOutDegNId(TNReverseTree));
  EXPECT_EQ(0, TSnap::GetMxOutDegNId(TNEReverseTree));

  // ComplicatedGraph

  IncorrectKeysComplicatedGraph.AddKey(3);
  IncorrectKeysComplicatedGraph.AddKey(4);

  CheckNotInSet(TSnap::GetMxOutDegNId(TUNComplicatedGraph),
                IncorrectKeysComplicatedGraph);
  // OP RS 2014/06/12, commented out, since several nodes can be returned
  //EXPECT_EQ(0, TSnap::GetMxOutDegNId(TNComplicatedGraph));

  IncorrectKeysComplicatedGraph.AddKey(2);

  CheckNotInSet(TSnap::GetMxOutDegNId(TNEComplicatedGraph),
                IncorrectKeysComplicatedGraph);
}

// Testing GetInDegCnt for TIntPrV
TEST_F(GraphTest, GetInDegCntIntTest) {
  TIntPr64V TUNExpectedLoopGraph;
  TIntPr64V TUNInDegCntLoopGraph;
  TIntPr64V TNExpectedLoopGraph;
  TIntPr64V TNInDegCntLoopGraph;
  TIntPr64V TNEExpectedLoopGraph;
  TIntPr64V TNEInDegCntLoopGraph;

  TIntPr64V TUNExpectedReverseTree;
  TIntPr64V TUNInDegCntReverseTree;
  TIntPr64V TNExpectedReverseTree;
  TIntPr64V TNInDegCntReverseTree;
  TIntPr64V TNEExpectedReverseTree;
  TIntPr64V TNEInDegCntReverseTree;

  TIntPr64V TUNExpectedComplicatedGraph;
  TIntPr64V TUNInDegCntComplicatedGraph;
  TIntPr64V TNExpectedComplicatedGraph;
  TIntPr64V TNInDegCntComplicatedGraph;
  TIntPr64V TNEExpectedComplicatedGraph;
  TIntPr64V TNEInDegCntComplicatedGraph;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TInt64Pr(2, 5));
  TSnap::GetInDegCnt(TUNLoopGraph, TUNInDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNInDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TInt64Pr(1, 4));
  TNExpectedLoopGraph.Add(TInt64Pr(2, 1));
  TSnap::GetInDegCnt(TNLoopGraph, TNInDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNInDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TInt64Pr(1, 3));
  TNEExpectedLoopGraph.Add(TInt64Pr(2, 2));
  TSnap::GetInDegCnt(TNELoopGraph, TNEInDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEInDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TInt64Pr(1, 2));
  TUNExpectedReverseTree.Add(TInt64Pr(2, 3));
  TSnap::GetInDegCnt(TUNReverseTree, TUNInDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNInDegCntReverseTree);

  TNExpectedReverseTree.Add(TInt64Pr(0, 1));
  TNExpectedReverseTree.Add(TInt64Pr(1, 4));
  TSnap::GetInDegCnt(TNReverseTree, TNInDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNInDegCntReverseTree);

  TNEExpectedReverseTree.Add(TInt64Pr(0, 1));
  TNEExpectedReverseTree.Add(TInt64Pr(1, 4));
  TSnap::GetInDegCnt(TNEReverseTree, TNEInDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEInDegCntReverseTree);

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TInt64Pr(0, 1));
  TUNExpectedComplicatedGraph.Add(TInt64Pr(2, 2));
  TUNExpectedComplicatedGraph.Add(TInt64Pr(3, 2));
  TSnap::GetInDegCnt(TUNComplicatedGraph, TUNInDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNInDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TInt64Pr(0, 2));
  TNExpectedComplicatedGraph.Add(TInt64Pr(2, 3));
  TSnap::GetInDegCnt(TNComplicatedGraph, TNInDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNInDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TInt64Pr(0, 2));
  TNEExpectedComplicatedGraph.Add(TInt64Pr(2, 2));
  TNEExpectedComplicatedGraph.Add(TInt64Pr(3, 1));
  TSnap::GetInDegCnt(TNEComplicatedGraph, TNEInDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEInDegCntComplicatedGraph);
}

// Testing GetInDegCnt for TFltPrV
TEST_F(GraphTest, GetInDegCntFltTest) {
  TFltPr64V TUNExpectedLoopGraph;
  TFltPr64V TUNInDegCntLoopGraph;
  TFltPr64V TNExpectedLoopGraph;
  TFltPr64V TNInDegCntLoopGraph;
  TFltPr64V TNEExpectedLoopGraph;
  TFltPr64V TNEInDegCntLoopGraph;

  TFltPr64V TUNExpectedReverseTree;
  TFltPr64V TUNInDegCntReverseTree;
  TFltPr64V TNExpectedReverseTree;
  TFltPr64V TNInDegCntReverseTree;
  TFltPr64V TNEExpectedReverseTree;
  TFltPr64V TNEInDegCntReverseTree;

  TFltPr64V TUNExpectedComplicatedGraph;
  TFltPr64V TUNInDegCntComplicatedGraph;
  TFltPr64V TNExpectedComplicatedGraph;
  TFltPr64V TNInDegCntComplicatedGraph;
  TFltPr64V TNEExpectedComplicatedGraph;
  TFltPr64V TNEInDegCntComplicatedGraph;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TFltPr(2, 5));
  TSnap::GetInDegCnt(TUNLoopGraph, TUNInDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNInDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TFltPr(1, 4));
  TNExpectedLoopGraph.Add(TFltPr(2, 1));
  TSnap::GetInDegCnt(TNLoopGraph, TNInDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNInDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TFltPr(1, 3));
  TNEExpectedLoopGraph.Add(TFltPr(2, 2));
  TSnap::GetInDegCnt(TNELoopGraph, TNEInDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEInDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TFltPr(1, 2));
  TUNExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetInDegCnt(TUNReverseTree, TUNInDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNInDegCntReverseTree);

  TNExpectedReverseTree.Add(TFltPr(0, 1));
  TNExpectedReverseTree.Add(TFltPr(1, 4));
  TSnap::GetInDegCnt(TNReverseTree, TNInDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNInDegCntReverseTree);

  TNEExpectedReverseTree.Add(TFltPr(0, 1));
  TNEExpectedReverseTree.Add(TFltPr(1, 4));
  TSnap::GetInDegCnt(TNEReverseTree, TNEInDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEInDegCntReverseTree);

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TFltPr(3, 2));
  TSnap::GetInDegCnt(TUNComplicatedGraph, TUNInDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNInDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TFltPr(0, 2));
  TNExpectedComplicatedGraph.Add(TFltPr(2, 3));
  TSnap::GetInDegCnt(TNComplicatedGraph, TNInDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNInDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TFltPr(0, 2));
  TNEExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TNEExpectedComplicatedGraph.Add(TFltPr(3, 1));
  TSnap::GetInDegCnt(TNEComplicatedGraph, TNEInDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEInDegCntComplicatedGraph);
}

// Testing GetOutDegCnt for TIntPrV
TEST_F(GraphTest, GetOutDegCntIntTest) {
  TIntPr64V TUNExpectedLoopGraph;
  TIntPr64V TUNOutDegCntLoopGraph;
  TIntPr64V TNExpectedLoopGraph;
  TIntPr64V TNOutDegCntLoopGraph;
  TIntPr64V TNEExpectedLoopGraph;
  TIntPr64V TNEOutDegCntLoopGraph;

  TIntPr64V TUNExpectedReverseTree;
  TIntPr64V TUNOutDegCntReverseTree;
  TIntPr64V TNExpectedReverseTree;
  TIntPr64V TNOutDegCntReverseTree;
  TIntPr64V TNEExpectedReverseTree;
  TIntPr64V TNEOutDegCntReverseTree;

  TIntPr64V TUNExpectedComplicatedGraph;
  TIntPr64V TUNOutDegCntComplicatedGraph;
  TIntPr64V TNExpectedComplicatedGraph;
  TIntPr64V TNOutDegCntComplicatedGraph;
  TIntPr64V TNEExpectedComplicatedGraph;
  TIntPr64V TNEOutDegCntComplicatedGraph;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TInt64Pr(2, 5));
  TSnap::GetOutDegCnt(TUNLoopGraph, TUNOutDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNOutDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TInt64Pr(1, 4));
  TNExpectedLoopGraph.Add(TInt64Pr(2, 1));
  TSnap::GetOutDegCnt(TNLoopGraph, TNOutDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNOutDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TInt64Pr(1, 3));
  TNEExpectedLoopGraph.Add(TInt64Pr(2, 2));
  TSnap::GetOutDegCnt(TNELoopGraph, TNEOutDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEOutDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TInt64Pr(1, 2));
  TUNExpectedReverseTree.Add(TInt64Pr(2, 3));
  TSnap::GetOutDegCnt(TUNReverseTree, TUNOutDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNOutDegCntReverseTree);

  TNExpectedReverseTree.Add(TInt64Pr(0, 2));
  TNExpectedReverseTree.Add(TInt64Pr(1, 2));
  TNExpectedReverseTree.Add(TInt64Pr(2, 1));
  TSnap::GetOutDegCnt(TNReverseTree, TNOutDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNOutDegCntReverseTree);

  TNEExpectedReverseTree.Add(TInt64Pr(0, 2));
  TNEExpectedReverseTree.Add(TInt64Pr(1, 2));
  TNEExpectedReverseTree.Add(TInt64Pr(2, 1));
  TSnap::GetOutDegCnt(TNEReverseTree, TNEOutDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEOutDegCntReverseTree);

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TInt64Pr(0, 1));
  TUNExpectedComplicatedGraph.Add(TInt64Pr(2, 2));
  TUNExpectedComplicatedGraph.Add(TInt64Pr(3, 2));
  TSnap::GetOutDegCnt(TUNComplicatedGraph, TUNOutDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNOutDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TInt64Pr(0, 2));
  TNExpectedComplicatedGraph.Add(TInt64Pr(2, 3));
  TSnap::GetOutDegCnt(TNComplicatedGraph, TNOutDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNOutDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TInt64Pr(0, 2));
  TNEExpectedComplicatedGraph.Add(TInt64Pr(2, 2));
  TNEExpectedComplicatedGraph.Add(TInt64Pr(3, 1));
  TSnap::GetOutDegCnt(TNEComplicatedGraph, TNEOutDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEOutDegCntComplicatedGraph);
}

// Testing GetOutDegCnt for TFltPrV
TEST_F(GraphTest, GetOutDegCntFltTest) {
  TFltPr64V TUNExpectedLoopGraph;
  TFltPr64V TUNOutDegCntLoopGraph;
  TFltPr64V TNExpectedLoopGraph;
  TFltPr64V TNOutDegCntLoopGraph;
  TFltPr64V TNEExpectedLoopGraph;
  TFltPr64V TNEOutDegCntLoopGraph;

  TFltPr64V TUNExpectedReverseTree;
  TFltPr64V TUNOutDegCntReverseTree;
  TFltPr64V TNExpectedReverseTree;
  TFltPr64V TNOutDegCntReverseTree;
  TFltPr64V TNEExpectedReverseTree;
  TFltPr64V TNEOutDegCntReverseTree;

  TFltPr64V TUNExpectedComplicatedGraph;
  TFltPr64V TUNOutDegCntComplicatedGraph;
  TFltPr64V TNExpectedComplicatedGraph;
  TFltPr64V TNOutDegCntComplicatedGraph;
  TFltPr64V TNEExpectedComplicatedGraph;
  TFltPr64V TNEOutDegCntComplicatedGraph;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TFltPr(2, 5));
  TSnap::GetOutDegCnt(TUNLoopGraph, TUNOutDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNOutDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TFltPr(1, 4));
  TNExpectedLoopGraph.Add(TFltPr(2, 1));
  TSnap::GetOutDegCnt(TNLoopGraph, TNOutDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNOutDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TFltPr(1, 3));
  TNEExpectedLoopGraph.Add(TFltPr(2, 2));
  TSnap::GetOutDegCnt(TNELoopGraph, TNEOutDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEOutDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TFltPr(1, 2));
  TUNExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetOutDegCnt(TUNReverseTree, TUNOutDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNOutDegCntReverseTree);

  TNExpectedReverseTree.Add(TFltPr(0, 2));
  TNExpectedReverseTree.Add(TFltPr(1, 2));
  TNExpectedReverseTree.Add(TFltPr(2, 1));
  TSnap::GetOutDegCnt(TNReverseTree, TNOutDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNOutDegCntReverseTree);

  TNEExpectedReverseTree.Add(TFltPr(0, 2));
  TNEExpectedReverseTree.Add(TFltPr(1, 2));
  TNEExpectedReverseTree.Add(TFltPr(2, 1));
  TSnap::GetOutDegCnt(TNEReverseTree, TNEOutDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEOutDegCntReverseTree);

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TFltPr(3, 2));
  TSnap::GetOutDegCnt(TUNComplicatedGraph, TUNOutDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNOutDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TFltPr(0, 2));
  TNExpectedComplicatedGraph.Add(TFltPr(2, 3));
  TSnap::GetOutDegCnt(TNComplicatedGraph, TNOutDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNOutDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TFltPr(0, 2));
  TNEExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TNEExpectedComplicatedGraph.Add(TFltPr(3, 1));
  TSnap::GetOutDegCnt(TNEComplicatedGraph, TNEOutDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEOutDegCntComplicatedGraph);
}

// Testing GetDegCnt for TIntPrV
TEST_F(GraphTest, GetDegCntIntTest) {
  TIntPr64V TUNExpectedLoopGraph;
  TIntPr64V TUNDegCntLoopGraph;
  TIntPr64V TNExpectedLoopGraph;
  TIntPr64V TNDegCntLoopGraph;
  TIntPr64V TNEExpectedLoopGraph;
  TIntPr64V TNEDegCntLoopGraph;

  TIntPr64V TUNExpectedReverseTree;
  TIntPr64V TUNDegCntReverseTree;
  TIntPr64V TNExpectedReverseTree;
  TIntPr64V TNDegCntReverseTree;
  TIntPr64V TNEExpectedReverseTree;
  TIntPr64V TNEDegCntReverseTree;

  TIntPr64V TUNExpectedComplicatedGraph;
  TIntPr64V TUNDegCntComplicatedGraph;
  TIntPr64V TNExpectedComplicatedGraph;
  TIntPr64V TNDegCntComplicatedGraph;
  TIntPr64V TNEExpectedComplicatedGraph;
  TIntPr64V TNEDegCntComplicatedGraph;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TInt64Pr(2, 5));
  TSnap::GetDegCnt(TUNLoopGraph, TUNDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TInt64Pr(2, 3));
  TNExpectedLoopGraph.Add(TInt64Pr(3, 2));
  TSnap::GetDegCnt(TNLoopGraph, TNDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TInt64Pr(2, 3));
  TNEExpectedLoopGraph.Add(TInt64Pr(4, 2));
  TSnap::GetDegCnt(TNELoopGraph, TNEDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TInt64Pr(1, 2));
  TUNExpectedReverseTree.Add(TInt64Pr(2, 3));
  TSnap::GetDegCnt(TUNReverseTree, TUNDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNDegCntReverseTree);

  TNExpectedReverseTree.Add(TInt64Pr(1, 2));
  TNExpectedReverseTree.Add(TInt64Pr(2, 3));
  TSnap::GetDegCnt(TNReverseTree, TNDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNDegCntReverseTree);

  TNEExpectedReverseTree.Add(TInt64Pr(1, 2));
  TNEExpectedReverseTree.Add(TInt64Pr(2, 3));
  TSnap::GetDegCnt(TNEReverseTree, TNEDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEDegCntReverseTree);

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TInt64Pr(0, 1));
  TUNExpectedComplicatedGraph.Add(TInt64Pr(2, 2));
  TUNExpectedComplicatedGraph.Add(TInt64Pr(3, 2));
  TSnap::GetDegCnt(TUNComplicatedGraph, TUNDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TInt64Pr(0, 1));
  TNExpectedComplicatedGraph.Add(TInt64Pr(2, 2));
  TNExpectedComplicatedGraph.Add(TInt64Pr(4, 2));
  TSnap::GetDegCnt(TNComplicatedGraph, TNDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TInt64Pr(0, 1));
  TNEExpectedComplicatedGraph.Add(TInt64Pr(2, 1));
  TNEExpectedComplicatedGraph.Add(TInt64Pr(3, 1));
  TNEExpectedComplicatedGraph.Add(TInt64Pr(4, 1));
  TNEExpectedComplicatedGraph.Add(TInt64Pr(5, 1));
  TSnap::GetDegCnt(TNEComplicatedGraph, TNEDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEDegCntComplicatedGraph);
}

// Testing GetDegCnt for TFltPrV
TEST_F(GraphTest, GetDegCntFltTest) {
  TFltPr64V TUNExpectedLoopGraph;
  TFltPr64V TUNDegCntLoopGraph;
  TFltPr64V TNExpectedLoopGraph;
  TFltPr64V TNDegCntLoopGraph;
  TFltPr64V TNEExpectedLoopGraph;
  TFltPr64V TNEDegCntLoopGraph;

  TFltPr64V TUNExpectedReverseTree;
  TFltPr64V TUNDegCntReverseTree;
  TFltPr64V TNExpectedReverseTree;
  TFltPr64V TNDegCntReverseTree;
  TFltPr64V TNEExpectedReverseTree;
  TFltPr64V TNEDegCntReverseTree;

  TFltPr64V TUNExpectedComplicatedGraph;
  TFltPr64V TUNDegCntComplicatedGraph;
  TFltPr64V TNExpectedComplicatedGraph;
  TFltPr64V TNDegCntComplicatedGraph;
  TFltPr64V TNEExpectedComplicatedGraph;
  TFltPr64V TNEDegCntComplicatedGraph;

  // LoopGraph

  TUNExpectedLoopGraph.Add(TFltPr(2, 5));
  TSnap::GetDegCnt(TUNLoopGraph, TUNDegCntLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNDegCntLoopGraph);

  TNExpectedLoopGraph.Add(TFltPr(2, 3));
  TNExpectedLoopGraph.Add(TFltPr(3, 2));
  TSnap::GetDegCnt(TNLoopGraph, TNDegCntLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNDegCntLoopGraph);

  TNEExpectedLoopGraph.Add(TFltPr(2, 3));
  TNEExpectedLoopGraph.Add(TFltPr(4, 2));
  TSnap::GetDegCnt(TNELoopGraph, TNEDegCntLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEDegCntLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(TFltPr(1, 2));
  TUNExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetDegCnt(TUNReverseTree, TUNDegCntReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNDegCntReverseTree);

  TNExpectedReverseTree.Add(TFltPr(1, 2));
  TNExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetDegCnt(TNReverseTree, TNDegCntReverseTree);
  CheckVectors(TNExpectedReverseTree, TNDegCntReverseTree);

  TNEExpectedReverseTree.Add(TFltPr(1, 2));
  TNEExpectedReverseTree.Add(TFltPr(2, 3));
  TSnap::GetDegCnt(TNEReverseTree, TNEDegCntReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEDegCntReverseTree);

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TUNExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TUNExpectedComplicatedGraph.Add(TFltPr(3, 2));
  TSnap::GetDegCnt(TUNComplicatedGraph, TUNDegCntComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNDegCntComplicatedGraph);

  TNExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TNExpectedComplicatedGraph.Add(TFltPr(2, 2));
  TNExpectedComplicatedGraph.Add(TFltPr(4, 2));
  TSnap::GetDegCnt(TNComplicatedGraph, TNDegCntComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNDegCntComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(TFltPr(0, 1));
  TNEExpectedComplicatedGraph.Add(TFltPr(2, 1));
  TNEExpectedComplicatedGraph.Add(TFltPr(3, 1));
  TNEExpectedComplicatedGraph.Add(TFltPr(4, 1));
  TNEExpectedComplicatedGraph.Add(TFltPr(5, 1));
  TSnap::GetDegCnt(TNEComplicatedGraph, TNEDegCntComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEDegCntComplicatedGraph);
}

// Testing GetDegSeqV
TEST_F(GraphTest, GetDegSeqVTest) {
  TInt64V TUNExpectedLoopGraph;
  TInt64V TUNDegVLoopGraph;
  TInt64V TNExpectedLoopGraph;
  TInt64V TNDegVLoopGraph;
  TInt64V TNEExpectedLoopGraph;
  TInt64V TNEDegVLoopGraph;

  TInt64V TUNExpectedReverseTree;
  TInt64V TUNDegVReverseTree;
  TInt64V TNExpectedReverseTree;
  TInt64V TNDegVReverseTree;
  TInt64V TNEExpectedReverseTree;
  TInt64V TNEDegVReverseTree;

  TInt64V TUNExpectedComplicatedGraph;
  TInt64V TUNDegVComplicatedGraph;
  TInt64V TNExpectedComplicatedGraph;
  TInt64V TNDegVComplicatedGraph;
  TInt64V TNEExpectedComplicatedGraph;
  TInt64V TNEDegVComplicatedGraph;

  // LoopGraph

  TUNExpectedLoopGraph.Add(2);
  TUNExpectedLoopGraph.Add(2);
  TUNExpectedLoopGraph.Add(2);
  TUNExpectedLoopGraph.Add(2);
  TUNExpectedLoopGraph.Add(2);
  TSnap::GetDegSeqV(TUNLoopGraph, TUNDegVLoopGraph);
  CheckVectors(TUNExpectedLoopGraph, TUNDegVLoopGraph);

  TNExpectedLoopGraph.Add(3);
  TNExpectedLoopGraph.Add(2);
  TNExpectedLoopGraph.Add(2);
  TNExpectedLoopGraph.Add(2);
  TNExpectedLoopGraph.Add(3);
  TSnap::GetDegSeqV(TNLoopGraph, TNDegVLoopGraph);
  CheckVectors(TNExpectedLoopGraph, TNDegVLoopGraph);

  TNEExpectedLoopGraph.Add(4);
  TNEExpectedLoopGraph.Add(2);
  TNEExpectedLoopGraph.Add(2);
  TNEExpectedLoopGraph.Add(2);
  TNEExpectedLoopGraph.Add(4);
  TSnap::GetDegSeqV(TNELoopGraph, TNEDegVLoopGraph);
  CheckVectors(TNEExpectedLoopGraph, TNEDegVLoopGraph);

  // ReverseTree

  TUNExpectedReverseTree.Add(2);
  TUNExpectedReverseTree.Add(2);
  TUNExpectedReverseTree.Add(2);
  TUNExpectedReverseTree.Add(1);
  TUNExpectedReverseTree.Add(1);
  TSnap::GetDegSeqV(TUNReverseTree, TUNDegVReverseTree);
  CheckVectors(TUNExpectedReverseTree, TUNDegVReverseTree);

  TNExpectedReverseTree.Add(2);
  TNExpectedReverseTree.Add(2);
  TNExpectedReverseTree.Add(2);
  TNExpectedReverseTree.Add(1);
  TNExpectedReverseTree.Add(1);
  TSnap::GetDegSeqV(TNReverseTree, TNDegVReverseTree);
  CheckVectors(TNExpectedReverseTree, TNDegVReverseTree);

  TNEExpectedReverseTree.Add(2);
  TNEExpectedReverseTree.Add(2);
  TNEExpectedReverseTree.Add(2);
  TNEExpectedReverseTree.Add(1);
  TNEExpectedReverseTree.Add(1);
  TSnap::GetDegSeqV(TNEReverseTree, TNEDegVReverseTree);
  CheckVectors(TNEExpectedReverseTree, TNEDegVReverseTree);

  // ComplicatedGraph

  TUNExpectedComplicatedGraph.Add(2);
  TUNExpectedComplicatedGraph.Add(3);
  TUNExpectedComplicatedGraph.Add(3);
  TUNExpectedComplicatedGraph.Add(0);
  TUNExpectedComplicatedGraph.Add(2);
  TSnap::GetDegSeqV(TUNComplicatedGraph, TUNDegVComplicatedGraph);
  CheckVectors(TUNExpectedComplicatedGraph, TUNDegVComplicatedGraph);

  TNExpectedComplicatedGraph.Add(2);
  TNExpectedComplicatedGraph.Add(4);
  TNExpectedComplicatedGraph.Add(4);
  TNExpectedComplicatedGraph.Add(0);
  TNExpectedComplicatedGraph.Add(2);
  TSnap::GetDegSeqV(TNComplicatedGraph, TNDegVComplicatedGraph);
  CheckVectors(TNExpectedComplicatedGraph, TNDegVComplicatedGraph);

  TNEExpectedComplicatedGraph.Add(2);
  TNEExpectedComplicatedGraph.Add(5);
  TNEExpectedComplicatedGraph.Add(4);
  TNEExpectedComplicatedGraph.Add(0);
  TNEExpectedComplicatedGraph.Add(3);
  TSnap::GetDegSeqV(TNEComplicatedGraph, TNEDegVComplicatedGraph);
  CheckVectors(TNEExpectedComplicatedGraph, TNEDegVComplicatedGraph);
}

// Testing GetDegSeqV with InDegV and OutDegV
TEST_F(GraphTest, GetDegSeqVInOutDegVTest) {
  TInt64V TUNExpectedInDegVLoopGraph;
  TInt64V TUNExpectedOutDegVLoopGraph;
  TInt64V TUNInDegVLoopGraph;
  TInt64V TUNOutDegVLoopGraph;
  TInt64V TNExpectedInDegVLoopGraph;
  TInt64V TNExpectedOutDegVLoopGraph;
  TInt64V TNInDegVLoopGraph;
  TInt64V TNOutDegVLoopGraph;
  TInt64V TNEExpectedInDegVLoopGraph;
  TInt64V TNEExpectedOutDegVLoopGraph;
  TInt64V TNEInDegVLoopGraph;
  TInt64V TNEOutDegVLoopGraph;

  TInt64V TUNExpectedInDegVReverseTree;
  TInt64V TUNExpectedOutDegVReverseTree;
  TInt64V TUNInDegVReverseTree;
  TInt64V TUNOutDegVReverseTree;
  TInt64V TNExpectedInDegVReverseTree;
  TInt64V TNExpectedOutDegVReverseTree;
  TInt64V TNInDegVReverseTree;
  TInt64V TNOutDegVReverseTree;
  TInt64V TNEExpectedInDegVReverseTree;
  TInt64V TNEExpectedOutDegVReverseTree;
  TInt64V TNEInDegVReverseTree;
  TInt64V TNEOutDegVReverseTree;

  TInt64V TUNExpectedInDegVComplicatedGraph;
  TInt64V TUNExpectedOutDegVComplicatedGraph;
  TInt64V TUNInDegVComplicatedGraph;
  TInt64V TUNOutDegVComplicatedGraph;
  TInt64V TNExpectedInDegVComplicatedGraph;
  TInt64V TNExpectedOutDegVComplicatedGraph;
  TInt64V TNInDegVComplicatedGraph;
  TInt64V TNOutDegVComplicatedGraph;
  TInt64V TNEExpectedInDegVComplicatedGraph;
  TInt64V TNEExpectedOutDegVComplicatedGraph;
  TInt64V TNEInDegVComplicatedGraph;
  TInt64V TNEOutDegVComplicatedGraph;

  // LoopGraph

  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedInDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TUNExpectedOutDegVLoopGraph.Add(2);
  TSnap::GetDegSeqV(TUNLoopGraph, TUNInDegVLoopGraph, TUNOutDegVLoopGraph);
  CheckVectors(TUNExpectedInDegVLoopGraph, TUNInDegVLoopGraph);
  CheckVectors(TUNExpectedOutDegVLoopGraph, TUNOutDegVLoopGraph);

  TNExpectedInDegVLoopGraph.Add(1);
  TNExpectedInDegVLoopGraph.Add(1);
  TNExpectedInDegVLoopGraph.Add(1);
  TNExpectedInDegVLoopGraph.Add(1);
  TNExpectedInDegVLoopGraph.Add(2);
  TNExpectedOutDegVLoopGraph.Add(2);
  TNExpectedOutDegVLoopGraph.Add(1);
  TNExpectedOutDegVLoopGraph.Add(1);
  TNExpectedOutDegVLoopGraph.Add(1);
  TNExpectedOutDegVLoopGraph.Add(1);
  TSnap::GetDegSeqV(TNLoopGraph, TNInDegVLoopGraph, TNOutDegVLoopGraph);
  CheckVectors(TNExpectedInDegVLoopGraph, TNInDegVLoopGraph);
  CheckVectors(TNExpectedOutDegVLoopGraph, TNOutDegVLoopGraph);

  TNEExpectedInDegVLoopGraph.Add(2);
  TNEExpectedInDegVLoopGraph.Add(1);
  TNEExpectedInDegVLoopGraph.Add(1);
  TNEExpectedInDegVLoopGraph.Add(1);
  TNEExpectedInDegVLoopGraph.Add(2);
  TNEExpectedOutDegVLoopGraph.Add(2);
  TNEExpectedOutDegVLoopGraph.Add(1);
  TNEExpectedOutDegVLoopGraph.Add(1);
  TNEExpectedOutDegVLoopGraph.Add(1);
  TNEExpectedOutDegVLoopGraph.Add(2);
  TSnap::GetDegSeqV(TNELoopGraph, TNEInDegVLoopGraph, TNEOutDegVLoopGraph);
  CheckVectors(TNEExpectedInDegVLoopGraph, TNEInDegVLoopGraph);
  CheckVectors(TNEExpectedOutDegVLoopGraph, TNEOutDegVLoopGraph);

  // ReverseTree

  TUNExpectedInDegVReverseTree.Add(2);
  TUNExpectedInDegVReverseTree.Add(2);
  TUNExpectedInDegVReverseTree.Add(2);
  TUNExpectedInDegVReverseTree.Add(1);
  TUNExpectedInDegVReverseTree.Add(1);
  TUNExpectedOutDegVReverseTree.Add(2);
  TUNExpectedOutDegVReverseTree.Add(2);
  TUNExpectedOutDegVReverseTree.Add(2);
  TUNExpectedOutDegVReverseTree.Add(1);
  TUNExpectedOutDegVReverseTree.Add(1);
  TSnap::GetDegSeqV(
      TUNReverseTree,
      TUNInDegVReverseTree,
      TUNOutDegVReverseTree);
  CheckVectors(TUNExpectedInDegVReverseTree, TUNInDegVReverseTree);
  CheckVectors(TUNExpectedOutDegVReverseTree, TUNOutDegVReverseTree);

  TNExpectedInDegVReverseTree.Add(0);
  TNExpectedInDegVReverseTree.Add(1);
  TNExpectedInDegVReverseTree.Add(1);
  TNExpectedInDegVReverseTree.Add(1);
  TNExpectedInDegVReverseTree.Add(1);
  TNExpectedOutDegVReverseTree.Add(2);
  TNExpectedOutDegVReverseTree.Add(1);
  TNExpectedOutDegVReverseTree.Add(1);
  TNExpectedOutDegVReverseTree.Add(0);
  TNExpectedOutDegVReverseTree.Add(0);
  TSnap::GetDegSeqV(TNReverseTree, TNInDegVReverseTree, TNOutDegVReverseTree);
  CheckVectors(TNExpectedInDegVReverseTree, TNInDegVReverseTree);
  CheckVectors(TNExpectedOutDegVReverseTree, TNOutDegVReverseTree);

  TNEExpectedInDegVReverseTree.Add(0);
  TNEExpectedInDegVReverseTree.Add(1);
  TNEExpectedInDegVReverseTree.Add(1);
  TNEExpectedInDegVReverseTree.Add(1);
  TNEExpectedInDegVReverseTree.Add(1);
  TNEExpectedOutDegVReverseTree.Add(2);
  TNEExpectedOutDegVReverseTree.Add(1);
  TNEExpectedOutDegVReverseTree.Add(1);
  TNEExpectedOutDegVReverseTree.Add(0);
  TNEExpectedOutDegVReverseTree.Add(0);
  TSnap::GetDegSeqV(
      TNEReverseTree,
      TNEInDegVReverseTree,
      TNEOutDegVReverseTree);
  CheckVectors(TNEExpectedInDegVReverseTree, TNEInDegVReverseTree);
  CheckVectors(TNEExpectedOutDegVReverseTree, TNEOutDegVReverseTree);

  // ComplicatedGraph

  TUNExpectedInDegVComplicatedGraph.Add(2);
  TUNExpectedInDegVComplicatedGraph.Add(3);
  TUNExpectedInDegVComplicatedGraph.Add(3);
  TUNExpectedInDegVComplicatedGraph.Add(0);
  TUNExpectedInDegVComplicatedGraph.Add(2);
  TUNExpectedOutDegVComplicatedGraph.Add(2);
  TUNExpectedOutDegVComplicatedGraph.Add(3);
  TUNExpectedOutDegVComplicatedGraph.Add(3);
  TUNExpectedOutDegVComplicatedGraph.Add(0);
  TUNExpectedOutDegVComplicatedGraph.Add(2);
  TSnap::GetDegSeqV(
      TUNComplicatedGraph,
      TUNInDegVComplicatedGraph,
      TUNOutDegVComplicatedGraph);
  CheckVectors(TUNExpectedInDegVComplicatedGraph, TUNInDegVComplicatedGraph);
  CheckVectors(TUNExpectedOutDegVComplicatedGraph, TUNOutDegVComplicatedGraph);

  TNExpectedInDegVComplicatedGraph.Add(0);
  TNExpectedInDegVComplicatedGraph.Add(2);
  TNExpectedInDegVComplicatedGraph.Add(2);
  TNExpectedInDegVComplicatedGraph.Add(0);
  TNExpectedInDegVComplicatedGraph.Add(2);
  TNExpectedOutDegVComplicatedGraph.Add(2);
  TNExpectedOutDegVComplicatedGraph.Add(2);
  TNExpectedOutDegVComplicatedGraph.Add(2);
  TNExpectedOutDegVComplicatedGraph.Add(0);
  TNExpectedOutDegVComplicatedGraph.Add(0);
  TSnap::GetDegSeqV(
      TNComplicatedGraph,
      TNInDegVComplicatedGraph,
      TNOutDegVComplicatedGraph);
  CheckVectors(TNExpectedInDegVComplicatedGraph, TNInDegVComplicatedGraph);
  CheckVectors(TNExpectedOutDegVComplicatedGraph, TNOutDegVComplicatedGraph);

  TNEExpectedInDegVComplicatedGraph.Add(0);
  TNEExpectedInDegVComplicatedGraph.Add(2);
  TNEExpectedInDegVComplicatedGraph.Add(2);
  TNEExpectedInDegVComplicatedGraph.Add(0);
  TNEExpectedInDegVComplicatedGraph.Add(3);
  TNEExpectedOutDegVComplicatedGraph.Add(2);
  TNEExpectedOutDegVComplicatedGraph.Add(3);
  TNEExpectedOutDegVComplicatedGraph.Add(2);
  TNEExpectedOutDegVComplicatedGraph.Add(0);
  TNEExpectedOutDegVComplicatedGraph.Add(0);
  TSnap::GetDegSeqV(
      TNEComplicatedGraph,
      TNEInDegVComplicatedGraph,
      TNEOutDegVComplicatedGraph);
  CheckVectors(TNEExpectedInDegVComplicatedGraph, TNEInDegVComplicatedGraph);
  CheckVectors(TNEExpectedOutDegVComplicatedGraph, TNEOutDegVComplicatedGraph);
}

// Testing GetNodeInDegV
TEST_F(GraphTest, GetNodeInDegVTest) {
  TIntPr64V TUNExpectedInDegVLoopGraph;
  TIntPr64V TUNInDegVLoopGraph;
  TIntPr64V TNExpectedInDegVLoopGraph;
  TIntPr64V TNInDegVLoopGraph;
  TIntPr64V TNEExpectedInDegVLoopGraph;
  TIntPr64V TNEInDegVLoopGraph;

  TIntPr64V TUNExpectedInDegVReverseTree;
  TIntPr64V TUNInDegVReverseTree;
  TIntPr64V TNExpectedInDegVReverseTree;
  TIntPr64V TNInDegVReverseTree;
  TIntPr64V TNEExpectedInDegVReverseTree;
  TIntPr64V TNEInDegVReverseTree;

  TIntPr64V TUNExpectedInDegVComplicatedGraph;
  TIntPr64V TUNInDegVComplicatedGraph;
  TIntPr64V TNExpectedInDegVComplicatedGraph;
  TIntPr64V TNInDegVComplicatedGraph;
  TIntPr64V TNEExpectedInDegVComplicatedGraph;
  TIntPr64V TNEInDegVComplicatedGraph;

  // LoopGraph

  TUNExpectedInDegVLoopGraph.Add(TInt64Pr(0, 2));
  TUNExpectedInDegVLoopGraph.Add(TInt64Pr(1, 2));
  TUNExpectedInDegVLoopGraph.Add(TInt64Pr(2, 2));
  TUNExpectedInDegVLoopGraph.Add(TInt64Pr(3, 2));
  TUNExpectedInDegVLoopGraph.Add(TInt64Pr(4, 2));
  TSnap::GetNodeInDegV(TUNLoopGraph, TUNInDegVLoopGraph);
  CheckVectors(TUNExpectedInDegVLoopGraph, TUNInDegVLoopGraph);

  TNExpectedInDegVLoopGraph.Add(TInt64Pr(0, 1));
  TNExpectedInDegVLoopGraph.Add(TInt64Pr(1, 1));
  TNExpectedInDegVLoopGraph.Add(TInt64Pr(2, 1));
  TNExpectedInDegVLoopGraph.Add(TInt64Pr(3, 1));
  TNExpectedInDegVLoopGraph.Add(TInt64Pr(4, 2));
  TSnap::GetNodeInDegV(TNLoopGraph, TNInDegVLoopGraph);
  CheckVectors(TNExpectedInDegVLoopGraph, TNInDegVLoopGraph);

  TNEExpectedInDegVLoopGraph.Add(TInt64Pr(0, 2));
  TNEExpectedInDegVLoopGraph.Add(TInt64Pr(1, 1));
  TNEExpectedInDegVLoopGraph.Add(TInt64Pr(2, 1));
  TNEExpectedInDegVLoopGraph.Add(TInt64Pr(3, 1));
  TNEExpectedInDegVLoopGraph.Add(TInt64Pr(4, 2));
  TSnap::GetNodeInDegV(TNELoopGraph, TNEInDegVLoopGraph);
  CheckVectors(TNEExpectedInDegVLoopGraph, TNEInDegVLoopGraph);

  // ReverseTree

  TUNExpectedInDegVReverseTree.Add(TInt64Pr(0, 2));
  TUNExpectedInDegVReverseTree.Add(TInt64Pr(1, 2));
  TUNExpectedInDegVReverseTree.Add(TInt64Pr(2, 2));
  TUNExpectedInDegVReverseTree.Add(TInt64Pr(3, 1));
  TUNExpectedInDegVReverseTree.Add(TInt64Pr(4, 1));
  TSnap::GetNodeInDegV(TUNReverseTree, TUNInDegVReverseTree);
  CheckVectors(TUNExpectedInDegVReverseTree, TUNInDegVReverseTree);

  TNExpectedInDegVReverseTree.Add(TInt64Pr(0, 0));
  TNExpectedInDegVReverseTree.Add(TInt64Pr(1, 1));
  TNExpectedInDegVReverseTree.Add(TInt64Pr(2, 1));
  TNExpectedInDegVReverseTree.Add(TInt64Pr(3, 1));
  TNExpectedInDegVReverseTree.Add(TInt64Pr(4, 1));
  TSnap::GetNodeInDegV(TNReverseTree, TNInDegVReverseTree);
  CheckVectors(TNExpectedInDegVReverseTree, TNInDegVReverseTree);

  TNEExpectedInDegVReverseTree.Add(TInt64Pr(0, 0));
  TNEExpectedInDegVReverseTree.Add(TInt64Pr(1, 1));
  TNEExpectedInDegVReverseTree.Add(TInt64Pr(2, 1));
  TNEExpectedInDegVReverseTree.Add(TInt64Pr(3, 1));
  TNEExpectedInDegVReverseTree.Add(TInt64Pr(4, 1));
  TSnap::GetNodeInDegV(TNEReverseTree, TNEInDegVReverseTree);
  CheckVectors(TNEExpectedInDegVReverseTree, TNEInDegVReverseTree);

  // ComplicatedGraph

  TUNExpectedInDegVComplicatedGraph.Add(TInt64Pr(0, 2));
  TUNExpectedInDegVComplicatedGraph.Add(TInt64Pr(1, 3));
  TUNExpectedInDegVComplicatedGraph.Add(TInt64Pr(2, 3));
  TUNExpectedInDegVComplicatedGraph.Add(TInt64Pr(3, 0));
  TUNExpectedInDegVComplicatedGraph.Add(TInt64Pr(4, 2));
  TSnap::GetNodeInDegV(TUNComplicatedGraph, TUNInDegVComplicatedGraph);
  CheckVectors(TUNExpectedInDegVComplicatedGraph, TUNInDegVComplicatedGraph);

  TNExpectedInDegVComplicatedGraph.Add(TInt64Pr(0, 0));
  TNExpectedInDegVComplicatedGraph.Add(TInt64Pr(1, 2));
  TNExpectedInDegVComplicatedGraph.Add(TInt64Pr(2, 2));
  TNExpectedInDegVComplicatedGraph.Add(TInt64Pr(3, 0));
  TNExpectedInDegVComplicatedGraph.Add(TInt64Pr(4, 2));
  TSnap::GetNodeInDegV(TNComplicatedGraph, TNInDegVComplicatedGraph);
  CheckVectors(TNExpectedInDegVComplicatedGraph, TNInDegVComplicatedGraph);

  TNEExpectedInDegVComplicatedGraph.Add(TInt64Pr(0, 0));
  TNEExpectedInDegVComplicatedGraph.Add(TInt64Pr(1, 2));
  TNEExpectedInDegVComplicatedGraph.Add(TInt64Pr(2, 2));
  TNEExpectedInDegVComplicatedGraph.Add(TInt64Pr(3, 0));
  TNEExpectedInDegVComplicatedGraph.Add(TInt64Pr(4, 3));
  TSnap::GetNodeInDegV(TNEComplicatedGraph, TNEInDegVComplicatedGraph);
  CheckVectors(TNEExpectedInDegVComplicatedGraph, TNEInDegVComplicatedGraph);
}

// Testing GetNodeOutDegV
TEST_F(GraphTest, GetNodeOutDegVTest) {
  TIntPr64V TUNExpectedOutDegVLoopGraph;
  TIntPr64V TUNOutDegVLoopGraph;
  TIntPr64V TNExpectedOutDegVLoopGraph;
  TIntPr64V TNOutDegVLoopGraph;
  TIntPr64V TNEExpectedOutDegVLoopGraph;
  TIntPr64V TNEOutDegVLoopGraph;

  TIntPr64V TUNExpectedOutDegVReverseTree;
  TIntPr64V TUNOutDegVReverseTree;
  TIntPr64V TNExpectedOutDegVReverseTree;
  TIntPr64V TNOutDegVReverseTree;
  TIntPr64V TNEExpectedOutDegVReverseTree;
  TIntPr64V TNEOutDegVReverseTree;

  TIntPr64V TUNExpectedOutDegVComplicatedGraph;
  TIntPr64V TUNOutDegVComplicatedGraph;
  TIntPr64V TNExpectedOutDegVComplicatedGraph;
  TIntPr64V TNOutDegVComplicatedGraph;
  TIntPr64V TNEExpectedOutDegVComplicatedGraph;
  TIntPr64V TNEOutDegVComplicatedGraph;

  // LoopGraph

  TUNExpectedOutDegVLoopGraph.Add(TInt64Pr(0, 2));
  TUNExpectedOutDegVLoopGraph.Add(TInt64Pr(1, 2));
  TUNExpectedOutDegVLoopGraph.Add(TInt64Pr(2, 2));
  TUNExpectedOutDegVLoopGraph.Add(TInt64Pr(3, 2));
  TUNExpectedOutDegVLoopGraph.Add(TInt64Pr(4, 2));
  TSnap::GetNodeOutDegV(TUNLoopGraph, TUNOutDegVLoopGraph);
  CheckVectors(TUNExpectedOutDegVLoopGraph, TUNOutDegVLoopGraph);

  TNExpectedOutDegVLoopGraph.Add(TInt64Pr(0, 2));
  TNExpectedOutDegVLoopGraph.Add(TInt64Pr(1, 1));
  TNExpectedOutDegVLoopGraph.Add(TInt64Pr(2, 1));
  TNExpectedOutDegVLoopGraph.Add(TInt64Pr(3, 1));
  TNExpectedOutDegVLoopGraph.Add(TInt64Pr(4, 1));
  TSnap::GetNodeOutDegV(TNLoopGraph, TNOutDegVLoopGraph);
  CheckVectors(TNExpectedOutDegVLoopGraph, TNOutDegVLoopGraph);

  TNEExpectedOutDegVLoopGraph.Add(TInt64Pr(0, 2));
  TNEExpectedOutDegVLoopGraph.Add(TInt64Pr(1, 1));
  TNEExpectedOutDegVLoopGraph.Add(TInt64Pr(2, 1));
  TNEExpectedOutDegVLoopGraph.Add(TInt64Pr(3, 1));
  TNEExpectedOutDegVLoopGraph.Add(TInt64Pr(4, 2));
  TSnap::GetNodeOutDegV(TNELoopGraph, TNEOutDegVLoopGraph);
  CheckVectors(TNEExpectedOutDegVLoopGraph, TNEOutDegVLoopGraph);

  // ReverseTree

  TUNExpectedOutDegVReverseTree.Add(TInt64Pr(0, 2));
  TUNExpectedOutDegVReverseTree.Add(TInt64Pr(1, 2));
  TUNExpectedOutDegVReverseTree.Add(TInt64Pr(2, 2));
  TUNExpectedOutDegVReverseTree.Add(TInt64Pr(3, 1));
  TUNExpectedOutDegVReverseTree.Add(TInt64Pr(4, 1));
  TSnap::GetNodeOutDegV(TUNReverseTree, TUNOutDegVReverseTree);
  CheckVectors(TUNExpectedOutDegVReverseTree, TUNOutDegVReverseTree);

  TNExpectedOutDegVReverseTree.Add(TInt64Pr(0, 2));
  TNExpectedOutDegVReverseTree.Add(TInt64Pr(1, 1));
  TNExpectedOutDegVReverseTree.Add(TInt64Pr(2, 1));
  TNExpectedOutDegVReverseTree.Add(TInt64Pr(3, 0));
  TNExpectedOutDegVReverseTree.Add(TInt64Pr(4, 0));
  TSnap::GetNodeOutDegV(TNReverseTree, TNOutDegVReverseTree);
  CheckVectors(TNExpectedOutDegVReverseTree, TNOutDegVReverseTree);

  TNEExpectedOutDegVReverseTree.Add(TInt64Pr(0, 2));
  TNEExpectedOutDegVReverseTree.Add(TInt64Pr(1, 1));
  TNEExpectedOutDegVReverseTree.Add(TInt64Pr(2, 1));
  TNEExpectedOutDegVReverseTree.Add(TInt64Pr(3, 0));
  TNEExpectedOutDegVReverseTree.Add(TInt64Pr(4, 0));
  TSnap::GetNodeOutDegV(TNEReverseTree, TNEOutDegVReverseTree);
  CheckVectors(TNEExpectedOutDegVReverseTree, TNEOutDegVReverseTree);

  // ComplicatedGraph

  TUNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(0, 2));
  TUNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(1, 3));
  TUNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(2, 3));
  TUNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(3, 0));
  TUNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(4, 2));
  TSnap::GetNodeOutDegV(TUNComplicatedGraph, TUNOutDegVComplicatedGraph);
  CheckVectors(TUNExpectedOutDegVComplicatedGraph, TUNOutDegVComplicatedGraph);

  TNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(0, 2));
  TNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(1, 2));
  TNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(2, 2));
  TNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(3, 0));
  TNExpectedOutDegVComplicatedGraph.Add(TInt64Pr(4, 0));
  TSnap::GetNodeOutDegV(TNComplicatedGraph, TNOutDegVComplicatedGraph);
  CheckVectors(TNExpectedOutDegVComplicatedGraph, TNOutDegVComplicatedGraph);

  TNEExpectedOutDegVComplicatedGraph.Add(TInt64Pr(0, 2));
  TNEExpectedOutDegVComplicatedGraph.Add(TInt64Pr(1, 3));
  TNEExpectedOutDegVComplicatedGraph.Add(TInt64Pr(2, 2));
  TNEExpectedOutDegVComplicatedGraph.Add(TInt64Pr(3, 0));
  TNEExpectedOutDegVComplicatedGraph.Add(TInt64Pr(4, 0));
  TSnap::GetNodeOutDegV(TNEComplicatedGraph, TNEOutDegVComplicatedGraph);
  CheckVectors(TNEExpectedOutDegVComplicatedGraph, TNEOutDegVComplicatedGraph);
}

// Testing CntUniqUndirEdges
TEST_F(GraphTest, CntUniqUndirEdgesTest) {
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TUNLoopGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNLoopGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNELoopGraph));

  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TUNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqUndirEdges(TNEReverseTree));

  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TUNComplicatedGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNComplicatedGraph));
  EXPECT_EQ(5, TSnap::CntUniqUndirEdges(TNEComplicatedGraph));
}

// Testing CntUniqDirEdges
TEST_F(GraphTest, CntUniqDirEdgesTest) {
  EXPECT_EQ(10, TSnap::CntUniqDirEdges(TUNLoopGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNLoopGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNELoopGraph));

  EXPECT_EQ(8, TSnap::CntUniqDirEdges(TUNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqDirEdges(TNReverseTree));
  EXPECT_EQ(4, TSnap::CntUniqDirEdges(TNEReverseTree));

  EXPECT_EQ(10, TSnap::CntUniqDirEdges(TUNComplicatedGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNComplicatedGraph));
  EXPECT_EQ(6, TSnap::CntUniqDirEdges(TNEComplicatedGraph));
}

// Testing CntUniqBiDirEdges
TEST_F(GraphTest, CntUniqBiDirEdgesTest) {
  EXPECT_EQ(5, TSnap::CntUniqBiDirEdges(TUNLoopGraph));
  EXPECT_EQ(1, TSnap::CntUniqBiDirEdges(TNLoopGraph));
  EXPECT_EQ(1, TSnap::CntUniqBiDirEdges(TNELoopGraph));

  EXPECT_EQ(4, TSnap::CntUniqBiDirEdges(TUNReverseTree));
  EXPECT_EQ(0, TSnap::CntUniqBiDirEdges(TNReverseTree));
  EXPECT_EQ(0, TSnap::CntUniqBiDirEdges(TNEReverseTree));

  EXPECT_EQ(5, TSnap::CntUniqBiDirEdges(TUNComplicatedGraph));
  EXPECT_EQ(1, TSnap::CntUniqBiDirEdges(TNComplicatedGraph));
  EXPECT_EQ(1, TSnap::CntUniqBiDirEdges(TNEComplicatedGraph));
}

// Testing CntSelfEdges
TEST_F(GraphTest, CntSelfEdgesTest) {
  EXPECT_EQ(0, TSnap::CntSelfEdges(TUNLoopGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNLoopGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNELoopGraph));

  EXPECT_EQ(0, TSnap::CntSelfEdges(TUNReverseTree));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNReverseTree));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNEReverseTree));

  EXPECT_EQ(0, TSnap::CntSelfEdges(TUNComplicatedGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNComplicatedGraph));
  EXPECT_EQ(0, TSnap::CntSelfEdges(TNEComplicatedGraph));

  EXPECT_EQ(1, TSnap::CntSelfEdges(TUNComplicatedGraphWithLoop));
  EXPECT_EQ(1, TSnap::CntSelfEdges(TNComplicatedGraphWithLoop));
  EXPECT_EQ(1, TSnap::CntSelfEdges(TNEComplicatedGraphWithLoop));
}

// Testing GetUnDir
TEST_F(GraphTest, GetUnDirTest) {
  TIntPr64V TNExpectedDegCnt;
  TIntPr64V TNEExpectedDegCnt;
  PUNGraph TUNGraphUnDir;
  PNGraph TNGraphUnDir;
  PNEGraph TNEGraphUnDir;

  // ComplicatedGraph

  TNExpectedDegCnt.Add(TInt64Pr(0, 1));
  TNExpectedDegCnt.Add(TInt64Pr(4, 2));
  TNExpectedDegCnt.Add(TInt64Pr(6, 2));

  TNEExpectedDegCnt.Add(TInt64Pr(0, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(4, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(6, 2));
  TNEExpectedDegCnt.Add(TInt64Pr(8, 1));

  TNGraphUnDir = TSnap::GetUnDir(TNComplicatedGraph);
  TNEGraphUnDir = TSnap::GetUnDir(TNEComplicatedGraph);

  CheckGraphs(TNExpectedDegCnt, TNGraphUnDir);
  CheckGraphs(TNEExpectedDegCnt, TNEGraphUnDir);
}

// Testing MakeUnDir
TEST_F(GraphTest, MakeUnDirTest) {
  TIntPr64V TNExpectedDegCnt;
  TIntPr64V TNEExpectedDegCnt;

  // ComplicatedGraph

  TNExpectedDegCnt.Add(TInt64Pr(0, 1));
  TNExpectedDegCnt.Add(TInt64Pr(4, 2));
  TNExpectedDegCnt.Add(TInt64Pr(6, 2));

  TNEExpectedDegCnt.Add(TInt64Pr(0, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(4, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(6, 2));
  TNEExpectedDegCnt.Add(TInt64Pr(8, 1));

  TSnap::MakeUnDir(TNComplicatedGraph);
  TSnap::MakeUnDir(TNEComplicatedGraph);

  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}

// Testing AddSelfEdges
TEST_F(GraphTest, AddSelfEdgesTest) {
  TIntPr64V TUNExpectedDegCnt;
  TIntPr64V TNExpectedDegCnt;
  TIntPr64V TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNExpectedDegCnt.Add(TInt64Pr(1, 1));
  TUNExpectedDegCnt.Add(TInt64Pr(3, 2));
  TUNExpectedDegCnt.Add(TInt64Pr(4, 2));

  TNExpectedDegCnt.Add(TInt64Pr(2, 1));
  TNExpectedDegCnt.Add(TInt64Pr(4, 2));
  TNExpectedDegCnt.Add(TInt64Pr(6, 2));

  TNEExpectedDegCnt.Add(TInt64Pr(2, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(4, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(5, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(6, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(7, 1));

  TSnap::AddSelfEdges(TUNComplicatedGraph);
  TSnap::AddSelfEdges(TNComplicatedGraph);
  TSnap::AddSelfEdges(TNEComplicatedGraph);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}

// Testing DelSelfEdges
TEST_F(GraphTest, DelSelfEdgesTest) {
  TIntPr64V TUNExpectedDegCnt;
  TIntPr64V TNExpectedDegCnt;
  TIntPr64V TNEExpectedDegCnt;

  // ComplicatedGraphWithLoop

  TUNExpectedDegCnt.Add(TInt64Pr(0, 1));
  TUNExpectedDegCnt.Add(TInt64Pr(2, 2));
  TUNExpectedDegCnt.Add(TInt64Pr(3, 2));

  TNExpectedDegCnt.Add(TInt64Pr(0, 1));
  TNExpectedDegCnt.Add(TInt64Pr(2, 2));
  TNExpectedDegCnt.Add(TInt64Pr(4, 2));

  TNEExpectedDegCnt.Add(TInt64Pr(0, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(2, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(3, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(4, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(5, 1));

  TSnap::DelSelfEdges(TUNComplicatedGraphWithLoop);
  TSnap::DelSelfEdges(TNComplicatedGraphWithLoop);
  TSnap::DelSelfEdges(TNEComplicatedGraphWithLoop);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraphWithLoop);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraphWithLoop);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraphWithLoop);
}

// Testing DelNodes
TEST_F(GraphTest, DelNodesTest) {
  TInt64V TUNDelNodes;
  TInt64V TNDelNodes;
  TInt64V TNEDelNodes;
  TIntPr64V TUNExpectedDegCnt;
  TIntPr64V TNExpectedDegCnt;
  TIntPr64V TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNDelNodes.Add(0);
  TNDelNodes.Add(0);
  TNEDelNodes.Add(0);

  TUNExpectedDegCnt.Add(TInt64Pr(0, 1));
  TUNExpectedDegCnt.Add(TInt64Pr(2, 3));

  TNExpectedDegCnt.Add(TInt64Pr(0, 1));
  TNExpectedDegCnt.Add(TInt64Pr(2, 1));
  TNExpectedDegCnt.Add(TInt64Pr(3, 2));

  TNEExpectedDegCnt.Add(TInt64Pr(0, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(3, 2));
  TNEExpectedDegCnt.Add(TInt64Pr(4, 1));

  TSnap::DelNodes(TUNComplicatedGraph, TUNDelNodes);
  TSnap::DelNodes(TNComplicatedGraph, TNDelNodes);
  TSnap::DelNodes(TNEComplicatedGraph, TNEDelNodes);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}


// Testing DelZeroDegNodes
TEST_F(GraphTest, DelZeroDegNodesTest) {
  TIntPr64V TUNExpectedDegCnt;
  TIntPr64V TNExpectedDegCnt;
  TIntPr64V TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNExpectedDegCnt.Add(TInt64Pr(2, 2));
  TUNExpectedDegCnt.Add(TInt64Pr(3, 2));

  TNExpectedDegCnt.Add(TInt64Pr(2, 2));
  TNExpectedDegCnt.Add(TInt64Pr(4, 2));

  TNEExpectedDegCnt.Add(TInt64Pr(2, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(3, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(4, 1));
  TNEExpectedDegCnt.Add(TInt64Pr(5, 1));

  TSnap::DelZeroDegNodes(TUNComplicatedGraph);
  TSnap::DelZeroDegNodes(TNComplicatedGraph);
  TSnap::DelZeroDegNodes(TNEComplicatedGraph);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}

// Testing DelDegKNodes
TEST_F(GraphTest, DelDegKNodesTest) {
  TIntPr64V TUNExpectedDegCnt;
  TIntPr64V TNExpectedDegCnt;
  TIntPr64V TNEExpectedDegCnt;

  // ComplicatedGraph

  TUNExpectedDegCnt.Add(TInt64Pr(0, 3));

  TNExpectedDegCnt.Add(TInt64Pr(0, 1));

  TNEExpectedDegCnt.Add(TInt64Pr(2, 2));

  TSnap::DelDegKNodes(TUNComplicatedGraph, 3, 3);
  TSnap::DelDegKNodes(TNComplicatedGraph, 2, 0);
  TSnap::DelDegKNodes(TNEComplicatedGraph, 2, 0);

  CheckGraphs(TUNExpectedDegCnt, TUNComplicatedGraph);
  CheckGraphs(TNExpectedDegCnt, TNComplicatedGraph);
  CheckGraphs(TNEExpectedDegCnt, TNEComplicatedGraph);
}

/////
// Tree Routines
////

// Testing IsTree
TEST_F(TreeTest, IsTreeTest) {
  int64 RootNId;

  EXPECT_TRUE(TSnap::IsTree(SingleNode, RootNId));
  EXPECT_EQ(1, RootNId);
  EXPECT_TRUE(TSnap::IsTree(Tree, RootNId));
  EXPECT_EQ(1, RootNId);
  EXPECT_FALSE(TSnap::IsTree(Forest, RootNId));
  EXPECT_EQ(-1, RootNId);
  EXPECT_FALSE(TSnap::IsTree(Circle, RootNId));
  EXPECT_EQ(-1, RootNId);
}

// Testing GetTreeRootNId
TEST_F(TreeTest, GetTreeRootNIdTest) {
  int64 RootNId = 1;

  EXPECT_EQ(RootNId, TSnap::GetTreeRootNId(SingleNode));
  EXPECT_EQ(RootNId, TSnap::GetTreeRootNId(Tree));
  //EXPECT_DEATH(TSnap::GetTreeRootNId(Forest), "");
  //EXPECT_DEATH(TSnap::GetTreeRootNId(Circle), "");
}

// Testing GetTreeSig
TEST_F(TreeTest, GetTreeSigTest) {
  int64 RootNId = 1;
  TInt64V SingleNodeSig;
  TInt64V TreeSig;
  TInt64V ForestSig;
  TInt64V CircleSig;
  TInt64V ExpectedSingleNodeSig;
  TInt64V ExpectedTreeSig;

  // SingleNode

  ExpectedSingleNodeSig.Add(0);
  TSnap::GetTreeSig(SingleNode, RootNId, SingleNodeSig);
  CheckVectors(ExpectedSingleNodeSig, SingleNodeSig);

  // Tree

  int64 NumNodesWithChildren = 6;
  int64 NumNodesWithChild = 1;
  int64 NumNodesWithNoChildren = 7;
  AddValues(ExpectedTreeSig, NumNodesWithChildren, 2);
  AddValues(ExpectedTreeSig, NumNodesWithChild, 1);
  AddValues(ExpectedTreeSig, NumNodesWithNoChildren, 0);

  // OP RS 2014/06/12, the code below commented out, needs more investigation
  //TSnap::GetTreeSig(Tree, RootNId, TreeSig);
  //CheckVectors(ExpectedTreeSig, TreeSig);

  // Forest

  //EXPECT_DEATH(TSnap::GetTreeSig(Forest, RootNId, ForestSig), "");

  // Circle

  //EXPECT_DEATH(TSnap::GetTreeSig(Circle, RootNId, CircleSig), "");
}

// Testing GetTreeSig with node map
TEST_F(TreeTest, GetTreeSigWithNodeMapTest) {
  int64 RootNId = 1;
  TInt64V SingleNodeSig;
  TInt64V TreeSig;
  TInt64V ForestSig;
  TInt64V CircleSig;
  TInt64V ExpectedSingleNodeSig;
  TInt64V ExpectedTreeSig;
  TIntPr64V SingleNodeNodeMap;
  TIntPr64V TreeNodeMap;
  TIntPr64V ForestNodeMap;
  TIntPr64V CircleNodeMap;
  TInt64V TreeNodeMapKeys;
  TIntPr64V ExpectedSingleNodeNodeMap;
  TInt64V ExpectedTreeNodeMapKeys;

  // SingleNode

  ExpectedSingleNodeSig.Add(0);
  ExpectedSingleNodeNodeMap.Add(TInt64Pr(1, 0));
  TSnap::GetTreeSig(SingleNode, RootNId, SingleNodeSig, SingleNodeNodeMap);
  // OP RS 2014/06/12, the code below commented out, needs more investigation
  //CheckVectors(ExpectedSingleNodeSig, SingleNodeSig);
  //CheckVectors(ExpectedSingleNodeNodeMap, SingleNodeNodeMap);

  // Tree

  int64 NumNodesWithChildren = 6;
  int64 NumNodesWithChild = 1;
  int64 NumNodesWithNoChildren = 7;
  AddValues(ExpectedTreeSig, NumNodesWithChildren, 2);
  AddValues(ExpectedTreeSig, NumNodesWithChild, 1);
  AddValues(ExpectedTreeSig, NumNodesWithNoChildren, 0);
  AddConsecutiveValues(ExpectedTreeNodeMapKeys, RootNId, Tree->GetNodes());

  // OP RS 2014/06/12, the code below commented out, needs more investigation
  // Checking only node IDs (not positions) for node map

  //TSnap::GetTreeSig(Tree, RootNId, TreeSig, TreeNodeMap);
  //GetKeys(TreeNodeMapKeys, TreeNodeMap);
  //ExpectedTreeNodeMapKeys.Sort();
  //TreeNodeMapKeys.Sort();
  //CheckVectors(ExpectedTreeSig, TreeSig);
  //CheckVectors(ExpectedTreeNodeMapKeys, TreeNodeMapKeys);

  // Forest
  //EXPECT_DEATH(
  //    TSnap::GetTreeSig(Forest, RootNId, ForestSig, ForestNodeMap), "");

  // Circle
  //EXPECT_DEATH(
  //    TSnap::GetTreeSig(Circle, RootNId, CircleSig, CircleNodeMap), "");
}

///
// HELPER METHODS
///

void CheckVectors(const TIntPr64V& Expected, const TIntPr64V& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val1.Val, Actual[i].Val1.Val);
    EXPECT_EQ(Expected[i].Val2.Val, Actual[i].Val2.Val);
  }
}

void CheckVectors(const TFltPr64V& Expected, const TFltPr64V& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val1.Val, Actual[i].Val1.Val);
    EXPECT_EQ(Expected[i].Val2.Val, Actual[i].Val2.Val);
  }
}

void CheckVectors(const TInt64V& Expected, const TInt64V& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val, Actual[i].Val);
  }
}

void CheckVectors(const TInt64Set& Expected, const TInt64Set& Actual) {
  ASSERT_EQ(Expected.Len(), Actual.Len());
  for (int64 i = 0; i < Actual.Len(); i++) {
    EXPECT_EQ(Expected[i].Val, Actual[i].Val);
  }
}

void CheckGraphs(const TIntPr64V& ExpectedDegCnt, const PUNGraph& ActualGraph) {
  TIntPr64V ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckGraphs(const TIntPr64V& ExpectedDegCnt, const PNGraph& ActualGraph) {
  TIntPr64V ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckGraphs(const TIntPr64V& ExpectedDegCnt, const PNEGraph& ActualGraph) {
  TIntPr64V ActualDegCnt;
  TSnap::GetDegCnt(ActualGraph, ActualDegCnt);

  CheckVectors(ExpectedDegCnt, ActualDegCnt);
}

void CheckNotInSet(int64 Key, const TInt64Set& Keys) {
  for (int64 i = 0; i < Keys.Len(); i++) {
    EXPECT_NE(Keys[i], Key);
  }
}

void AddValues(TInt64V& Vector, int64 NumValues, int64 Value) {
  for (int64 i = 0; i < NumValues; i++) {
    Vector.Add(Value);
  }
}

void AddConsecutiveValues(TInt64V& Vector, int64 Start, int64 End) {
  for (int64 Id = Start; Id <= End; Id++) {
    Vector.Add(Id);
  }
}

void GetKeys(TInt64V& Keys, const TIntPr64V& Map) {
  for (int64 Id = 0; Id < Map.Len(); Id++) {
    Keys.Add(Map[Id].Val1.Val);
  }
}
