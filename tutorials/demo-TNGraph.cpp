#include "Snap.h"

// Print graph statistics
void PrintGStats(const char s[], PNGraph Graph) {
  printf("graph %s, nodes %s, edges %s, empty %s\n", s,
            TUInt64::GetStr(Graph->GetNodes()).CStr(),
            TUInt64::GetStr(Graph->GetEdges()).CStr(),
            Graph->Empty() ? "yes" : "no");
}

// Test the default constructor
void DefaultConstructor() {
  PNGraph Graph;

  Graph = TNGraph::New();
  PrintGStats("DefaultConstructor:Graph",Graph);
}

// Test node, edge creation
void ManipulateNodesEdges() {
  int NNodes = 10000;
  int NEdges = 100000;
  const char *FName = "demo.graph.dat";

  PNGraph Graph;
  PNGraph Graph1;
  PNGraph Graph2;
  int i;
  int n;
  int64 NCount;
  int64 ECount1;
  int64 ECount2;
  int x,y;
  bool t;

  Graph = TNGraph::New();
  t = Graph->Empty();

  // create the nodes
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }
  t = Graph->Empty();
  n = Graph->GetNodes();

  // create random edges
  NCount = NEdges;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    // Graph->GetEdges() is not correct for the loops (x == y),
    // skip the loops in this test
    if (x != y  &&  !Graph->IsEdge(x,y)) {
      n = Graph->AddEdge(x, y);
      NCount--;
    }
  }
  PrintGStats("ManipulateNodesEdges:Graph",Graph);

  // traverse the nodes, count them
  NCount = 0;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NCount++;
    // get the node ID and its in- and out-degree
    printf("node id %s has in-degree %s and out-degree %s\n",
            TInt64::GetStr(NI.GetId()).CStr(),
            TInt64::GetStr(NI.GetInDeg()).CStr(),
            TInt64::GetStr(NI.GetOutDeg()).CStr());
  }

  // traverse all the in-edges of all the nodes, count them
  ECount1 = 0;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetInDeg(); e++) {
      ECount1++;
      // get the node ID and its in-neighbour
      printf("node id %s has in-neighbor %s\n",
                TInt64::GetStr(NI.GetId()).CStr(),
                TInt64::GetStr(NI.GetInNId(e)).CStr());
    }
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      // get the node ID and its out-neighbour
      printf("node id %s has out-neighbor %s\n",
                TInt64::GetStr(NI.GetId()).CStr(),
                TInt64::GetStr(NI.GetOutNId(e)).CStr());
    }
  }

  // traverse all the edges directly, count them
  ECount2 = 0;
  for (TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    ECount2++;
    // get the source and the destination nodes of the edge
    printf("edge from source node %s to destination node %s\n",
              TInt64::GetStr(EI.GetSrcNId()).CStr(),
              TInt64::GetStr(EI.GetDstNId()).CStr());
  }
  printf("ManipulateNodesEdges:Graph, nodes %s, edges1 %s, edges2 %s\n",
              TInt64::GetStr(NCount).CStr(),
              TInt64::GetStr(ECount1).CStr(),
              TInt64::GetStr(ECount2).CStr());

  // assignment
  Graph1 = TNGraph::New();
  *Graph1 = *Graph;
  PrintGStats("ManipulateNodesEdges:Graph1",Graph1);

  // save the graph
  {
    TFOut FOut(FName);
    Graph->Save(FOut);
    FOut.Flush();
  }

  // load the graph
  {
    TFIn FIn(FName);
    Graph2 = TNGraph::Load(FIn);
  }
  PrintGStats("ManipulateNodesEdges:Graph2",Graph2);

  // remove all the nodes and edges
  for (i = 0; i < NNodes; i++) {
    n = Graph->GetRndNId();
    Graph->DelNode(n);
  }

  PrintGStats("ManipulateNodesEdges:Graph",Graph);

  Graph1->Clr();
  PrintGStats("ManipulateNodesEdges:Graph1",Graph1);
}

// Test small graph
void GetSmallGraph() {
  PNGraph Graph;

  Graph = TNGraph::GetSmallGraph();
  PrintGStats("GetSmallGraph:Graph",Graph);
}

int main(int argc, char* argv[]) {
  DefaultConstructor();
  ManipulateNodesEdges();
  GetSmallGraph();
}

