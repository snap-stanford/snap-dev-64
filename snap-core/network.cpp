/////////////////////////////////////////////////
void TNEANet::LoadNetworkShM(TShMIn& ShMIn) {
  MxNId = TInt64(ShMIn);
  MxEId = TInt64(ShMIn);

  LoadTNodeFunctor NodeFn;
  NodeH.LoadShM(ShMIn, NodeFn);

  EdgeH.LoadShM(ShMIn);
  KeyToIndexTypeN.LoadShM(ShMIn);
  KeyToIndexTypeE.LoadShM(ShMIn);

  KeyToDenseN.LoadShM(ShMIn);
  KeyToDenseE.LoadShM(ShMIn);

  IntDefaultsN.LoadShM(ShMIn);
  IntDefaultsE.LoadShM(ShMIn);
  StrDefaultsN.LoadShM(ShMIn);
  StrDefaultsE.LoadShM(ShMIn);
  FltDefaultsE.LoadShM(ShMIn);
  FltDefaultsE.LoadShM(ShMIn);

  LoadVecFunctor VecFn;
  VecOfIntVecsN.LoadShM(ShMIn, VecFn);
  VecOfIntVecsE.LoadShM(ShMIn, VecFn);
  /* Strings and floats are complicated, so don't optimize on these */
  VecOfStrVecsN.Load(ShMIn);
  VecOfStrVecsE.Load(ShMIn);
  VecOfFltVecsN.Load(ShMIn);
  VecOfFltVecsE.Load(ShMIn);

  LoadVecOfVecFunctor VecVecFn;
  VecOfIntVecVecsN.LoadShM(ShMIn, VecVecFn);
  VecOfIntVecVecsE.LoadShM(ShMIn, VecVecFn);

  LoadHashOfVecFunctor HashVecFn;
  VecOfIntHashVecsN.LoadShM(ShMIn, HashVecFn);
  VecOfIntHashVecsE.LoadShM(ShMIn, HashVecFn);

  /* Attributes are complicated so load these straight */
  SAttrN.Load(ShMIn);
  SAttrE.Load(ShMIn);
}

// Attribute Node Edge Network
bool TNEANet::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNEANet::TNet, Flag);
}

bool TNEANet::TNodeI::IsInNId(const int64& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int64 edge = 0; edge < Node.GetInDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetInEId(edge)).GetSrcNId())
      return true;
  }
  return false;
}

bool TNEANet::TNodeI::IsOutNId(const int64& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int64 edge = 0; edge < Node.GetOutDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetOutEId(edge)).GetDstNId())
      return true;
  }
  return false;
}

void TNEANet::AttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (!NodeAttrIsDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANet::AttrValueNI(const TInt64& NId , TStrIntPr64H::TIter NodeHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (!NodeAttrIsDeleted(NId, NodeHI)) {
      Values.Add(GetNodeAttrValue(NId, NodeHI));
    }
    NodeHI++;
  }
}

void TNEANet::IntAttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntType && !NodeAttrIsIntDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANet::IntAttrValueNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TInt64V& Values) const {
  Values = TVec<TInt64, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntType && !NodeAttrIsIntDeleted(NId, NodeHI)) {
      TInt64 val = this->VecOfIntVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId));
      Values.Add(val);
    }
    NodeHI++;
  }
}

void TNEANet::IntVAttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntVType) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANet::IntVAttrValueNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TVec<TInt64V, int64>& Values) const {
  Values = TVec<TInt64V, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntVType) {
      TInt64 index = NodeHI.GetDat().Val2;
      TStr attr =  NodeHI.GetKey();
      TInt loc = CheckDenseOrSparseN(attr);
      if (loc == 1) {
        TInt64V val = this->VecOfIntVecVecsN.GetVal(index).GetVal(NodeH.GetKeyId(NId));
        if (val.Len() != 0) Values.Add(val);
      } else {
        const THash<TInt64, TInt64V, int64>& NewHash = VecOfIntHashVecsN[index];
        if (NewHash.IsKey(NodeH.GetKeyId(NId))) {
          Values.Add(NewHash[NodeH.GetKeyId(NId)]);
        }
      }
    }
    NodeHI++;
  }
}

void TNEANet::StrAttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == StrType && !NodeAttrIsStrDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANet::StrAttrValueNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == StrType && !NodeAttrIsStrDeleted(NId, NodeHI)) {
      TStr val = this->VecOfStrVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId));
      Values.Add(val);
    }
    NodeHI++;
  }
}

void TNEANet::FltAttrNameNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == FltType && !NodeAttrIsFltDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }
}

void TNEANet::FltAttrValueNI(const TInt64& NId, TStrIntPr64H::TIter NodeHI, TFlt64V& Values) const {
  Values = TVec<TFlt, int64>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == FltType && !NodeAttrIsFltDeleted(NId, NodeHI)) {
      TFlt val = (this->VecOfFltVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId)));
      Values.Add(val);
    }
    NodeHI++;
  }
}

bool TNEANet::IsAttrDeletedN(const int64& NId, const TStr& attr) const {
  bool IntDel = IsIntAttrDeletedN(NId, attr);
  bool StrDel = IsStrAttrDeletedN(NId, attr);
  bool FltDel = IsFltAttrDeletedN(NId, attr);
  bool IntVDel = IsIntVAttrDeletedN(NId, attr);
  return IntDel || StrDel || FltDel || IntVDel;
}

bool TNEANet::IsIntAttrDeletedN(const int64& NId, const TStr& attr) const {
  return NodeAttrIsIntDeleted(NId, KeyToIndexTypeN.GetI(attr));
}

bool TNEANet::IsIntVAttrDeletedN(const int64& NId, const TStr& attr) const {
  return NodeAttrIsIntVDeleted(NId, KeyToIndexTypeN.GetI(attr));
}

bool TNEANet::IsStrAttrDeletedN(const int64& NId, const TStr& attr) const {
  return NodeAttrIsStrDeleted(NId, KeyToIndexTypeN.GetI(attr));
}

bool TNEANet::IsFltAttrDeletedN(const int64& NId, const TStr& attr) const {
  return NodeAttrIsFltDeleted(NId, KeyToIndexTypeN.GetI(attr));
}

bool TNEANet::NodeAttrIsDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  bool IntDel = NodeAttrIsIntDeleted(NId, NodeHI);
  bool StrDel = NodeAttrIsStrDeleted(NId, NodeHI);
  bool FltDel = NodeAttrIsFltDeleted(NId, NodeHI);
  bool IntVDel = NodeAttrIsIntVDeleted(NId, NodeHI);
  return IntDel || StrDel || FltDel || IntVDel;
}

bool TNEANet::NodeAttrIsIntDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != IntType) {
    return false;
  }
  return (GetIntAttrDefaultN(NodeHI.GetKey()) == this->VecOfIntVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

bool TNEANet::NodeAttrIsIntVDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != IntVType) {
    return false;
  }
  return (TInt64V() == this->VecOfIntVecVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

bool TNEANet::NodeAttrIsStrDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != StrType) {
    return false;
  }
  return (GetStrAttrDefaultN(NodeHI.GetKey()) == this->VecOfStrVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

bool TNEANet::NodeAttrIsFltDeleted(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != FltType) {
    return false;
  }
  return (GetFltAttrDefaultN(NodeHI.GetKey()) == this->VecOfFltVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

TStr TNEANet::GetNodeAttrValue(const int64& NId, const TStrIntPr64H::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsN.GetVal(
      this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId))).GetStr();
  } else if (NodeHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId));
  } else if (NodeHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsN.GetVal(
      this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId))).GetStr();
  }
  return TStr();
}

void TNEANet::AttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANet::AttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, EdgeHI)) {
      Values.Add(GetEdgeAttrValue(EId, EdgeHI));
    }
    EdgeHI++;
  }
}

void TNEANet::IntAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANet::IntAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TInt64V& Values) const {
  Values = TVec<TInt64, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, EdgeHI)) {
      TInt64 val = (this->VecOfIntVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    EdgeHI++;
  }
}

void TNEANet::IntVAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntVType) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANet::IntVAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TVec<TInt64V, int64>& Values) const {
  Values = TVec<TInt64V, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntVType) {
      TInt64 index = EdgeHI.GetDat().Val2;
      TStr attr =  EdgeHI.GetKey();
      TInt loc = CheckDenseOrSparseE(attr);
      if (loc == 1) {
        TInt64V val = this->VecOfIntVecVecsE.GetVal(index).GetVal(EdgeH.GetKeyId(EId));
        if (val.Len() != 0) Values.Add(val);
      } else {
        const THash<TInt64, TInt64V, int64>& NewHash = VecOfIntHashVecsE[index];
        if (NewHash.IsKey(EdgeH.GetKeyId(EId))) {
          Values.Add(NewHash[EdgeH.GetKeyId(EId)]);
        }
      }
    }
    EdgeHI++;
  }
}

void TNEANet::StrAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANet::StrAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, EdgeHI)) {
      TStr val = this->VecOfStrVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId);
      Values.Add(val);
    }
    EdgeHI++;
  }
}

void TNEANet::FltAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }
}

void TNEANet::FltAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter EdgeHI, TFlt64V& Values) const {
  Values = TVec<TFlt, int64>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, EdgeHI)) {
      TFlt val = (this->VecOfFltVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    EdgeHI++;
  }
}

bool TNEANet::IsAttrDeletedE(const int64& EId, const TStr& attr) const {
  bool IntDel = IsIntAttrDeletedE(EId, attr);
  bool IntVDel = IsIntVAttrDeletedE(EId, attr);
  bool StrDel = IsStrAttrDeletedE(EId, attr);
  bool FltDel = IsFltAttrDeletedE(EId, attr);
  return IntDel || StrDel || FltDel || IntVDel;
}

bool TNEANet::IsIntAttrDeletedE(const int64& EId, const TStr& attr) const {
  return EdgeAttrIsIntDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::IsIntVAttrDeletedE(const int64& EId, const TStr& attr) const {
  return EdgeAttrIsIntVDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::IsStrAttrDeletedE(const int64& EId, const TStr& attr) const {
  return EdgeAttrIsStrDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::IsFltAttrDeletedE(const int64& EId, const TStr& attr) const {
  return EdgeAttrIsFltDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::EdgeAttrIsDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  bool IntDel = EdgeAttrIsIntDeleted(EId, EdgeHI);
  bool IntVDel = EdgeAttrIsIntVDeleted(EId, EdgeHI);
  bool StrDel = EdgeAttrIsStrDeleted(EId, EdgeHI);
  bool FltDel = EdgeAttrIsFltDeleted(EId, EdgeHI);
  return IntDel || StrDel || FltDel || IntVDel;
}

bool TNEANet::EdgeAttrIsIntDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == IntType &&
    GetIntAttrDefaultE(EdgeHI.GetKey()) == this->VecOfIntVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

bool TNEANet::EdgeAttrIsIntVDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == IntVType &&
    TInt64V() == this->VecOfIntVecVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

bool TNEANet::EdgeAttrIsStrDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == StrType &&
    GetStrAttrDefaultE(EdgeHI.GetKey()) == this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

bool TNEANet::EdgeAttrIsFltDeleted(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == FltType &&
    GetFltAttrDefaultE(EdgeHI.GetKey()) == this->VecOfFltVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

TStr TNEANet::GetEdgeAttrValue(const int64& EId, const TStrIntPr64H::TIter& EdgeHI) const {
  if (EdgeHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId))).GetStr();
  } else if (EdgeHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId));
  } else if (EdgeHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId))).GetStr();
  }
  return TStr();
}

int64 TNEANet::AddNode(int64 NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %s already exists", TInt64::GetStr(NId).CStr()));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  AddAttributes(NId);
  return NId;
}

int64 TNEANet::AddNodeUnchecked(int64 NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  AddAttributes(NId);
  return NId;
}

int64 TNEANet::AddAttributes(const int64 NId) {
  int64 i;
  // update attribute columns
  for (i = 0; i < VecOfIntVecsN.Len(); i++) {
    TVec<TInt64, int64>& IntVec = VecOfIntVecsN[i];
    int64 KeyId = NodeH.GetKeyId(NId);
    if (IntVec.Len() > KeyId) {
      IntVec[KeyId] = TInt64::Mn;
    } else {
      IntVec.Ins(KeyId, TInt64::Mn);
    }
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
    int64 KeyId = NodeH.GetKeyId(NId);
    if (StrVec.Len() > KeyId) {
      StrVec[KeyId] = TStr();
    } else {
      StrVec.Ins(KeyId, TStr());
    }
  }
  TVec<TStr, int64> DefStrVec = TVec<TStr, int64>();
  StrDefaultsN.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr, int64>& StrVec = VecOfStrVecsN[KeyToIndexTypeN.GetDat(DefStrVec[i]).Val2];
    StrVec[NodeH.GetKeyId(NId)] = GetStrAttrDefaultN(attr);
  }
  for (i = 0; i < VecOfFltVecsN.Len(); i++) {
    TVec<TFlt, int64>& FltVec = VecOfFltVecsN[i];
    int64 KeyId = NodeH.GetKeyId(NId);
    if (FltVec.Len() > KeyId) {
      FltVec[KeyId] = TFlt::Mn;
    } else {
      FltVec.Ins(KeyId, TFlt::Mn);
    }
  }
  TVec<TStr, int64> DefFltVec = TVec<TStr, int64>();
  FltDefaultsN.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt, int64>& FltVec = VecOfFltVecsN[KeyToIndexTypeN.GetDat(DefFltVec[i]).Val2];
    FltVec[NodeH.GetKeyId(NId)] = GetFltAttrDefaultN(attr);
  }
  for (i = 0; i < VecOfIntVecVecsN.Len(); i++) {
    TVec<TInt64V, int64>& IntVecV = VecOfIntVecVecsN[i];
    int64 KeyId = NodeH.GetKeyId(NId);
    if (IntVecV.Len() > KeyId) {
      IntVecV[KeyId] = TInt64V();
    } else {
      IntVecV.Ins(KeyId, TInt64V());
    }
  }
  return NId;
}

void TNEANet::DelNode(const int64& NId) {
  int64 i;
  TInt64 Id(NId);
  SAttrN.DelSAttrId(Id);
  const TNode& Node = GetNode(NId);
  for (int64 out = 0; out < Node.GetOutDeg(); out++) {
    const int64 EId = Node.GetOutEId(out);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetSrcNId() == NId);
    GetNode(Edge.GetDstNId()).InEIdV.DelIfIn(EId);
    // Delete from Edge Attributes
    for (i = 0; i < VecOfIntVecsE.Len(); i++) {
      TVec<TInt64, int64>& IntVec = VecOfIntVecsE[i];
      IntVec[EdgeH.GetKeyId(EId)] = TInt64::Mn;
    }
    for (i = 0; i < VecOfStrVecsE.Len(); i++) {
      TVec<TStr, int64>& StrVec = VecOfStrVecsE[i];
      StrVec[EdgeH.GetKeyId(EId)] = TStr();
    }
    for (i = 0; i < VecOfFltVecsE.Len(); i++) {
      TVec<TFlt, int64>& FltVec = VecOfFltVecsE[i];
      FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
    }
    for (i = 0; i < VecOfIntVecVecsE.Len(); i++) {
      TVec<TInt64V, int64>& IntVecV = VecOfIntVecVecsE[i];
      IntVecV[EdgeH.GetKeyId(EId)] = TInt64V();
    }
    EdgeH.DelKey(EId);
    for (i = 0; i < VecOfIntHashVecsE.Len(); i++) {
      THash<TInt64, TInt64V, int64>& IntHashV = VecOfIntHashVecsE[i];
      if (IntHashV.IsKey(EdgeH.GetKeyId(EId))) {
        IntHashV.DelKey(EdgeH.GetKeyId(EId));
      }
    }
  }
  for (int64 in = 0; in < Node.GetInDeg(); in++) {
    const int64 EId = Node.GetInEId(in);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetDstNId() == NId);
    GetNode(Edge.GetSrcNId()).OutEIdV.DelIfIn(EId);
    // Delete from Edge Attributes
    for (i = 0; i < VecOfIntVecsE.Len(); i++) {
      TVec<TInt64, int64>& IntVec = VecOfIntVecsE[i];
      IntVec[EdgeH.GetKeyId(EId)] = TInt64::Mn;
    }
    for (i = 0; i < VecOfStrVecsE.Len(); i++) {
      TVec<TStr, int64>& StrVec = VecOfStrVecsE[i];
      StrVec[EdgeH.GetKeyId(EId)] = TStr();
    }
    for (i = 0; i < VecOfFltVecsE.Len(); i++) {
      TVec<TFlt, int64>& FltVec = VecOfFltVecsE[i];
      FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
    }
    for (i = 0; i < VecOfIntVecVecsE.Len(); i++) {
      TVec<TInt64V, int64>& IntVecV = VecOfIntVecVecsE[i];
      IntVecV[EdgeH.GetKeyId(EId)] = TInt64V();
    }
    for (i = 0; i < VecOfIntHashVecsE.Len(); i++) {
      THash<TInt64, TInt64V, int64>& IntHashV = VecOfIntHashVecsE[i];
      if (IntHashV.IsKey(EdgeH.GetKeyId(EId))) {
        IntHashV.DelKey(EdgeH.GetKeyId(EId));
      }
    }
    EdgeH.DelKey(EId);
  }

  for (i = 0; i < VecOfIntVecsN.Len(); i++) {
    TVec<TInt64, int64>& IntVec = VecOfIntVecsN[i];
    IntVec[NodeH.GetKeyId(NId)] = TInt64::Mn;
  }
  for (i = 0; i < VecOfStrVecsN.Len(); i++) {
    TVec<TStr, int64>& StrVec = VecOfStrVecsN[i];
    StrVec[NodeH.GetKeyId(NId)] = TStr();
  }
  for (i = 0; i < VecOfFltVecsN.Len(); i++) {
    TVec<TFlt, int64>& FltVec = VecOfFltVecsN[i];
    FltVec[NodeH.GetKeyId(NId)] = TFlt::Mn;
  }
  for (i = 0; i < VecOfIntVecVecsN.Len(); i++) {
    TVec<TInt64V, int64>& IntVecV = VecOfIntVecVecsN[i];
    IntVecV[NodeH.GetKeyId(NId)] = TInt64V();
  }
  for (i = 0; i < VecOfIntHashVecsN.Len(); i++) {
    THash<TInt64, TInt64V, int64>& IntHashV = VecOfIntHashVecsN[i];
    if (IntHashV.IsKey(NodeH.GetKeyId(NId))) {
      IntHashV.DelKey(NodeH.GetKeyId(NId));
    }
  }
  NodeH.DelKey(NId);
}

int64 TNEANet::AddEdge(const int64& SrcNId, const int64& DstNId, int64 EId) {
  int64 i;

  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %s already exists", TInt64::GetStr(EId).CStr()));
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%s or %s not a node.", TInt64::GetStr(SrcNId).CStr(), TInt64::GetStr(DstNId).CStr()).CStr());
  EdgeH.AddDat(EId, TEdge(EId, SrcNId, DstNId));
  GetNode(SrcNId).OutEIdV.AddSorted(EId);
  GetNode(DstNId).InEIdV.AddSorted(EId);

  // update attribute columns
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt64, int64>& IntVec = VecOfIntVecsE[i];
    int64 KeyId = EdgeH.GetKeyId(EId);
    if (IntVec.Len() > KeyId) {
      IntVec[KeyId] = TInt64::Mn;
    } else {
      IntVec.Ins(KeyId, TInt64::Mn);
    }
  }
  TVec<TStr, int64> DefIntVec = TVec<TStr, int64>();
  IntDefaultsE.GetKeyV(DefIntVec);
  for (i = 0; i < DefIntVec.Len(); i++) {
    TStr attr = DefIntVec[i];
    TVec<TInt64, int64>& IntVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(DefIntVec[i]).Val2];
    IntVec[EdgeH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfIntVecVecsE.Len(); i++) {
    TVec<TInt64V, int64>& IntVecV = VecOfIntVecVecsE[i];
    IntVecV.Ins(EdgeH.GetKeyId(EId), TInt64V());
  }

  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr, int64>& StrVec = VecOfStrVecsE[i];
    int64 KeyId = EdgeH.GetKeyId(EId);
    if (StrVec.Len() > KeyId) {
      StrVec[KeyId] = TStr();
    } else {
      StrVec.Ins(KeyId, TStr());
    }
  }
  TVec<TStr, int64> DefStrVec = TVec<TStr, int64>();
  StrDefaultsE.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr, int64>& StrVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(DefStrVec[i]).Val2];
    StrVec[EdgeH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt, int64>& FltVec = VecOfFltVecsE[i];
    int64 KeyId = EdgeH.GetKeyId(EId);
    if (FltVec.Len() > KeyId) {
      FltVec[KeyId] = TFlt::Mn;
    } else {
      FltVec.Ins(KeyId, TFlt::Mn);
    }
  }
  TVec<TStr, int64> DefFltVec = TVec<TStr, int64>();
  FltDefaultsE.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt, int64>& FltVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(DefFltVec[i]).Val2];
    FltVec[EdgeH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  }
  return EId;
}

void TNEANet::DelEdge(const int64& EId) {
  int64 i;

  IAssert(IsEdge(EId));
  const int64 SrcNId = GetEdge(EId).GetSrcNId();
  const int64 DstNId = GetEdge(EId).GetDstNId();
  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
  GetNode(DstNId).InEIdV.DelIfIn(EId);
  TInt64 Id(EId);
  SAttrE.DelSAttrId(Id);

  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt64, int64>& IntVec = VecOfIntVecsE[i];
    IntVec[EdgeH.GetKeyId(EId)] = TInt64::Mn;
  }
  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr, int64>& StrVec = VecOfStrVecsE[i];
    StrVec[EdgeH.GetKeyId(EId)] = TStr();
  }
  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt, int64>& FltVec = VecOfFltVecsE[i];
    FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
  }

  EdgeH.DelKey(EId);
}

// delete all edges between the two nodes
void TNEANet::DelEdge(const int64& SrcNId, const int64& DstNId, const bool& IsDir) {
  int64 EId = 0;
  bool Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  IAssert(Edge); // there is at least one edge
  while (Edge) {
    DelEdge(EId);
    Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  }
}

bool TNEANet::IsEdge(const int64& SrcNId, const int64& DstNId, int64& EId, const bool& IsDir) const {
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

void TNEANet::GetNIdV(TInt64V& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int64 N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N));
  }
}

void TNEANet::GetEIdV(TInt64V& EIdV) const {
  EIdV.Gen(GetEdges(), 0);
  for (int64 E=EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    EIdV.Add(EdgeH.GetKey(E));
  }
}

void TNEANet::Defrag(const bool& OnlyNodeLinks) {
  for (int64 kid = NodeH.FFirstKeyId(); NodeH.FNextKeyId(kid); ) {
    TNode& Node = NodeH[kid];
    Node.InEIdV.Pack();  Node.OutEIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
  if (! OnlyNodeLinks && ! EdgeH.IsKeyIdEqKeyN()) { EdgeH.Defrag(); }
}

bool TNEANet::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int64 N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-edge list of node %s is not sorted.", TInt64::GetStr(Node.GetId()).CStr());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-edge list of node %s is not sorted.", TInt64::GetStr(Node.GetId()).CStr());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edge ids
    int64 prevEId = -1;
    for (int64 e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsEdge(Node.GetOutEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %s of node %s does not exist.",  TInt64::GetStr(Node.GetOutEId(e)).CStr(), TInt64::GetStr(Node.GetId()).CStr());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetOutEId(e)) {
        const TStr Msg = TStr::Fmt("Node %s has duplidate out-edge id %s.", TInt64::GetStr(Node.GetId()).CStr(), TInt64::GetStr(Node.GetOutEId(e)).CStr());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetOutEId(e);
    }
    // check in-edge ids
    prevEId = -1;
    for (int64 e = 0; e < Node.GetInDeg(); e++) {
      if (! IsEdge(Node.GetInEId(e))) {
      const TStr Msg = TStr::Fmt("Out-edge id %s of node %s does not exist.",  TInt64::GetStr(Node.GetInEId(e)).CStr(), TInt64::GetStr(Node.GetId()).CStr());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetInEId(e)) {
        const TStr Msg = TStr::Fmt("Node %s has duplidate out-edge id %s.", TInt64::GetStr(Node.GetId()).CStr(), TInt64::GetStr(Node.GetInEId(e)).CStr());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetInEId(e);
    }
  }
  for (int64 E = EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    const TEdge& Edge = EdgeH[E];
    if (! IsNode(Edge.GetSrcNId())) {
      const TStr Msg = TStr::Fmt("Edge %s source node %s does not exist.", TInt64::GetStr(Edge.GetId()).CStr(), TInt64::GetStr(Edge.GetSrcNId()).CStr());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! IsNode(Edge.GetDstNId())) {
      const TStr Msg = TStr::Fmt("Edge %s destination node %s does not exist.", TInt64::GetStr(Edge.GetId()).CStr(), TInt64::GetStr(Edge.GetDstNId()).CStr());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
  }
  return RetVal;
}

void TNEANet::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  const int EdgePlaces = (int) ceil(log10((double) GetEdges()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node-Edge Network with Attributes: nodes: %s, edges: %s\n", TInt64::GetStr(GetNodes()).CStr(), TInt64::GetStr(GetEdges()).CStr());
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    fprintf(OutF, "  %*s:", NodePlaces, TInt64::GetStr(NodeI.GetId()).CStr());
    fprintf(OutF, "    in[%s]", TInt64::GetStr(NodeI.GetInDeg()).CStr());
    for (int64 edge = 0; edge < NodeI.GetInDeg(); edge++) {
      fprintf(OutF, " %*s", EdgePlaces, TInt64::GetStr(NodeI.GetInEId(edge)).CStr()); }
    //fprintf(OutF, "\n");
    fprintf(OutF, "    out[%s]", TInt64::GetStr(NodeI.GetOutDeg()).CStr());
    for (int64 edge = 0; edge < NodeI.GetOutDeg(); edge++) {
      fprintf(OutF, " %*s", EdgePlaces, TInt64::GetStr(NodeI.GetOutEId(edge)).CStr()); }
    //fprintf(OutF, "\n");

    // load node attributes
    TInt64V IntAttrN;
    IntAttrValueNI(NodeI.GetId(), IntAttrN);
    fprintf(OutF, "    nai[%s]", TInt64::GetStr(IntAttrN.Len()).CStr());
    for (int64 i = 0; i < IntAttrN.Len(); i++) {
      fprintf(OutF, " %*s", NodePlaces, TInt64::GetStr(IntAttrN[i]()).CStr()); }
    //fprintf(OutF, "\n");

    TStr64V StrAttrN;
    StrAttrValueNI(NodeI.GetId(), StrAttrN);
    fprintf(OutF, "    nas[%s]", TInt64::GetStr(StrAttrN.Len()).CStr());
    for (int64 i = 0; i < StrAttrN.Len(); i++) {
      fprintf(OutF, " %*s", NodePlaces, StrAttrN[i].CStr()); }
    //fprintf(OutF, "\n");

    TFlt64V FltAttrN;
    FltAttrValueNI(NodeI.GetId(), FltAttrN);
    fprintf(OutF, "    naf[%s]", TInt64::GetStr(FltAttrN.Len()).CStr());
    for (int64 i = 0; i < FltAttrN.Len(); i++) {
      fprintf(OutF, " %*f", NodePlaces, FltAttrN[i]()); }
    //fprintf(OutF, "\n");
    fprintf(OutF, "\n");
  }
  for (TEdgeI EdgeI = BegEI(); EdgeI < EndEI(); EdgeI++) {
    fprintf(OutF, "  %*s:  %*s  ->  %*s", EdgePlaces, TInt64::GetStr(EdgeI.GetId()).CStr(), NodePlaces, TInt64::GetStr(EdgeI.GetSrcNId()).CStr(), NodePlaces, TInt64::GetStr(EdgeI.GetDstNId()).CStr());

    // load edge attributes
    TInt64V IntAttrE;
    IntAttrValueEI(EdgeI.GetId(), IntAttrE);
    fprintf(OutF, "    eai[%s]", TInt64::GetStr(IntAttrE.Len()).CStr());
    for (int64 i = 0; i < IntAttrE.Len(); i++) {
      fprintf(OutF, " %*s", EdgePlaces, TInt64::GetStr(IntAttrE[i]()).CStr());
    }
    TStr64V StrAttrE;
    StrAttrValueEI(EdgeI.GetId(), StrAttrE);
    fprintf(OutF, "    eas[%s]", TInt64::GetStr(StrAttrE.Len()).CStr());
    for (int64 i = 0; i < StrAttrE.Len(); i++) {
      fprintf(OutF, " %*s", EdgePlaces, StrAttrE[i].CStr());
    }
    TFlt64V FltAttrE;
    FltAttrValueEI(EdgeI.GetId(), FltAttrE);
    fprintf(OutF, "    eaf[%s]", TInt64::GetStr(FltAttrE.Len()).CStr());
    for (int64 i = 0; i < FltAttrE.Len(); i++) {
      fprintf(OutF, " %*f", EdgePlaces, FltAttrE[i]());
    }
    fprintf(OutF, "\n");
  }
  //fprintf(OutF, "\n");
}

bool TNEANet::IsEulerian(int64 *StartNId) {
  if (!TSnap::IsConnected(PNEANet(this))) { return false; }
  bool startfound = false, finishfound = false;

  for (TNEANet::TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    int64 indeg = NI.GetInDeg(), outdeg = NI.GetOutDeg();
    if (indeg != outdeg) {
      if (indeg - outdeg == 1) { // must be destination of path
        if (finishfound) {
          return false;
        } else {
          finishfound = true;
        }
      } else if (outdeg - indeg == 1) { // must be source of path
        if (startfound) {
          return false;
        } else {
          if (StartNId) { *StartNId = NI.GetId(); }
          startfound = true;
        }
      } else { // abs(indeg - outdeg) > 1
        return false;
      }
    }
  }

  if (!startfound && !finishfound) { // all nodes have equal indeg/outdeg
    if (StartNId) { *StartNId = BegNI().GetId(); } // cycle can begin anywhere
    return true;
  } else if (startfound && finishfound) { // exactly one start and finish node
    return true;
  } else { // one start but no finish, or vice versa
    return false;
  }
}

void TNEANet::AddPath(THash<TInt64, TVec<TInt64V, int64> >& AllPaths, const TInt64V& ToAddPath, TInt64 CurrNId, TInt64V& ResultPath) {
  for (int64 i = 0; i < ToAddPath.Len(); i++) {
    if (AllPaths.IsKey(CurrNId)) { // Pause traversing current path and recursively traverse paths reachable from here
      TVec<TInt64V, int64>& PathsStartingHere = AllPaths.GetDat(CurrNId);
      while (!PathsStartingHere.Empty()) {
        TInt64V RecursivePathToAdd = PathsStartingHere.Last();
        PathsStartingHere.DelLast(); // so recursive calls embark on new paths
        AddPath(AllPaths, RecursivePathToAdd, CurrNId, ResultPath);
      }
    }
    ResultPath.Add(ToAddPath[i]);
    CurrNId = GetEI(ToAddPath[i]).GetDstNId();
  }
}

bool TNEANet::GetEulerPath(TInt64V& Path) {
  Path = TInt64V();
  int64 StartNId;
  if (!IsEulerian(&StartNId)) { return false; }

  THash<TInt64, TInt64Set> OutboundEdges; // nodeid -> (list of outbound edges (id relative to node, not absolute eid))
  THash<TInt64, TInt64Set> SelfEdges; // nodeid -> (list of self edges (id relative to node))
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    TInt64Set out, self;
    for (int64 i = 0; i < NI.GetOutDeg(); i++) {
      TEdgeI edge = GetEI(NI.GetOutEId(i));
      if (edge.GetDstNId() == edge.GetSrcNId()) { // self edge
        self.AddKey(i);
      } else {
        out.AddKey(i);
      }
    }
    OutboundEdges.AddDat(NI.GetId(), out);
    SelfEdges.AddDat(NI.GetId(), self);
  }

  // Use Hierholzer's algorithm to generate Euler path.
  THash<TInt64, TVec<TInt64V, int64> > Paths; // Node id -> all paths that start at this node

  // 1. Create a set of edge-disjoint paths that span all edges of the graph.
  // All paths except possibly the first are cycles.
  bool firstpath = true;
  while (!OutboundEdges.Empty()) {
    TInt64V currpath;
    TInt64 PathStartNId = (firstpath ? StartNId : OutboundEdges.BegI().GetKey().Val);
    firstpath = false;
    TNodeI currnode = GetNI(PathStartNId);

    while (OutboundEdges.IsKey(currnode.GetId())) {
      TInt64Set& curroutbound = OutboundEdges.GetDat(currnode.GetId());
      TInt64Set& currself = SelfEdges.GetDat(currnode.GetId());

      if (!currself.Empty()) { // First time visit; get self edges out of the way
        for (int i = 0; i < currself.Len(); i++) { currpath.Add(currnode.GetOutEId(currself[i])); }
        currself.Clr();
      }

      // Visit the first available edge out of this node.
      TInt64 outrelid = curroutbound.BegI().GetKey();
      TInt64 outeid = currnode.GetOutEId(outrelid);
      TEdgeI outedge = GetEI(outeid);
      currpath.Add(outeid);
      // Delete the visited edge from the node's list. If the list is empty, delete the node from OutboundEdges.
      curroutbound.DelKey(outrelid);
      if (curroutbound.Empty()) { OutboundEdges.DelKey(currnode.GetId()); }
      currnode = GetNI(outedge.GetDstNId());
    }
    if (!Paths.IsKey(PathStartNId)) { Paths.AddKey(PathStartNId); }
    Paths.GetDat(PathStartNId).Add(currpath); // we're stuck at the current node so we completed a path
  }

  // 2. Traverse the first/"main" path. Each time we encounter a node with a different cycle
  // starting at it, recursively add it to the result path, then continue with the main path.
  TVec<TInt64V, int64>& PathsFromStart = Paths.GetDat(StartNId);
  PathsFromStart.Swap(0, PathsFromStart.Len()-1); // so main path can be removed efficiently
  TInt64V MainPath = PathsFromStart.Last();
  PathsFromStart.DelLast();
  AddPath(Paths, MainPath, StartNId, Path);

  return true;
}

// Attribute related functions

int64 TNEANet::AddIntAttrDatN(const int64& NId, const TInt64& value, const TStr& attr) {
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
      NewVec.Ins(i, GetIntAttrDefaultN(attr));
    }
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfIntVecsN.Add(NewVec);
  }
  return 0;
}

int64 TNEANet::AddIntVAttrDatN(const int64& NId, const TInt64V& value, const TStr& attr, TBool UseDense) {
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TInt location = CheckDenseOrSparseN(attr);
  if (location==-1) {
    AddIntVAttrN(attr, UseDense);
    location = CheckDenseOrSparseN(attr);
  }
  if (UseDense) {
    IAssertR(location != 0, TStr::Fmt("NodeId %d exists for %s in sparse representation", NId, attr.CStr()));
    TVec<TInt64V, int64>& NewVec = VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)] = value;
  } else {
    IAssertR(location != 1, TStr::Fmt("NodeId %d exists for %s in dense representation", NId, attr.CStr()));
    THash<TInt64, TInt64V, int64>& NewHash = VecOfIntHashVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewHash.AddDat(NodeH.GetKeyId(NId), value);
  }

  return 0;
}

int64 TNEANet::AppendIntVAttrDatN(const int64& NId, const TInt64& value, const TStr& attr, TBool UseDense) {
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TInt location = CheckDenseOrSparseN(attr);
  if (location==-1) {
    AddIntVAttrN(attr, UseDense);
    location = CheckDenseOrSparseN(attr);
  }
  if (UseDense) {
    IAssertR(location != 0, TStr::Fmt("NodeId %d exists for %s in sparse representation", NId, attr.CStr()));
    TVec<TInt64V, int64>& NewVec = VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)].Add(value);
  } else {
    IAssertR(location != 1, TStr::Fmt("NodeId %d exists for %s in dense representation", NId, attr.CStr()));
    THash<TInt64, TInt64V, int64>& NewHash = VecOfIntHashVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewHash[NodeH.GetKeyId(NId)].Add(value);
  }
  return 0;
}

int64 TNEANet::DelFromIntVAttrDatN(const int64& NId, const TInt64& value, const TStr& attr) {
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TInt location = CheckDenseOrSparseN(attr);
  if (location != -1) {
    if (location == 1) {
      TVec<TInt64V, int64>& NewVec = VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
      if (!NewVec[NodeH.GetKeyId(NId)].DelIfIn(value)) {
        return -1;
      }
    } else {
      THash<TInt64, TInt64V, int64>& NewHash = VecOfIntHashVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
      if (!NewHash[NodeH.GetKeyId(NId)].DelIfIn(value)) {
        return -1;
      }
    }
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANet::AddStrAttrDatN(const int64& NId, const TStr& value, const TStr& attr) {
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
        NewVec.Ins(i, GetStrAttrDefaultN(attr));
    }
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfStrVecsN.Add(NewVec);
  }
  return 0;
}

int64 TNEANet::AddFltAttrDatN(const int64& NId, const TFlt& value, const TStr& attr) {
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

int64 TNEANet::AddIntAttrDatE(const int64& EId, const TInt64& value, const TStr& attr) {
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
      NewVec.Ins(i, GetIntAttrDefaultE(attr));
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfIntVecsE.Add(NewVec);
  }
  return 0;
}

int64 TNEANet::AddIntVAttrDatE(const int64& EId, const TInt64V& value, const TStr& attr, TBool UseDense) {
  if (!IsNode(EId)) {
    // AddNode(NId);
    return -1;
  }
  TInt location = CheckDenseOrSparseE(attr);
  if (location==-1) {
    AddIntVAttrE(attr, UseDense);
    location = CheckDenseOrSparseE(attr);
  }
  if (UseDense) {
    IAssertR(location != 0, TStr::Fmt("EdgeID %d exists for %s in sparse representation", EId, attr.CStr()));
    TVec<TInt64V, int64>& NewVec = VecOfIntVecVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)] = value;
  } else {
    IAssertR(location != 1, TStr::Fmt("NodeId %d exists for %s in dense representation", EId, attr.CStr()));
    THash<TInt64, TInt64V, int64>& NewHash = VecOfIntHashVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewHash.AddDat(EdgeH.GetKeyId(EId), value);
  }
  return 0;
}

int64 TNEANet::AppendIntVAttrDatE(const int64& EId, const TInt64& value, const TStr& attr, TBool UseDense) {
  if (!IsNode(EId)) {
    // AddNode(NId);
    return -1;
  }
  TInt location = CheckDenseOrSparseE(attr);
  if (location==-1) return -1;
  if (UseDense) {
    IAssertR(location != 0, TStr::Fmt("Edge %d exists for %s in sparse representation", EId, attr.CStr()));
    TVec<TInt64V, int64>& NewVec = VecOfIntVecVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)].Add(value);
  } else {
    IAssertR(location != 1, TStr::Fmt("Edge %d exists for %s in dense representation", EId, attr.CStr()));
    THash<TInt64, TInt64V, int64>& NewHash = VecOfIntHashVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewHash[EdgeH.GetKeyId(EId)].Add(value);
  }
  return 0;
}

int64 TNEANet::AddStrAttrDatE(const int64& EId, const TStr& value, const TStr& attr) {
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
      NewVec.Ins(i, GetStrAttrDefaultE(attr));
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfStrVecsE.Add(NewVec);
  }
  return 0;
}

int64 TNEANet::AddFltAttrDatE(const int64& EId, const TFlt& value, const TStr& attr) {
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

TVec<TFlt, int64>& TNEANet::GetFltAttrVecE(const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
}

int64 TNEANet::GetFltKeyIdE(const int64& EId) {
  return EdgeH.GetKeyId(EId);
}

TInt64 TNEANet::GetIntAttrDatN(const int64& NId, const TStr& attr) {
  return VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TInt64V TNEANet::GetIntVAttrDatN(const int64& NId, const TStr& attr) const {
  TInt location = CheckDenseOrSparseN(attr);
  if (location != 0) return VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
  else return VecOfIntHashVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TStr TNEANet::GetStrAttrDatN(const int64& NId, const TStr& attr) {
  return VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TFlt TNEANet::GetFltAttrDatN(const int64& NId, const TStr& attr) {
  return VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TInt64 TNEANet::GetIntAttrIndDatN(const int64& NId, const int64& index) {
  return VecOfIntVecsN[index][NodeH.GetKeyId(NId)];
}

TStr TNEANet::GetStrAttrIndDatN(const int64& NId, const int64& index) {
  return VecOfStrVecsN[index][NodeH.GetKeyId(NId)];
}

TFlt TNEANet::GetFltAttrIndDatN(const int64& NId, const int64& index) {
  return VecOfFltVecsN[index][NodeH.GetKeyId(NId)];
}

int64 TNEANet::GetIntAttrIndN(const TStr& attr) {
  return KeyToIndexTypeN.GetDat(attr).Val2.Val;
}

int64 TNEANet::GetAttrIndN(const TStr& attr) {
  return KeyToIndexTypeN.GetDat(attr).Val2.Val;
}

TInt64 TNEANet::GetIntAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TInt64V TNEANet::GetIntVAttrDatE(const int64& EId, const TStr& attr) {
  TInt location = CheckDenseOrSparseE(attr);
  if (location != 0) return VecOfIntVecVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
  else return VecOfIntHashVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TStr TNEANet::GetStrAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TFlt TNEANet::GetFltAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TInt64 TNEANet::GetIntAttrIndDatE(const int64& EId, const int64& index) {
  return VecOfIntVecsE[index][EdgeH.GetKeyId(EId)];
}

TStr TNEANet::GetStrAttrIndDatE(const int64& EId, const int64& index) {
  return VecOfStrVecsE[index][EdgeH.GetKeyId(EId)];
}

TFlt TNEANet::GetFltAttrIndDatE(const int64& EId, const int64& index) {
  return VecOfFltVecsE[index][EdgeH.GetKeyId(EId)];
}

int64 TNEANet::GetIntAttrIndE(const TStr& attr) {
  return KeyToIndexTypeE.GetDat(attr).Val2.Val;
}

int64 TNEANet::GetAttrIndE(const TStr& attr) {
  return KeyToIndexTypeE.GetDat(attr).Val2.Val;
}

int64 TNEANet::DelAttrDatN(const int64& NId, const TStr& attr) {
  TInt64 vecType = KeyToIndexTypeN(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetIntAttrDefaultN(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetStrAttrDefaultN(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetFltAttrDefaultN(attr);
  } else if (vecType ==IntVType) {
    TInt location = CheckDenseOrSparseN(attr);
    if (location == 0) VecOfIntHashVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = TInt64V();
    else VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = TInt64V();
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANet::DelAttrDatE(const int64& EId, const TStr& attr) {
  // TODO(nkhadke): add error checking
  TInt64 vecType = KeyToIndexTypeE(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  } else if (vecType == IntVType) {
    TInt location = CheckDenseOrSparseE(attr);
    if (location == 0) VecOfIntHashVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = TInt64V();
    else VecOfIntVecVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = TInt64V();
  } else {
    return -1;
  }
  return 0;
}

int64 TNEANet::DelAllAttrDatN() {
  for (TStrIntPr64H::TIter it = KeyToIndexTypeN.BegI(); it < KeyToIndexTypeN.EndI(); it++) {
    TStr AttrName = it.GetKey();
    TInt64 vecType = it.GetDat().Val1;
    TInt64 AttrIndex = it.GetDat().Val2;
    if (vecType == IntType) {
      VecOfIntVecsN[AttrIndex] = TVec<TInt64, int64>();
    } else if (vecType == StrType) {
      VecOfStrVecsN[AttrIndex] = TVec<TStr, int64>();
    } else if (vecType == FltType) {
      VecOfFltVecsN[AttrIndex] = TVec<TFlt, int64>();
    } else if (vecType == IntVType) {
      VecOfIntVecVecsN[AttrIndex] = TVec<TInt64V, int64>();
    } else {
      return -1;
    }
  }
  SAttrN.Clr();
  return 0;
}

int64 TNEANet::DelAllAttrDatE() {
  for (TStrIntPr64H::TIter it = KeyToIndexTypeE.BegI(); it < KeyToIndexTypeE.EndI(); it++) {
    TStr AttrName = it.GetKey();
    TInt64 vecType = it.GetDat().Val1;
    TInt64 AttrIndex = it.GetDat().Val2;
    if (vecType == IntType) {
      VecOfIntVecsE[AttrIndex] = TVec<TInt64, int64>();
    } else if (vecType == StrType) {
      VecOfStrVecsE[AttrIndex] = TVec<TStr, int64>();
    } else if (vecType == FltType) {
      VecOfFltVecsE[AttrIndex] = TVec<TFlt, int64>();
    } else if (vecType == IntVType) {
      VecOfIntVecVecsE[AttrIndex] = TVec<TInt64V, int64>();
    } else {
      return -1;
    }
  }
  SAttrE.Clr();
  return 0;
}

int64 TNEANet::AddIntAttrN(const TStr& attr, TInt64 defaultValue){
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

int64 TNEANet::AddIntVAttrN(const TStr& attr, TBool UseDense){
  TInt CurrLen;
  if (UseDense) {
    CurrLen = VecOfIntVecVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TInt64Pr(IntVType, CurrLen));
    KeyToDenseN.AddDat(attr, true);
    TVec<TInt64V, int64> NewVec = TVec<TInt64V, int64>(MxNId);
    VecOfIntVecVecsN.Add(NewVec);
  } else {
    CurrLen = VecOfIntHashVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TInt64Pr(IntVType, CurrLen));
    KeyToDenseN.AddDat(attr, false);
    THash<TInt64, TInt64V, int64> NewHash;
    VecOfIntHashVecsN.Add(NewHash);
  }
  return 0;
}

int64 TNEANet::AddStrAttrN(const TStr& attr, TStr defaultValue) {
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

int64 TNEANet::AddFltAttrN(const TStr& attr, TFlt defaultValue) {
  // TODO(nkhadke): add error checking
  int64 i;
  TInt64 CurrLen;
  TVec<TFlt, int64> NewVec;
  CurrLen = VecOfFltVecsN.Len();
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

int64 TNEANet::AddIntAttrE(const TStr& attr, TInt64 defaultValue){
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

int64 TNEANet::AddIntVAttrE(const TStr& attr, TBool UseDense){
  TInt64 CurrLen;
  if (UseDense) {
    CurrLen = VecOfIntVecVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TInt64Pr(IntVType, CurrLen));
    KeyToDenseE.AddDat(attr, true);
    TVec<TInt64V, int64> NewVec = TVec<TInt64V, int64>(MxEId);
    VecOfIntVecVecsE.Add(NewVec);
  } else {
    CurrLen = VecOfIntHashVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TInt64Pr(IntVType, CurrLen));
    KeyToDenseE.AddDat(attr, false);
    THash<TInt64, TInt64V, int64> NewHash;
    VecOfIntHashVecsE.Add(NewHash);
  }
  return 0;
}

int64 TNEANet::AddStrAttrE(const TStr& attr, TStr defaultValue) {
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

int64 TNEANet::AddFltAttrE(const TStr& attr, TFlt defaultValue) {
  int64 i;
  TInt64 CurrLen;
  TVec<TFlt, int64> NewVec;
  CurrLen = VecOfFltVecsE.Len();
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

int64 TNEANet::DelAttrN(const TStr& attr) {
  TInt64 vecType = KeyToIndexTypeN(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TInt64, int64>();
    if (IntDefaultsN.IsKey(attr)) {
      IntDefaultsN.DelKey(attr);
    }
  } else if (vecType == StrType) {
    VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TStr, int64>();
    if (StrDefaultsN.IsKey(attr)) {
      StrDefaultsN.DelKey(attr);
    }
  } else if (vecType == FltType) {
    VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TFlt, int64>();
    if (FltDefaultsN.IsKey(attr)) {
      FltDefaultsN.DelKey(attr);
    }
  } else if (vecType == IntVType) {
    TInt location = CheckDenseOrSparseN(attr);
    if (location == 1) {
      VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TInt64V, int64>();
    } else if (location == 0) {
      VecOfIntHashVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = THash<TInt64, TInt64V, int64>();
    }
    if (location != -1) {
      KeyToDenseN.DelKey(attr);
    }
  } else {
    return -1;
  }
  KeyToIndexTypeN.DelKey(attr);
  return 0;
}

int64 TNEANet::DelAttrE(const TStr& attr) {
  TInt64 vecType = KeyToIndexTypeE(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TInt64, int64>();
    if (IntDefaultsE.IsKey(attr)) {
      IntDefaultsE.DelKey(attr);
    }
  } else if (vecType == StrType) {
    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TStr, int64>();
    if (StrDefaultsE.IsKey(attr)) {
      StrDefaultsE.DelKey(attr);
    }
  } else if (vecType == FltType) {
    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TFlt, int64>();
    if (FltDefaultsE.IsKey(attr)) {
      FltDefaultsE.DelKey(attr);
    }
  } else {
    return -1;
  }
  KeyToIndexTypeE.DelKey(attr);
  return 0;
}

// Return a small graph on 5 nodes and 6 edges.
PNEANet TNEANet::GetSmallGraph() {
  PNEANet Net = TNEANet::New();
  for (int64 i = 0; i < 5; i++) { Net->AddNode(i); }
  Net->AddEdge(0,1);  Net->AddEdge(0,2);
  Net->AddEdge(0,3);  Net->AddEdge(0,4);
  Net->AddEdge(1,2);  Net->AddEdge(1,2);
  return Net;
}

void TNEANet::GetAttrNNames(TStr64V& IntAttrNames, TStr64V& FltAttrNames, TStr64V& StrAttrNames) const {
  for (TStrIntPr64H::TIter it = KeyToIndexTypeN.BegI(); it < KeyToIndexTypeN.EndI(); it++) {
    if (it.GetDat().GetVal1() == IntType) {
      IntAttrNames.Add(it.GetKey());
    }
    if (it.GetDat().GetVal1() == FltType) {
      FltAttrNames.Add(it.GetKey());
    }
    if (it.GetDat().GetVal1() == StrType) {
      StrAttrNames.Add(it.GetKey());
    }
  }
}

void TNEANet::GetIntVAttrNNames(TStr64V& IntVAttrNames) const {
  for (TStrIntPr64H::TIter it = KeyToIndexTypeN.BegI(); it < KeyToIndexTypeN.EndI(); it++) {
    if (it.GetDat().GetVal1() == IntVType) {
      IntVAttrNames.Add(it.GetKey());
    }
  }
}


void TNEANet::GetAttrENames(TStr64V& IntAttrNames, TStr64V& FltAttrNames, TStr64V& StrAttrNames) const {
  for (TStrIntPr64H::TIter it = KeyToIndexTypeE.BegI(); it < KeyToIndexTypeE.EndI(); it++) {
    if (it.GetDat().GetVal1() == IntType) {
      IntAttrNames.Add(it.GetKey());
    }
    if (it.GetDat().GetVal1() == FltType) {
      FltAttrNames.Add(it.GetKey());
    }
    if (it.GetDat().GetVal1() == StrType) {
      StrAttrNames.Add(it.GetKey());
    }
  }
}

void TNEANet::GetIntVAttrENames(TStr64V& IntVAttrNames) const {
  for (TStrIntPr64H::TIter it = KeyToIndexTypeE.BegI(); it < KeyToIndexTypeE.EndI(); it++) {
    if (it.GetDat().GetVal1() == IntVType) {
      IntVAttrNames.Add(it.GetKey());
    }
  }
}


TFlt TNEANet::GetWeightOutEdges(const TNodeI& NI, const TStr& attr) {
  TNode Node = GetNode(NI.GetId());
  TInt64V OutEIdV = Node.OutEIdV;
  TFlt total = 0;
  int64 len = Node.OutEIdV.Len();
  for (int64 i = 0; i < len; i++) {
    total += GetFltAttrDatE(Node.OutEIdV[i], attr);
  }
  return total;
}

void TNEANet::GetWeightOutEdgesV(TFlt64V& OutWeights, const TFlt64V& AttrVal) {
  for (TEdgeI it = BegEI(); it < EndEI(); it++) {
    int64 EId = it.GetId();
    int64 SrcId = it.GetSrcNId();
    OutWeights[SrcId] +=AttrVal[GetFltKeyIdE(EId)];
  }
}

bool TNEANet::IsFltAttrN(const TStr& attr) {
  return (KeyToIndexTypeN.IsKey(attr) &&
    KeyToIndexTypeN.GetDat(attr).Val1 == FltType);
}

bool TNEANet::IsIntAttrN(const TStr& attr) {
  return (KeyToIndexTypeN.IsKey(attr) &&
    KeyToIndexTypeN.GetDat(attr).Val1 == IntType);
}

bool TNEANet::IsStrAttrN(const TStr& attr) {
  return (KeyToIndexTypeN.IsKey(attr) &&
    KeyToIndexTypeN.GetDat(attr).Val1 == StrType);
}

bool TNEANet::IsIntVAttrN(const TStr& attr) {
  return (KeyToIndexTypeN.IsKey(attr) &&
    KeyToIndexTypeN.GetDat(attr).Val1 == IntVType);
}

bool TNEANet::IsFltAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == FltType);
}

bool TNEANet::IsIntAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == IntType);
}

bool TNEANet::IsStrAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == StrType);
}

bool TNEANet::IsIntVAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == IntVType);
}


int64 TNEANet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TInt64& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TNEANet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TInt64& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TNEANet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TNEANet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TNEANet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TNEANet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TNEANet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TInt64& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TNEANet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TInt64& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TNEANet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TNEANet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TNEANet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TNEANet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TNEANet::DelSAttrDatN(const TInt64& NId, const TStr& AttrName) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrName);
}

int64 TNEANet::DelSAttrDatN(const TInt64& NId, const TInt64& AttrId) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrId);
}

int64 TNEANet::GetSAttrVN(const TInt64& NId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsNode(NId)) {
    return -1;
  }
  SAttrN.GetSAttrV(NId, AttrType, AttrV);
  return 0;
}

int64 TNEANet::GetIdVSAttrN(const TStr& AttrName, TInt64V& IdV) const {
  return SAttrN.GetIdVSAttr(AttrName, IdV);
}

int64 TNEANet::GetIdVSAttrN(const TInt64& AttrId, TInt64V& IdV) const {
  return SAttrN.GetIdVSAttr(AttrId, IdV);
}

int64 TNEANet::AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt64& AttrId) {
  return SAttrN.AddSAttr(Name, AttrType, AttrId);
}

int64 TNEANet::GetSAttrIdN(const TStr& Name, TInt64& AttrId, TAttrType& AttrType) const {
  return SAttrN.GetSAttrId(Name, AttrId, AttrType);
}

int64 TNEANet::GetSAttrNameN(const TInt64& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrN.GetSAttrName(AttrId, Name, AttrType);
}

int64 TNEANet::AddSAttrDatE(const TInt64& EId, const TStr& AttrName, const TInt64& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TNEANet::AddSAttrDatE(const TInt64& EId, const TInt64& AttrId, const TInt64& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TNEANet::AddSAttrDatE(const TInt64& EId, const TStr& AttrName, const TFlt& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TNEANet::AddSAttrDatE(const TInt64& EId, const TInt64& AttrId, const TFlt& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TNEANet::AddSAttrDatE(const TInt64& EId, const TStr& AttrName, const TStr& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TNEANet::AddSAttrDatE(const TInt64& EId, const TInt64& AttrId, const TStr& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TNEANet::GetSAttrDatE(const TInt64& EId, const TStr& AttrName, TInt64& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TNEANet::GetSAttrDatE(const TInt64& EId, const TInt64& AttrId, TInt64& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TNEANet::GetSAttrDatE(const TInt64& EId, const TStr& AttrName, TFlt& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TNEANet::GetSAttrDatE(const TInt64& EId, const TInt64& AttrId, TFlt& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TNEANet::GetSAttrDatE(const TInt64& EId, const TStr& AttrName, TStr& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TNEANet::GetSAttrDatE(const TInt64& EId, const TInt64& AttrId, TStr& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TNEANet::DelSAttrDatE(const TInt64& EId, const TStr& AttrName) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.DelSAttrDat(EId, AttrName);
}

int64 TNEANet::DelSAttrDatE(const TInt64& EId, const TInt64& AttrId) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.DelSAttrDat(EId, AttrId);
}

int64 TNEANet::GetSAttrVE(const TInt64& EId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  SAttrE.GetSAttrV(EId, AttrType, AttrV);
  return 0;
}

int64 TNEANet::GetIdVSAttrE(const TStr& AttrName, TInt64V& IdV) const {
  return SAttrE.GetIdVSAttr(AttrName, IdV);
}

int64 TNEANet::GetIdVSAttrE(const TInt64& AttrId, TInt64V& IdV) const {
  return SAttrE.GetIdVSAttr(AttrId, IdV);
}

int64 TNEANet::AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt64& AttrId) {
  return SAttrE.AddSAttr(Name, AttrType, AttrId);
}

int64 TNEANet::GetSAttrIdE(const TStr& Name, TInt64& AttrId, TAttrType& AttrType) const {
  return SAttrE.GetSAttrId(Name, AttrId, AttrType);
}

int64 TNEANet::GetSAttrNameE(const TInt64& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrE.GetSAttrName(AttrId, Name, AttrType);
}

/////////////////////////////////////////////////
// Undirected Graph
bool TUndirNet::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TUndirNet::TNet, Flag);
}

// Add a node of ID NId to the graph.
int64 TUndirNet::AddNode(int64 NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %s already exists", TInt64::GetStr(NId).CStr()));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

int64 TUndirNet::AddNodeUnchecked(int64 NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// Add a node of ID NId to the graph and create edges to all nodes in vector NbrNIdV.
int64 TUndirNet::AddNode(const int64& NId, const TInt64V& NbrNIdV) {
  int64 NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(! IsNode(NId), TStr::Fmt("NodeId %s already exists", TInt64::GetStr(NId).CStr()));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.NIdV = NbrNIdV;
  Node.NIdV.Sort();
  NEdges += Node.GetDeg();
  for (int64 i = 0; i < NbrNIdV.Len(); i++) {
    GetNode(NbrNIdV[i]).NIdV.AddSorted(NewNId);
  }
  return NewNId;
}

// Add a node of ID NId to the graph and create edges to all nodes in the vector NIdVId in the vector pool Pool).
int64 TUndirNet::AddNode(const int64& NId, const TVecPool<TInt64, int64>& Pool, const int64& NIdVId) {
  int64 NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %s already exists", TInt64::GetStr(NId).CStr()));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.NIdV.GenExt(Pool.GetValVPt(NIdVId), Pool.GetVLen(NIdVId));
  Node.NIdV.Sort();
  NEdges += Node.GetDeg();
  return NewNId;
}

// Delete node of ID NId from the graph.
void TUndirNet::DelNode(const int64& NId) {
  { AssertR(IsNode(NId), TStr::Fmt("NodeId %s does not exist", TInt64::GetStr(NId).CStr()));
  TInt64 Id(NId);
  SAttrN.DelSAttrId(Id);
  TNode& Node = GetNode(NId);
  NEdges -= Node.GetDeg();
  for (int64 e = 0; e < Node.GetDeg(); e++) {
    const int64 nbr = Node.GetNbrNId(e);
    if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int64 n = N.NIdV.SearchBin(NId);
    IAssert(n != -1); // if NId points to N, then N also should point back
    if (n!= -1) { N.NIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
}

int64 TUndirNet::GetEdges() const {
  //int Edges = 0;
  //for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
  //  Edges += NodeH[N].GetDeg();
  //}
  //return Edges/2;
  return NEdges;
}

// Add an edge between SrcNId and DstNId to the graph.
int64 TUndirNet::AddEdge(const int64& SrcNId, const int64& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%s or %s not a node.", TInt::GetStr(SrcNId).CStr(), DstNId).CStr());
  if (IsEdge(SrcNId, DstNId)) { return -2; } // edge already exists
  GetNode(SrcNId).NIdV.AddSorted(DstNId);
  if (SrcNId!=DstNId) { // not a self edge
    GetNode(DstNId).NIdV.AddSorted(SrcNId); }
  NEdges++;
  return -1; // no edge id
}

// Add an edge between SrcNId and DstNId to the graph.
int64 TUndirNet::AddEdgeUnchecked(const int64& SrcNId, const int64& DstNId) {
  GetNode(SrcNId).NIdV.Add(DstNId);
  if (SrcNId!=DstNId) { // not a self edge
    GetNode(DstNId).NIdV.Add(SrcNId); }
  NEdges++;
  return -1; // no edge id
}

// Delete an edge between node IDs SrcNId and DstNId from the graph.
void TUndirNet::DelEdge(const int64& SrcNId, const int64& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  const int64 n = N.NIdV.SearchBin(DstNId);
  TInt64Pr Id(SrcNId, DstNId);
  SAttrE.DelSAttrId(Id);
  if (n!= -1) { N.NIdV.Del(n);  NEdges--; } }
  if (SrcNId != DstNId) { // not a self edge
    TNode& N = GetNode(DstNId);
    const int64 n = N.NIdV.SearchBin(SrcNId);
    if (n!= -1) { N.NIdV.Del(n); }
  }
}

// Test whether an edge between node IDs SrcNId and DstNId exists the graph.
bool TUndirNet::IsEdge(const int64& SrcNId, const int64& DstNId) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) return false;
  return GetNode(SrcNId).IsNbrNId(DstNId);
}

// Return an iterator referring to edge (SrcNId, DstNId) in the graph.
TUndirNet::TEdgeI TUndirNet::GetEI(const int64& SrcNId, const int64& DstNId) const {
  const int64 MnNId = TMath::Mn(SrcNId, DstNId);
  const int64 MxNId = TMath::Mx(SrcNId, DstNId);
  const TNodeI SrcNI = GetNI(MnNId);
  const int64 NodeN = SrcNI.NodeHI.GetDat().NIdV.SearchBin(MxNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}


// Get a vector IDs of all nodes in the graph.
void TUndirNet::GetNIdV(TInt64V& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int64 N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

// Defragment the graph.
void TUndirNet::Defrag(const bool& OnlyNodeLinks) {
  for (int64 n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    NodeH[n].NIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) {
    NodeH.Defrag();
  }
}

// Check the graph data structure for internal consistency.
bool TUndirNet::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int64 N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.NIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
      RetVal=false;
    }
    int64 prevNId = -1;
    for (int64 e = 0; e < Node.GetDeg(); e++) {
      if (! IsNode(Node.GetNbrNId(e))) {
        const TStr Msg = TStr::Fmt("Edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetNbrNId(e), Node.GetNbrNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetNbrNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplicate edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetNbrNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      prevNId = Node.GetNbrNId(e);
    }
  }
  int64 EdgeCnt = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) { EdgeCnt++; }
  if (EdgeCnt != GetEdges()) {
    const TStr Msg = TStr::Fmt("Number of edges counter is corrupted: GetEdges():%d, EdgeCount:%d.", GetEdges(), EdgeCnt);
    if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
    RetVal=false;
  }
  return RetVal;
}

// Print the graph in a human readable form to an output stream OutF.
void TUndirNet::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nUndirected Node Graph: nodes: %s, edges: %s\n", TInt64::GetStr(GetNodes()).CStr(), TInt64::GetStr(GetEdges()).CStr());
  for (int64 N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*s [%s] ", NodePlaces, TInt64::GetStr(Node.GetId()).CStr(), TInt64::GetStr(Node.GetDeg()).CStr());
    for (int64 edge = 0; edge < Node.GetDeg(); edge++) {
      fprintf(OutF, " %*s", NodePlaces, TInt64::GetStr(Node.GetNbrNId(edge)).CStr()); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}


// Return a small graph on 5 nodes and 5 edges.
PUndirNet TUndirNet::GetSmallGraph() {
  PUndirNet Graph = TUndirNet::New();
  for (int64 i = 0; i < 5; i++) { Graph->AddNode(i); }
  Graph->AddEdge(0,1);  Graph->AddEdge(0,2);
  Graph->AddEdge(0,3);  Graph->AddEdge(0,4);
  Graph->AddEdge(1,2);
  return Graph;
}

int64 TUndirNet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TInt64& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TUndirNet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TInt64& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TUndirNet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TUndirNet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TUndirNet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TUndirNet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TUndirNet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TInt64& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TUndirNet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TInt64& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TUndirNet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TUndirNet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TUndirNet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TUndirNet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TUndirNet::DelSAttrDatN(const TInt64& NId, const TStr& AttrName) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrName);
}

int64 TUndirNet::DelSAttrDatN(const TInt64& NId, const TInt64& AttrId) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrId);
}

int64 TUndirNet::GetSAttrVN(const TInt64& NId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsNode(NId)) {
    return -1;
  }
  SAttrN.GetSAttrV(NId, AttrType, AttrV);
  return 0;
}

int64 TUndirNet::GetIdVSAttrN(const TStr& AttrName, TInt64V& IdV) const {
  return SAttrN.GetIdVSAttr(AttrName, IdV);
}

int64 TUndirNet::GetIdVSAttrN(const TInt64& AttrId, TInt64V& IdV) const {
  return SAttrN.GetIdVSAttr(AttrId, IdV);
}

int64 TUndirNet::AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt64& AttrId) {
  return SAttrN.AddSAttr(Name, AttrType, AttrId);
}

int64 TUndirNet::GetSAttrIdN(const TStr& Name, TInt64& AttrId, TAttrType& AttrType) const {
  return SAttrN.GetSAttrId(Name, AttrId, AttrType);
}

int64 TUndirNet::GetSAttrNameN(const TInt64& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrN.GetSAttrName(AttrId, Name, AttrType);
}

TInt64Pr TUndirNet::OrderEdgeNodes(const int64& SrcNId, const int64& DstNId) const{
  if (SrcNId < DstNId) {
    return TInt64Pr(SrcNId, DstNId);
  }
  return TInt64Pr(DstNId, SrcNId);
}

int64 TUndirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, const TInt64& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TUndirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, const TInt64& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TUndirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, const TFlt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TUndirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, const TFlt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TUndirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, const TStr& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TUndirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, const TStr& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TUndirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, TInt64& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TUndirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, TInt64& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TUndirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, TFlt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TUndirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, TFlt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TUndirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, TStr& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TUndirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, TStr& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TUndirNet::DelSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.DelSAttrDat(EId, AttrName);
}

int64 TUndirNet::DelSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.DelSAttrDat(EId, AttrId);
}

int64 TUndirNet::GetSAttrVE(const int64& SrcNId, const int64& DstNId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId = OrderEdgeNodes(SrcNId, DstNId);
  SAttrE.GetSAttrV(EId, AttrType, AttrV);
  return 0;
}

int64 TUndirNet::GetIdVSAttrE(const TStr& AttrName, TIntPr64V& IdV) const {
  return SAttrE.GetIdVSAttr(AttrName, IdV);
}

int64 TUndirNet::GetIdVSAttrE(const TInt64& AttrId, TIntPr64V& IdV) const {
  return SAttrE.GetIdVSAttr(AttrId, IdV);
}

int64 TUndirNet::AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt64& AttrId) {
  return SAttrE.AddSAttr(Name, AttrType, AttrId);
}

int64 TUndirNet::GetSAttrIdE(const TStr& Name, TInt64& AttrId, TAttrType& AttrType) const {
  return SAttrE.GetSAttrId(Name, AttrId, AttrType);
}

int64 TUndirNet::GetSAttrNameE(const TInt64& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrE.GetSAttrName(AttrId, Name, AttrType);
}

/////////////////////////////////////////////////
// Directed Node Graph
bool TDirNet::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TDirNet::TNet, Flag);
}

int64 TDirNet::AddNode(int64 NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

int64 TDirNet::AddNodeUnchecked(int64 NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// add a node with a list of neighbors
// (use TDirNet::IsOk to check whether the graph is consistent)
int64 TDirNet::AddNode(const int64& NId, const TInt64V& InNIdV, const TInt64V& OutNIdV) {
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
// (use TDirNet::IsOk to check whether the graph is consistent)
int64 TDirNet::AddNode(const int64& NId, const TVecPool<TInt64, int64>& Pool, const int64& SrcVId, const int64& DstVId) {
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

void TDirNet::DelNode(const int64& NId) {
  { TNode& Node = GetNode(NId);
  TInt64 Id(NId);
  SAttrN.DelSAttrId(Id);
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
}

int64 TDirNet::GetEdges() const {
  int64 edges=0;
  for (int64 N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    edges+=NodeH[N].GetOutDeg();
  }
  return edges;
}

int64 TDirNet::AddEdge(const int64& SrcNId, const int64& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  //IAssert(! IsEdge(SrcNId, DstNId));
  if (IsEdge(SrcNId, DstNId)) { return -2; }
  GetNode(SrcNId).OutNIdV.AddSorted(DstNId);
  GetNode(DstNId).InNIdV.AddSorted(SrcNId);
  return -1; // no edge id
}

int64 TDirNet::AddEdgeUnchecked(const int64& SrcNId, const int64& DstNId) {
  GetNode(SrcNId).OutNIdV.Add(DstNId);
  GetNode(DstNId).InNIdV.Add(SrcNId);
  return -1; // no edge id
}

void TDirNet::DelEdge(const int64& SrcNId, const int64& DstNId, const bool& IsDir) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  TInt64Pr Id(SrcNId, DstNId);
  SAttrE.DelSAttrId(Id);
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

bool TDirNet::IsEdge(const int64& SrcNId, const int64& DstNId, const bool& IsDir) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) { return false; }
  if (IsDir) { return GetNode(SrcNId).IsOutNId(DstNId); }
  else { return GetNode(SrcNId).IsOutNId(DstNId) || GetNode(DstNId).IsOutNId(SrcNId); }
}

TDirNet::TEdgeI TDirNet::GetEI(const int64& SrcNId, const int64& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int64 NodeN = SrcNI.NodeHI.GetDat().OutNIdV.SearchBin(DstNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

void TDirNet::GetNIdV(TInt64V& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int64 N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

void TDirNet::Defrag(const bool& OnlyNodeLinks) {
  for (int64 n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    TNode& Node = NodeH[n];
    Node.InNIdV.Pack();  Node.OutNIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
}

// for each node check that their neighbors are also nodes
bool TDirNet::IsOk(const bool& ThrowExcept) const {
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

void TDirNet::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node Graph: nodes: %s, edges: %s\n", TInt64::GetStr(GetNodes()).CStr(), TInt64::GetStr(GetEdges()).CStr());
  for (int64 N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*s]\n", NodePlaces, TInt64::GetStr(Node.GetId()).CStr());
    fprintf(OutF, "    in [%s]", TInt64::GetStr(Node.GetInDeg()).CStr());
    for (int64 edge = 0; edge < Node.GetInDeg(); edge++) {
      fprintf(OutF, " %*s", NodePlaces, TInt64::GetStr(Node.GetInNId(edge)).CStr()); }
    fprintf(OutF, "\n    out[%s]", TInt64::GetStr(Node.GetOutDeg()).CStr());
    for (int64 edge = 0; edge < Node.GetOutDeg(); edge++) {
      fprintf(OutF, " %*s", NodePlaces, TInt64::GetStr(Node.GetOutNId(edge)).CStr()); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

PDirNet TDirNet::GetSmallGraph() {
  PDirNet G = TDirNet::New();
  for (int64 i = 0; i < 5; i++) { G->AddNode(i); }
  G->AddEdge(0,1); G->AddEdge(1,2); G->AddEdge(0,2);
  G->AddEdge(1,3); G->AddEdge(3,4); G->AddEdge(2,3);
  return G;
}

int64 TDirNet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TInt64& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TDirNet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TInt64& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TDirNet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TDirNet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TDirNet::AddSAttrDatN(const TInt64& NId, const TStr& AttrName, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int64 TDirNet::AddSAttrDatN(const TInt64& NId, const TInt64& AttrId, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int64 TDirNet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TInt64& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TDirNet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TInt64& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TDirNet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TDirNet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TDirNet::GetSAttrDatN(const TInt64& NId, const TStr& AttrName, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int64 TDirNet::GetSAttrDatN(const TInt64& NId, const TInt64& AttrId, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int64 TDirNet::DelSAttrDatN(const TInt64& NId, const TStr& AttrName) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrName);
}

int64 TDirNet::DelSAttrDatN(const TInt64& NId, const TInt64& AttrId) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrId);
}

int64 TDirNet::GetSAttrVN(const TInt64& NId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsNode(NId)) {
    return -1;
  }
  SAttrN.GetSAttrV(NId, AttrType, AttrV);
  return 0;
}

int64 TDirNet::GetIdVSAttrN(const TStr& AttrName, TInt64V& IdV) const {
  return SAttrN.GetIdVSAttr(AttrName, IdV);
}

int64 TDirNet::GetIdVSAttrN(const TInt64& AttrId, TInt64V& IdV) const {
  return SAttrN.GetIdVSAttr(AttrId, IdV);
}

int64 TDirNet::AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt64& AttrId) {
  return SAttrN.AddSAttr(Name, AttrType, AttrId);
}

int64 TDirNet::GetSAttrIdN(const TStr& Name, TInt64& AttrId, TAttrType& AttrType) const {
  return SAttrN.GetSAttrId(Name, AttrId, AttrType);
}

int64 TDirNet::GetSAttrNameN(const TInt64& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrN.GetSAttrName(AttrId, Name, AttrType);
}

int64 TDirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, const TInt64& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TDirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, const TInt64& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TDirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, const TFlt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TDirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, const TFlt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TDirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, const TStr& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int64 TDirNet::AddSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, const TStr& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int64 TDirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, TInt64& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TDirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, TInt64& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TDirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, TFlt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TDirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, TFlt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TDirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName, TStr& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int64 TDirNet::GetSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId, TStr& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int64 TDirNet::DelSAttrDatE(const int64& SrcNId, const int64& DstNId, const TStr& AttrName) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.DelSAttrDat(EId, AttrName);
}

int64 TDirNet::DelSAttrDatE(const int64& SrcNId, const int64& DstNId, const TInt64& AttrId) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  return SAttrE.DelSAttrDat(EId, AttrId);
}

int64 TDirNet::GetSAttrVE(const int64& SrcNId, const int64& DstNId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TInt64Pr EId(SrcNId, DstNId);
  SAttrE.GetSAttrV(EId, AttrType, AttrV);
  return 0;
}

int64 TDirNet::GetIdVSAttrE(const TStr& AttrName, TIntPr64V& IdV) const {
  return SAttrE.GetIdVSAttr(AttrName, IdV);
}

int64 TDirNet::GetIdVSAttrE(const TInt64& AttrId, TIntPr64V& IdV) const {
  return SAttrE.GetIdVSAttr(AttrId, IdV);
}

int64 TDirNet::AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt64& AttrId) {
  return SAttrE.AddSAttr(Name, AttrType, AttrId);
}

int64 TDirNet::GetSAttrIdE(const TStr& Name, TInt64& AttrId, TAttrType& AttrType) const {
  return SAttrE.GetSAttrId(Name, AttrId, AttrType);
}

int64 TDirNet::GetSAttrNameE(const TInt64& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrE.GetSAttrName(AttrId, Name, AttrType);
}
