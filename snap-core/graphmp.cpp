/////////////////////////////////////////////////
#ifdef GCC_ATOMIC

/////////////////////////////////////////////////
// Directed Node Graph MP
bool TNGraphMP::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNGraphMP::TNet, Flag);
}

int64 TNGraphMP::AddNode(int64 NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

int64 TNGraphMP::AddNodeUnchecked(int64 NId) {
  if (IsNode(NId)) { return NId;}
  MxNId = TMath::Mx(NId+1, MxNId());
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// add a node with a list of neighbors
// (use TNGraphMP::IsOk to check whether the graph is consistent)
int64 TNGraphMP::AddNode(const int64& NId, const TInt64V& InNIdV, const TInt64V& OutNIdV) {
  int64 NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.InNIdV = InNIdV;
  Node.OutNIdV = OutNIdV;
  Node.InNIdV.Sort();
  Node.OutNIdV.Sort();
  return NewNId;
}

// add a node from a vector pool
// (use TNGraphMP::IsOk to check whether the graph is consistent)
int64 TNGraphMP::AddNode(const int64& NId, const TVecPool<TInt64, int64>& Pool, const int64& SrcVId, const int64& DstVId) {
  int64 NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.InNIdV.GenExt(Pool.GetValVPt(SrcVId), Pool.GetVLen(SrcVId));
  Node.OutNIdV.GenExt(Pool.GetValVPt(DstVId), Pool.GetVLen(DstVId));
  Node.InNIdV.Sort();
  Node.OutNIdV.Sort();
  return NewNId;
}

void TNGraphMP::DelNode(const int64& NId) {
#if 0
  { TNode& Node = GetNode(NId);
  for (int64 e = 0; e < Node.GetOutDeg(); e++) {
  const int64 nbr = Node.GetOutNId(e);
  if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int64 n = N.InNIdV.SearchBin(NId);
    if (n!= -1) { N.InNIdV.Del(n); }
  }
  for (int64 e = 0; e < Node.GetInDeg(); e++) {
  const int64 nbr = Node.GetInNId(e);
  if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int64 n = N.OutNIdV.SearchBin(NId);
    if (n!= -1) { N.OutNIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
#endif
}

int64 TNGraphMP::GetEdges() const {
  int64 edges=0;
  for (int64 N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    edges+=NodeH[N].GetOutDeg();
  }
  return edges;
}

int64 TNGraphMP::AddEdge(const int64& SrcNId, const int64& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  //IAssert(! IsEdge(SrcNId, DstNId));
  if (IsEdge(SrcNId, DstNId)) { return -2; }
  GetNode(SrcNId).OutNIdV.AddSorted(DstNId);
  GetNode(DstNId).InNIdV.AddSorted(SrcNId);
  return -1; // edge id
}

int64 TNGraphMP::AddEdgeUnchecked(const int64& SrcNId, const int64& DstNId) {
  GetNode(SrcNId).OutNIdV.Add(DstNId);
  GetNode(DstNId).InNIdV.Add(SrcNId);
  return -1; // edge id
}

void TNGraphMP::DelEdge(const int64& SrcNId, const int64& DstNId, const bool& IsDir) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  const int64 n = N.OutNIdV.SearchBin(DstNId);
  if (n!= -1) { N.OutNIdV.Del(n); } }
  { TNode& N = GetNode(DstNId);
  const int64 n = N.InNIdV.SearchBin(SrcNId);
  if (n!= -1) { N.InNIdV.Del(n); } }
  if (! IsDir) {
    { TNode& N = GetNode(SrcNId);
    const int64 n = N.InNIdV.SearchBin(DstNId);
    if (n!= -1) { N.InNIdV.Del(n); } }
    { TNode& N = GetNode(DstNId);
    const int64 n = N.OutNIdV.SearchBin(SrcNId);
    if (n!= -1) { N.OutNIdV.Del(n); } }
  }
}

bool TNGraphMP::IsEdge(const int64& SrcNId, const int64& DstNId, const bool& IsDir) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) { return false; }
  if (IsDir) { return GetNode(SrcNId).IsOutNId(DstNId); }
  else { return GetNode(SrcNId).IsOutNId(DstNId) || GetNode(DstNId).IsOutNId(SrcNId); }
}

TNGraphMP::TEdgeI TNGraphMP::GetEI(const int64& SrcNId, const int64& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int64 NodeN = SrcNI.NodeHI.GetDat().OutNIdV.SearchBin(DstNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

void TNGraphMP::GetNIdV(TInt64V& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int64 N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

void TNGraphMP::Defrag(const bool& OnlyNodeLinks) {
#if 0
  for (int64 n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    TNode& Node = NodeH[n];
    Node.InNIdV.Pack();  Node.OutNIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
#endif
}

// for each node check that their neighbors are also nodes
bool TNGraphMP::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int64 N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edges
    int64 prevNId = -1;
    for (int64 e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsNode(Node.GetOutNId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetOutNId(e), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetOutNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetOutNId(e);
    }
    // check in-edges
    prevNId = -1;
    for (int64 e = 0; e < Node.GetInDeg(); e++) {
      if (! IsNode(Node.GetInNId(e))) {
        const TStr Msg = TStr::Fmt("In-edge %d <-- %d: node %d does not exist.",
          Node.GetId(), Node.GetInNId(e), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetInNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate in-edge %d <-- %d.",
          Node.GetId(), Node.GetId(), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetInNId(e);
    }
  }
  return RetVal;
}

void TNGraphMP::Dump(FILE *OutF) const {
  const int64 NodePlaces = (int64) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node Graph: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (int64 N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*d]\n", NodePlaces, Node.GetId());
    fprintf(OutF, "    in [%d]", Node.GetInDeg());
    for (int64 edge = 0; edge < Node.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetInNId(edge)); }
    fprintf(OutF, "\n    out[%d]", Node.GetOutDeg());
    for (int64 edge = 0; edge < Node.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetOutNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

PNGraphMP TNGraphMP::GetSmallGraph() {
  PNGraphMP G = TNGraphMP::New();
  for (int64 i = 0; i < 5; i++) { G->AddNode(i); }
  G->AddEdge(0,1); G->AddEdge(1,2); G->AddEdge(0,2);
  G->AddEdge(1,3);
  G->AddEdge(3,4);
  G->AddEdge(2,3);
  return G;
}


int64 TNGraphMP::AddOutEdge1(int64& SrcIdx, const int64& SrcNId, const int64& DstNId) {
  bool Found;
  int64 SrcKeyId;

  //SrcKeyId = NodeH.AddKey2(SrcIdx, SrcNId, Found);
  SrcKeyId = NodeH.AddKey12(SrcIdx, SrcNId, Found);
  if (!Found) {
    NodeH[SrcKeyId] = TNode(SrcNId);
    //MxNId = TMath::Mx(SrcNId+1, MxNId());
  }
  //GetNode(SrcNId).OutNIdV.Add(DstNId);
  //NodeH[SrcKeyId].OutNIdV.Add1(DstNId);

  // TODO:RS, edge lists need to be sorted at the end

  SrcIdx = SrcKeyId;
  return Found;
}

int64 TNGraphMP::AddInEdge1(int64& DstIdx, const int64& SrcNId, const int64& DstNId) {
  bool Found;
  int64 DstKeyId;

  //DstKeyId = NodeH.AddKey2(DstIdx, DstNId, Found);
  DstKeyId = NodeH.AddKey12(DstIdx, DstNId, Found);
  if (!Found) {
    NodeH[DstKeyId] = TNode(DstNId);
    //MxNId = TMath::Mx(DstNId+1, MxNId());
  }
  //GetNode(DstNId).InNIdV.Add(SrcNId);
  //NodeH[DstKeyId].InNIdV.Add1(SrcNId);

  // TODO:RS, edge lists need to be sorted at the end

  DstIdx = DstKeyId;
  return Found;
}

void TNGraphMP::AddOutEdge2(const int64& SrcNId, const int64& DstNId) {
  NodeH[NodeH.GetKeyId(SrcNId)].OutNIdV.AddMP(DstNId);
}

void TNGraphMP::AddInEdge2(const int64& SrcNId, const int64& DstNId) {
  NodeH[NodeH.GetKeyId(DstNId)].InNIdV.AddMP(SrcNId);
}

// add a node with a list of neighbors
// (use TNGraphMP::IsOk to check whether the graph is consistent)
void TNGraphMP::AddNodeWithEdges(const TInt64& NId, TInt64V& InNIdV, TInt64V& OutNIdV) {
  int64 NodeIdx = abs((NId.GetPrimHashCd()) % Reserved());
  int64 NodeKeyId = NodeH.AddKey13(NodeIdx, NId);
  NodeH[NodeKeyId] = TNode(NId);
  NodeH[NodeKeyId].InNIdV.MoveFrom(InNIdV);
  NodeH[NodeKeyId].OutNIdV.MoveFrom(OutNIdV);  
}
#endif // GCC_ATOMIC

