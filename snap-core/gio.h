/////////////////////////////////////////////////
// Loading and saving graphs from/to various file formats.
namespace TSnap {

//const TStr EDGES_START("#EDGES");
const TStr EDGES_START = ("#EDGES");
const TStr NODES_START = ("#NODES");
const TStr END_SENTINEL = ("#END");
const TStr SRC_ID_NAME = ("SrcNId");
const TStr DST_ID_NAME = ("DstNId");
const TStr NID_NAME = ("NId");
const TStr INT_TYPE_PREFIX = ("Int");
const TStr FLT_TYPE_PREFIX = ("Flt");
const TStr STR_TYPE_PREFIX = ("Str");
const TStr NULL_VAL = ("__null__");

/// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 edge per line (whitespace separated columns, integer node ids).
template <class PGraph> PGraph LoadEdgeList(const TStr& InFNm, const int64& SrcColId=0, const int64& DstColId=1);
/// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 edge per line ('Separator' separated columns, integer node ids).
template <class PGraph> PGraph LoadEdgeList(const TStr& InFNm, const int64& SrcColId, const int64& DstColId, const char& Separator);
/// Loads a network from the text file InFNm with 1 node/edge per line ('Separator' separated columns, integer node id(s) + node/edge attributes).
PNEANet LoadEdgeListNet(const TStr& InFNm, const char& Separator);
/// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 edge per line (whitespace separated columns, arbitrary string node ids).
template <class PGraph> PGraph LoadEdgeListStr(const TStr& InFNm, const int64& SrcColId=0, const int64& DstColId=1);
/// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 edge per line (whitespace separated columns, arbitrary string node ids).
template <class PGraph> PGraph LoadEdgeListStr(const TStr& InFNm, const int64& SrcColId, const int64& DstColId, TStrHash<TInt64, TStrPool, int64>& StrToNIdH);
/// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 node and all its edges in a single line.
template <class PGraph> PGraph LoadConnList(const TStr& InFNm);
/// Loads a (directed, undirected or multi) graph from a text file InFNm with 1 node and all its edges in a single line.
template <class PGraph> PGraph LoadConnListStr(const TStr& InFNm, TStrHash<TInt64, TStrPool, int64>& StrToNIdH);

/// Loads a (directed, undirected or multi) graph from Pajek .PAJ format file. ##LoadPajek
template <class PGraph> PGraph LoadPajek(const TStr& InFNm);
/// Loads a directed network in the DyNetML format. Loads only the first network in the file FNm.
PNGraph LoadDyNet(const TStr& FNm);
/// Loads directed networks in the DyNetML format. Loads all the networks in the file FNm.
TVec<PNGraph> LoadDyNetGraphV(const TStr& FNm);

//TODO: Sparse/Dense adjacency matrix which values we threshold at Thresh to obtain an adjacency matrix.
//template <class PGraph> PGraph LoadAdjMtx(const TStr& FNm, const int Thresh);
//TODO: Load from a GML file format (http://en.wikipedia.org/wiki/Graph_Modelling_Language)
//template <class PGraph> PGraph LoadGml(const TStr& FNm, const int Thresh);


/// Saves a graph into a text file. Each line contains two columns and encodes a single edge: <source node id><tab><destination node id>
template <class PGraph> void SaveEdgeList(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc=TStr());
/// Saves a network into a text file. Each line encodes either an edge or a node, along with its attributes.
void SaveEdgeListNet(const PNEANet& Graph, const TStr& OutFNm, const TStr& Desc);
/// Saves a graph in a Pajek .NET format.
template <class PGraph> void SavePajek(const PGraph& Graph, const TStr& OutFNm);
/// Saves a graph in a Pajek .NET format.
template <class PGraph> void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStr64H& NIdColorH);
/// Saves a graph in a Pajek .NET format.
template <class PGraph> void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStr64H& NIdColorH, const TIntStr64H& NIdLabelH);
/// Saves a graph in a Pajek .NET format.
template <class PGraph> void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStr64H& NIdColorH, const TIntStr64H& NIdLabelH, const TIntStr64H& EIdColorH);
/// Saves a graph in a MATLAB sparse matrix format.
template <class PGraph> void SaveMatlabSparseMtx(const PGraph& Graph, const TStr& OutFNm);
/// Save a graph in GraphVizp .DOT format. ##SaveGViz_NIdColorH
template<class PGraph> void SaveGViz(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc=TStr(), const bool& NodeLabels=false, const TIntStr64H& NIdColorH=TIntStr64H());
/// Save a graph in GraphVizp .DOT format.  ##SaveGViz_NIdLabelH
template<class PGraph> void SaveGViz(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc, const TIntStr64H& NIdLabelH);

//TODO:  Save to a GML file format (http://en.wikipedia.org/wiki/Graph_Modelling_Language)
//template <class PGraph> SaveGml(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc);

/////////////////////////////////////////////////
// Implementation

/// Loads the format saved by TSnap::SaveEdgeList() ##LoadEdgeList
template <class PGraph>
PGraph LoadEdgeList(const TStr& InFNm, const int64& SrcColId, const int64& DstColId) {
  TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
  PGraph Graph = PGraph::TObj::New();
  int64 SrcNId, DstNId;

  while (Ss.Next()) {
    if (! Ss.GetInt64(SrcColId, SrcNId) || ! Ss.GetInt64(DstColId, DstNId)) { continue; }
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    Graph->AddEdge(SrcNId, DstNId);
  }
  Graph->Defrag();
  return Graph;
}

/// Loads the format saved by TSnap::SaveEdgeList() if we set Separator='\t'. ##LoadEdgeList_Separator
template <class PGraph>
PGraph LoadEdgeList(const TStr& InFNm, const int64& SrcColId, const int64& DstColId, const char& Separator) {
  TSsParser Ss(InFNm, Separator);
  PGraph Graph = PGraph::TObj::New();
  int64 SrcNId, DstNId;
  while (Ss.Next()) {
    if (! Ss.GetInt64(SrcColId, SrcNId) || ! Ss.GetInt64(DstColId, DstNId)) { continue; }
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    Graph->AddEdge(SrcNId, DstNId);
  }
  Graph->Defrag();
  return Graph;
}

/// Loads the format saved by TSnap::SaveEdgeList(), where node IDs are strings ##LoadEdgeListStr
template <class PGraph>
PGraph LoadEdgeListStr(const TStr& InFNm, const int64& SrcColId, const int64& DstColId) {
  TSsParser Ss(InFNm, ssfWhiteSep);
  PGraph Graph = PGraph::TObj::New();
  TStrHash<TInt64, TStrPool, int64> StrToNIdH(Mega(1), true); // hash-table mapping strings to integer node ids
  while (Ss.Next()) {
    const int64 SrcNId = StrToNIdH.AddKey(Ss[SrcColId]);
    const int64 DstNId = StrToNIdH.AddKey(Ss[DstColId]);
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    Graph->AddEdge(SrcNId, DstNId);
  }
  Graph->Defrag();
  return Graph;
}

/// Loads the format saved by TSnap::SaveEdgeList(), where node IDs are strings and mapping of strings to node ids are stored ##LoadEdgeListStr_StrToNIdH
template <class PGraph>
PGraph LoadEdgeListStr(const TStr& InFNm, const int64& SrcColId, const int64& DstColId, TStrHash<TInt64, TStrPool, int64>& StrToNIdH) {
  TSsParser Ss(InFNm, ssfWhiteSep);
  PGraph Graph = PGraph::TObj::New();
  while (Ss.Next()) {
    const int64 SrcNId = StrToNIdH.AddKey(Ss[SrcColId]);
    const int64 DstNId = StrToNIdH.AddKey(Ss[DstColId]);
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    Graph->AddEdge(SrcNId, DstNId);
  }
  Graph->Defrag();
  return Graph;
}

/// Loads Whitespace separated file of several columns: <source node id> <destination node id1> <destination node id2> ##LoadConnList
template <class PGraph>
PGraph LoadConnList(const TStr& InFNm) {
  TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
  PGraph Graph = PGraph::TObj::New();
  while (Ss.Next()) {
    if (! Ss.IsInt64(0)) { continue; }
    const int64 SrcNId = Ss.GetInt64(0);
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    for (int64 dst = 1; dst < Ss.Len(); dst++) {
      const int64 DstNId = Ss.GetInt64(dst);
      if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
      Graph->AddEdge(SrcNId, DstNId);
    }
  }
  Graph->Defrag();
  return Graph;
}

/// Loads Whitespace separated file of several columns: <source node id> <destination node id1> <destination node id2>, with a mapping of strings to node IDs. ##LoadConnListStr
template <class PGraph> 
PGraph LoadConnListStr(const TStr& InFNm, TStrHash<TInt64, TStrPool, int64>& StrToNIdH) {
  TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
  PGraph Graph = PGraph::TObj::New();
  while (Ss.Next()) {
    const int64 SrcNId = StrToNIdH.AddDatId(Ss[0]);
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    for (int64 dst = 1; dst < Ss.Len(); dst++) {
      const int64 DstNId = StrToNIdH.AddDatId(Ss[dst]);
      if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
      Graph->AddEdge(SrcNId, DstNId);
    }
  }
  Graph->Defrag();
  return Graph;
}

template <class PGraph>
PGraph LoadPajek(const TStr& InFNm) {
  PGraph Graph = PGraph::TObj::New();
  TSsParser Ss(InFNm, ssfSpaceSep, true, true, true);
  while ((Ss.Len()==0 || strstr(Ss[0], "*vertices") == NULL) && ! Ss.Eof()) {
    Ss.Next();  Ss.ToLc(); }
  // nodes
  bool EdgeList = true;
  EAssert(strstr(Ss[0], "*vertices") != NULL);
  while (Ss.Next()) {
    Ss.ToLc();
    if (Ss.Len()>0 && Ss[0][0] == '%') { continue; } // comment
    if (strstr(Ss[0], "*arcslist")!=NULL || strstr(Ss[0],"*edgeslist")!=NULL) { EdgeList=false; break; } 
    if (strstr(Ss[0], "*arcs")!=NULL || strstr(Ss[0],"*edges")!=NULL) { break; } // arcs are directed, edges are undirected
    Graph->AddNode(Ss.GetInt64(0));
  }
  // edges
  while (Ss.Next()) {
    if (Ss.Len()>0 && Ss[0][0] == '%') { continue; } // comment
    if (Ss.Len()>0 && Ss[0][0] == '*') { break; }
    if (EdgeList) {
      // <source> <destination> <weight>
      if (Ss.Len() >= 3 && Ss.IsInt64(0) && Ss.IsInt64(1)) {
        Graph->AddEdge(Ss.GetInt64(0), Ss.GetInt64(1)); }
    } else {
      // <source> <destination1> <destination2> <destination3> ...
      const int64 SrcNId = Ss.GetInt64(0);
      for (int64 i = 1; i < Ss.Len(); i++) {
        Graph->AddEdge(SrcNId, Ss.GetInt64(i)); }
    }
  }
  return Graph;
}

template <class PGraph>
void SaveEdgeList(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc) {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) { fprintf(F, "# Directed graph: %s \n", OutFNm.CStr()); } 
  else { fprintf(F, "# Undirected graph (each unordered pair of nodes is saved once): %s\n", OutFNm.CStr()); }
  if (! Desc.Empty()) { fprintf(F, "# %s\n", Desc.CStr()); }
  fprintf(F, "# Nodes: %s Edges: %s\n", TInt64::GetStr(Graph->GetNodes()).CStr(), TInt64::GetStr(Graph->GetEdges()).CStr());
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) { fprintf(F, "# FromNodeId\tToNodeId\n"); }
  else { fprintf(F, "# NodeId\tNodeId\n"); }
  for (typename PGraph::TObj::TEdgeI ei = Graph->BegEI(); ei < Graph->EndEI(); ei++) {
    fprintf(F, "%s\t%s\n", TInt64::GetStr(ei.GetSrcNId()).CStr(), TInt64::GetStr(ei.GetDstNId()).CStr());
  }
  fclose(F);
}

template <class PGraph>
void SavePajek(const PGraph& Graph, const TStr& OutFNm) {
  TInt64H NIdToIdH(Graph->GetNodes(), true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "*Vertices %lld\n", Graph->GetNodes());
  int64 i = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, i++) {
    fprintf(F, "%lld  \"%lld\" ic Red fos 10\n", i+1, NI.GetId()); // ic: internal color, fos: font size
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
    fprintf(F, "*Arcs %lld\n", Graph->GetEdges()); } // arcs are directed, edges are undirected
  else {
    fprintf(F, "*Edges %lld\n", Graph->GetEdges());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int64 SrcNId = NIdToIdH.GetDat(EI.GetSrcNId());
    const int64 DstNId = NIdToIdH.GetDat(EI.GetDstNId());
    fprintf(F, "%lld %lld %lld c Black\n", SrcNId, DstNId, 1); // width=1
  }
  fclose(F);
}

/// NIdColorH maps node ids to node colors. Default node color is Red.
/// See http://vlado.fmf.uni-lj.si/pub/networks/pajek/doc/pajekman.pdf for a list of supported color names.
template <class PGraph>
void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStr64H& NIdColorH) {
  TInt64H NIdToIdH(Graph->GetNodes(), true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "*Vertices %lld\n", Graph->GetNodes());
  int64 i = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, i++) {
    fprintf(F, "%lld  \"%lld\" ic %s fos 10\n", i+1, NI.GetId(),
      NIdColorH.IsKey(NI.GetId()) ? NIdColorH.GetDat(NI.GetId()).CStr() : "Red");
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
    fprintf(F, "*Arcs %lld\n", Graph->GetEdges()); } // arcs are directed, edges are undirected
  else {
    fprintf(F, "*Edges %lld\n", Graph->GetEdges());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int64 SrcNId = NIdToIdH.GetDat(EI.GetSrcNId());
    const int64 DstNId = NIdToIdH.GetDat(EI.GetDstNId());
    fprintf(F, "%lld %lld %lld c Black\n", SrcNId, DstNId, 1);
  }
  fclose(F);
}

/// NIdColorH maps node ids to node colors. Default node color is Red.
/// NIdLabelH maps node ids to node string labels.
/// See http://vlado.fmf.uni-lj.si/pub/networks/pajek/doc/pajekman.pdf for a list of supported color names.
template <class PGraph>
void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStr64H& NIdColorH, const TIntStr64H& NIdLabelH) {
  TInt64H NIdToIdH(Graph->GetNodes(), true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "*Vertices %lld\n", Graph->GetNodes());
  int64 i = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, i++) {
    fprintf(F, "%lld  \"%s\" ic %s fos 10\n", i+1,
      NIdLabelH.IsKey(NI.GetId()) ? NIdLabelH.GetDat(NI.GetId()).CStr() : TStr::Fmt("%lld", NI.GetId()).CStr(),
      NIdColorH.IsKey(NI.GetId()) ? NIdColorH.GetDat(NI.GetId()).CStr() : "Red");
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
    fprintf(F, "*Arcs %lld\n", Graph->GetEdges()); } // arcs are directed, edges are undirected
  else {
    fprintf(F, "*Edges %lld\n", Graph->GetEdges());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int64 SrcNId = NIdToIdH.GetDat(EI.GetSrcNId());
    const int64 DstNId = NIdToIdH.GetDat(EI.GetDstNId());
    fprintf(F, "%lld %lld %lld c Black\n", SrcNId, DstNId, 1);
  }
  fclose(F);
}

/// NIdColorH maps node ids to node colors. Default node color is Red.
/// NIdLabelH maps node ids to node string labels.
/// EIdColorH maps edge ids to node colors. Default edge color is Black.
/// See http://vlado.fmf.uni-lj.si/pub/networks/pajek/doc/pajekman.pdf for a list of supported color names.
template <class PGraph>
void SavePajek(const PGraph& Graph, const TStr& OutFNm, const TIntStr64H& NIdColorH, const TIntStr64H& NIdLabelH, const TIntStr64H& EIdColorH) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfMultiGraph)); // network needs to have edge ids
  TInt64H NIdToIdH(Graph->GetNodes(), true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "*Vertices %lld\n", Graph->GetNodes());
  int64 i = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, i++) {
    fprintf(F, "%lld  \"%s\" ic %s fos 10\n", i+1,
      NIdLabelH.IsKey(NI.GetId()) ? NIdLabelH.GetDat(NI.GetId()).CStr() : TStr::Fmt("%lld", NI.GetId()).CStr(),
      NIdColorH.IsKey(NI.GetId()) ? NIdColorH.GetDat(NI.GetId()).CStr() : "Red");
    NIdToIdH.AddDat(NI.GetId(), i+1);
  }
  if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
    fprintf(F, "*Arcs %lld\n", Graph->GetEdges()); } // arcs are directed, edges are undirected
  else {
    fprintf(F, "*Edges %lld\n", Graph->GetEdges());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int64 SrcNId = NIdToIdH.GetDat(EI.GetSrcNId());
    const int64 DstNId = NIdToIdH.GetDat(EI.GetDstNId());
    fprintf(F, "%lld %lld 1 c %s\n", SrcNId, DstNId,
      EIdColorH.IsKey(EI.GetId()) ? EIdColorH.GetDat(EI.GetId()).CStr() : "Black");
  }
  fclose(F);
}

/// Each line contains a tuple of 3 values: <source node id><tab><destination node id><tab>1
template <class PGraph>
void SaveMatlabSparseMtx(const PGraph& Graph, const TStr& OutFNm) {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  TInt64Set NIdSet(Graph->GetNodes()); // so that
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NIdSet.AddKey(NI.GetId());
  }
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int64 Src = NIdSet.GetKeyId(EI.GetSrcNId())+1;
    const int64 Dst = NIdSet.GetKeyId(EI.GetDstNId())+1;
    fprintf(F, "%lld\t%lld\t1\n", Src, Dst);
    if (! HasGraphFlag(typename PGraph::TObj, gfDirected) && Src!=Dst) {
      fprintf(F, "%lld\t%lld\t1\n", Dst, Src);
    }
  }
  fclose(F);
}

template<class PGraph>
void SaveGViz(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc, const bool& NodeLabels, const TIntStr64H& NIdColorH) {
  const bool IsDir = HasGraphFlag(typename PGraph::TObj, gfDirected);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  if (! Desc.Empty()) fprintf(F, "/*****\n%s\n*****/\n\n", Desc.CStr());
  if (IsDir) { fprintf(F, "digraph G {\n"); } else { fprintf(F, "graph G {\n"); }
  fprintf(F, "  graph [splines=false overlap=false]\n"); //size=\"12,10\" ratio=fill
  // node  [width=0.3, height=0.3, label=\"\", style=filled, color=black]
  // node  [shape=box, width=0.3, height=0.3, label=\"\", style=filled, fillcolor=red]
  fprintf(F, "  node  [shape=ellipse, width=0.3, height=0.3%s]\n", NodeLabels?"":", label=\"\"");
  // node colors
  //for (int i = 0; i < NIdColorH.Len(); i++) {
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NIdColorH.IsKey(NI.GetId())) {
      fprintf(F, "  %lld [style=filled, fillcolor=\"%s\"];\n", NI.GetId(), NIdColorH.GetDat(NI.GetId()).CStr()); }
    else {
      fprintf(F, "  %lld ;\n", NI.GetId());
    }
  }
  // edges
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetOutDeg()==0 && NI.GetInDeg()==0 && !NIdColorH.IsKey(NI.GetId())) {
      fprintf(F, "%lld;\n", NI.GetId()); }
    else {
      for (int64 e = 0; e < NI.GetOutDeg(); e++) {
        if (! IsDir && NI.GetId() > NI.GetOutNId(e)) { continue; }
        fprintf(F, "  %lld %s %lld;\n", NI.GetId(), IsDir?"->":"--", NI.GetOutNId(e));
      }
    }
  }
  if (! Desc.Empty()) {
    fprintf(F, "  label = \"\\n%s\\n\";", Desc.CStr());
    fprintf(F, "  fontsize=24;\n");
  }
  fprintf(F, "}\n");
  fclose(F);
}

template<class PGraph>
void SaveGViz(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc, const TIntStr64H& NIdLabelH) {
  const bool IsDir = Graph->HasFlag(gfDirected);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  if (! Desc.Empty()) fprintf(F, "/*****\n%s\n*****/\n\n", Desc.CStr());
  if (IsDir) { fprintf(F, "digraph G {\n"); } else { fprintf(F, "graph G {\n"); }
  fprintf(F, "  graph [splines=true overlap=false]\n"); //size=\"12,10\" ratio=fill
  fprintf(F, "  node  [shape=ellipse, width=0.3, height=0.3]\n");
  // node colors
  //for (int i = 0; i < NodeLabelH.Len(); i++) {
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    fprintf(F, "  %lld [label=\"%s\"];\n", NI.GetId(), NIdLabelH.GetDat(NI.GetId()).CStr());
}
  // edges
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetOutDeg()==0 && NI.GetInDeg()==0 && ! NIdLabelH.IsKey(NI.GetId())) {
      fprintf(F, "%lld;\n", NI.GetId()); }
    else {
      for (int64 e = 0; e < NI.GetOutDeg(); e++) {
        if (! IsDir && NI.GetId() > NI.GetOutNId(e)) { continue; }
        fprintf(F, "  %lld %s %lld;\n", NI.GetId(), IsDir?"->":"--", NI.GetOutNId(e));
      }
    }
  }
  if (! Desc.Empty()) {
    fprintf(F, "  label = \"\\n%s\\n\";", Desc.CStr());
    fprintf(F, "  fontsize=24;\n");
  }
  fprintf(F, "}\n");
  fclose(F);
}

} // namespace TSnap
