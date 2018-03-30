/*! \file subgraph.h
    \brief Functions and templates to generate subgraphs.
*/

/// Main namespace for all the Snap global entities.
namespace TSnap {

/////////////////////////////////////////////////
// Convert between graph types and get subgraphs

// node subgraphs
/// Returns an induced subgraph of graph Graph with NIdV nodes. ##TSnap::GetSubGraph
template<class PGraph> PGraph GetSubGraph(const PGraph& Graph, const TInt64V& NIdV);
/// Returns an induced subgraph of an undirected graph Graph with NIdV nodes with an optional node renumbering. ##TSnap::GetSubGraph-1
PUNGraph GetSubGraph(const PUNGraph& Graph, const TInt64V& NIdV, const bool& RenumberNodes=false);
// Returns an induced subgraph of a directed graph Graph with NIdV nodes with an optional node renumbering. ##TSnap::GetSubGraph-2
PNGraph GetSubGraph(const PNGraph& Graph, const TInt64V& NIdV, const bool& RenumberNodes=false);
// TODO ROK 2012/08/15 PNGraph GetSubGraph is not documented by doxygen.
//  It is combined with PUNGraph GetSubGraph.

// edge subgraphs
/// Returns a subgraph of graph Graph with EIdV edges. ##TSnap::GetESubGraph
template<class PGraph> PGraph GetESubGraph(const PGraph& Graph, const TInt64V& EIdV);
// Returns a subgraph of graph Graph with EdgeV edges. ##TSnap::GetESubGraph-1
template<class PGraph> PGraph GetESubGraph(const PGraph& Graph, const TIntPr64V& EdgeV);
// TODO ROK 2012/08/15 PGraph GetESubGraph TIntPrV is not documented by doxygen.
//  It is combined with PGraph GetESubGraph TIntV.
/// Returns a subgraph of graph Graph with edges where edge data matches the parameters. ##TSnap::GetEDatSubGraph
template<class PGraph, class TEdgeDat> PGraph GetEDatSubGraph(const PGraph& Graph, const TEdgeDat& EDat, const int64& Cmp);
/// Returns a subgraph of graph Graph with NIdV nodes and edges where edge data matches the parameters. ##TSnap::GetEDatSubGraph-1
template<class PGraph, class TEdgeDat> PGraph GetEDatSubGraph(const PGraph& Graph, const TInt64V& NIdV, const TEdgeDat& EDat, const int64& Cmp);

// convert between the graphs. Does NOT copy the data
/// Performs conversion of graph InGraph with an optional node renumbering. ##TSnap::ConvertGraph
template<class POutGraph, class PInGraph> POutGraph ConvertGraph(const PInGraph& InGraph, const bool& RenumberNodes=false);
// convert between the graphs. Does NOT copy the data
/// Performs conversion of graph InGraph with an optional node renumbering. ##TSnap::ConvertGraphFast
template<class POutGraph, class PInGraph> POutGraph ConvertGraphFast(const PInGraph& InGraph, const bool& RenumberNodes=false);
/// Performs conversion of graph InGraph with an optional node renumbering. ##TSnap::ConvertMultiGraph
template<class POutGraph, class PInGraph> POutGraph ConvertMultiGraph(const PInGraph& InGraph, const bool& RenumberNodes=false);
/// Performs conversion of TTable InTable to graph ##TSnap::ConvertGraphTable
template<class POutGraph, class PInTable> POutGraph ConvertGraphTable(const PInTable& InTable, const TStr64V& ColNames);
/// Returns an induced subgraph of graph InGraph with NIdV nodes with an optional node renumbering. ##TSnap::ConvertSubGraph
template<class POutGraph, class PInGraph> POutGraph ConvertSubGraph(const PInGraph& InGraph, const TInt64V& NIdV, const bool& RenumberNodes=false);
// TODO RS 2012/08/14 find out why TSnap::ConvertSubGraph<PUNGraph>(NGraph, NIdV, true) aborts
/// Returns a subgraph of graph InGraph with EIdV edges with an optional node renumbering. ##TSnap::ConvertESubGraph
template<class POutGraph, class PInGraph> POutGraph ConvertESubGraph(const PInGraph& InGraph, const TInt64V& EIdV, const bool& RenumberNodes=false);
// does not work on multigraphs

// get random (induced) subgraphs 
/// Returns an induced random subgraph of graph Graph with NNodes nodes. ##TSnap::GetRndSubGraph
template<class PGraph> PGraph GetRndSubGraph(const PGraph& Graph, const int64& NNodes);
/// Returns a random subgraph of graph Graph with NEdges edges. ##TSnap::GetRndESubGraph
template<class PGraph> PGraph GetRndESubGraph(const PGraph& Graph, const int64& NEdges);

// get egonet of a center node
/// Returns the egonet of node CtrNId as center in undirected graph Graph. And returns number of edges around the egonet.
PUNGraph GetEgonet(const PUNGraph& Graph, const int64 CtrNId, int64& ArndEdges);
/// Returns the egonet of node CtrNId as center in directed graph Graph. And returns number of edges go in and out the egonet.
PNGraph GetEgonet(const PNGraph& Graph, const int64 CtrNId, int64& InEdges, int64& OutEdges);

/////////////////////////////////////////////////
// Implementation
namespace TSnapDetail {
// Slow for small sub-graphs as it traverses all the edges of Graph
template <class PGraph, bool IsMultiGraph>
struct TGetSubGraph {
  static PGraph Do(const PGraph& Graph, const TInt64V& NIdV) {
    PGraph NewGraphPt = PGraph::TObj::New();
    typename PGraph::TObj& NewGraph = *NewGraphPt;
    NewGraph.Reserve(NIdV.Len(), -1);
    for (int64 n = 0; n < NIdV.Len(); n++) {
      if (Graph->IsNode(NIdV[n])) {
        NewGraph.AddNode(Graph->GetNI(NIdV[n])); }
    }
    for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
      if (NewGraph.IsNode(EI.GetSrcNId()) && NewGraph.IsNode(EI.GetDstNId())) {
        NewGraph.AddEdge(EI); }
    }
    NewGraph.Defrag();
    return NewGraphPt;
  }
};

template <class PGraph> 
struct TGetSubGraph<PGraph, false> { // not multigraph
  static PGraph Do(const PGraph& Graph, const TInt64V& NIdV) {
    CAssert(! HasGraphFlag(typename PGraph::TObj, gfMultiGraph));
    PGraph NewGraphPt = PGraph::TObj::New();
    typename PGraph::TObj& NewGraph = *NewGraphPt;
    NewGraph.Reserve(NIdV.Len(), -1);
    TInt64Set NodeSet;
    for (int64 n = 0; n < NIdV.Len(); n++) {
      if (! HasGraphFlag(typename PGraph::TObj, gfNodeDat)) {
        if (Graph->IsNode(NIdV[n])) { NewGraph.AddNode(NIdV[n]); NodeSet.AddKey(NIdV[n]); } }
      else {
        if (Graph->IsNode(NIdV[n])) { NewGraph.AddNode(Graph->GetNI(NIdV[n])); NodeSet.AddKey(NIdV[n]); } }
    }
    for (int64 n = 0; n < NodeSet.Len(); n++) {
      const int64 SrcNId = NodeSet[n];
      const typename PGraph::TObj::TNodeI NI = Graph->GetNI(SrcNId);
      for (int64 edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int64 OutNId = NI.GetOutNId(edge);
        if (NewGraph.IsNode(OutNId)) {
          if (! HasGraphFlag(typename PGraph::TObj, gfEdgeDat)) { 
            NewGraph.AddEdge(SrcNId, OutNId); }
          else { 
            NewGraph.AddEdge(Graph->GetEI(SrcNId, OutNId)); } // also copy data
        }
      }
    }
    NewGraph.Defrag();
    return NewGraphPt;
  }
};
}; // TSnapDetail

template<class PGraph> 
PGraph GetSubGraph(const PGraph& Graph, const TInt64V& NIdV) {
  return TSnapDetail::TGetSubGraph<PGraph, HasGraphFlag(typename PGraph::TObj, gfMultiGraph)>
    ::Do(Graph, NIdV);
}

template<class PGraph> 
PGraph GetESubGraph(const PGraph& Graph, const TInt64V& EIdV) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfMultiGraph));
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  NewGraph.Reserve(-1, EIdV.Len());
  for (int64 edge = 0; edge < EIdV.Len(); edge++) {
    const int64 EId = EIdV[edge];
    IAssert(Graph->IsEdge(EId));
    const typename PGraph::TObj::TEdgeI EI = Graph->GetEI(EId);
    if (! NewGraph.IsNode(EI.GetSrcNId())) {
      NewGraph.AddNode(Graph->GetNI(EI.GetSrcNId())); 
    }
    if (! NewGraph.IsNode(EI.GetDstNId())) {
      NewGraph.AddNode(Graph->GetNI(EI.GetDstNId())); 
    }
    NewGraph.AddEdge(EI);
  }
  return NewGraphPt;
}

template<class PGraph> 
PGraph GetESubGraph(const PGraph& Graph, const TIntPr64V& EdgeV) {
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  NewGraph.Reserve(-1, EdgeV.Len());
  for (int64 edge = 0; edge < EdgeV.Len(); edge++) {
    const int64 SrcNId = EdgeV[edge].Val1;
    const int64 DstNId = EdgeV[edge].Val2;
    const typename PGraph::TObj::TEdgeI EI = Graph->GetEI(SrcNId, DstNId);
    if (! NewGraph.IsNode(EI.GetSrcNId())) {
      NewGraph.AddNode(Graph->GetNI(EI.GetSrcNId())); 
    }
    if (! NewGraph.IsNode(EI.GetDstNId())) {
      NewGraph.AddNode(Graph->GetNI(EI.GetDstNId())); 
    }
    NewGraph.AddEdge(EI);
  }
  return NewGraphPt;
}

// Get a subgraph on NIdV nodes, where edge data is Cmp (-1:less, 0:equal, 1:greater) than EDat
template<class PGraph, class TEdgeDat> 
PGraph GetEDatSubGraph(const PGraph& Graph, const TEdgeDat& EDat, const int64& Cmp) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfEdgeDat));
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    if ((Cmp==1 && EI()>EDat) || (Cmp==-1 && EI()<EDat) || (Cmp==0 && EI()==EDat)) {
      if (! NewGraph.IsNode(EI.GetSrcNId())) {
        NewGraph.AddNode(Graph->GetNI(EI.GetSrcNId()));
      }
      if (! NewGraph.IsNode(EI.GetDstNId())) {
        NewGraph.AddNode(Graph->GetNI(EI.GetDstNId()));
      }
      NewGraph.AddEdge(EI); 
    }
  }
  return NewGraphPt;
}

// Get a subgraph on NIdV nodes, where edge data is Cmp (-1:less, 0:equal, 1:greater) than EDat
template<class PGraph, class TEdgeDat> 
PGraph GetEDatSubGraph(const PGraph& Graph, const TInt64V& NIdV, const TEdgeDat& EDat, const int64& Cmp) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfEdgeDat));
  PGraph NewGraphPt = PGraph::TObj::New();
  typename PGraph::TObj& NewGraph = *NewGraphPt;
  NewGraph.Reserve(NIdV.Len(), -1);
  for (int64 n = 0; n < NIdV.Len(); n++) {
    NewGraph.AddNode(Graph->GetNI(NIdV[n])); 
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    if (NewGraph.IsNode(EI.GetSrcNId()) && NewGraph.IsNode(EI.GetDstNId()) &&
     ((Cmp==1 && EI()>EDat)|| (Cmp==-1 && EI()<EDat) || (Cmp==0 && EI()==EDat))) {
      NewGraph.AddEdge(EI); }
  }
  NewGraph.Defrag();
  return NewGraphPt;
}

// Converts between different types of graphs/networks
// Node/edge data is not copied between the graphs.
template<class POutGraph, class PInGraph> 
POutGraph ConvertGraph(const PInGraph& InGraph, const bool& RenumberNodes) {
  POutGraph OutGraphPt = POutGraph::TObj::New();
  typename POutGraph::TObj& OutGraph = *OutGraphPt;
  OutGraph.Reserve(InGraph->GetNodes(), InGraph->GetEdges());
  if (! RenumberNodes) {
    for (typename PInGraph::TObj::TNodeI NI = InGraph->BegNI(); NI < InGraph->EndNI(); NI++) {
      OutGraph.AddNode(NI.GetId());
    }
    for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
      OutGraph.AddEdge(EI.GetSrcNId(), EI.GetDstNId());
      if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) { // add edge in the other direction
        OutGraph.AddEdge(EI.GetDstNId(), EI.GetSrcNId()); }
    }
  } else { // renumber nodes so that node ids are 0...N-1
    TInt64Set NIdSet(InGraph->GetNodes());
    for (typename PInGraph::TObj::TNodeI NI = InGraph->BegNI(); NI < InGraph->EndNI(); NI++) {
      const int64 nid = NIdSet.AddKey(NI.GetId());
      OutGraph.AddNode(nid);
    }
    for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
      const int64 SrcNId = NIdSet.GetKeyId(EI.GetSrcNId());
      const int64 DstNId = NIdSet.GetKeyId(EI.GetDstNId());
      OutGraph.AddEdge(SrcNId, DstNId);
      if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) {
        OutGraph.AddEdge(DstNId, SrcNId); }
    }
  }
  //OutGraph.Defrag();
  return OutGraphPt;
}

// Converts between different types of graphs/networks
// Node/edge data is not copied between the graphs.
template<class POutGraph, class PInGraph> 
POutGraph ConvertGraphFast(const PInGraph& InGraph, const bool& RenumberNodes) {
  POutGraph OutGraphPt = POutGraph::TObj::New();
  typename POutGraph::TObj& OutGraph = *OutGraphPt;
  OutGraph.Reserve(InGraph->GetNodes(), InGraph->GetEdges());

  if (! RenumberNodes) {
    for (typename PInGraph::TObj::TNodeI NI = InGraph->BegNI(); NI < InGraph->EndNI(); NI++) {
      // Note that we don't distinguish between directed or undirected graphs
      // That logic is handled by the implementation of AddNodeWithNeighbors
      OutGraph.AddNodeWithNeighbors(NI.GetId(), NI.GetInNIdV(), NI.GetOutNIdV());
    }
  } else { // renumber nodes so that node ids are 0...N-1 */
    TInt64Set NIdSet(InGraph->GetNodes());
    for (typename PInGraph::TObj::TNodeI NI = InGraph->BegNI(); NI < InGraph->EndNI(); NI++) {
      NIdSet.AddKey(NI.GetId());
    }
    for (typename PInGraph::TObj::TNodeI NI = InGraph->BegNI(); NI < InGraph->EndNI(); NI++) {
      //const int64 nid = NIdSet.GetKeyId(NI.GetId());
   
      // TODO: Renumber the ids on the edge vectors as well.

    }
  }

  return OutGraphPt;
}

// Converts graphs to multigraph networks
// Node/edge data is not copied between the graphs.
template<class POutGraph, class PInGraph> 
POutGraph ConvertMultiGraph(const PInGraph& InGraph, const bool& RenumberNodes) {
  POutGraph OutGraphPt = POutGraph::TObj::New();
  typename POutGraph::TObj& OutGraph = *OutGraphPt;
  OutGraph.Reserve(InGraph->GetNodes(), InGraph->GetEdges());

  if (! RenumberNodes) {
    for (typename PInGraph::TObj::TNodeI NI = InGraph->BegNI(); NI < InGraph->EndNI(); NI++) {
      OutGraph.AddNode(NI.GetId());
    }
    for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
      OutGraph.AddEdgeUnsorted(EI.GetSrcNId(), EI.GetDstNId());
      if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) { // add edge in the other direction
        OutGraph.AddEdgeUnsorted(EI.GetDstNId(), EI.GetSrcNId());
      }
    }
  } else {

    // TODO: Node renumbers
  }

  return OutGraphPt;
}

template <class POutGraph>
POutGraph ConvertGraphTable(const PTable& InTable, const TStr64V& ColNames) {

  POutGraph OutGraphPt = POutGraph::TObj::New();
  typename POutGraph::TObj& OutGraph = *OutGraphPt;

  TTable TableByDst(*InTable);

  TStr64V ReversedColNames;
  ReversedColNames.Add(ColNames[1]);
  ReversedColNames.Add(ColNames[0]);

  InTable->Order(ColNames);
  TableByDst.Order(ReversedColNames);

  TRowIterator RI = InTable->BegRI();
  TRowIterator DstRI = TableByDst.BegRI();
  TInt64 ColIdx = 0;
  while (RI != InTable->EndRI() && DstRI != TableByDst.EndRI()) {
    // Get the first NId
    TInt64 NId = RI.GetIntAttr(ColIdx);
    if (DstRI.GetIntAttr(ColIdx+1) < NId) {
      NId = DstRI.GetIntAttr(ColIdx+1);
    }

    TInt64V InNIdV;
    TInt64V OutNIdV;
    // Fill in the Node Id vectors for "NId"
    while (DstRI.GetIntAttr(ColIdx+1) == NId) {
      InNIdV.Add(DstRI.GetIntAttr(ColIdx));
      DstRI++;
    }

    while (RI.GetIntAttr(ColIdx) == NId) {
      OutNIdV.Add(RI.GetIntAttr(ColIdx+1));
      RI++;
    }
    // Add the nodes with edge vectors to the OutGraph
    OutGraph.AddNodeWithNeighbors(NId, InNIdV, OutNIdV);
  }

  // The method below does not uses AddNodeWithNeighbors and 
  // AddEdge should uses TVec::AddFastSorted
  //
  /* TRowIterator RI = InTable->BegRI(); */
  /* TInt64 ColIdx = 0; */
  /* for (; RI != InTable->EndRI(); RI++) { */
  /*   TInt64 Src = RI.GetIntAttr(ColIdx); */
  /*   TInt64 Dst = RI.GetIntAttr(ColIdx+1); */
  /*   if (!OutGraph.IsNode(Src)) { OutGraph.AddNode(Src); } */
  /*   if (!OutGraph.IsNode(Dst)) { OutGraph.AddNode(Dst); } */
  /*   OutGraph.AddEdge(Src, Dst); */
  /* } */
  
  return OutGraphPt;

}

namespace TSnapDetail {
// Slow for small sub-graphs as it traverses all the edges of Graph
template <class POutGraph, class PInGraph, bool IsMultiGraph>
struct TConvertSubGraph {
  static POutGraph Do(const PInGraph& InGraph, const TInt64V& NIdV, const bool& RenumberNodes) {
    POutGraph OutGraphPt = POutGraph::TObj::New();
    typename POutGraph::TObj& OutGraph = *OutGraphPt;
    if (! RenumberNodes) {
      for (int64 n = 0; n < NIdV.Len(); n++) {
        OutGraph.AddNode(NIdV[n]);
      }
      for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
        if (! OutGraph.IsNode(EI.GetSrcNId()) || ! OutGraph.IsNode(EI.GetDstNId())) { continue; }
        OutGraph.AddEdge(EI.GetSrcNId(), EI.GetDstNId());
        if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) {
          OutGraph.AddEdge(EI.GetDstNId(), EI.GetSrcNId());
        }
      }
    } else { // renumber nodes so that node ids are 0...N-1
      TInt64Set NIdSet(InGraph->GetNodes());
      for (int64 n = 0; n < NIdV.Len(); n++) {
        const int64 NId = NIdSet.AddKey(NIdV[n]);
        OutGraph.AddNode(NId);
      }
      for (typename PInGraph::TObj::TEdgeI EI = InGraph->BegEI(); EI < InGraph->EndEI(); EI++) {
        const int64 SrcNId = NIdSet.GetKeyId(EI.GetSrcNId());
        const int64 DstNId = NIdSet.GetKeyId(EI.GetDstNId());
        if (! OutGraph.IsNode(SrcNId) || ! OutGraph.IsNode(DstNId)) { continue; }
        OutGraph.AddEdge(SrcNId, DstNId);
        if (! HasGraphFlag(typename PInGraph::TObj, gfDirected) && HasGraphFlag(typename POutGraph::TObj, gfDirected)) {
          OutGraph.AddEdge(DstNId, SrcNId);
        }
      }
    }
    OutGraph.Defrag();
    return OutGraphPt;
  }
};

template <class POutGraph, class PInGraph>
struct TConvertSubGraph<POutGraph, PInGraph, false> { // InGraph is not multigraph
  static POutGraph Do(const PInGraph& InGraph, const TInt64V& NIdV, const bool& RenumberNodes) {
    POutGraph OutGraphPt = POutGraph::TObj::New();
    typename POutGraph::TObj& OutGraph = *OutGraphPt;
    if (! RenumberNodes) {
      for (int64 n = 0; n < NIdV.Len(); n++) {
        OutGraph.AddNode(NIdV[n]); }
      for (int64 n = 0; n < NIdV.Len(); n++) {
        typename PInGraph::TObj::TNodeI NI = InGraph->GetNI(NIdV[n]);
        for (int64 e = 0; e < NI.GetOutDeg(); e++) {
          const int64 dst = NI.GetOutNId(e);
          if (! OutGraph.IsNode(dst)) { continue; }
          OutGraph.AddEdge(NIdV[n], dst);
        }
      }
    } else { // renumber nodes so that node ids are 0...N-1
      TInt64Set NIdSet(InGraph->GetNodes());
      for (int64 n = 0; n < NIdV.Len(); n++) {
        const int64 NId = NIdSet.AddKey(NIdV[n]);
        OutGraph.AddNode(NId);
      }
      for (int64 n = 0; n < NIdV.Len(); n++) {
        typename PInGraph::TObj::TNodeI NI = InGraph->GetNI(NIdV[n]);
        const int64 src = NIdSet.GetKey(NIdV[n]);
        for (int64 e = 0; e < NI.GetOutDeg(); e++) {
          const int64 dst = NIdSet.GetKey(NI.GetOutNId(e));
          if (! OutGraph.IsNode(dst)) { continue; }
          OutGraph.AddEdge(src, dst);
        }
      }
    }
    OutGraph.Defrag();
    return OutGraphPt;
  }
};
} // TSnapDetail

// May be slow as it traverses all the edges of the in-graph to create the sub-graph
template<class POutGraph, class PInGraph> 
POutGraph ConvertSubGraph(const PInGraph& InGraph, const TInt64V& NIdV, const bool& RenumberNodes) {
  return TSnapDetail::TConvertSubGraph<POutGraph, PInGraph, HasGraphFlag(typename PInGraph::TObj, gfMultiGraph)>::Do(InGraph, NIdV, RenumberNodes);
}

template<class POutGraph, class PInGraph> 
POutGraph ConvertESubGraph(const PInGraph& InGraph, const TInt64V& EIdV, const bool& RenumberNodes) {
  CAssert(HasGraphFlag(typename PInGraph::TObj, gfMultiGraph)); // needs to have explicit edges
  POutGraph NewGraphPt = POutGraph::TObj::New();
  typename POutGraph::TObj& NewGraph = *NewGraphPt;
  NewGraph.Reserve(-1, EIdV.Len());
  if (! RenumberNodes) {
    for (int64 edge = 0; edge < EIdV.Len(); edge++) {
      const int64 EId = EIdV[edge];
      IAssert(InGraph->IsEdge(EId));
      const typename PInGraph::TObj::TEdgeI EI = InGraph->GetEI(EId);
      const int64 SrcNId = EI.GetSrcNId();
      const int64 DstNId = EI.GetDstNId();
      if (! NewGraph.IsNode(SrcNId)) {
        NewGraph.AddNode(SrcNId); }
      if (! NewGraph.IsNode(DstNId)) {
        NewGraph.AddNode(DstNId); }
      NewGraph.AddEdge(SrcNId, DstNId);
    }
  } else {
    // renumber nodes so that node ids are 0...N-1
    TInt64Set NIdSet(InGraph->GetNodes());
    for (int64 edge = 0; edge < EIdV.Len(); edge++) {
      const int64 EId = EIdV[edge];
      IAssert(InGraph->IsEdge(EId));
      const typename PInGraph::TObj::TEdgeI EI = InGraph->GetEI(EId);
      const int64 SrcNId = NIdSet.AddKey(EI.GetSrcNId()); // map node ids
      const int64 DstNId = NIdSet.AddKey(EI.GetDstNId());
      if (! NewGraph.IsNode(SrcNId)) {
        NewGraph.AddNode(SrcNId); }
      if (! NewGraph.IsNode(DstNId)) {
        NewGraph.AddNode(DstNId); }
      NewGraph.AddEdge(SrcNId, DstNId);
    }
  }
  return NewGraphPt;
}

template<class PGraph> 
PGraph GetRndSubGraph(const PGraph& Graph, const int64& NNodes) {
  IAssert(NNodes <= Graph->GetNodes());
  TInt64V NIdV;
  Graph->GetNIdV(NIdV);
  NIdV.Shuffle(TInt::Rnd);
  NIdV.Del(NNodes, NIdV.Len()-1);
  IAssert(NIdV.Len() == NNodes);
  return GetSubGraph(Graph, NIdV);
}

template<class PGraph> 
PGraph GetRndESubGraph(const PGraph& Graph, const int64& NEdges) {
  CAssert(! HasGraphFlag(typename PGraph::TObj, gfMultiGraph));
  TIntPr64V EdgeV(Graph->GetEdges(), 0);
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    EdgeV.Add(TInt64Pr(EI.GetSrcNId(), EI.GetDstNId()));
  }
  EdgeV.Shuffle(TInt::Rnd);
  EdgeV.Del(NEdges, EdgeV.Len()-1);
  IAssert(EdgeV.Len() == NEdges);
  return GetESubGraph(Graph, EdgeV);
}

} // namespace TSnap
