#ifdef GCC_ATOMIC

/////////////////////////////////////////////////
// Attribute Node Edge Network
bool TNEANetMP::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNEANetMP::TNet, Flag);
}

bool TNEANetMP::TNodeI::IsInNId(const int64& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int64 edge = 0; edge < Node.GetInDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetInEId(edge)).GetSrcNId())
      return true;
  }
  return false;
}

bool TNEANetMP::TNodeI::IsOutNId(const int64& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int64 edge = 0; edge < Node.GetOutDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetOutEId(edge)).GetDstNId())
      return true;
  }
  return false;
}

void TNEANetMP::AttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (!NodeAttrIsDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANetMP::AttrValueNI(const TInt64& NId , TStrIntPr64H::TIter NodeHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (!NodeAttrIsDeleted(NId, NodeHI)) {
      Values.Add(GetNodeAttrValue(NId, NodeHI));
    }
    NodeHI++;
  }
}

void TNEANetMP::IntAttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntType && !NodeAttrIsIntDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANetMP::IntAttrValueNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TInt64V& Values) const {
  Values = TVec<TInt64, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntType && !NodeAttrIsIntDeleted(NId, NodeHI)) {
      TInt64 val = this->VecOfIntVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId));
      Values.Add(val);
    }
    NodeHI++;
  }
}

void TNEANetMP::StrAttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == StrType && !NodeAttrIsStrDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANetMP::StrAttrValueNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Values) const {
  Values = TVec<TStr,int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == StrType && !NodeAttrIsStrDeleted(NId, NodeHI)) {
      TStr val = this->VecOfStrVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId));
      Values.Add(val);
    }
    NodeHI++;
  }
}

void TNEANetMP::FltAttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == FltType && !NodeAttrIsFltDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANetMP::FltAttrValueNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TFlt64V& Values) const {
  Values = TVec<TFlt, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == FltType && !NodeAttrIsFltDeleted(NId, NodeHI)) {
      TFlt val = (this->VecOfFltVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId)));
      Values.Add(val);
    }
    NodeHI++;
  }
}

bool TNEANetMP::NodeAttrIsDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  bool IntDel = NodeAttrIsIntDeleted(NId, NodeHI);
  bool StrDel = NodeAttrIsStrDeleted(NId, NodeHI);
  bool FltDel = NodeAttrIsFltDeleted(NId, NodeHI);
  return IntDel || StrDel || FltDel;
}

bool TNEANetMP::NodeAttrIsIntDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  return (NodeHI.GetDat().Val1 == IntType &&
    GetIntAttrDefaultN(NodeHI.GetKey()) == this->VecOfIntVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

bool TNEANetMP::NodeAttrIsStrDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  return (NodeHI.GetDat().Val1 == StrType &&
    GetStrAttrDefaultN(NodeHI.GetKey()) == this->VecOfStrVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

bool TNEANetMP::NodeAttrIsFltDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  return (NodeHI.GetDat().Val1 == FltType &&
    GetFltAttrDefaultN(NodeHI.GetKey()) == this->VecOfFltVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

TStr TNEANetMP::GetNodeAttrValue(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsN.GetVal(
      this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId))).GetStr();
  } else if(NodeHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId));
  } else if (NodeHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsN.GetVal(
      this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId))).GetStr();
  }
  return TStr::GetNullStr();
}

void TNEANetMP::AttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANetMP::AttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, EdgeHI)) {
      Values.Add(GetNodeAttrValue(EId, EdgeHI));
    }
    EdgeHI++;
  }
}

void TNEANetMP::IntAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANetMP::IntAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TInt64V& Values) const {
  Values = TVec<TInt64, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, EdgeHI)) {
      TInt64 val = (this->VecOfIntVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    EdgeHI++;
  }
}

void TNEANetMP::StrAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANetMP::StrAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, EdgeHI)) {
      TStr val = this->VecOfStrVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId);
      Values.Add(val);
    }
    EdgeHI++;
  }
}

void TNEANetMP::FltAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANetMP::FltAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TFlt64V& Values) const {
  Values = TVec<TFlt, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, EdgeHI)) {
      TFlt val = (this->VecOfFltVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    EdgeHI++;
  }
}

bool TNEANetMP::EdgeAttrIsDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  bool IntDel = EdgeAttrIsIntDeleted(EId, EdgeHI);
  bool StrDel = EdgeAttrIsStrDeleted(EId, EdgeHI);
  bool FltDel = EdgeAttrIsFltDeleted(EId, EdgeHI);
  return IntDel || StrDel || FltDel;
}

bool TNEANetMP::EdgeAttrIsIntDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == IntType &&
    GetIntAttrDefaultE(EdgeHI.GetKey()) == this->VecOfIntVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

bool TNEANetMP::EdgeAttrIsStrDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == StrType &&
    GetStrAttrDefaultE(EdgeHI.GetKey()) == this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

bool TNEANetMP::EdgeAttrIsFltDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == FltType &&
    GetFltAttrDefaultE(EdgeHI.GetKey()) == this->VecOfFltVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

TStr TNEANetMP::GetEdgeAttrValue(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  if (EdgeHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId))).GetStr();
  } else if(EdgeHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId));
  } else if (EdgeHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId))).GetStr();
  }
  return TStr::GetNullStr();
}

int64 TNEANetMP::AddNode(int64 NId) {
  int64 i;
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  // update attribute columns
  NodeH.AddDat(NId, TNode(NId));
  for (i = 0; i < VecOfIntVecsN.Len(); i++) {
    TVec<TInt64, int64>& IntVec = VecOfIntVecsN[i];
    IntVec.Ins(NodeH.GetKeyId(NId), TInt64::Mn);
  }
  TVec<TStr, int64> DefIntVec = TVec<TStr, int64>();
  IntDefaultsN.GetKeyV(DefIntVec);
  for (i = 0; i < DefIntVec.Len(); i++) {
    TStr attr = DefIntVec[i];
    TVec<TInt64, int64>& IntVec = VecOfIntVecsN[KeyToIndexTypeN.GetDat(DefIntVec[i]).Val2];
    IntVec[NodeH.GetKeyId(NId)] = GetIntAttrDefaultN(attr);
  }
  for (i = 0; i < VecOfStrVecsN.Len(); i++) {
    TVec<TStr, int64>& StrVec = VecOfStrVecsN[i];
    StrVec.Ins(NodeH.GetKeyId(NId), TStr::GetNullStr());
  }
  TVec<TStr, int64> DefStrVec = TVec<TStr, int64>();
  IntDefaultsN.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr, int64>& StrVec = VecOfStrVecsN[KeyToIndexTypeN.GetDat(DefStrVec[i]).Val2];
    StrVec[NodeH.GetKeyId(NId)] = GetStrAttrDefaultN(attr);
  }
  for (i = 0; i < VecOfFltVecsN.Len(); i++) {
    TVec<TFlt, int64>& FltVec = VecOfFltVecsN[i];
    FltVec.Ins(NodeH.GetKeyId(NId), TFlt::Mn);
  }
  TVec<TStr, int64> DefFltVec = TVec<TStr, int64>();
  FltDefaultsN.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt, int64>& FltVec = VecOfFltVecsN[KeyToIndexTypeN.GetDat(DefFltVec[i]).Val2];
    FltVec[NodeH.GetKeyId(NId)] = GetFltAttrDefaultN(attr);
  }
  return NId;
}

// add a node with a list of neighbors
// (use TNGraphMP::IsOk to check whether the graph is consistent)
void TNEANetMP::AddNodeWithEdges(const TInt64& NId, TInt64V& InEIdV, TInt64V& OutEIdV) {
  int64 NodeIdx = abs((NId.GetPrimHashCd()) % Reserved());
  int64 NodeKeyId = NodeH.AddKey13(NodeIdx, NId);
  MxNId = TMath::Mx(NId+1, MxNId());
  NodeH[NodeKeyId] = TNode(NId);
  NodeH[NodeKeyId].InEIdV.MoveFrom(InEIdV);
  NodeH[NodeKeyId].OutEIdV.MoveFrom(OutEIdV);
}

//void TNEANetMP::DelNode(const int& NId) {
//  int i;
//
//  const TNode& Node = GetNode(NId);
//  for (int out = 0; out < Node.GetOutDeg(); out++) {
//    const int EId = Node.GetOutEId(out);
//    const TEdge& Edge = GetEdge(EId);
//    IAssert(Edge.GetSrcNId() == NId);
//    GetNode(Edge.GetDstNId()).InEIdV.DelIfIn(EId);
//    // Delete from Edge Attributes
//    for (i = 0; i < VecOfIntVecsE.Len(); i++) {
//      TVec<TInt>& IntVec = VecOfIntVecsE[i];
//      IntVec[EdgeH.GetKeyId(EId)] = TInt::Mn;
//    }
//    for (i = 0; i < VecOfStrVecsE.Len(); i++) {
//      TVec<TStr>& StrVec = VecOfStrVecsE[i];
//      StrVec[EdgeH.GetKeyId(EId)] = TStr::GetNullStr();
//    }
//    for (i = 0; i < VecOfFltVecsE.Len(); i++) {
//      TVec<TFlt>& FltVec = VecOfFltVecsE[i];
//      FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
//    }
//    EdgeH.DelKey(EId);
//  }
//  for (int in = 0; in < Node.GetInDeg(); in++) {
//    const int EId = Node.GetInEId(in);
//    const TEdge& Edge = GetEdge(EId);
//    IAssert(Edge.GetDstNId() == NId);
//    GetNode(Edge.GetSrcNId()).OutEIdV.DelIfIn(EId);
//    // Delete from Edge Attributes
//    for (i = 0; i < VecOfIntVecsE.Len(); i++) {
//      TVec<TInt>& IntVec = VecOfIntVecsE[i];
//      IntVec[EdgeH.GetKeyId(EId)] = TInt::Mn;
//    }
//    for (i = 0; i < VecOfStrVecsE.Len(); i++) {
//      TVec<TStr>& StrVec = VecOfStrVecsE[i];
//      StrVec[EdgeH.GetKeyId(EId)] = TStr::GetNullStr();
//    }
//    for (i = 0; i < VecOfFltVecsE.Len(); i++) {
//      TVec<TFlt>& FltVec = VecOfFltVecsE[i];
//      FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
//    }
//    EdgeH.DelKey(EId);
//  }
//
//  for (i = 0; i < VecOfIntVecsN.Len(); i++) {
//    TVec<TInt>& IntVec = VecOfIntVecsN[i];
//    IntVec[NodeH.GetKeyId(NId)] = TInt::Mn;
//  }
//  for (i = 0; i < VecOfStrVecsN.Len(); i++) {
//    TVec<TStr>& StrVec = VecOfStrVecsN[i];
//    StrVec[NodeH.GetKeyId(NId)] = TStr::GetNullStr();
//  }
//  for (i = 0; i < VecOfFltVecsN.Len(); i++) {
//    TVec<TFlt>& FltVec = VecOfFltVecsN[i];
//    FltVec[NodeH.GetKeyId(NId)] = TFlt::Mn;
//  }
//  NodeH.DelKey(NId);
//}

int64 TNEANetMP::AddEdge(const int64& SrcNId, const int64& DstNId, int64 EId) {
  int64 i;

  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  EdgeH.AddDat(EId, TEdge(EId, SrcNId, DstNId));
  GetNode(SrcNId).OutEIdV.AddSorted(EId);
  GetNode(DstNId).InEIdV.AddSorted(EId);

  // update attribute columns
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt64, int64>& IntVec = VecOfIntVecsE[i];
    IntVec.Ins(EdgeH.GetKeyId(EId), TInt64::Mn);
  }
  TVec<TStr, int64> DefIntVec = TVec<TStr, int64>();
  IntDefaultsE.GetKeyV(DefIntVec);
  for (i = 0; i < DefIntVec.Len(); i++) {
    TStr attr = DefIntVec[i];
    TVec<TInt64, int64>& IntVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(DefIntVec[i]).Val2];
    IntVec[EdgeH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr, int64>& StrVec = VecOfStrVecsE[i];
    StrVec.Ins(EdgeH.GetKeyId(EId), TStr::GetNullStr());
  }
  TVec<TStr, int64> DefStrVec = TVec<TStr, int64>();
  IntDefaultsE.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr, int64>& StrVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(DefStrVec[i]).Val2];
    StrVec[EdgeH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt, int64>& FltVec = VecOfFltVecsE[i];
    FltVec.Ins(EdgeH.GetKeyId(EId), TFlt::Mn);
  }
  TVec<TStr, int64> DefFltVec = TVec<TStr, int64>();
  FltDefaultsE.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt, int64>& FltVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(DefFltVec[i]).Val2];
    FltVec[NodeH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  }
  return EId;
}

void TNEANetMP::AddEdgeUnchecked(const TInt64& EId, const int64 SrcNId, const int64 DstNId) {
  int64 Idx = abs((EId.GetPrimHashCd()) % ReservedE());
  int64 KeyId = EdgeH.AddKey13(Idx, EId);
  MxEId = TMath::Mx(EId+1, MxEId());
  EdgeH[KeyId] = TEdge(EId, SrcNId, DstNId);
}

//void TNEANetMP::DelEdge(const int& EId) {
//  int i;
//
//  IAssert(IsEdge(EId));
//  const int SrcNId = GetEdge(EId).GetSrcNId();
//  const int DstNId = GetEdge(EId).GetDstNId();
//  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
//  GetNode(DstNId).InEIdV.DelIfIn(EId);
//  EdgeH.DelKey(EId);
//
//  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
//    TVec<TInt>& IntVec = VecOfIntVecsE[i];
//    IntVec.Ins(EdgeH.GetKeyId(EId), TInt::Mn);
//  }
//  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
//    TVec<TStr>& StrVec = VecOfStrVecsE[i];
//    StrVec.Ins(EdgeH.GetKeyId(EId), TStr::GetNullStr());
//  }
//  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
//    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
//    FltVec.Ins(EdgeH.GetKeyId(EId), TFlt::Mn);
//  }
//}

//// delete all edges between the two nodes
//void TNEANetMP::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
//  int EId = 0;
//  bool Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
//  IAssert(Edge); // there is at least one edge
//  while (Edge) {
//    DelEdge(EId);
//    Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
//  }
//}

bool TNEANetMP::IsEdge(const int64& SrcNId, const int64& DstNId, int64& EId, const bool& IsDir) const {
  const TNode& SrcNode = GetNode(SrcNId);
  for (int64 edge = 0; edge < SrcNode.GetOutDeg(); edge++) {
    const TEdge& Edge = GetEdge(SrcNode.GetOutEId(edge));
    if (DstNId == Edge.GetDstNId()) {
      EId = Edge.GetId();
      return true;
    }
  }
  if (! IsDir) {
    for (int64 edge = 0; edge < SrcNode.GetInDeg(); edge++) {
      const TEdge& Edge = GetEdge(SrcNode.GetInEId(edge));
      if (DstNId == Edge.GetSrcNId()) {
        EId = Edge.GetId();
        return true;
      }
    }
  }
  return false;
}

void TNEANetMP::GetNIdV(TInt64V& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int64 N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N));
  }
}

void TNEANetMP::GetEIdV(TInt64V& EIdV) const {
  EIdV.Gen(GetEdges(), 0);
  for (int64 E=EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    EIdV.Add(EdgeH.GetKey(E));
  }
}

void TNEANetMP::Defrag(const bool& OnlyNodeLinks) {
  #if 0
  for (int64 kid = NodeH.FFirstKeyId(); NodeH.FNextKeyId(kid); ) {
    TNode& Node = NodeH[kid];
    Node.InEIdV.Pack();  Node.OutEIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
  if (! OnlyNodeLinks && ! EdgeH.IsKeyIdEqKeyN()) { EdgeH.Defrag(); }
  #endif
}

bool TNEANetMP::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int64 N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edge ids
    int64 prevEId = -1;
    for (int64 e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsEdge(Node.GetOutEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetOutEId(e), Node.GetId());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetOutEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetOutEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetOutEId(e);
    }
    // check in-edge ids
    prevEId = -1;
    for (int64 e = 0; e < Node.GetInDeg(); e++) {
      if (! IsEdge(Node.GetInEId(e))) {
      const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetInEId(e), Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetInEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetInEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetInEId(e);
    }
  }
  for (int64 E = EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    const TEdge& Edge = EdgeH[E];
    if (! IsNode(Edge.GetSrcNId())) {
      const TStr Msg = TStr::Fmt("Edge %d source node %d does not exist.", Edge.GetId(), Edge.GetSrcNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! IsNode(Edge.GetDstNId())) {
      const TStr Msg = TStr::Fmt("Edge %d destination node %d does not exist.", Edge.GetId(), Edge.GetDstNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
  }
  return RetVal;
}

void TNEANetMP::Dump(FILE *OutF) const {
  const int64 NodePlaces = (int64) ceil(log10((double) GetNodes()));
  const int64 EdgePlaces = (int64) ceil(log10((double) GetEdges()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node-Edge Network: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    fprintf(OutF, "  %*d]\n", NodePlaces, NodeI.GetId());
    // load node attributes
    TInt64V IntAttrN;
    IntAttrValueNI(NodeI.GetId(), IntAttrN);
    fprintf(OutF, "    nai[%d]", IntAttrN.Len());
    for (int64 i = 0; i < IntAttrN.Len(); i++) {
      fprintf(OutF, " %*i", NodePlaces, IntAttrN[i]()); }
    TStr64V StrAttrN;
    StrAttrValueNI(NodeI.GetId(), StrAttrN);
    fprintf(OutF, "    nas[%d]", StrAttrN.Len());
    for (int64 i = 0; i < StrAttrN.Len(); i++) {
      fprintf(OutF, " %*s", NodePlaces, StrAttrN[i]()); }
    TFlt64V FltAttrN;
    FltAttrValueNI(NodeI.GetId(), FltAttrN);
    fprintf(OutF, "    naf[%d]", FltAttrN.Len());
    for (int64 i = 0; i < FltAttrN.Len(); i++) {
      fprintf(OutF, " %*f", NodePlaces, FltAttrN[i]()); }

    fprintf(OutF, "    in[%d]", NodeI.GetInDeg());
    for (int64 edge = 0; edge < NodeI.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetInEId(edge)); }
    fprintf(OutF, "\n");
    fprintf(OutF, "    out[%d]", NodeI.GetOutDeg());
    for (int64 edge = 0; edge < NodeI.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetOutEId(edge)); }
    fprintf(OutF, "\n");
  }
  for (TEdgeI EdgeI = BegEI(); EdgeI < EndEI(); EdgeI++) {
    fprintf(OutF, "  %*d]  %*d  ->  %*d", EdgePlaces, EdgeI.GetId(), NodePlaces, EdgeI.GetSrcNId(), NodePlaces, EdgeI.GetDstNId());

    // load edge attributes
    TInt64V IntAttrE;
    IntAttrValueEI(EdgeI.GetId(), IntAttrE);
    fprintf(OutF, "    eai[%d]", IntAttrE.Len());
    for (int64 i = 0; i < IntAttrE.Len(); i++) {
      fprintf(OutF, " %*i", EdgePlaces, IntAttrE[i]());
    }
    TStr64V StrAttrE;
    StrAttrValueEI(EdgeI.GetId(), StrAttrE);
    fprintf(OutF, "    eas[%d]", StrAttrE.Len());
    for (int64 i = 0; i < StrAttrE.Len(); i++) {
      fprintf(OutF, " %*s", EdgePlaces, StrAttrE[i]());
    }
    TFlt64V FltAttrE;
    FltAttrValueEI(EdgeI.GetId(), FltAttrE);
    fprintf(OutF, "    eaf[%d]", FltAttrE.Len());
    for (int64 i = 0; i < FltAttrE.Len(); i++) {
      fprintf(OutF, " %*f", EdgePlaces, FltAttrE[i]());
    }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

// Attribute related function

int64 TNEANetMP::AddIntAttrDatN(const int64& NId, const TInt64& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TInt64, int64>& NewVec = VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)] = value;
  } else {
    CurrLen = VecOfIntVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TInt64Pr(IntType, CurrLen));
    TVec<TInt64, int64> NewVec = TVec<TInt64, int64>();
    for (i = 0; i < MxNId; i++) {
      NewVec.Ins(i, (TInt64) TInt64::Mn);
    }
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfIntVecsN.Add(NewVec);
  }
  return 0;
}

int64 TNEANetMP::AddStrAttrDatN(const int64& NId, const TStr& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TStr, int64>& NewVec = VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)] = value;
  } else {
    CurrLen = VecOfStrVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TInt64Pr(StrType, CurrLen));
    TVec<TStr, int64> NewVec = TVec<TStr, int64>();
    for (i = 0; i < MxNId; i++) {
        NewVec.Ins(i, (TStr) TStr::GetNullStr() );
    }
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfStrVecsN.Add(NewVec);
  }
  return 0;
}

int64 TNEANetMP::AddFltAttrDatN(const int64& NId, const TFlt& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;

  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TFlt, int64>& NewVec = VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)] = value;
  } else {
    CurrLen = VecOfFltVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TInt64Pr(FltType, CurrLen));
    TVec<TFlt, int64> NewVec = TVec<TFlt, int64>();
    for (i = 0; i < MxNId; i++) {
      NewVec.Ins(i, GetFltAttrDefaultN(attr));
    }
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfFltVecsN.Add(NewVec);
  }
  return 0;
}


int64 TNEANetMP::AddIntAttrDatE(const int64& EId, const TInt64& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TInt64, int64>& NewVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfIntVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TInt64Pr(IntType, CurrLen));
    TVec<TInt64, int64> NewVec = TVec<TInt64, int64>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, (TInt64) TInt64::Mn);
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfIntVecsE.Add(NewVec);
  }
  return 0;
}


int64 TNEANetMP::AddStrAttrDatE(const int64& EId, const TStr& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TStr, int64>& NewVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfStrVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TInt64Pr(StrType, CurrLen));
    TVec<TStr, int64> NewVec = TVec<TStr, int64>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, (TStr) TStr::GetNullStr());
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfStrVecsE.Add(NewVec);
  }
  return 0;
}

int64 TNEANetMP::AddFltAttrDatE(const int64& EId, const TFlt& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;

  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TFlt, int64>& NewVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfFltVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TInt64Pr(FltType, CurrLen));
    TVec<TFlt, int64> NewVec = TVec<TFlt, int64>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetFltAttrDefaultE(attr));
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfFltVecsE.Add(NewVec);
  }
  return 0;
}

TVec<TFlt, int64>& TNEANetMP::GetFltAttrVecE(const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
}

int64 TNEANetMP::GetFltKeyIdE(const int64& EId) {
  return EdgeH.GetKeyId(EId);
}

TInt64 TNEANetMP::GetIntAttrDatN(const int64& NId, const TStr& attr) {
  return VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TInt64 TNEANetMP::GetIntAttrIndDatN(const int64& NId, const int64& index) {
  return VecOfIntVecsN[index][NodeH.GetKeyId(NId)];
}

int64 TNEANetMP::GetIntAttrIndN(const TStr& attr) {
  return KeyToIndexTypeN.GetDat(attr).Val2.Val;
}

TStr TNEANetMP::GetStrAttrDatN(const int64& NId, const TStr& attr) {
  return VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TFlt TNEANetMP::GetFltAttrDatN(const int64& NId, const TStr& attr) {
  return VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TInt64 TNEANetMP::GetIntAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TInt64 TNEANetMP::GetIntAttrIndDatE(const int64& EId, const int64& index) {
  return VecOfIntVecsE[index][EdgeH.GetKeyId(EId)];
}

int64 TNEANetMP::GetIntAttrIndE(const TStr& attr) {
  return KeyToIndexTypeE.GetDat(attr).Val2.Val;
}

TStr TNEANetMP::GetStrAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TFlt TNEANetMP::GetFltAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

int64 TNEANetMP::DelAttrDatN(const int64& NId, const TStr& attr) {
  TInt64 vecType = KeyToIndexTypeN(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetIntAttrDefaultN(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetStrAttrDefaultN(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetFltAttrDefaultN(attr);
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANetMP::DelAttrDatE(const int64& EId, const TStr& attr) {
  // TODO(nkhadke): add error checking
  TInt64 vecType = KeyToIndexTypeE(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANetMP::AddIntAttrN(const TStr& attr, TInt64 defaultValue){
  int64 i;
  TInt64 CurrLen;
  TVec<TInt64, int64> NewVec;
  CurrLen = VecOfIntVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TInt64Pr(IntType, CurrLen));
  NewVec = TVec<TInt64, int64>();
  for (i = 0; i < MxNId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfIntVecsN.Add(NewVec);
  if (!IntDefaultsN.IsKey(attr)) {
    IntDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANetMP::AddStrAttrN(const TStr& attr, TStr defaultValue) {
  int64 i;
  TInt64 CurrLen;
  TVec<TStr, int64> NewVec;
  CurrLen = VecOfStrVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TInt64Pr(StrType, CurrLen));
  NewVec = TVec<TStr, int64>();
  for (i = 0; i < MxNId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfStrVecsN.Add(NewVec);
  if (!StrDefaultsN.IsKey(attr)) {
    StrDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANetMP::AddFltAttrN(const TStr& attr, TFlt defaultValue) {
  // TODO(nkhadke): add error checking
  int64 i;
  TInt64 CurrLen;
  TVec<TFlt, int64> NewVec;
  CurrLen = VecOfStrVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TInt64Pr(FltType, CurrLen));
  NewVec = TVec<TFlt, int64>();
  for (i = 0; i < MxNId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfFltVecsN.Add(NewVec);
  if (!FltDefaultsN.IsKey(attr)) {
    FltDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANetMP::AddIntAttrE(const TStr& attr, TInt64 defaultValue){
  // TODO(nkhadke): add error checking
  int64 i;
  TInt64 CurrLen;
  TVec<TInt64, int64> NewVec;
  CurrLen = VecOfIntVecsE.Len();
  KeyToIndexTypeE.AddDat(attr, TInt64Pr(IntType, CurrLen));
  NewVec = TVec<TInt64, int64>();
  for (i = 0; i < MxEId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfIntVecsE.Add(NewVec);
  if (!IntDefaultsE.IsKey(attr)) {
    IntDefaultsE.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANetMP::AddStrAttrE(const TStr& attr, TStr defaultValue) {
  int64 i;
  TInt64 CurrLen;
  TVec<TStr, int64> NewVec;
  CurrLen = VecOfStrVecsE.Len();
  KeyToIndexTypeE.AddDat(attr, TInt64Pr(StrType, CurrLen));
  NewVec = TVec<TStr, int64>();
  for (i = 0; i < MxEId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfStrVecsE.Add(NewVec);
  if (!StrDefaultsE.IsKey(attr)) {
    StrDefaultsE.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANetMP::AddFltAttrE(const TStr& attr, TFlt defaultValue) {
  int64 i;
  TInt64 CurrLen;
  TVec<TFlt, int64> NewVec;
  CurrLen = VecOfStrVecsE.Len();
  KeyToIndexTypeE.AddDat(attr, TInt64Pr(FltType, CurrLen));
  NewVec = TVec<TFlt, int64>();
  for (i = 0; i < MxEId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfFltVecsE.Add(NewVec);
  if (!FltDefaultsE.IsKey(attr)) {
    FltDefaultsE.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

//int TNEANetMP::DelAttrN(const TStr& attr) {
//  TInt vecType = KeyToIndexTypeN(attr).Val1;
//  if (vecType == IntType) {
//    VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TInt>();
//    if (IntDefaultsN.IsKey(attr)) {
//      IntDefaultsN.DelKey(attr);
//    }
//  } else if (vecType == StrType) {
//    VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TStr>();
//    if (StrDefaultsN.IsKey(attr)) {
//      StrDefaultsN.DelKey(attr);
//    }
//  } else if (vecType == FltType) {
//    VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TFlt>();
//    if (FltDefaultsN.IsKey(attr)) {
//      FltDefaultsN.DelKey(attr);
//    }
//  } else {
//    return -1;
//  }
//  KeyToIndexTypeN.DelKey(attr);
//  return 0;
//}

//int TNEANetMP::DelAttrE(const TStr& attr) {
//  TInt vecType = KeyToIndexTypeE(attr).Val1;
//  if (vecType == IntType) {
//    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TInt>();
//    if (IntDefaultsE.IsKey(attr)) {
//      IntDefaultsE.DelKey(attr);
//    }
//  } else if (vecType == StrType) {
//    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TStr>();
//    if (StrDefaultsE.IsKey(attr)) {
//      StrDefaultsE.DelKey(attr);
//    }
//  } else if (vecType == FltType) {
//    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TFlt>();
//    if (FltDefaultsE.IsKey(attr)) {
//      FltDefaultsE.DelKey(attr);
//    }
//  } else {
//    return -1;
//  }
//  KeyToIndexTypeE.DelKey(attr);
//  return 0;
//}

TFlt TNEANetMP::GetWeightOutEdges(const TNodeI& NI, const TStr& attr) {
  TNode Node = GetNode(NI.GetId());
  TInt64V OutEIdV = Node.OutEIdV;
  TFlt total = 0;
  int64 len = Node.OutEIdV.Len();
  for (int64 i = 0; i < len; i++) {
    total += GetFltAttrDatE(Node.OutEIdV[i], attr);
  }
  return total;
}

void TNEANetMP::GetWeightOutEdgesV(TFlt64V& OutWeights, const TFlt64V& AttrVal) {
  for (TEdgeI it = BegEI(); it < EndEI(); it++) {
    int64 EId = it.GetId();
    int64 SrcId = it.GetSrcNId();
    OutWeights[SrcId] +=AttrVal[GetFltKeyIdE(EId)];
  }
}

bool TNEANetMP::IsFltAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == FltType);
}

bool TNEANetMP::IsIntAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == IntType);
}

bool TNEANetMP::IsStrAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == StrType);
}

#endif // GCC_ATOMIC
