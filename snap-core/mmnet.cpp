////////////////////////////////////////////////
// Mutimodal Network
//#include <iostream> 

TStr TModeNet::GetNeighborCrossName(const TStr& CrossName, bool isOutEdge, const bool sameMode, bool isDir) const {
  TStr Cpy(CrossName);
  if (!isDir || !sameMode) { return Cpy; }
  if (isOutEdge) {
    Cpy += ":SRC";
  } else {
    Cpy += ":DST";
  }
  return Cpy;
}

void TModeNet::GetNeighborCrossNames(TStr64V& Names) const {
  Names.Gen(NeighborTypes.Len(), 0);
  for(THash<TStr,TBool,int64>::TIter it = NeighborTypes.BegI(); it < NeighborTypes.EndI(); it++) {
    TStr CNName = it.GetKey();
    TCrossNet& CN = MMNet->GetCrossNetByName(CNName);
    bool sameMode = (CN.GetMode1() == CN.GetMode2()), isDir = CN.IsDirected();
    Names.Add(GetNeighborCrossName(CNName, true, sameMode, isDir));
    if (isDir && sameMode) Names.Add(GetNeighborCrossName(CNName, false, sameMode, isDir));
  }
}

void TModeNet::ClrNbr(const TStr& CrossNetName, const bool& outEdge, const bool& sameMode, bool& isDir) {
  TStr Name = GetNeighborCrossName(CrossNetName, outEdge, sameMode, isDir);
  TVec<TInt64V, int64> Attrs(MxNId);
  int64 index = KeyToIndexTypeN.GetDat(Name).Val2;
  VecOfIntVecVecsN[index] = Attrs;
}

void TModeNet::Clr() {
  TStr64V CNets;
  NeighborTypes.GetKeyV(CNets);
  for (int64 i=0; i < CNets.Len(); i++) {
    MMNet->GetCrossNetByName(CNets[i]).Clr();
  }
  TNEANet::Clr();
}

int64 TModeNet::AddNeighbor(const int64& NId, const int64& EId, bool outEdge, const int64 linkId, const bool sameMode, bool isDir){
  TStr CrossName = MMNet->GetCrossName(linkId);
  return AddNeighbor(NId, EId, outEdge, CrossName, sameMode, isDir);
}

int64 TModeNet::AddNeighbor(const int64& NId, const int64& EId, bool outEdge, const TStr& CrossName, const bool sameMode, bool isDir){
  if (!NeighborTypes.IsKey(CrossName)) {
    AddNbrType(CrossName, sameMode, isDir);
  }
  TStr Name = GetNeighborCrossName(CrossName, outEdge, sameMode, isDir);
  return AppendIntVAttrDatN(NId, EId, Name); 
}

int64 TModeNet::DelNeighbor(const int64& NId, const int64& EId, bool outEdge, const TStr& CrossName, const bool sameMode, bool isDir){
  if (!NeighborTypes.IsKey(CrossName)) {
    return -1;
  }
  TStr Name = GetNeighborCrossName(CrossName, outEdge, sameMode, isDir);
  return DelFromIntVAttrDatN(NId, EId, Name);
}

int64 TModeNet::DelNeighbor(const int64& NId, const int64& EId, bool outEdge, const TInt64& linkId, const bool sameMode, bool isDir){
  TStr CrossName = MMNet->GetCrossName(linkId);
  return DelNeighbor(NId, EId, outEdge, CrossName, sameMode, isDir);
}

void TModeNet::DelNode(const int64& NId) {
  TStr64V Names;
  GetCrossNetNames(Names);
  for (int64 i=0; i < Names.Len(); i++) {
    TCrossNet& Cross = MMNet->GetCrossNetByName(Names[i]);
    TInt64V OutEIds;
    GetNeighborsByCrossNet(NId, Names[i], OutEIds, true);
    for (int64 j=0; j < OutEIds.Len(); j++) {
      Cross.DelEdge(OutEIds[j].Val);
    }
    if (Cross.IsDirect && Cross.Mode1 == Cross.Mode2) {
      TInt64V InEIds;
      GetNeighborsByCrossNet(NId, Names[i], InEIds, false);
      for (int64 j=0; j < InEIds.Len(); j++) {
        Cross.DelEdge(InEIds[j].Val);
      }
    }
  }
  TNEANet::DelNode(NId);
}

void TModeNet::SetParentPointer(TMMNet* parent) {
  MMNet = parent;
}

int64 TModeNet::AddNbrType(const TStr& CrossName, const bool sameMode, bool isDir) {
  //IAssertR(!NeighborTypes.IsKey(CrossName),TStr::Fmt("Neighbor Cross Types already exists: %s", CrossName.CStr()));
  if (NeighborTypes.IsKey(CrossName)) { return -1; } //Don't want to add nbr type multiple times
  bool hasSingleVector = (!isDir || !sameMode);
  NeighborTypes.AddDat(CrossName, hasSingleVector);
  // add crossnet attr type
  if(hasSingleVector) { 
    AddIntVAttrN(CrossName);
  } else { // directed and same mode, need :SRC and :DST distinction
    AddIntVAttrN(CrossName + ":SRC");
    AddIntVAttrN(CrossName + ":DST");
  }

  return 0;
}

int64 TModeNet::DelNbrType(const TStr& CrossName) {
  //IAssertR(!NeighborTypes.IsKey(CrossName),TStr::Fmt("Neighbor Cross Types already exists: %s", CrossName.CStr()));
  bool hasSingleVector = NeighborTypes.GetDat(CrossName);
  NeighborTypes.DelKey(CrossName);
  if (hasSingleVector) {
    return DelAttrN(CrossName);
  } else {
    TStr InName = GetNeighborCrossName(CrossName, true, true, true);
    TStr OutName = GetNeighborCrossName(CrossName, false, true, true);
    if (DelAttrN(InName) == -1 || DelAttrN(OutName) == -1) {
      return -1;
    }
  }
  return 0;
}

void TModeNet::GetNeighborsByCrossNet(const int64& NId, TStr& Name, TInt64V& Neighbors, const bool isOutEId) const{
  IAssertR(NeighborTypes.IsKey(Name), TStr::Fmt("Cross Type does not exist: %s", Name.GetCStr()));
  TBool hasSingleVector = NeighborTypes.GetDat(Name);
  if (hasSingleVector) {
    Neighbors = GetIntVAttrDatN(NId, Name);
  } else {
    TStr DirectionalName = GetNeighborCrossName(Name, isOutEId, true, true);
    Neighbors = GetIntVAttrDatN(NId, DirectionalName);
  }
}

void TModeNet::CopyNodesWithoutNeighbors(const TModeNet& Src, TModeNet& Dst, const TInt64V& ToCopyIds) {
  TInt64V NewlyAdded;

  // copy nodes and sparse attributes
  for(int64 i = 0; i < ToCopyIds.Len(); i++) {
    TInt64 Id = ToCopyIds[i];
    IAssertR(Src.IsNode(Id), TStr::Fmt("No node with id %d in source mode", Id.Val));
    if (Dst.IsNode(Id)) { continue; }
    Dst.AddNode(Id);
    NewlyAdded.Add(Id);
    CopyAllSAttrN(Id, atInt, Src, Dst);
    CopyAllSAttrN(Id, atFlt, Src, Dst);
    CopyAllSAttrN(Id, atStr, Src, Dst);
  }

  // copy all non-IntV dense attributes 
  for(TStrIntPr64H::TIter it = Src.KeyToIndexTypeN.BegI(); it < Src.KeyToIndexTypeN.EndI(); it++) {
    const TStr& AttrName = it.GetKey();
    TInt64 AttrType = it.GetDat().GetVal1();
    TInt64 AttrIndex = it.GetDat().GetVal2();
    for(int64 i = 0; i < NewlyAdded.Len(); i++) {
      TInt64 Id = NewlyAdded[i];
      if (AttrType == IntType) {
        Dst.AddIntAttrDatN(Id, Src.VecOfIntVecsN[AttrIndex][Src.NodeH.GetKeyId(Id)], AttrName);
      } else if (AttrType == FltType) {
        Dst.AddFltAttrDatN(Id, Src.VecOfFltVecsN[AttrIndex][Src.NodeH.GetKeyId(Id)], AttrName);
      } else if (AttrType == StrType) {  
        Dst.AddStrAttrDatN(Id, Src.VecOfStrVecsN[AttrIndex][Src.NodeH.GetKeyId(Id)], AttrName);
      }
    }
  }
  
  // fetch names of all non-crossnet intv attrs, then copy values into Dst
  TStr64V CNAttrNames;
  Src.GetNeighborCrossNames(CNAttrNames);
  CNAttrNames.Sort();
  TStr64V IntVAttrNames;
  Src.GetIntVAttrNNames(IntVAttrNames);
  IntVAttrNames.Sort();
  TStr64V NonCrossnetIntVAttrNames;
  IntVAttrNames.Diff(CNAttrNames, NonCrossnetIntVAttrNames);
  for(TStr64V::TIter it = NonCrossnetIntVAttrNames.BegI(); it < NonCrossnetIntVAttrNames.EndI(); it++) {
    const TStr& AttrName = *it;
    TInt64 AttrIndex = Src.KeyToIndexTypeN.GetDat(AttrName).GetVal2();
    for(int64 i = 0; i < NewlyAdded.Len(); i++) {
      TInt64 Id = NewlyAdded[i];
      Dst.AddIntVAttrDatN(Id, Src.VecOfIntVecVecsN[AttrIndex][Src.NodeH.GetKeyId(Id)], AttrName);
    }
  }
}


int64 TModeNet::AddIntVAttrByVecN(const TStr& attr, TVec<TInt64V, int64>& Attrs){
  TInt64 CurrLen;
  TVec<TInt64V, int64> NewVec;
  CurrLen = VecOfIntVecVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TInt64Pr(IntVType, CurrLen));
  VecOfIntVecVecsN.Add(Attrs);
  return 0;
}

void TModeNet::RemoveCrossNets(TModeNet& Result, TStr64V& CrossNets) {
  const TModeNet& self = *this;
  Result = TModeNet(self, false);
  for (TStrIntPr64H::TIter it = KeyToIndexTypeN.BegI(); it < KeyToIndexTypeN.EndI(); it++) {
    TStr AttrName = it.GetKey();
    TInt64 AttrType = it.GetDat().GetVal1();
    TInt64 AttrIndex = it.GetDat().GetVal2();
    if (AttrType != IntVType) {
      Result.KeyToIndexTypeN.AddDat(AttrName, it.GetDat());
    } else {
      TStr WithoutSuffix = AttrName;
      bool removeSuffix = false;
      if (AttrName.IsSuffix(":SRC") || AttrName.IsSuffix(":DST")) {
        WithoutSuffix = AttrName.GetSubStr(0, AttrName.Len()-5);
        removeSuffix = true;
      }
      bool isSingleVNbrAttr = (NeighborTypes.IsKey(AttrName) && NeighborTypes.GetDat(AttrName));
      bool isMultiVNbrAttr = (removeSuffix && NeighborTypes.IsKey(WithoutSuffix) && !NeighborTypes.GetDat(WithoutSuffix));
      if (isSingleVNbrAttr || isMultiVNbrAttr) {
        TStr NbrName = isSingleVNbrAttr ? AttrName : WithoutSuffix;
        if (CrossNets.IsIn(NbrName)) {
          Result.AddNbrType(NbrName, removeSuffix, removeSuffix);
          TVec<TInt64V, int64>& Attrs = VecOfIntVecVecsN[AttrIndex];
          Result.AddIntVAttrByVecN(AttrName, Attrs);
        }
      } else {
        TVec<TInt64V, int64>& Attrs = VecOfIntVecVecsN[AttrIndex];
        Result.AddIntVAttrByVecN(AttrName, Attrs);
      }
    }
  }
}

int64 TModeNet::GetAttrTypeN(const TStr& attr) const {
  if (KeyToIndexTypeN.IsKey(attr)) {
    return KeyToIndexTypeN.GetDat(attr).Val1;
  }
  return -1;
}

void TModeNet::CopyAllSAttrN(TInt64 Id, const TAttrType& AttrType, const TModeNet& Src, TModeNet& Dst) {
  TAttrPrV SparseAttrs;
  Src.SAttrN.GetSAttrV(Id, AttrType, SparseAttrs);
  for(TAttrPrV::TIter it = SparseAttrs.BegI(); it < SparseAttrs.EndI(); it++) {
    TStr AttrName = it->Val1;
    if (AttrType == atInt) {
      TInt64 AttrDatI;
      if (Src.SAttrN.GetSAttrDat(Id, AttrName, AttrDatI) != -1) { Dst.SAttrN.AddSAttrDat(Id, AttrName, AttrDatI); }
    } else if (AttrType == atFlt) {
      TFlt AttrDatF;
      if (Src.SAttrN.GetSAttrDat(Id, AttrName, AttrDatF) != -1) { Dst.SAttrN.AddSAttrDat(Id, AttrName, AttrDatF); }
    } else if (AttrType == atStr) {
      TStr AttrDatS;
      if (Src.SAttrN.GetSAttrDat(Id, AttrName, AttrDatS) != -1) { Dst.SAttrN.AddSAttrDat(Id, AttrName, AttrDatS); }
    }
  }
}

int64 TCrossNet::GetAttrTypeE(const TStr& attr) const {
  if (KeyToIndexTypeE.IsKey(attr)) {
    return KeyToIndexTypeE.GetDat(attr).Val1;
  }
  return -1;
}

void TCrossNet::Clr() {
  CrossH.Clr();
  MxEId=0;
  KeyToIndexTypeE.Clr();
  IntDefaultsE.Clr();
  StrDefaultsE.Clr();
  FltDefaultsE.Clr();
  VecOfIntVecsE.Clr();
  VecOfStrVecsE.Clr();
  VecOfFltVecsE.Clr(); 
  Net->ClrNbr(Mode1, CrossNetId, true, Mode1==Mode2, IsDirect.Val);
  Net->ClrNbr(Mode2, CrossNetId, false, Mode1==Mode2, IsDirect.Val); 
}

int64 TCrossNet::AddEdge(const int64& sourceNId, const int64& destNId, int64 EId){
  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  if (Net != NULL) {
    TModeNet& M1 = Net->TModeNetH.GetDat(this->Mode1);
    TModeNet& M2 = Net->TModeNetH.GetDat(this->Mode2);
    if (!M1.IsNode(sourceNId) || !M2.IsNode(destNId)) { return -1; }
    TStr ThisCrossName = Net->GetCrossName(this->CrossNetId);
    M1.AddNeighbor(sourceNId, EId, true, ThisCrossName, Mode1==Mode2, IsDirect);
    M2.AddNeighbor(destNId, EId, false, ThisCrossName, Mode1==Mode2, IsDirect);
  }
  TCrossNet::TCrossEdge newEdge(EId, sourceNId, destNId);
  CrossH.AddDat(EId, newEdge);
  int64 i;
  // update attribute columns
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt64, int64>& IntVec = VecOfIntVecsE[i];
    int64 KeyId = CrossH.GetKeyId(EId);
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
    IntVec[CrossH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr, int64>& StrVec = VecOfStrVecsE[i];
    int64 KeyId = CrossH.GetKeyId(EId);
    if (StrVec.Len() > KeyId) {
      StrVec[KeyId] = TStr::GetNullStr();
    } else {
      StrVec.Ins(KeyId, TStr::GetNullStr());
    }
  }
  TVec<TStr, int64> DefStrVec = TVec<TStr, int64>();
  StrDefaultsE.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr, int64>& StrVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(DefStrVec[i]).Val2];
    StrVec[CrossH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt, int64>& FltVec = VecOfFltVecsE[i];
    int64 KeyId = CrossH.GetKeyId(EId);
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
    FltVec[CrossH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  }
  return EId;
}

int64 TCrossNet::DelEdge(const int64& EId) {
  TCrossEdge& Edge = CrossH.GetDat(EId);
  int64 srcNode = Edge.SrcNId;
  int64 dstNode = Edge.DstNId;
  TStr ThisCrossName = Net->GetCrossName(this->CrossNetId);
  Net->GetModeNetById(this->Mode1).DelNeighbor(srcNode, EId, true, ThisCrossName, Mode1==Mode2, IsDirect);
  Net->GetModeNetById(this->Mode2).DelNeighbor(dstNode, EId, false, ThisCrossName, Mode1==Mode2, IsDirect);
  int64 i;
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt64, int64>& IntVec = VecOfIntVecsE[i];
    IntVec[CrossH.GetKeyId(EId)] =  TInt64::Mn;
  }
  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr, int64>& StrVec = VecOfStrVecsE[i];
    StrVec[CrossH.GetKeyId(EId)] =  TStr::GetNullStr();
  }
  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt, int64>& FltVec = VecOfFltVecsE[i];
    FltVec[CrossH.GetKeyId(EId)] = TFlt::Mn;
  }
  CrossH.DelKey(EId);
  return 0;
}

void TCrossNet::GetEIdV(TInt64V& EIdV) const {
  EIdV.Gen(GetEdges(), 0);
  for (int64 E=CrossH.FFirstKeyId(); CrossH.FNextKeyId(E); ) {
    EIdV.Add(CrossH.GetKey(E));
  }
}

void TCrossNet::SetParentPointer(TMMNet* parent) {
  Net = parent;
}

void TCrossNet::AttrNameEI(const TInt64& EId, TStrIntPr64H::TIter CrossHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!CrossHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, CrossHI)) {
      Names.Add(CrossHI.GetKey());
    }
    CrossHI++;
  }  
}

void TCrossNet::AttrValueEI(const TInt64& EId, TStrIntPr64H::TIter CrossHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!CrossHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, CrossHI)) {
      Values.Add(GetEdgeAttrValue(EId, CrossHI));
    }
    CrossHI++;
  }
}

void TCrossNet::IntAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter CrossHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, CrossHI)) {
      Names.Add(CrossHI.GetKey());
    }
    CrossHI++;
  }  
}

void TCrossNet::IntAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter CrossHI, TInt64V& Values) const {
  Values = TVec<TInt64, int64>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, CrossHI)) {
      TInt64 val = (this->VecOfIntVecsE.GetVal(CrossHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    CrossHI++;
  }  
}

void TCrossNet::StrAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter CrossHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, CrossHI)) {
      Names.Add(CrossHI.GetKey());
    }
    CrossHI++;
  }  
}

void TCrossNet::StrAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter CrossHI, TStr64V& Values) const {
  Values = TVec<TStr, int64>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, CrossHI)) {
      TStr val = this->VecOfStrVecsE.GetVal(CrossHI.GetDat().Val2).GetVal(EId);
      Values.Add(val);
    }
    CrossHI++;
  }  
}

void TCrossNet::FltAttrNameEI(const TInt64& EId, TStrIntPr64H::TIter CrossHI, TStr64V& Names) const {
  Names = TVec<TStr, int64>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, CrossHI)) {
      Names.Add(CrossHI.GetKey());
    }
    CrossHI++;
  }  
}

void TCrossNet::FltAttrValueEI(const TInt64& EId, TStrIntPr64H::TIter CrossHI, TFlt64V& Values) const {
  Values = TVec<TFlt, int64>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, CrossHI)) {
      TFlt val = (this->VecOfFltVecsE.GetVal(CrossHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    CrossHI++;
  }  
}

bool TCrossNet::IsFltAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == FltType);
}

bool TCrossNet::IsIntAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == IntType);
}

bool TCrossNet::IsStrAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == StrType);
}

bool TCrossNet::IsAttrDeletedE(const int64& EId, const TStr& attr) const {
  bool IntDel = IsIntAttrDeletedE(EId, attr);
  bool StrDel = IsStrAttrDeletedE(EId, attr);
  bool FltDel = IsFltAttrDeletedE(EId, attr);
  return IntDel || StrDel || FltDel;
}

bool TCrossNet::IsIntAttrDeletedE(const int64& EId, const TStr& attr) const {
  return EdgeAttrIsIntDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TCrossNet::IsStrAttrDeletedE(const int64& EId, const TStr& attr) const {
  return EdgeAttrIsStrDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TCrossNet::IsFltAttrDeletedE(const int64& EId, const TStr& attr) const {
  return EdgeAttrIsFltDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TCrossNet::EdgeAttrIsDeleted(const int64& EId, const TStrIntPr64H::TIter& CrossHI) const {
  bool IntDel = EdgeAttrIsIntDeleted(EId, CrossHI);
  bool StrDel = EdgeAttrIsStrDeleted(EId, CrossHI);
  bool FltDel = EdgeAttrIsFltDeleted(EId, CrossHI);
  return IntDel || StrDel || FltDel;
}

bool TCrossNet::EdgeAttrIsIntDeleted(const int64& EId, const TStrIntPr64H::TIter& CrossHI) const {
  return (CrossHI.GetDat().Val1 == IntType &&
          GetIntAttrDefaultE(CrossHI.GetKey()) == this->VecOfIntVecsE.GetVal(
          this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId)));
}

bool TCrossNet::EdgeAttrIsStrDeleted(const int64& EId, const TStrIntPr64H::TIter& CrossHI) const {
  return (CrossHI.GetDat().Val1 == StrType &&
          GetStrAttrDefaultE(CrossHI.GetKey()) == this->VecOfStrVecsE.GetVal(
          this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId)));
}

bool TCrossNet::EdgeAttrIsFltDeleted(const int64& EId, const TStrIntPr64H::TIter& CrossHI) const {
  return (CrossHI.GetDat().Val1 == FltType &&
          GetFltAttrDefaultE(CrossHI.GetKey()) == this->VecOfFltVecsE.GetVal(
          this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId)));
}

TStr TCrossNet::GetEdgeAttrValue(const int64& EId, const TStrIntPr64H::TIter& CrossHI) const {
  if (CrossHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId))).GetStr();
  } else if (CrossHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId));
  } else if (CrossHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId))).GetStr();
  }
  return TStr::GetNullStr();
}

int64 TCrossNet::AddIntAttrDatE(const int64& EId, const TInt64& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
    return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TInt64, int64>& NewVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[CrossH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfIntVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TInt64Pr(IntType, CurrLen));
    TVec<TInt64, int64> NewVec = TVec<TInt64, int64>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetIntAttrDefaultE(attr));
    }
    NewVec[CrossH.GetKeyId(EId)] = value;
    VecOfIntVecsE.Add(NewVec);
  }
  return 0;
}

int64 TCrossNet::AddStrAttrDatE(const int64& EId, const TStr& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
    return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TStr, int64>& NewVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[CrossH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfStrVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TInt64Pr(StrType, CurrLen));
    TVec<TStr, int64> NewVec = TVec<TStr, int64>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetStrAttrDefaultE(attr));
    }
    NewVec[CrossH.GetKeyId(EId)] = value;
    VecOfStrVecsE.Add(NewVec);
  }
  return 0;
} 

int64 TCrossNet::AddFltAttrDatE(const int64& EId, const TFlt& value, const TStr& attr) {
  int64 i;
  TInt64 CurrLen;

  if (!IsEdge(EId)) {
    //AddEdge(EId);
    return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TFlt, int64>& NewVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[CrossH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfFltVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TInt64Pr(FltType, CurrLen));
    TVec<TFlt, int64> NewVec = TVec<TFlt, int64>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetFltAttrDefaultE(attr));
    }
    NewVec[CrossH.GetKeyId(EId)] = value;
    VecOfFltVecsE.Add(NewVec);
  }
  return 0;
}

TInt64 TCrossNet::GetIntAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)];
}

TStr TCrossNet::GetStrAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)];
}

TFlt TCrossNet::GetFltAttrDatE(const int64& EId, const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)];
}

int64 TCrossNet::DelAttrDatE(const int64& EId, const TStr& attr) {
  // TODO(nkhadke): add error checking
  TInt64 vecType = KeyToIndexTypeE(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  } else {
    return -1;
  }
  return 0;
}

int64 TCrossNet::AddIntAttrE(const TStr& attr, TInt64 defaultValue){
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

int64 TCrossNet::AddStrAttrE(const TStr& attr, TStr defaultValue) {
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

int64 TCrossNet::AddFltAttrE(const TStr& attr, TFlt defaultValue) {
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

int64 TCrossNet::DelAttrE(const TStr& attr) {
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

int64 TCrossNet::DelAllAttrDatE() {
  for(TStrIntPr64H::TIter it = KeyToIndexTypeE.BegI(); it < KeyToIndexTypeE.EndI(); it++) {
    TStr AttrName = it.GetKey();
    TInt64 vecType = it.GetDat().Val1;
    TInt64 AttrIndex = it.GetDat().Val2;
    if (vecType == IntType) {
      VecOfIntVecsE[AttrIndex] = TVec<TInt64, int64>();
    } else if (vecType == StrType) {
      VecOfStrVecsE[AttrIndex] = TVec<TStr, int64>();
    } else if (vecType == FltType) {
      VecOfFltVecsE[AttrIndex] = TVec<TFlt, int64>();
    } else {
      return -1;
    }
  }
  return 0;
}

void TCrossNet::CopyEdges(const TCrossNet& Src, TCrossNet& Dst, const TInt64V& ToCopyIds) {
  TInt64V NewlyAdded;
  // copy edges with original orientations
  for(int64 i = 0; i < ToCopyIds.Len(); i++) {
    TInt64 EId = ToCopyIds[i];
    IAssertR(Src.IsEdge(EId), TStr::Fmt("No edge with id %d in source crossnet", EId.Val));
    if (Dst.IsEdge(EId)) { continue; }
    TInt64 NId1 = Src.GetEdgeI(EId).GetSrcNId(), NId2 = Src.GetEdgeI(EId).GetDstNId();
    Dst.AddEdge(NId1, NId2, EId);
    NewlyAdded.Add(EId);
  }

  // copy all attributes
  for(TStrIntPr64H::TIter it = Src.KeyToIndexTypeE.BegI(); it < Src.KeyToIndexTypeE.EndI(); it++) {
    const TStr& AttrName = it.GetKey();
    TInt64 AttrType = it.GetDat().GetVal1();
    TInt64 AttrIndex = it.GetDat().GetVal2();
    for(int64 i = 0; i < NewlyAdded.Len(); i++) {
      TInt64 EId = NewlyAdded[i];
      if (AttrType == IntType) {
        Dst.AddIntAttrDatE(EId, Src.VecOfIntVecsE[AttrIndex][Src.CrossH.GetKeyId(EId)], AttrName);
      } else if (AttrType == FltType) {
        Dst.AddFltAttrDatE(EId, Src.VecOfFltVecsE[AttrIndex][Src.CrossH.GetKeyId(EId)], AttrName);
      } else if (AttrType == StrType) {
        Dst.AddStrAttrDatE(EId, Src.VecOfStrVecsE[AttrIndex][Src.CrossH.GetKeyId(EId)], AttrName);
      }
    }
  }
}

PBPGraph TCrossNet::GetBipartiteGraph(){
  PBPGraph Graph = TBPGraph::New();
  for (TCrossEdgeI EI = BegEdgeI(); EI != EndEdgeI(); EI++){
    if (!Graph->IsLNode(EI.GetSrcNId()))
      Graph->AddNode(EI.GetSrcNId(), true);
    if (!Graph->IsRNode(EI.GetDstNId()))
      Graph->AddNode(EI.GetDstNId(), false);
    Graph->AddEdge(EI.GetSrcNId(), EI.GetDstNId());
  }
  return Graph;
}

void TMMNet::LoadNetworkShm(TShMIn& ShMin) {
  MxModeId = TInt64(ShMin);
  MxCrossNetId = TInt64(ShMin);
  TModeNetInit Fm;
  TModeNetH.LoadShM(ShMin, Fm);
  TCrossNetInit Fc;
  TCrossNetH.LoadShM(ShMin, Fc);
  ModeIdToNameH.LoadShM(ShMin);
  ModeNameToIdH.LoadShM(ShMin);
  CrossIdToNameH.LoadShM(ShMin);
  CrossNameToIdH.LoadShM(ShMin);
  for (THash<TInt64, TModeNet, int64>::TIter it = TModeNetH.BegI(); it < TModeNetH.EndI(); it++) {
    it.GetDat().SetParentPointer(this);
  }
  for (THash<TInt64, TCrossNet, int64>::TIter it = TCrossNetH.BegI(); it < TCrossNetH.EndI(); it++) {
    it.GetDat().SetParentPointer(this);
  }
}

int64 TMMNet::AddModeNet(const TStr& ModeName) {
  if (ModeNameToIdH.IsKey(ModeName)) {
    return -1;
  }
  TInt64 ModeId = TInt64(MxModeId);
  MxModeId++;
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  TModeNet NewGraph(ModeId);
  NewGraph.SetParentPointer(this);
  TModeNetH.AddDat(ModeId, NewGraph);
  return ModeId;
}

int64 TMMNet::AddCrossNet(const TStr& ModeName1, const TStr& ModeName2, const TStr& CrossNetName, bool isDir) {
  TInt64 ModeId1 = GetModeId(ModeName1);
  TInt64 ModeId2 = GetModeId(ModeName2);
  return AddCrossNet(ModeId1, ModeId2, CrossNetName, isDir);
}

int64 TMMNet::AddCrossNet(const TInt64& ModeId1, const TInt64& ModeId2, const TStr& CrossNetName, bool isDir) {
  if (CrossNameToIdH.IsKey(CrossNetName)) {
    return -1;
  }
  TInt64 CrossNetId = TInt64(MxCrossNetId);
  MxCrossNetId++;
  CrossIdToNameH.AddDat(CrossNetId, CrossNetName);
  CrossNameToIdH.AddDat(CrossNetName, CrossNetId);

  TCrossNet Cross = TCrossNet(ModeId1, ModeId2, isDir, CrossNetId);
  Cross.SetParentPointer(this);
  TCrossNetH.AddDat(CrossNetId, Cross);

  TModeNetH.GetDat(ModeId1).AddNbrType(CrossNetName, ModeId1==ModeId2, isDir);
  TModeNetH.GetDat(ModeId2).AddNbrType(CrossNetName, ModeId1==ModeId2, isDir);

  return CrossNetId;
}

int64 TMMNet::DelCrossNet(const TInt64& CrossNetId) {
  return DelCrossNet(CrossIdToNameH.GetDat(CrossNetId));
}

int64 TMMNet::DelCrossNet(const TStr& CrossNet) {
  IAssertR(CrossNameToIdH.IsKey(CrossNet),TStr::Fmt("No such link type: %s", CrossNet.CStr()));
  TInt64 CrossNetId = CrossNameToIdH.GetDat(CrossNet);
  TInt64 Mode1 = GetCrossNetById(CrossNetId).Mode1;
  TInt64 Mode2 = GetCrossNetById(CrossNetId).Mode2;
  if (GetModeNetById(Mode1).DelNbrType(CrossNet) == -1 || (Mode1 != Mode2 && GetModeNetById(Mode2).DelNbrType(CrossNet) == -1)) {
    return -1;
  }
  CrossNameToIdH.DelKey(CrossNet);
  CrossIdToNameH.DelKey(CrossNetId);
  GetCrossNetById(CrossNetId).SetParentPointer(NULL);
  TCrossNetH.DelKey(CrossNetId);
  return 0;
}

int64 TMMNet::DelModeNet(const TInt64& ModeId) {
  TStr64V CrossNets;
  GetModeNetById(ModeId).GetCrossNetNames(CrossNets);
  for (int64 i = 0; i < CrossNets.Len(); i++) {
    if (DelCrossNet(CrossNets[i]) == -1) {
      return -1;
    }
  }
  TStr ModeName = ModeIdToNameH.GetDat(ModeId);
  ModeNameToIdH.DelKey(ModeName);
  ModeIdToNameH.DelKey(ModeId);
  GetModeNetById(ModeId).SetParentPointer(NULL);
  TModeNetH.DelKey(ModeId);
  return 0;
}

int64 TMMNet::DelModeNet(const TStr& ModeName) {
  IAssertR(ModeNameToIdH.IsKey(ModeName), TStr::Fmt("No such mode with name: %s", ModeName.CStr()));
  return DelModeNet(ModeNameToIdH.GetDat(ModeName));
}

TModeNet& TMMNet::GetModeNetByName(const TStr& ModeName) const {
  //IAssertR(ModeNameToIdH.IsKey(ModeName),TStr::Fmt("No such mode name: %s", ModeName.CStr()));
  return GetModeNetById(ModeNameToIdH.GetDat(ModeName));
}

TModeNet& TMMNet::GetModeNetById(const TInt64& ModeId) const {
  //  IAssertR(ModeId < TModeNetH.Len(), TStr::Fmt("Mode with id %d does not exist", ModeId));
  TModeNet &Net = (const_cast<TMMNet *>(this))->TModeNetH.GetDat(ModeId);
  return Net;
}
TCrossNet& TMMNet::GetCrossNetByName(const TStr& CrossName) const{
  //IAssertR(CrossNameToIdH.IsKey(CrossName),TStr::Fmt("No such link name: %s", CrossName.CStr()));
  return GetCrossNetById(CrossNameToIdH.GetDat(CrossName));
}
TCrossNet& TMMNet::GetCrossNetById(const TInt64& CrossId) const{
  //IAssertR(CrossIdToNameH.IsKey(CrossId),TStr::Fmt("No link with id %d exists", CrossId));
  TCrossNet& CrossNet = (const_cast<TMMNet *>(this))->TCrossNetH.GetDat(CrossId);
  return CrossNet;
}

int64 TMMNet::AddMode(const TStr& ModeName, const TInt64& ModeId, const TModeNet& ModeNet) {
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);
  MxModeId = MAX(MxModeId.Val, ModeId+1); 

  TModeNetH.AddDat(ModeId, ModeNet);
  TModeNetH[ModeId].SetParentPointer(this);
  return ModeId;
}

int64 TMMNet::CopyModeWithoutNodes(const PMMNet& Src, PMMNet& Dst, const TInt64& ModeId) {
  IAssertR(Src->ModeIdToNameH.IsKey(ModeId), TStr::Fmt("No mode with id %d", ModeId.Val));
  if (Dst->ModeIdToNameH.IsKey(ModeId)) { return -1; }

  TStr ModeName = Src->GetModeName(ModeId);
  Dst->ModeIdToNameH.AddDat(ModeId, ModeName);
  Dst->ModeNameToIdH.AddDat(ModeName, ModeId);
  Dst->MxModeId = MAX(Dst->MxModeId.Val, ModeId+1);

  // create new modenet. copy id and dense non-crossnet attr names/defaults but nothing else
  TModeNet& SrcMode = Src->GetModeNetById(ModeId);
  TModeNet DstMode(ModeId);
  DstMode.KeyToIndexTypeN = SrcMode.KeyToIndexTypeN; DstMode.IntDefaultsN = SrcMode.IntDefaultsN;
  DstMode.StrDefaultsN = SrcMode.StrDefaultsN; DstMode.FltDefaultsN = SrcMode.FltDefaultsN; 
  DstMode.VecOfIntVecsN = SrcMode.VecOfIntVecsN; DstMode.VecOfStrVecsN = SrcMode.VecOfStrVecsN;
  DstMode.VecOfFltVecsN = SrcMode.VecOfFltVecsN; DstMode.VecOfIntVecVecsN = SrcMode.VecOfIntVecVecsN;
  DstMode.DelAllAttrDatN();
  // delete all crossnet attribute names
  TStr64V CNAttrNames;
  SrcMode.GetNeighborCrossNames(CNAttrNames);
  for(int i = 0; i < CNAttrNames.Len(); i++) {
    DstMode.DelAttrN(CNAttrNames[i]);
  }

  DstMode.SetParentPointer(&*Dst); 
  Dst->TModeNetH.AddDat(ModeId, DstMode);
  return ModeId;
}

int64 TMMNet::AddCrossNet(const TStr& CrossNetName, const TInt64& CrossNetId, const TCrossNet& CrossNet) {
  CrossIdToNameH.AddDat(CrossNetId, CrossNetName);
  CrossNameToIdH.AddDat(CrossNetName, CrossNetId);

  TCrossNetH.AddDat(CrossNetId, CrossNet);
  TCrossNetH[CrossNetId].SetParentPointer(this);
  return CrossNetId;
}

int64 TMMNet::CopyCrossNetWithoutEdges(const PMMNet& Src, PMMNet& Dst, const TInt64& CrossId) {
  IAssertR(Src->CrossIdToNameH.IsKey(CrossId), TStr::Fmt("No crossnet with id %d", CrossId.Val));
  if (Dst->CrossIdToNameH.IsKey(CrossId)) { return -1; }

  TStr CrossName = Src->GetCrossName(CrossId);
  Dst->CrossIdToNameH.AddDat(CrossId, CrossName);
  Dst->CrossNameToIdH.AddDat(CrossName, CrossId);
  Dst->MxCrossNetId = MAX(Dst->MxCrossNetId.Val, CrossId+1);

  TCrossNet& SrcCross = Src->GetCrossNetById(CrossId);
  TCrossNet DstCross(SrcCross.GetMode1(), SrcCross.GetMode2(), SrcCross.IsDirected(), CrossId);
  DstCross.KeyToIndexTypeE = SrcCross.KeyToIndexTypeE; DstCross.IntDefaultsE = SrcCross.IntDefaultsE;
  DstCross.StrDefaultsE = SrcCross.StrDefaultsE; DstCross.FltDefaultsE = SrcCross.FltDefaultsE;
  DstCross.VecOfIntVecsE = SrcCross.VecOfIntVecsE; DstCross.VecOfStrVecsE = SrcCross.VecOfStrVecsE;
  DstCross.VecOfFltVecsE = SrcCross.VecOfFltVecsE;
  DstCross.DelAllAttrDatE();
  DstCross.SetParentPointer(&*Dst);

  const TInt64 &ModeId1 = DstCross.GetMode1(), &ModeId2 = DstCross.GetMode2();
  Dst->TCrossNetH.AddDat(CrossId, DstCross);
  Dst->TModeNetH.GetDat(ModeId1).AddNbrType(CrossName, ModeId1==ModeId2, DstCross.IsDirect);
  Dst->TModeNetH.GetDat(ModeId2).AddNbrType(CrossName, ModeId1==ModeId2, DstCross.IsDirect);
  return CrossId;
}

void TMMNet::ClrNbr(const TInt64& ModeId, const TInt64& CrossNetId, const bool& outEdge, const bool& sameMode, bool& isDir) {
  TStr CrossNetName = CrossIdToNameH[CrossNetId];
  TModeNetH[ModeId].ClrNbr(CrossNetName, outEdge, sameMode, isDir);
}

PMMNet TMMNet::GetSubgraphByCrossNet(TStr64V& CrossNetTypes) {
  PMMNet Result = New();
  TInt64 MxMode = 0;
  TInt64 MxCross = 0;
  TInt64H ModeH;
  for(int64 i = 0; i < CrossNetTypes.Len(); i++) {
    TStr CrossName = CrossNetTypes[i];
    TInt64 OldId = CrossNameToIdH.GetDat(CrossName);
    TInt64 NewId = MxCross++;
    TCrossNet NewCrossNet(TCrossNetH.GetDat(OldId));
    TInt64 OldModeId1 = NewCrossNet.Mode1;
    TInt64 OldModeId2 = NewCrossNet.Mode2;
    TInt64 NewModeId1, NewModeId2;
    if (ModeH.IsKey(OldModeId1)) {
      NewModeId1 = ModeH.GetDat(OldModeId1);
    } else {
      NewModeId1 = MxMode++;
      ModeH.AddDat(OldModeId1, NewModeId1);
    }
    if (ModeH.IsKey(OldModeId2)) {
      NewModeId2 = ModeH.GetDat(OldModeId2);
    } else {
      NewModeId2 = MxMode++;
      ModeH.AddDat(OldModeId2, NewModeId2);
    }
    NewCrossNet.Mode1 = NewModeId1;
    NewCrossNet.Mode2 = NewModeId2;
    NewCrossNet.CrossNetId = NewId;
    Result->AddCrossNet(CrossName, NewId, NewCrossNet);
  }
  for(TInt64H::TIter it = ModeH.BegI(); it < ModeH.EndI(); it++) {
    TStr ModeName = ModeIdToNameH.GetDat(it.GetKey());
    TInt64 NewModeId = it.GetDat();
    TModeNet NewModeNet;
    TModeNetH.GetDat(it.GetKey()).RemoveCrossNets(NewModeNet, CrossNetTypes);
    NewModeNet.ModeId = NewModeId;
    Result->AddMode(ModeName, NewModeId, NewModeNet);
  }
  Result->MxModeId = MxMode;
  Result->MxCrossNetId = MxCross;
  return Result;
}

PMMNet TMMNet::GetSubgraphByModeNet(TStr64V& ModeNetTypes) {
  THash<TInt64, TBool, int64> ModeTypeIds;
  for (int i = 0; i < ModeNetTypes.Len(); i++) {
    ModeTypeIds.AddDat(ModeNameToIdH.GetDat(ModeNetTypes[i]), true);
  }
  TStr64V CrossNetTypes;
  for (THash<TInt64, TCrossNet, int64>::TIter it = TCrossNetH.BegI(); it < TCrossNetH.EndI(); it++) {
    TCrossNet& CrossNet = it.GetDat();
    if (ModeTypeIds.IsKey(CrossNet.Mode1) && ModeTypeIds.IsKey(CrossNet.Mode2)) {
      CrossNetTypes.Add(CrossIdToNameH.GetDat(it.GetKey()));
      ModeTypeIds[CrossNet.Mode1] = false;
      ModeTypeIds[CrossNet.Mode2] = false;
    }
  }

  PMMNet Result = GetSubgraphByCrossNet(CrossNetTypes);
  TInt64 MxMode = Result->MxModeId;
  TStr64V EmptyCrossNetTypes;
  for (THash<TInt64, TBool, int64>::TIter it = ModeTypeIds.BegI(); it < ModeTypeIds.EndI(); it++) {
    if (it.GetDat().Val) {
      TStr ModeName = ModeIdToNameH.GetDat(it.GetKey());
      TInt64 NewModeId = MxMode++;
      TModeNet NewModeNet;
      TModeNetH.GetDat(it.GetKey()).RemoveCrossNets(NewModeNet, EmptyCrossNetTypes);
      NewModeNet.ModeId = NewModeId;
      Result->AddMode(ModeName, NewModeId, NewModeNet);
    }
  }
  Result->MxModeId = MxMode;
  return Result;
}

void TMMNet::ValidateCrossNetMetapaths(const TInt64& StartModeId, const TInt64V& StartNodeIds, const TVec<TInt64V>& Metapaths, TVec<TBoolV>& CrossOrientations) {
  // Ensure the start mode exists in the network
  IAssertR(ModeIdToNameH.IsKey(StartModeId), TStr::Fmt("Mode with id %d does not exist", StartModeId.Val));

  // Ensure all metapaths have same start mode; ensure all crossnets exist in network; ensure dst mode of CN j matches src mode of CN j+1. Also set crossnet orientations.
  for(int64 i = 0; i < Metapaths.Len(); i++) {
    TBoolV& OriV = CrossOrientations[i];
    for(int64 j = 0; j < Metapaths[i].Len(); j++) {
      TInt64 CNij_Id = Metapaths[i][j];
      IAssertR(CrossIdToNameH.IsKey(CNij_Id), TStr::Fmt("Crossnet with id %d does not exist", CNij_Id.Val));
      const TCrossNet& CNij = GetCrossNetById(CNij_Id);

      TBool predicate, orientation;
      TStr assertmsg;      
      if (CNij.IsDirected()) {
        orientation = true;
        if (j == 0) {
          predicate = (CNij.GetMode1() == StartModeId);
          assertmsg = TStr::Fmt("Mode1 %d of Metapaths[%d][0] (crossnet id %d, directed) does not match StartModeId %d",
                                CNij.GetMode1(), i, CNij_Id.Val, StartModeId.Val);
        } else {
          TInt64 CNij_PredId = Metapaths[i][j-1];
          const TCrossNet& CNij_Pred = GetCrossNetById(CNij_PredId);
          TInt64 CNij_PredDst = (OriV.Last().Val ? CNij_Pred.GetMode2() : CNij_Pred.GetMode1());
          predicate = (CNij.GetMode1() == CNij_PredDst);
          assertmsg = TStr::Fmt("Mode1 %d of Metapaths[%d][%d] (crossnet id %d, directed) of does not match "
                                "Metapaths[%d][%d]'s destination mode %d", CNij.GetMode1(), i, j, CNij_Id.Val,
                                i, j-1, CNij_PredDst.Val);
        }
      } else {
        if (j == 0) {
          predicate = (CNij.GetMode1() == StartModeId || CNij.GetMode2() == StartModeId);
          orientation = (CNij.GetMode1() == StartModeId);
          assertmsg = TStr::Fmt("Neither Mode1 %d nor Mode2 %d of Metapaths[%d][0] (crossnet id %d, undirected) matches "
                                "StartModeId %d", CNij.GetMode1(), CNij.GetMode2(), i, CNij_Id.Val, StartModeId.Val);
        } else {
          TInt64 CNij_PredId = Metapaths[i][j-1];
          const TCrossNet& CNij_Pred = GetCrossNetById(CNij_PredId);
          TInt64 CNij_PredDst = (OriV.Last().Val ? CNij_Pred.GetMode2() : CNij_Pred.GetMode1());
          predicate = (CNij.GetMode1() == CNij_PredDst || CNij.GetMode2() == CNij_PredDst);
          orientation = (CNij.GetMode1() == CNij_PredDst);
          assertmsg = TStr::Fmt("Neither Mode1 %d nor Mode2 %d of Metapaths[%d][%d] (crossnet id %d, undirected) matches "
                                "Metapaths[%d][%d]'s destination mode %d", CNij.GetMode1(), CNij.GetMode2(), i, j, 
                                CNij_Id.Val, i, j-1, CNij_PredDst.Val);
        }
      }
      IAssertR(predicate, assertmsg);            
      OriV.Add(orientation);
    }    
  }

  // Ensure all nodes in StartNodeIds exist in the given mode
  TModeNet& StartMode = GetModeNetById(StartModeId);
  for(TInt64V::TIter it = StartNodeIds.BegI(); it < StartNodeIds.EndI(); it++) {
    IAssertR(StartMode.IsNode(*it), TStr::Fmt("Node with id %d does not exist in starting mode with id %d", it->Val, 
             StartModeId.Val));
  }
}

PMMNet TMMNet::GetSubgraphByCrossNetMetapaths(const TInt64& StartModeId, const TInt64V& StartNodeIds, const TVec<TInt64V>& Metapaths) {
  TVec<TBoolV> CrossOrientations(Metapaths.Len(), Metapaths.Len()); // for undirected edges. true = treat mode1 as src, false = treat mode2 as src. Must be true for directed.
  ValidateCrossNetMetapaths(StartModeId, StartNodeIds, Metapaths, CrossOrientations);

  // Initialization: start with an active set of nodes in the start mode
  PMMNet Result = New();
  TMMNet::CopyModeWithoutNodes(this, Result, StartModeId);
  TModeNet& StartMode = GetModeNetById(StartModeId), & StartMode_Copy = Result->GetModeNetById(StartModeId);
  TModeNet::CopyNodesWithoutNeighbors(StartMode, StartMode_Copy, StartNodeIds);

  // At each crossnet of each metapath, discover all new modes/nodes reachable from current via crossnet, and add to result
  for(int64 i = 0; i < Metapaths.Len(); i++) {
    TInt64Set ActiveNodes;
    ActiveNodes.AddKeyV(StartNodeIds);

    for(int64 j = 0; j < Metapaths[i].Len(); j++) {
      TInt64Set NodesToAdd;
      TInt64V EdgesToAdd;

      TInt64 CNId = Metapaths[i][j];
      TStr CNName = GetCrossName(CNId);
      TCrossNet& CN = GetCrossNetById(CNId);
      
      // create copy of the metapath crossnet and its destination mode, determined by CrossOrientations
      TInt64 SrcId = (CrossOrientations[i][j].Val ? CN.GetMode1() : CN.GetMode2());
      TInt64 DstId = (CrossOrientations[i][j].Val ? CN.GetMode2() : CN.GetMode1());
      TMMNet::CopyModeWithoutNodes(this, Result, DstId);
      TModeNet& SrcMode = GetModeNetById(SrcId);
      TModeNet& DstMode = GetModeNetById(DstId), & DstMode_Copy = Result->GetModeNetById(DstId);
      TMMNet::CopyCrossNetWithoutEdges(this, Result, CNId);
      TCrossNet& CN_Copy = Result->GetCrossNetById(CNId);
      
      // get nodes and edges to add to mode and crossnet
      for(TInt64Set::TIter SrcNIt = ActiveNodes.BegI(); SrcNIt < ActiveNodes.EndI(); SrcNIt++) {
        TInt64 SrcNId = *SrcNIt;
        TInt64V CNNeighboringEdges;
        SrcMode.GetNeighborsByCrossNet(SrcNId, CNName, CNNeighboringEdges, true);
        for(TInt64V::TIter EdgeIt = CNNeighboringEdges.BegI(); EdgeIt < CNNeighboringEdges.EndI(); EdgeIt++) {          
          const TCrossNet::TCrossEdge& edge = CN.GetEdge(*EdgeIt);
          
          // If the crossnet is undirected, this edge may have the current node as the src type or dst type.
          // Therefore find the endpoint that matches SrcNId and visit the other endpoint.
          TInt64 DstNId = (edge.GetSrcNId() == SrcNId ? edge.GetDstNId() : edge.GetSrcNId());
          NodesToAdd.AddKey(DstNId);
          EdgesToAdd.Add(*EdgeIt);
        }
      }
      
      // copy nodes and edges that were discovered
      TInt64V temp; 
      NodesToAdd.GetKeyV(temp);
      TModeNet::CopyNodesWithoutNeighbors(DstMode, DstMode_Copy, temp);
      TCrossNet::CopyEdges(CN, CN_Copy, EdgesToAdd);
      ActiveNodes = NodesToAdd;
    }
  }

  return Result;
}

PNEANet TMMNet::ToNetwork(TInt64V& CrossNetTypes, TIntStrStrTr64V& NodeAttrMap, TVec<TTriple<TInt64, TStr, TStr>, int64>& EdgeAttrMap) {
  TIntPrInt64H NodeMap;
  THash<TInt64Pr, TInt64Pr, int64> EdgeMap;
  THashSet<TInt64, int64> Modes;
  PNEANet NewNet = TNEANet::New();
  //Add nodes and edges
  for (int64 i = 0; i < CrossNetTypes.Len(); i++) {
    TCrossNet& CrossNet = GetCrossNetById(CrossNetTypes[i]);
    TInt64 Mode1 = CrossNet.GetMode1();
    TInt64 Mode2 = CrossNet.GetMode2();
    Modes.AddKey(Mode1);
    Modes.AddKey(Mode2);
    bool isDirected = CrossNet.IsDirected();
    for(TCrossNet::TCrossEdgeI EdgeI = CrossNet.BegEdgeI(); EdgeI != CrossNet.EndEdgeI(); EdgeI++) {
      int64 srcNode = EdgeI.GetSrcNId();
      int64 dstNode = EdgeI.GetDstNId();
      TInt64Pr SrcNodeMapping(Mode1, srcNode);
      int64 srcId = 0;
      if (NodeMap.IsKey(SrcNodeMapping)) {
        srcId = NodeMap.GetDat(SrcNodeMapping);
      } else {
        srcId = NewNet->AddNode();
        NodeMap.AddDat(SrcNodeMapping, srcId);
      }
      TInt64Pr DstNodeMapping(Mode2, dstNode);
      int64 dstId = 0;
      if (NodeMap.IsKey(DstNodeMapping)) {
        dstId = NodeMap.GetDat(DstNodeMapping);
      } else {
        dstId = NewNet->AddNode();
        NodeMap.AddDat(DstNodeMapping, dstId);
      }
      int64 edgeId = EdgeI.GetId();
      TInt64Pr EdgeMapping(CrossNetTypes[i], edgeId);
      int64 newEId = NewNet->AddEdge(srcId, dstId);
      int64 otherEId = -1;
      if (!isDirected) {
        otherEId = NewNet->AddEdge(dstId, srcId);
      }
      EdgeMap.AddDat(EdgeMapping, TInt64Pr(newEId, otherEId));
    }
  }

  for (THashSet<TInt64, int64>::TIter it = Modes.BegI(); it != Modes.EndI(); it++) {
    TModeNet &ModeNet = GetModeNetById(it.GetKey());
    TInt64 ModeId = it.GetKey();
    for(TModeNet::TNodeI NodeIt = ModeNet.BegMMNI(); NodeIt != ModeNet.EndMMNI(); NodeIt++) {
      TInt64Pr NodeKey(ModeId, NodeIt.GetId());
      if (!NodeMap.IsKey(NodeKey)) {
        int64 newId = NewNet->AddNode();
        NodeMap.AddDat(NodeKey, newId);
      }
    }
  }

  //Add attributes
  NewNet->AddIntAttrN(TStr("Mode"));
  NewNet->AddIntAttrN(TStr("Id"));
  NewNet->AddIntAttrE(TStr("CrossNet"));
  NewNet->AddIntAttrE(TStr("Id"));
  for(TIntPrInt64H::TIter it = NodeMap.BegI(); it != NodeMap.EndI(); it++) {
    NewNet->AddIntAttrDatN(it.GetDat(), it.GetKey().GetVal1(), TStr("Mode"));
    NewNet->AddIntAttrDatN(it.GetDat(), it.GetKey().GetVal2(), TStr("Id"));
  }
  for(THash<TInt64Pr, TInt64Pr, int64>::TIter it = EdgeMap.BegI(); it != EdgeMap.EndI(); it++) {
    NewNet->AddIntAttrDatE(it.GetDat().GetVal1(), it.GetKey().GetVal1(), TStr("CrossNet"));
    NewNet->AddIntAttrDatE(it.GetDat().GetVal1(), it.GetKey().GetVal2(), TStr("Id"));
    if (it.GetDat().GetVal2() != -1) {
      NewNet->AddIntAttrDatE(it.GetDat().GetVal2(), it.GetKey().GetVal1(), TStr("CrossNet"));
      NewNet->AddIntAttrDatE(it.GetDat().GetVal2(), it.GetKey().GetVal2(), TStr("Id"));
    }
  }

  for (int64 i = 0; i < NodeAttrMap.Len(); i++) {
    //mode, orig attr, new attr
    TInt64 ModeId = NodeAttrMap[i].Val1;
    TStr OrigAttr = NodeAttrMap[i].Val2;
    TStr NewAttr = NodeAttrMap[i].Val3;
    TModeNet& Net = GetModeNetById(ModeId);
    int64 type = Net.GetAttrTypeN(OrigAttr);
    if (type == TModeNet::IntType) {
      NewNet->AddIntAttrN(NewAttr, Net.GetIntAttrDefaultN(OrigAttr));
      for(TModeNet::TNodeI it = Net.BegMMNI(); it != Net.EndMMNI(); it++) {
        TInt64Pr OldNId(ModeId, it.GetId());
        int64 NewId = NodeMap.GetDat(OldNId);
        int64 Val = Net.GetIntAttrDatN(it.GetId(), OrigAttr);
        NewNet->AddIntAttrDatN(NewId, Val, NewAttr);
      }
    } else if (type == TModeNet::FltType) {
      NewNet->AddFltAttrN(NewAttr, Net.GetFltAttrDefaultN(OrigAttr));
      for(TModeNet::TNodeI it = Net.BegMMNI(); it != Net.EndMMNI(); it++) {
        TInt64Pr OldNId(ModeId, it.GetId());
        int64 NewId = NodeMap.GetDat(OldNId);
        TFlt Val = Net.GetFltAttrDatN(it.GetId(), OrigAttr);
        NewNet->AddFltAttrDatN(NewId, Val, NewAttr);
      }

    } else if (type == TModeNet::StrType) {
      NewNet->AddStrAttrN(NewAttr, Net.GetStrAttrDefaultN(OrigAttr));
      for(TModeNet::TNodeI it = Net.BegMMNI(); it != Net.EndMMNI(); it++) {
        TInt64Pr OldNId(ModeId, it.GetId());
        int64 NewId = NodeMap.GetDat(OldNId);
        TStr Val = Net.GetStrAttrDatN(it.GetId(), OrigAttr);
        NewNet->AddStrAttrDatN(NewId, Val, NewAttr);
      }
    } else if (type == TModeNet::IntVType) {
      NewNet->AddIntVAttrN(NewAttr);
      for(TModeNet::TNodeI it = Net.BegMMNI(); it != Net.EndMMNI(); it++) {
        TInt64Pr OldNId(ModeId, it.GetId());
        int64 NewId = NodeMap.GetDat(OldNId);
        TInt64V Val = Net.GetIntVAttrDatN(it.GetId(), OrigAttr);
        NewNet->AddIntVAttrDatN(NewId, Val, NewAttr);
      }
    }
  }

  for (int64 i = 0; i < EdgeAttrMap.Len(); i++) {
    //mode, orig attr, new attr
    TInt64 CrossId = EdgeAttrMap[i].Val1;
    TStr OrigAttr = EdgeAttrMap[i].Val2;
    TStr NewAttr = EdgeAttrMap[i].Val3;
    TCrossNet& Net = GetCrossNetById(CrossId);
    int64 type = Net.GetAttrTypeE(OrigAttr);
    if (type == TCrossNet::IntType) {
      NewNet->AddIntAttrE(NewAttr, Net.GetIntAttrDefaultE(OrigAttr));
      for(TCrossNet::TCrossEdgeI it = Net.BegEdgeI(); it != Net.EndEdgeI(); it++) {
        TInt64Pr OldNId(CrossId, it.GetId());
        TInt64Pr NewId = EdgeMap.GetDat(OldNId);
        int64 Val = Net.GetIntAttrDatE(it.GetId(), OrigAttr);
        NewNet->AddIntAttrDatE(NewId.Val1, Val, NewAttr);
        if (NewId.Val2 != -1) {
          NewNet->AddIntAttrDatE(NewId.Val2, Val, NewAttr);
        }
      }
    } else if (type == TCrossNet::FltType) {
      NewNet->AddFltAttrE(NewAttr, Net.GetFltAttrDefaultE(OrigAttr));
      for(TCrossNet::TCrossEdgeI it = Net.BegEdgeI(); it != Net.EndEdgeI(); it++) {
        TInt64Pr OldNId(CrossId, it.GetId());
        TInt64Pr NewId = EdgeMap.GetDat(OldNId);
        TFlt Val = Net.GetFltAttrDatE(it.GetId(), OrigAttr);
        NewNet->AddFltAttrDatE(NewId.Val1, Val, NewAttr);
        if (NewId.Val2 != -1) {
          NewNet->AddFltAttrDatE(NewId.Val2, Val, NewAttr);
        }
      }

    } else if (type == TCrossNet::StrType) {
      NewNet->AddStrAttrE(NewAttr, Net.GetStrAttrDefaultE(OrigAttr));
      for(TCrossNet::TCrossEdgeI it = Net.BegEdgeI(); it != Net.EndEdgeI(); it++){
        TInt64Pr OldNId(CrossId, it.GetId());
        TInt64Pr NewId = EdgeMap.GetDat(OldNId);
        TStr Val = Net.GetStrAttrDatE(it.GetId(), OrigAttr);
        NewNet->AddStrAttrDatE(NewId.Val1, Val, NewAttr);
        if (NewId.Val2 != -1) {
          NewNet->AddStrAttrDatE(NewId.Val2, Val, NewAttr);
        }
      }
    }
  }
  return NewNet;
}

PNEANet TMMNet::ToNetwork2(TInt64V& CrossNetTypes, TIntStrPr64VH& NodeAttrMap, THash<TInt64, TVec<TPair<TStr, TStr>, int64 >, int64 >& EdgeAttrMap) {
  TIntPrInt64H NodeMap;
  THashSet<TInt64, int64> Modes;
  PNEANet NewNet = TNEANet::New();
  NewNet->AddIntAttrN(TStr("Mode"));
  NewNet->AddIntAttrN(TStr("Id"));
  NewNet->AddIntAttrE(TStr("CrossNet"));
  NewNet->AddIntAttrE(TStr("Id"));
  
  //Add nodes and edges
  for (int64 i = 0; i < CrossNetTypes.Len(); i++) {
    TCrossNet& CrossNet = GetCrossNetById(CrossNetTypes[i]);
    TStrPr64V CNetAttrs;
    if (EdgeAttrMap.IsKey(CrossNetTypes[i])) {
      CNetAttrs = EdgeAttrMap.GetDat(CrossNetTypes[i]);
    }
    TInt64 Mode1 = CrossNet.GetMode1();
    TInt64 Mode2 = CrossNet.GetMode2();
    TModeNet& Mode1Net = GetModeNetById(Mode1);
    TModeNet& Mode2Net = GetModeNetById(Mode2);
    TStrPr64V Mode1Attrs;
    if (NodeAttrMap.IsKey(Mode1)) {
      Mode1Attrs = NodeAttrMap.GetDat(Mode1);
    }
    TStrPr64V Mode2Attrs;
    if (NodeAttrMap.IsKey(Mode2)) {
      Mode2Attrs = NodeAttrMap.GetDat(Mode2);
    } 
    Modes.AddKey(Mode1);
    Modes.AddKey(Mode2);
    bool isDirected = CrossNet.IsDirected();
    for(TCrossNet::TCrossEdgeI EdgeI = CrossNet.BegEdgeI(); EdgeI != CrossNet.EndEdgeI(); EdgeI++) {
      int64 srcNode = EdgeI.GetSrcNId();
      int64 dstNode = EdgeI.GetDstNId();
      TInt64Pr SrcNodeMapping(Mode1, srcNode);
      int64 srcId = 0;
      if (NodeMap.IsKey(SrcNodeMapping)) {
        srcId = NodeMap.GetDat(SrcNodeMapping);
      } else {
        srcId = NewNet->AddNode();
        NodeMap.AddDat(SrcNodeMapping, srcId);
        NewNet->AddIntAttrDatN(srcId, srcNode, TStr("Id"));
        NewNet->AddIntAttrDatN(srcId, Mode1, TStr("Mode"));
        AddNodeAttributes(NewNet, Mode1Net, Mode1Attrs, Mode1, srcNode, srcId);
      }
      TInt64Pr DstNodeMapping(Mode2, dstNode);
      int64 dstId = 0;
      if (NodeMap.IsKey(DstNodeMapping)) {
        dstId = NodeMap.GetDat(DstNodeMapping);
      } else {
        dstId = NewNet->AddNode();
        NodeMap.AddDat(DstNodeMapping, dstId);
        NewNet->AddIntAttrDatN(dstId, dstNode, TStr("Id"));
        NewNet->AddIntAttrDatN(dstId, Mode2, TStr("Mode"));
        AddNodeAttributes(NewNet, Mode2Net, Mode2Attrs, Mode2, dstNode, dstId);
      }
      int64 edgeId = EdgeI.GetId();
      int64 newEId = NewNet->AddEdge(srcId, dstId);
      NewNet->AddIntAttrDatE(newEId, edgeId, TStr("Id"));
      NewNet->AddIntAttrDatE(newEId, CrossNetTypes[i], TStr("CrossNet"));
      AddEdgeAttributes(NewNet, CrossNet, CNetAttrs, CrossNetTypes[i], edgeId, newEId);
      if (!isDirected) {
        int64 otherEId = NewNet->AddEdge(dstId, srcId);
        NewNet->AddIntAttrDatE(otherEId, edgeId, TStr("Id"));
        NewNet->AddIntAttrDatE(otherEId, CrossNetTypes[i], TStr("CrossNet"));
        AddEdgeAttributes(NewNet, CrossNet, CNetAttrs, CrossNetTypes[i], edgeId, otherEId);
      }
    }
  }

  for (THashSet<TInt64, int64>::TIter it = Modes.BegI(); it != Modes.EndI(); it++) {
    TInt64 ModeId = it.GetKey();
    TModeNet &ModeNet = GetModeNetById(ModeId);
    TStrPr64V ModeAttrs = NodeAttrMap.GetDat(ModeId);
    for(TModeNet::TNodeI NodeIt = ModeNet.BegMMNI(); NodeIt != ModeNet.EndMMNI(); NodeIt++) {
      TInt64Pr NodeKey(ModeId, NodeIt.GetId());
      if (!NodeMap.IsKey(NodeKey)) {
        int64 newId = NewNet->AddNode();
        NodeMap.AddDat(NodeKey, newId);
        AddNodeAttributes(NewNet, ModeNet, ModeAttrs, ModeId, NodeIt.GetId(), newId);
      }
    }
  }

  return NewNet;
}

PNEANet TMMNet::GetMetagraph() {
  PNEANet Result = TNEANet::New(GetModeNets(), GetCrossNets());
  Result->AddStrAttrN("ModeName");
  Result->AddIntAttrN("Weight");
  Result->AddStrAttrE("CrossName");
  Result->AddIntAttrE("Weight");
  Result->AddIntAttrE("Directed", true);
  Result->AddIntAttrE("Reverse", -1);
  
  for(TModeNetI modeit = BegModeNetI(); modeit < EndModeNetI(); modeit++) {
    TInt64 ModeId = modeit.GetModeId();
    TStr ModeName = modeit.GetModeName();
    Result->AddNode(ModeId);
    Result->AddStrAttrDatN(ModeId, ModeName, "ModeName");
    Result->AddIntAttrDatN(ModeId, modeit.GetModeNet().GetNodes(), "Weight");
  }

  TInt64Set undirs; // crossids of undirected crossnets
  for(TCrossNetI crossit = BegCrossNetI(); crossit < EndCrossNetI(); crossit++) {
    TInt64 CrossId = crossit.GetCrossId();
    TStr CrossName = crossit.GetCrossName();
    const TCrossNet& CN = crossit.GetCrossNet();
    Result->AddEdge(CN.GetMode1(), CN.GetMode2(), CrossId);
    Result->AddStrAttrDatE(CrossId, CrossName, "CrossName");
    Result->AddIntAttrDatE(CrossId, CN.GetEdges(), "Weight");
    if (!CN.IsDirected()) {
      Result->AddIntAttrDatE(CrossId, false, "Directed");
      undirs.AddKey(CrossId);
    }
  }

  // For undirected crossnets, add another edge with opposite orientation from the first.
  // Addition only done now so that new edges have ids greater than any crossnet id.
  for(TInt64Set::TIter it = undirs.BegI(); it < undirs.EndI(); it++) {
    TInt64 CrossId = it.GetKey();
    TStr CrossName = GetCrossName(CrossId);
    const TCrossNet& CN = GetCrossNetById(CrossId);
    TInt64 NewEId = Result->AddEdge(CN.GetMode2(), CN.GetMode1());
    Result->AddStrAttrDatE(NewEId, CrossName, "CrossName");
    Result->AddIntAttrDatE(NewEId, CN.GetEdges(), "Weight");
    Result->AddIntAttrDatE(NewEId, false, "Directed");
    Result->AddIntAttrDatE(CrossId, NewEId, "Reverse");
    Result->AddIntAttrDatE(NewEId, CrossId, "Reverse");
  }

  return Result;
}


void TMMNet::GetPartitionRanges(TIntPr64V& Partitions, const TInt64& NumPartitions, const TInt64& MxLen) const {
  int64 reminder = MxLen.Val % NumPartitions.Val;
  TInt64 PartitionSize = MxLen/NumPartitions;
  TInt64 CurrStart = 0;
  //  bool done = false;
  for  (int64 i=0; i < reminder; i++) {
    TInt64 CurrEnd = CurrStart + PartitionSize+ 1;
    Partitions.Add(TInt64Pr(CurrStart, CurrEnd));
    CurrStart = CurrEnd;
  }
  for  (int64 i=0; i < NumPartitions.Val - reminder; i++) {
    TInt64 CurrEnd = CurrStart + PartitionSize;
    Partitions.Add(TInt64Pr(CurrStart, CurrEnd));
    CurrStart = CurrEnd;
  }

}

#ifdef GCC_ATOMIC
PNEANetMP TMMNet::ToNetworkMP(TStr64V& CrossNetNames) {
 
  TStrInt64H CrossNetStart;
  THashSet<TInt64, int64> ModeSet;
  int64 offset = 0;
  int64 NumEdges = 0;
  //TVec<TCrossNet> CrossNets;
  //std::cerr<<"here 0\n";
  for (int64 i=0; i < CrossNetNames.Len(); i++) {
    //	CrossNets.Add(GetCrossNetByName(CrossNetNames[i]));
    CrossNetStart.AddDat(CrossNetNames[i], offset);
    TCrossNet& CrossNet = GetCrossNetByName(CrossNetNames[i]);
    int64 factor = CrossNet.IsDirected() ? 1 : 2;
    offset += (CrossNet.GetMxEId() * factor);
    NumEdges += (CrossNet.GetEdges() * factor);
    ModeSet.AddKey(CrossNet.GetMode1());
    ModeSet.AddKey(CrossNet.GetMode2());
  }
  //std::cerr<<"here2\n";
  int64 MxEId = offset;

  int64 NumNodes = 0;
  for (THashSet<TInt64, int64>::TIter MI = ModeSet.BegI(); MI < ModeSet.EndI(); MI++) {
    TModeNet& ModeNet = GetModeNetById(MI.GetKey());
    NumNodes += ModeNet.GetNodes();
  }
  //std::cerr<<"here3\n";
  THashMP<TInt64Pr, TInt64, int64> NodeMap(NumNodes);
  THashMP<TInt64Pr, TInt64Pr, int64> EdgeMap(NumEdges);
  PNEANetMP NewNet = TNEANetMP::New(NumNodes, NumEdges);
  //std::cerr<<"here4\n";
  int64 num_threads = omp_get_max_threads();
  // int64 num_threads = 21;
  offset = 0;
  for (THashSet<TInt64, int64>::TIter MI = ModeSet.BegI(); MI < ModeSet.EndI(); MI++) {
    TInt64 ModeId = MI.GetKey();
    TModeNet& ModeNet = GetModeNetById(ModeId);
    TInt64V KeyIds;
    ModeNet.NodeH.GetKeyV(KeyIds);
    TIntPr64V NodePartitions;
    GetPartitionRanges(NodePartitions, num_threads, KeyIds.Len());
    //std::cerr<<"got partitions for mode "<<ModeId<<"\n";
    int64 curr_nid;
#pragma omp parallel for schedule(static) private(curr_nid)
    for (int64 i = 0; i < NodePartitions.Len(); i++) {
      //std::cerr<<"Entering first loop "<<i<<"\n";
      TInt64 CurrStart = NodePartitions[i].GetVal1();
      TInt64 CurrEnd = NodePartitions[i].GetVal2();
      curr_nid = offset + CurrStart;
      for (int64 idx = CurrStart; idx < CurrEnd ; idx++) {
	
        int64 n_i = KeyIds[idx];
        if (ModeNet.IsNode(n_i)) {
          //Collect neighbors
          TInt64V InNbrs;
          TInt64V OutNbrs;
          for (int64 j=0; j < CrossNetNames.Len(); j++) {
            TStr CrossNetName = TStr(CrossNetNames[j].CStr());
            if (ModeNet.NeighborTypes.IsKey(CrossNetName)) {
              if (ModeNet.NeighborTypes.GetDat(CrossNetName)) {
                
                TInt64V Neighbors;
                ModeNet.GetNeighborsByCrossNet(n_i, CrossNetName, Neighbors);
                int64 edge_offset = CrossNetStart.GetDat(CrossNetName);
                TCrossNet& CrossNet = GetCrossNetByName(CrossNetName);
                //TCrossNet* CrossNet = &CrossNets[j];
                bool isDir = CrossNet.IsDirected();
                bool isOutNbr = CrossNet.GetMode1() == ModeId;
                int64 factor = isDir ? 1 : 2;

                int64 id_offset = isDir || isOutNbr ? 0 : 1;
                if (!isDir && CrossNet.GetMode1() == CrossNet.GetMode2()) {
                  id_offset = n_i == CrossNet.GetEdge(n_i).GetSrcNId() ? 0 : 1;
                }

                for (int64 k = 0; k < Neighbors.Len(); k++) {
                  if (isOutNbr && id_offset == 0) {
                    OutNbrs.Add(edge_offset + Neighbors[k]*factor + id_offset);
                  } else {
                    InNbrs.Add(edge_offset + Neighbors[k]*factor + id_offset);
                  }
                  if (!isDir) {
                    int opp_offset = id_offset == 1 ? 0 : 1;
                    if (isOutNbr && id_offset == 0) {
                      InNbrs.Add(edge_offset + Neighbors[k]*factor + opp_offset);
                    } else {
                      OutNbrs.Add(edge_offset + Neighbors[k]*factor + opp_offset);
                    }
                  }
                }
              } else {
                TInt64V TempOut;
                ModeNet.GetNeighborsByCrossNet(n_i, CrossNetName, TempOut, true);
                OutNbrs.AddV(TempOut);
                TInt64V TempIn;
                ModeNet.GetNeighborsByCrossNet(n_i, CrossNetName, TempIn, false);
                InNbrs.AddV(TempIn);
              }
            }
            //std::cerr<<"DOne \""<<CrossNetNames[j].GetCStr()<<"\"\n";
          }
	  //std::cerr<<"Done for all crossnets\n";
          NewNet->AddNodeWithEdges(curr_nid, InNbrs, OutNbrs);
          TInt64Pr NodeKey(MI.GetKey(), n_i);
          NodeMap.AddDat(NodeKey, curr_nid);
          curr_nid++;
        }
      }
      //std::cerr<<"Exiting first loop "<<i<<"\n";
    }
  
    offset += KeyIds.Len();
  }
  NewNet->SetNodes(offset);
  //std::cerr<<"here 5\n";
  for (int64 j=0; j < CrossNetNames.Len(); j++) {
    TStr CrossNetName = CrossNetNames[j];
    TCrossNet& CrossNet = GetCrossNetByName(CrossNetName);
    TInt64 CrossNetId = GetCrossId(CrossNetName);
    TInt64 Mode1 = CrossNet.GetMode1();
    TInt64 Mode2 = CrossNet.GetMode2();

    TIntPr64V EdgePartitions;
    GetPartitionRanges(EdgePartitions, num_threads, CrossNet.MxEId);
    int64 curr_eid;
    offset = CrossNetStart.GetDat(CrossNetNames[j]);
    int64 factor = CrossNet.IsDirected() ? 1 : 2;
#pragma omp parallel for schedule(static) private(curr_eid)
    for (int64 i = 0; i < EdgePartitions.Len(); i++) {
      TInt64 CurrStart = EdgePartitions[i].GetVal1();
      TInt64 CurrEnd = EdgePartitions[i].GetVal2();
      for (int64 e_i = CurrStart; e_i < CurrEnd ; e_i++) {
        curr_eid = offset + factor*e_i;
        if (CrossNet.IsEdge(e_i)) {
          int64 new_eid = curr_eid;
          TInt64Pr EdgeKey(CrossNetId, e_i);
          TCrossNet::TCrossEdgeI edge = CrossNet.GetEdgeI(e_i);
          int64 srcNode = edge.GetSrcNId();
          int64 dstNode = edge.GetDstNId();
          TInt64Pr NodeKeySrc(Mode1, srcNode);
          TInt64Pr NodeKeyDst(Mode2, dstNode);
          int64 newSrc = NodeMap.GetDat(NodeKeySrc);
          int64 newDst = NodeMap.GetDat(NodeKeyDst);
          NewNet->AddEdgeUnchecked(curr_eid, newSrc, newDst);
          curr_eid++;
          int64 otherEId = -1;
          if (!CrossNet.IsDirected()) {
            otherEId = curr_eid;
            NewNet->AddEdgeUnchecked(otherEId, newDst, newSrc);
          }
          EdgeMap.AddDat(EdgeKey, TInt64Pr(new_eid, otherEId));
        }
      }
    }
  }
  NewNet->SetEdges(MxEId);
  NewNet->ReserveAttr(2, 0, 0, 2, 0, 0);
  //std::cerr<<"here6\n";
  //Add attributes
  NewNet->AddIntAttrN(TStr("Mode"));
  NewNet->AddIntAttrN(TStr("Id"));
  NewNet->AddIntAttrE(TStr("CrossNet"));
  NewNet->AddIntAttrE(TStr("Id"));

  TIntPr64V NewNodeIds;
  NodeMap.GetKeyV(NewNodeIds);

#pragma omp parallel for schedule(static)
  for(int64 i = 0; i < NewNodeIds.Len(); i++) {
    NewNet->AddIntAttrDatN(NodeMap.GetDat(NewNodeIds[i]), NewNodeIds[i].GetVal1(), TStr("Mode"));
    NewNet->AddIntAttrDatN(NodeMap.GetDat(NewNodeIds[i]), NewNodeIds[i].GetVal2(), TStr("Id"));
  }

  //std::cerr<<"Here8\n";
  TIntPr64V NewEdgeIds;
  EdgeMap.GetKeyV(NewEdgeIds);
#pragma omp parallel for schedule(static)
  for(int64 i = 0; i < NewEdgeIds.Len(); i++) {
    NewNet->AddIntAttrDatE(EdgeMap.GetDat(NewEdgeIds[i]).GetVal1(), NewEdgeIds[i].GetVal2(), TStr("Id"));
    NewNet->AddIntAttrDatE(EdgeMap.GetDat(NewEdgeIds[i]).GetVal1(), NewEdgeIds[i].GetVal1(), TStr("CrossNet"));
    if (EdgeMap.GetDat(NewEdgeIds[i]).GetVal2() != -1) {
      NewNet->AddIntAttrDatE(EdgeMap.GetDat(NewEdgeIds[i]).GetVal2(), NewEdgeIds[i].GetVal1(), TStr("CrossNet"));
      NewNet->AddIntAttrDatE(EdgeMap.GetDat(NewEdgeIds[i]).GetVal2(), NewEdgeIds[i].GetVal2(), TStr("Id"));
    }
  }
  //std::cerr<<"Here9\n";
  return NewNet;
}
#endif // GCC_ATOMIC

int64 TMMNet::AddNodeAttributes(PNEANet& NewNet, TModeNet& Net, TVec<TPair<TStr, TStr>, int64 >& Attrs, int64 ModeId, int64 oldId, int64 NId) {
  for (int64 i = 0; i < Attrs.Len(); i++) {
    //mode, orig attr, new attr
    TStr OrigAttr = Attrs[i].Val1;
    TStr NewAttr = Attrs[i].Val2;
    int64 type = Net.GetAttrTypeN(OrigAttr);
    if (type == TModeNet::IntType) {
      TInt64Pr OldNId(ModeId, oldId);
      TInt64 Val = Net.GetIntAttrDatN(oldId, OrigAttr);
      NewNet->AddIntAttrDatN(NId, Val, NewAttr);
    } else if (type == TModeNet::FltType) {
      TInt64Pr OldNId(ModeId, oldId);
      TFlt Val = Net.GetFltAttrDatN(oldId, OrigAttr);
      NewNet->AddFltAttrDatN(NId, Val, NewAttr);
    } else if (type == TModeNet::StrType) {
      TInt64Pr OldNId(ModeId, oldId);
      TStr Val = Net.GetStrAttrDatN(oldId, OrigAttr);
      NewNet->AddStrAttrDatN(NId, Val, NewAttr);
    } else if (type == TModeNet::IntVType) {
      TInt64Pr OldNId(ModeId, oldId);
      TInt64V Val = Net.GetIntVAttrDatN(oldId, OrigAttr);
      NewNet->AddIntVAttrDatN(NId, Val, NewAttr);
    }
  }
  return 0;
}

int64 TMMNet::AddEdgeAttributes(PNEANet& NewNet, TCrossNet& Net, TVec<TPair<TStr, TStr>, int64 >& Attrs, int64 CrossId, int64 oldId, int64 EId) {
  for (int64 i = 0; i < Attrs.Len(); i++) {
    //mode, orig attr, new attr
    TStr OrigAttr = Attrs[i].Val1;
    TStr NewAttr = Attrs[i].Val2;
    int64 type = Net.GetAttrTypeE(OrigAttr);
    if (type == TCrossNet::IntType) {
      TInt64Pr OldNId(CrossId, oldId);
      TInt64 Val = Net.GetIntAttrDatE(oldId, OrigAttr);
      NewNet->AddIntAttrDatE(EId, Val, NewAttr);
    } else if (type == TCrossNet::FltType) {
      TInt64Pr OldNId(CrossId, oldId);
      TFlt Val = Net.GetFltAttrDatE(oldId, OrigAttr);
      NewNet->AddFltAttrDatE(EId, Val, NewAttr);
    } else if (type == TCrossNet::StrType) {
      TInt64Pr OldNId(CrossId, oldId);
      TStr Val = Net.GetStrAttrDatE(oldId, OrigAttr);
      NewNet->AddStrAttrDatE(EId, Val, NewAttr);
    }
  }
  return 0;
}
