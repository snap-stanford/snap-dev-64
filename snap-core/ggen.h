// TODO ROK, Jure included basic documentation, finalize reference doc

/////////////////////////////////////////////////
// Graph Generators
namespace TSnap {

/////////////////////////////////////////////////
// Deterministic graphs
/// Generates a 2D-grid graph of Rows rows and Cols columns.
template <class PGraph> PGraph GenGrid(const int64& Rows, const int64& Cols, const bool& IsDir=true);
/// Generates a graph with star topology. Node id 0 is in the center and then links to all other nodes.
template <class PGraph> PGraph GenStar(const int64& Nodes, const bool& IsDir=true);
/// Generates a circle graph where every node creates out-links to NodeOutDeg forward nodes.
template <class PGraph> PGraph GenCircle(const int64& Nodes, const int64& NodeOutDeg=1, const bool& IsDir=true);
/// Generates a complete graph on Nodes nodes. Graph has no self-loops.
template <class PGraph> PGraph GenFull(const int64& Nodes);
/// Generates a tree graph of Levels levels with every parent having Fanout children.
template <class PGraph> PGraph GenTree(const int64& Fanout, const int64& Levels, const bool& IsDir=true, const bool& ChildPointsToParent=true);
/// Generates a Ravasz-Barabasi deterministic scale-free graph.
template <class PGraph> PGraph GenBaraHierar(const int64& Levels, const bool& IsDir=true);

/////////////////////////////////////////////////
// Random graphs

/// Generates an Erdos-Renyi random graph.
template <class PGraph> PGraph GenRndGnm(const int64& Nodes, const int64& Edges, const bool& IsDir=true, TRnd& Rnd=TInt::Rnd);
/// Generates a random bipartite graph.
PBPGraph GenRndBipart(const int64& LeftNodes, const int64& RightNodes, const int64& Edges, TRnd& Rnd=TInt::Rnd);
/// Generates a random graph where each node has degree exactly NodeDeg.
PUNGraph GenRndDegK(const int64& Nodes, const int64& NodeDeg, const int64& NSwitch=100, TRnd& Rnd=TInt::Rnd);
/// Generates a random scale-free graph with power-law degree distribution.
PUNGraph GenRndPowerLaw(const int64& Nodes, const double& PowerExp, const bool& ConfModel=true, TRnd& Rnd=TInt::Rnd);
/// Generates a random graph with exact degree sequence.
PUNGraph GenDegSeq(const TInt64V& DegSeqV, TRnd& Rnd=TInt::Rnd);
/// Generates a power-law degree distribution using Barabasi-Albert model of scale-free graphs.
PUNGraph GenPrefAttach(const int64& Nodes, const int64& NodeOutDeg, TRnd& Rnd=TInt::Rnd);
/// Generates a random scale-free graph using the Geometric Preferential model.
PUNGraph GenGeoPrefAttach(const int64& Nodes, const int64& OutDeg, const double& Beta, TRnd& Rnd=TInt::Rnd);
/// Generates a randomly small-world graph using the Watts-Strogatz model.
PUNGraph GenSmallWorld(const int64& Nodes, const int64& NodeOutDeg, const double& RewireProb, TRnd& Rnd=TInt::Rnd);
/// Generates a random undirect graph with a given degree sequence.
PUNGraph GenConfModel(const TInt64V& DegSeqV, TRnd& Rnd=TInt::Rnd);
/// Generates a random Forest Fire, directed graph with given probabilities.
PNGraph GenForestFire(const int64& Nodes, const double& FwdProb, const double& BckProb);
/// Generates a random scale-free network using the Copying Model.
PNGraph GenCopyModel(const int64& Nodes, const double& Beta, TRnd& Rnd=TInt::Rnd);
/// Generates a R-MAT graph using recursive descent into a 2x2 matrix [A,B; C, 1-(A+B+C)].
PNGraph GenRMat(const int64& Nodes, const int64& Edges, const double& A, const double& B, const double& C, TRnd& Rnd=TInt::Rnd);
/// Generates a R-Mat graph, with a synthetic copy of the Epinions social network.
PNGraph GenRMatEpinions();

  
/// Rewire a random undirected graph. Keeps node degrees the same, but randomly rewires the edges.
PUNGraph GenRewire(const PUNGraph& Graph, const int64& NSwitch=100, TRnd& Rnd=TInt::Rnd);
/// Rewire a random directed graph. Keeps node degrees the same, but randomly rewires the edges.
PNGraph  GenRewire(const PNGraph& Graph, const int64& NSwitch=100, TRnd& Rnd=TInt::Rnd);
/// Rewire a random bipartite graph. Keeps node degrees the same, but randomly rewires the edges.
PBPGraph GenRewire(const PBPGraph& Graph, const int64& NSwitch=100, TRnd& Rnd=TInt::Rnd);
/// Generate a random graph using (approximately) the same node degrees as in G using the configuration model.
PUNGraph GenConfModel(const PUNGraph& G);
  
/////////////////////////////////////////////////
// Implementation
template <class PGraph>
PGraph GenGrid(const int64& Rows, const int64& Cols, const bool& IsDir) {
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Rows*Cols, 4*Rows*Cols);
  int64 node, r, c;
  for (node = 0; node < Rows * Cols; node++) {
    Graph.AddNode(node); }
  for (r = 0; r < Rows; r++) {
    for (c = 0; c < Cols; c++) {
      const int64 nodeId = Cols*r + c;
      if (r < Rows-1) { // bottom node
        Graph.AddEdge(nodeId, nodeId+Cols); 
        if (Graph.HasFlag(gfDirected) && ! IsDir) { 
          Graph.AddEdge(nodeId+Cols, nodeId); }
      }
      if (c < Cols-1) { // right node
        Graph.AddEdge(nodeId, nodeId+1); 
        if (Graph.HasFlag(gfDirected) && ! IsDir) { 
          Graph.AddEdge(nodeId+1, nodeId); }
      }
    }
  }
  return GraphPt;
}

template <class PGraph>
PGraph GenStar(const int64& Nodes, const bool& IsDir) {
  PGraph Graph = PGraph::TObj::New();
  Graph->Reserve(Nodes, Nodes);
  Graph->AddNode(0);
  for (int64 n = 1; n < Nodes; n++) {
    Graph->AddNode(n);
    Graph->AddEdge(0, n);
    if (Graph->HasFlag(gfDirected) && ! IsDir) { Graph->AddEdge(n, 0); }
  }
  return Graph;
}

template <class PGraph>
PGraph GenCircle(const int64& Nodes, const int64& NodeOutDeg, const bool& IsDir) {
  PGraph Graph = PGraph::TObj::New();
  Graph->Reserve(Nodes, Nodes*NodeOutDeg);
  for (int64 n = 0; n < Nodes; n++) {
    Graph->AddNode(n); }
  for (int64 n = 0; n < Nodes; n++) {
    for (int64 x = 0; x < NodeOutDeg; x++) {
      Graph->AddEdge(n, (n+x+1) % Nodes);
      if (Graph->HasFlag(gfDirected) && ! IsDir) { Graph->AddEdge((n+x+1) % Nodes, n); }
    }
  }
  return Graph;
}

template <class PGraph>
PGraph GenFull(const int64& Nodes) {
  PGraph Graph = PGraph::TObj::New();
  Graph->Reserve(Nodes, Nodes*Nodes);
  for (int64 n = 0; n < Nodes; n++) {
    Graph->AddNode(n); }
  for (int64 n1 = 0; n1 < Nodes; n1++) {
    for (int64 n2 = 0; n2 < Nodes; n2++) {
      if (n1 != n2) { Graph->AddEdge(n1, n2); }
    }
  }
  return Graph;
}

template <class PGraph>
PGraph GenTree(const int64& Fanout, const int64& Levels, const bool& IsDir, const bool& ChildPointsToParent) {
  const int64 Nodes = (int64) (pow(double(Fanout), double(Levels+1)) - 1) / (Fanout - 1);
  const int64 Edges = Nodes - 1;
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Nodes, Edges);
  int64 node;
  for (node = 0; node < Nodes; node++) {
    Graph.AddNode(node); }
  // non-leaf nodes
  for (node = 0; node < (int64) Nodes - (int64) pow(double(Fanout), double(Levels)); node++) {
    for (int64 edge = 1; edge <= Fanout; edge++) {
      if (IsDir) {
        if (ChildPointsToParent) { Graph.AddEdge(Fanout*node+edge, node); }
        else { Graph.AddEdge(node, Fanout*node+edge); }
      } else {
        Graph.AddEdge(node, Fanout*node+edge); // link children
        Graph.AddEdge(Fanout*node+edge, node);
      }
    }
  }
  return GraphPt;
}

/// Corners of the graph are recursively expanded with miniature copies of the
/// base graph (below). The graph has power-law degree distribution with the
/// exponent 1+ln(5)/ln(4) and clustering coefficient with power-law decay 
/// exponent -1.
/// Base graph:

/// \verbatim
///   o---o
///   |\ /|
///   | o |
///   |/ \|
///   o---o
/// \endverbatim

/// See: Hierarchical organization in complex networks. Ravasz and Barabasi.
/// URL: http://arxiv.org/abs/cond-mat/0206130
template <class PGraph>
PGraph GenBaraHierar(const int64& Levels, const bool& IsDir) {
  const int64 Nodes = (int64) TMath::Round(TMath::Power(5, Levels));
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Nodes, -1);
  // base graph
  for (int64 i = 0; i < 5; i++) { Graph.AddNode(i); }
  Graph.AddEdge(1,2);  Graph.AddEdge(2,3);
  Graph.AddEdge(3,4);  Graph.AddEdge(4,1);
  Graph.AddEdge(1,0);  Graph.AddEdge(3,0);
  Graph.AddEdge(2,0);  Graph.AddEdge(4,0);
  // expansion
  const int64 CenterId = 0;
  for (int64 lev = 1; lev < Levels+1; lev++) {
    const int64 MxNId = Graph.GetNodes();
    // make 4 duplicate copies
    for (int64 d = 0; d < 4; d++) {
      for (int64 n = 0; n < MxNId; n++) { Graph.AddNode(); }
      for (int64 n = 0; n < MxNId; n++) {
        typename PGraph::TObj::TNodeI NI = Graph.GetNI(n);
        const int64 SrcId = n+MxNId*(d+1);
        for (int64 e = 0; e < NI.GetOutDeg(); e++) {
          Graph.AddEdge(SrcId, NI.GetOutNId(e)+MxNId*(d+1));
        }
      }
    }
    // add edges to the center
    //const int LevPow = (int)TMath::Round(TMath::Power(5,lev-1));
    for (int64 n = MxNId; n < Graph.GetNodes(); n++) {
      //typename PGraph::TObj::TNodeI NI = Graph.GetNI(n);
      const int64 SrcId = n;
      int64 Pow = 1;  bool Skip = false;
      for (int64 p = 1; p <= lev; p++) {
        if (SrcId % (5*Pow) < Pow) { Skip=true; break; }
        Pow *= 5;
      }
      if (Skip) { continue; }
      Graph.AddEdge(SrcId, CenterId);
    }
  }
  return GraphPt;
}

template <class PGraph>
PGraph GenRndGnm(const int64& Nodes, const int64& Edges, const bool& IsDir, TRnd& Rnd) {
  PGraph GraphPt = PGraph::New();
  typename PGraph::TObj& Graph = *GraphPt;
  Graph.Reserve(Nodes, Edges);
  IAssertR((1.0 * (Nodes-1) / 2 * (IsDir ? 2 : 1)) >= (1.0 * Edges / Nodes), TStr::Fmt("Not enough nodes (%d), for edges (%d).", Nodes, Edges));
  for (int64 node = 0; node < Nodes; node++) {
    IAssert(Graph.AddNode(node) == node);
  }
  for (int64 edge = 0; edge < Edges; ) {
    const int64 SrcNId = Rnd.GetUniDevInt(Nodes);
    const int64 DstNId = Rnd.GetUniDevInt(Nodes);
    if (SrcNId != DstNId && Graph.AddEdge(SrcNId, DstNId) != -2) { // is new edge
      if (! IsDir) { Graph.AddEdge(DstNId, SrcNId); }
      edge++;
    }
  }
  return GraphPt;
}

namespace TSnapDetail {
/// Returns a random edge in a graph Graph where the edge does not touch nodes NId1 and NId2.
template <class PGraph>
TInt64Pr GetRndEdgeNonAdjNode(const PGraph& Graph, int64 NId1, int64 NId2) {
  typename PGraph::TObj::TNodeI NI1, NI2;
  const int NNodes = Graph->GetNodes();
  int64 OutDeg = -1;
  int iter = 0;
  do {
    NI1 = Graph->GetRndNI();
    OutDeg = NI1.GetOutDeg();
    if (iter++ >= NNodes*2) { return TInt64Pr(-1, -1); }
  } while (OutDeg == 0);
  NI2 = Graph->GetNI(NI1.GetOutNId(TInt::Rnd.GetUniDevInt(OutDeg)));
  int64 runs = 0;
  while (NI1.IsNbrNId(NId1) || NI1.IsNbrNId(NId2) || NI2.IsNbrNId(NId1) || NI2.IsNbrNId(NId2) || NI1.GetId()==NI2.GetId()) {
    int iter = 0;
    do {
      NI1 = Graph->GetRndNI();
      OutDeg = NI1.GetOutDeg();
      if (iter++ >= NNodes*2) { return TInt64Pr(-1, -1); }
    } while (OutDeg == 0);
    NI2 = Graph->GetNI(NI1.GetOutNId(TInt::Rnd.GetUniDevInt(OutDeg)));
    if (runs++ >= 1000) { return TInt64Pr(-1, -1); }
  }
  return TInt64Pr(NI1.GetId(), NI2.GetId());
}

} // namespace TSnapDetail

}; // namespace TSnap
