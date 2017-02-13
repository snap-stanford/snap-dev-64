/////////////////////////////////////////////////
// Graph Hash Table Key
const int64 TGraphKey::RoundTo = 4; // round to 4 decimal places

TGraphKey::TGraphKey(const TSFlt64V& GraphSigV) : Nodes(-1), EdgeV(), SigV(), VariantId(0) {
  SigV.Gen(GraphSigV.Len());
  for (int64 i = 0; i < GraphSigV.Len(); i++) {
    SigV[i] = TFlt(TMath::Round(GraphSigV[i], RoundTo));
  }
}

TGraphKey::TGraphKey(const TInt64V& GraphSigV) : Nodes(-1), EdgeV(), SigV(), VariantId(0) {
  SigV.Gen(GraphSigV.Len());
  for (int64 i = 0; i < GraphSigV.Len(); i++) {
    SigV[i] = TFlt(GraphSigV[i]());
  }
}

TGraphKey::TGraphKey(const TFlt64V& GraphSigV) : Nodes(-1), EdgeV(), SigV(), VariantId(0) {
  SigV.Gen(GraphSigV.Len());
  for (int64 i = 0; i < GraphSigV.Len(); i++) {
    SigV[i] = TFlt(TMath::Round(GraphSigV[i], RoundTo));
  }
}

TGraphKey::TGraphKey(const TGraphKey& GraphKey) : Nodes(GraphKey.Nodes),
  EdgeV(GraphKey.EdgeV), SigV(GraphKey.SigV), VariantId(GraphKey.VariantId) {
}

TGraphKey::TGraphKey(TSIn& SIn) : Nodes(SIn), EdgeV(SIn), SigV(SIn), VariantId(SIn) { }

void TGraphKey::Save(TSOut& SOut) const {
  Nodes.Save(SOut);  EdgeV.Save(SOut);
  SigV.Save(SOut);  VariantId.Save(SOut);
}

TGraphKey& TGraphKey::operator = (const TGraphKey& GraphKey) {
  if (this != &GraphKey) {
    Nodes = GraphKey.Nodes;
    EdgeV = GraphKey.EdgeV;
    SigV = GraphKey.SigV;
    VariantId = GraphKey.VariantId;
  }
  return *this;
}

PNGraph TGraphKey::GetNGraph() const {
  PNGraph G = TNGraph::New();
  for (int64 i = 0; i < GetNodes(); i++) G->AddNode(i);
  for (int64 e = 0; e < GetEdges(); e++) {
    G->AddEdge(EdgeV[e].Val1, EdgeV[e].Val2);
  }
  G->Defrag();
  return G;
}

// renumbers nodes
void TGraphKey::TakeGraph(const PNGraph& Graph) {
  TInt64H NodeIdH;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NodeIdH.AddKey(NI.GetId()); }
  Nodes = Graph->GetNodes();
  EdgeV.Gen(Nodes, 0);
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int64 NewNId = NodeIdH.GetKeyId(NI.GetId());
    for (int64 i = 0; i < NI.GetOutDeg(); i++) {
      EdgeV.Add(TInt64Pr(NewNId, NodeIdH.GetKeyId(NI.GetOutNId(i))));
    }
  }
  EdgeV.Sort(true);
  EdgeV.Pack();
}

void TGraphKey::TakeGraph(const PNGraph& Graph, TIntPr64V& NodeMap) {
  TInt64Set NodeIdH;
  int64 n = 0;
  NodeMap.Gen(Graph->GetNodes(), 0);
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, n++) {
    NodeIdH.AddKey(NI.GetId());
    NodeMap.Add(TInt64Pr(NI.GetId(), n));
  }
  Nodes = Graph->GetNodes();
  EdgeV.Gen(Nodes, 0);
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int64 NewNId = NodeIdH.GetKeyId(NI.GetId());
    for (int64 i = 0; i < NI.GetOutDeg(); i++) {
      EdgeV.Add(TInt64Pr(NewNId, NodeIdH.GetKeyId(NI.GetOutNId(i))));
    }
  }
  EdgeV.Sort(true);
  EdgeV.Pack();
}

void TGraphKey::TakeSig(const PNGraph& Graph, const int64& MnSvdGraph64, const int64& MxSvdGraph64) {
  IAssertR(MnSvdGraph64 > TInt::Mx || MxSvdGraph64 > TInt::Mx, TStr::Fmt("Taking Signature is only supported in 32-bits").CStr());
  int MnSvdGraph = (int) MnSvdGraph64;
  int MxSvdGraph = (int) MxSvdGraph64;
  const int Edges = Graph->GetEdges();
  Nodes = Graph->GetNodes();
  VariantId = 0;
  SigV.Gen(2+Nodes, 0);
  // degree sequence
  TIntPrV DegV(Nodes, 0);
  for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
    DegV.Add(TIntPr(NodeI.GetInDeg(), NodeI.GetOutDeg()));
  }
  DegV.Sort(false);
  // compose the signature: nodes, edges, sorted in- and out- degree sequence
  SigV.Add(TFlt(Nodes));
  SigV.Add(TFlt(Edges));
  for (int i = 0; i < DegV.Len(); i++) {
    SigV.Add(DegV[i].Val1());
    SigV.Add(DegV[i].Val2());
  }
  // singular values signature
  //   it turns out that it is cheaper to do brute force isomorphism
  //   checking than to calculate SVD and then check isomorphism
  if (Nodes >= MnSvdGraph && Nodes < MxSvdGraph) {
    // perform full SVD
    TFltVV AdjMtx(Nodes+1, Nodes+1);
    TFltV SngValV;
    TFltVV LSingV, RSingV;
    TIntH NodeIdH;
    // create adjecency matrix
    for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      NodeIdH.AddKey(NodeI.GetId());
    }
    for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      const int NodeId = NodeIdH.GetKeyId(NodeI.GetId()) + 1;
      for (int e = 0; e < NodeI.GetOutDeg(); e++) {
        const int DstNId = NodeIdH.GetKeyId(NodeI.GetOutNId(e)) + 1;  // no self edges
        if (NodeId != DstNId) AdjMtx.At(NodeId, DstNId) = 1;
      }
    }
    try { // can fail to converge but results seem to be good
      TSvd::Svd(AdjMtx, LSingV, SngValV, RSingV);
    } catch(...) {
      printf("\n***No SVD convergence: G(%s, %s): SngValV.Len():%d\n", TInt64::GetStr(Nodes()).CStr(), TInt64::GetStr(Graph->GetEdges()).CStr(), SngValV.Len());
    }
    // round singular values
    SngValV.Sort(false);
    for (int i = 0; i < SngValV.Len(); i++) {
      SigV.Add(TMath::Round(SngValV[i], RoundTo));
    }
  }
  //printf("SIG:\n");  for (int i = 0; i < SigV.Len(); i++) { printf("\t%f\n", SigV[i]); }
  SigV.Pack();
}

void TGraphKey::SaveTxt(FILE *F) const {
  fprintf(F, "#GraphKey. Nodes: %s.  Edges: %s\n", TInt64::GetStr(GetNodes()).CStr(), TInt64::GetStr(GetEdges()).CStr());
  for (int64 i = 0; i < EdgeV.Len(); i++) {
    fprintf(F,"  %s\t%s\n", TInt64::GetStr(EdgeV[i].Val1()).CStr(), TInt64::GetStr(EdgeV[i].Val2()).CStr());
  }
}

void TGraphKey::SaveGViz(const TStr& OutFNm, const TStr& Desc, const TStr& NodeAttrs, const int64& Size) const {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "/*****\n");
  fprintf(F, "  Graph (%s, %s)\n", TInt64::GetStr(GetNodes()).CStr(), TInt64::GetStr(GetEdges()).CStr());
  //if (! Desc.Empty()) fprintf(F, "  %s\n", Desc.CStr());
  fprintf(F, "*****/\n\n");
  fprintf(F, "digraph G {\n");
  if (Size != -1) fprintf(F, "  size=\"%s,%s\";\n", TInt64::GetStr(Size).CStr(), TInt64::GetStr(Size).CStr());
  fprintf(F, "  graph [splines=true overlap=false]\n"); //size=\"12,10\" ratio=fill
  if (NodeAttrs.Empty()) fprintf(F, "  node  [shape=ellipse, width=0.3, height=0.3]\n");
  else fprintf(F, "  node  [shape=ellipse, %s]\n", NodeAttrs.CStr());
  if (! EdgeV.Empty()) {
    for (int64 e = 0; e < EdgeV.Len(); e++) {
      fprintf(F, "  %s -> %s;\n", TInt64::GetStr(EdgeV[e].Val1()).CStr(), TInt64::GetStr(EdgeV[e].Val2()).CStr()); }
  } else {
    for (int64 n = 0; n < Nodes; n++) { fprintf(F, "  %s;\n", TInt64::GetStr(n).CStr()); }
  }
  if (! Desc.Empty()) {
    fprintf(F, "  label = \"\\n%s\\n\";", Desc.CStr());
    fprintf(F, "  fontsize=24;\n");
  }
  fprintf(F, "}\n");
  fclose(F);
}

// width=0.3, height=0.3, label=\"\", style=filled, color=black
void TGraphKey::DrawGViz(const TStr& OutFNm, const TStr& Desc, const TStr& NodeAttrs, const int64& Size) const {
  const TStr DotFNm = OutFNm.GetFMid()+".dot";
  SaveGViz(DotFNm, Desc, NodeAttrs, Size);
  TSnap::TSnapDetail::GVizDoLayout(DotFNm, OutFNm, gvlDot);
}

bool TGraphKey::IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TInt64V& NodeIdMap) {
  const TIntPr64V& EdgeV1 = Key1.EdgeV;
  const TIntPr64V& EdgeV2 = Key2.EdgeV;
  if (Key1.Nodes != Key2.Nodes || EdgeV1.Len() != EdgeV2.Len()) { return false; }
  for (int64 e1 = 0; e1 < EdgeV1.Len(); e1++) {
    const TInt64Pr Edge2(NodeIdMap[EdgeV1[e1].Val1], NodeIdMap[EdgeV1[e1].Val2]);
    if (EdgeV2.SearchBin(Edge2) == -1) return false;
  }
  return true;
}

bool TGraphKey::IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TVec<TInt64V, int64>& NodeIdMapV) {
  int64 IsoPermId;
  return IsIsomorph(Key1, Key2, NodeIdMapV, IsoPermId);
}

bool TGraphKey::IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TVec<TInt64V, int64>& NodeIdMapV, int64& IsoPermId) {
  const TIntPr64V& EdgeV1 = Key1.EdgeV;
  const TIntPr64V& EdgeV2 = Key2.EdgeV;
  //for (int i = 0; i < EdgeV1.Len(); i++) printf("\t%d - %d\n", EdgeV1[i].Val1, EdgeV1[i].Val2);  printf("\n");
  //for (int i = 0; i < EdgeV2.Len(); i++) printf("\t%d - %d\n", EdgeV2[i].Val1, EdgeV2[i].Val2);
  if (Key1.Nodes != Key2.Nodes || EdgeV1.Len() != EdgeV2.Len()) return false;
  const int64 Nodes = NodeIdMapV[0].Len();
  // fast adjecency matrix
  TInt64V AdjMtx2(Nodes*Nodes);
  for (int64 i = 0; i < EdgeV2.Len(); i++) {
    AdjMtx2[EdgeV2[i].Val1*Nodes + EdgeV2[i].Val2] = 1;
  }
  for (int64 perm = 0; perm < NodeIdMapV.Len(); perm++) {
    const TInt64V& NodeIdMap = NodeIdMapV[perm];
    bool IsIso = true;
    for (int64 e1 = 0; e1 < EdgeV1.Len(); e1++) {
      const int64 NId1 = NodeIdMap[EdgeV1[e1].Val1];
      const int64 NId2 = NodeIdMap[EdgeV1[e1].Val2];
      if (AdjMtx2[NId1*Nodes + NId2] != 1) {
        IsIso = false;  break; }
    }
    if (IsIso) {
      IsoPermId = perm;
      return true; }
  }
  IsoPermId = -1;
  return false;
}

/////////////////////////////////////////////////
// Simple Edge Graph
bool TSimpleGraph::Join(const TSimpleGraph& G1, const TSimpleGraph& G2) {
  const int64 Edges1 = G1.GetEdges();
  const int64 Edges2 = G2.GetEdges();
  const TIntPr64V& EdgeV1 = G1.EdgeV;
  const TIntPr64V& EdgeV2 = G2.EdgeV;
  const int MxEdges = Edges1+1;
  if (GetEdges() != MxEdges) EdgeV.Gen(MxEdges);
  IAssert(Edges1 == Edges2);

  int64 e=0, g1=0, g2=0;
  while (g1 < Edges1 && g2 < Edges2) {
    if (e == MxEdges) return false;
    if (abs(g1 - g2) > 1) return false;
    if (EdgeV1[g1] == EdgeV2[g2]) { e++;  g1++;  g2++; }
    else if (EdgeV1[g1] < EdgeV2[g2]) { e++;  g1++; }
    else { e++;  g2++; }
  }

  e=0; g1=0; g2=0;
  while (g1 < Edges1 && g2 < Edges2) {
    if (EdgeV1[g1] == EdgeV2[g2]) {
      EdgeV[e] = EdgeV1[g1];  e++;  g1++;  g2++; }
    else if (EdgeV1[g1] < EdgeV2[g2]) {
      EdgeV[e] = EdgeV1[g1];  e++;  g1++; }
    else {
      EdgeV[e] = EdgeV2[g2];  e++;  g2++; }
  }
  if (g1 == Edges1 && g2 == Edges2 && e == MxEdges) return true;
  if (e+1 == MxEdges) {
    if (g1+1 == Edges1 && g2 == Edges2) {
      EdgeV[e] = EdgeV1.Last();
      return true;
    }
    if (g1 == Edges1 && g2+1 == Edges2) {
      EdgeV[e] = EdgeV2.Last();
      return true;
    }
  }
  return false;
}

void TSimpleGraph::Dump(const TStr& Desc) const {
  if (! Desc.Empty()) printf("%s. Edges: %s\n", Desc.CStr(), TInt64::GetStr(EdgeV.Len()).CStr());
  else printf("Edges: %s\n", TInt64::GetStr(EdgeV.Len()).CStr());
  for (int64 i = 0; i < EdgeV.Len(); i++) {
    printf("\t%s\t%s\n", TInt64::GetStr(EdgeV[i].Val1()).CStr(), TInt64::GetStr(EdgeV[i].Val2()).CStr());
  }
}

void TSubGraphsEnum::Gen2Graphs() {
  // singe edge sub-graphs
  SgV.Gen(NGraph->GetEdges(), 0);
  TSimpleGraph SimpleG;
  TIntPr64V& EdgeV = SimpleG.GetEdgeV();
  EdgeV.Gen(1);
  for (TNGraph::TNodeI NI = NGraph->BegNI(); NI < NGraph->EndNI(); NI++) {
    for (int64 e = 0; e < NI.GetOutDeg(); e++) {
      EdgeV[0] = TInt64Pr(NI.GetId(), NI.GetOutNId(e));
      SgV.Add(SimpleG);
    }
  }
  SgV.Sort();
  // two edge sub-graphs
  EdgeV.Gen(2);
  for (int64 g1 = 0; g1 < SgV.Len()-1; g1++) {
    const TInt64Pr& E1 = SgV[g1].GetEdgeV()[0];
    for (int64 g2 = g1+1; g2 < SgV.Len(); g2++) {
      const TInt64Pr& E2 = SgV[g2].GetEdgeV()[0];
      if (E1.Val2 == E2.Val1 || E1.Val1 == E2.Val2 || E1.Val1 == E2.Val1 || E1.Val2 == E2.Val2) {
        EdgeV[0] = TMath::Mn(E1, E2);
        EdgeV[1] = TMath::Mx(E1, E2);
        SimpleG.Dump();
        NextSgV.Add(SimpleG);
      }
    }
  }
  SgV.MoveFrom(NextSgV);
}

void TSubGraphsEnum::EnumSubGraphs(const int64& MaxEdges) {
  TExeTm ExeTm;
  Gen2Graphs();
  printf("  %2d edge graphs:  %s\t[%s]\n", 2, TInt64::GetStr(SgV.Len()).CStr(), ExeTm.GetTmStr());  ExeTm.Tick();
  //for (int i = 0; i < SgV.Len(); i++) { SgV[i].Dump(TStr::Fmt("  %d", i+1)); }
  //printf("**************************************************************\n");
  TSimpleGraph SimpleG;
  TIntPr64V& EdgeV = SimpleG.GetEdgeV();
  // multiple edge sub-graphs
  for (int64 edges = 3; edges <= MaxEdges; edges++) {
    EdgeV.Clr();
    printf("  %2s edge graphs:", TInt64::GetStr(edges).CStr());
    for (int64 g1 = 0; g1 < SgV.Len()-1; g1++) {
      for (int64 g2 = g1+1; g2 < SgV.Len(); g2++) {
        if (SimpleG.Join(SgV[g1], SgV[g2])) { NextSgV.Add(SimpleG); }
      }
    }
    printf("  candidates: %8s [%s]", TInt64::GetStr(NextSgV.Len()).CStr(), ExeTm.GetTmStr());  ExeTm.Tick();
    NextSgV.Sort();
    SgV.Gen(NextSgV.Len(), 0);
    SgV.Add(NextSgV[0]);
    for (int64 i = 1; i < NextSgV.Len(); i++) {
      if (SgV.Last() != NextSgV[i]) {
        SgV.Add(NextSgV[i]);
      }
    }
    NextSgV.Clr(false);
    printf("  total: %8s [%s]\n", TInt64::GetStr(SgV.Len()).CStr(), ExeTm.GetTmStr());  ExeTm.Tick();
    //for (int i = 0; i < SgV.Len(); i++) { SgV[i].Dump(TStr::Fmt("  %d", i+1)); }
    //printf("**************************************************************\n");
  }
}

void TSubGraphsEnum::RecurBfs(const int64& MxDepth) {
  TExeTm ExeTm;
  SgV.Clr(true);
  for (TNGraph::TNodeI NI = NGraph->BegNI(); NI < NGraph->EndNI(); NI++) {
    TSimpleGraph SimpleG;
    RecurBfs(NI.GetId(), MxDepth, SimpleG);
    //NGraph->DelNode(NI.GetId());
    printf(".");
  }
  printf("\ncandidates: %s\n", TInt64::GetStr(SgV.Len()).CStr());
  SgV.Sort();
  int64 Cnt = 1;
  for (int64 i = 1; i < SgV.Len(); i++) {
    if (SgV[i-1] != SgV[i]) Cnt++;
  }
  printf("distinct:   %s\t[%s]\n",TInt64::GetStr(Cnt).CStr(), ExeTm.GetTmStr());
}

void TSubGraphsEnum::RecurBfs(const int64& NId, const int64& Depth, TSimpleGraph& PrevG) {
  if (Depth == 0) {
    TIntPr64V& EdgeV = PrevG();
    EdgeV.Sort();
    for (int64 i = 1; i < EdgeV.Len(); i++) {
      if (EdgeV[i-1] == EdgeV[i]) { return; }
    }
    SgV.Add(PrevG);
    return;
  }
  const TNGraph::TNodeI NI = NGraph ->GetNI(NId);
  for (int64 e = 0; e < NI.GetOutDeg(); e++) {
    TSimpleGraph CurG = PrevG;
    CurG.AddEdge(NI.GetId(), NI.GetOutNId(e));
    RecurBfs(NI.GetOutNId(e), Depth-1, CurG);
  }
  for (int64 e = 0; e < NI.GetInDeg(); e++) {
    TSimpleGraph CurG = PrevG;
    CurG.AddEdge(NI.GetInNId(e), NI.GetId());
    RecurBfs(NI.GetInNId(e), Depth-1, CurG);
  }
}

void TSubGraphsEnum::RecurBfs1(const int64& MxDepth) {
  TExeTm ExeTm;
  SgV.Clr(true);
  for (TNGraph::TNodeI NI = NGraph->BegNI(); NI < NGraph->EndNI(); NI++) {
    TSimpleGraph SimpleG;
    RecurBfs1(NI.GetId(), MxDepth);
    //NGraph->DelNode(NI.GetId());
    printf(".");
  }
  printf("candidates: %s\n", TInt64::GetStr(SgV.Len()).CStr());
  SgV.Sort();
  int64 Cnt = 1;
  for (int64 i = 1; i < SgV.Len(); i++) {
    if (SgV[i-1]!=SgV[i]) {
      //SgV[i].Dump();
      Cnt++;
    }
  }
  printf("distinct:   %s\t[%s]\n", TInt64::GetStr(Cnt).CStr(), ExeTm.GetTmStr());
}

void TSubGraphsEnum::RecurBfs1(const int64& NId, const int64& Depth) {
  if (Depth == 0) {
    TIntPr64V EdgeV;
    EdgeH.GetKeyV(EdgeV);
    EdgeV.Sort();
    SgV.Add(EdgeV);
    return;
  }
  const TNGraph::TNodeI NI = NGraph ->GetNI(NId);
  for (int64 e = 0; e < NI.GetOutDeg(); e++) {
    const TInt64Pr Edge(NId, NI.GetOutNId(e));
    if (! EdgeH.IsKey(Edge)) {
      EdgeH.AddKey(Edge);
      RecurBfs1(NI.GetOutNId(e), Depth-1);
      EdgeH.DelKey(Edge);
    }
  }
  for (int64 e = 0; e < NI.GetInDeg(); e++) {
    const TInt64Pr Edge(NI.GetInNId(e), NId);
    if (! EdgeH.IsKey(Edge)) {
      EdgeH.AddKey(Edge);
      RecurBfs1(NI.GetInNId(e), Depth-1);
      EdgeH.DelKey(Edge);
    }
  }
}
