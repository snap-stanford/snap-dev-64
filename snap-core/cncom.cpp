/////////////////////////////////////////////////
// Connected Components
void TCnCom::Dump(const TCnComV& CnComV, const TStr& Desc) {
  if (! Desc.Empty()) { printf("%s:\n", Desc.CStr()); }
  for (int64 cc = 0; cc < CnComV.Len(); cc++) {
    const TInt64V& NIdV = CnComV[cc].NIdV;
    printf("%s : ", TInt64::GetStr(NIdV.Len()).CStr());
    for (int64 i = 0; i < NIdV.Len(); i++) { printf(" %s", TInt64::GetStr(NIdV[i].Val).CStr()); }
    printf("\n");
  }
}

void TCnCom::SaveTxt(const TCnComV& CnComV, const TStr& FNm, const TStr& Desc) {
  FILE *F = fopen(FNm.CStr(), "wt");
  if (! Desc.Empty()) { fprintf(F, "# %s\n", Desc.CStr()); }
  fprintf(F, "# Connected Components:\t%s\n", TInt64::GetStr(CnComV.Len()).CStr());
  fprintf(F, "# Connected components (format: <Size>\\t<NodeId1>\\t<NodeId2>...)\n");
  for (int64 cc = 0; cc < CnComV.Len(); cc++) {
    const TInt64V& NIdV = CnComV[cc].NIdV;
    fprintf(F, "%s", TInt64::GetStr(NIdV.Len()).CStr());
    for (int i = 0; i < NIdV.Len(); i++) { fprintf(F, "\t%s", TInt64::GetStr(NIdV[i].Val).CStr()); }
    fprintf(F, "\n");
  }
  fclose(F);
}

/////////////////////////////////////////////////
// Connected Components
namespace TSnap {

void GetBiConSzCnt(const PUNGraph& Graph, TIntPr64V& SzCntV) {
  TCnComV BiCnComV;
  GetBiCon(Graph, BiCnComV);
  TInt64H SzCntH;
  for (int64 c =0; c < BiCnComV.Len(); c++) {
    SzCntH.AddDat(BiCnComV[c].Len()) += 1;
  }
  SzCntH.GetKeyDatPrV(SzCntV);
  SzCntV.Sort();
}

void GetBiCon(const PUNGraph& Graph, TCnComV& BiCnComV) {
  TBiConVisitor Visitor(Graph->GetNodes());
  TCnCom::GetDfsVisitor(Graph, Visitor);
  BiCnComV = Visitor.CnComV;
}

void GetArtPoints(const PUNGraph& Graph, TInt64V& ArtNIdV) {
  TArtPointVisitor Visitor(Graph->GetNodes());
  TCnCom::GetDfsVisitor(Graph, Visitor);
  Visitor.ArtSet.GetKeyV(ArtNIdV);
}

// bridges are edges in the size 2 biconnected components
void GetEdgeBridges(const PUNGraph& Graph, TIntPr64V& EdgeV) {
  TCnComV BiCnComV;
  GetBiCon(Graph, BiCnComV);
  TInt64PrSet EdgeSet;
  for (int64 c = 0; c < BiCnComV.Len(); c++) {
    const TInt64V& NIdV = BiCnComV[c].NIdV; 
    if (NIdV.Len() == 2) {
      EdgeSet.AddKey(TInt64Pr(TMath::Mn(NIdV[0], NIdV[1]), TMath::Mx(NIdV[0], NIdV[1]))); 
    }
  }
  EdgeSet.GetKeyV(EdgeV);
}

// Algorithm: Find all bridges, remove them from the graph, find largest component K
// now add all bridges that do not touch K, find connected components
void Get1CnComSzCnt(const PUNGraph& Graph, TIntPr64V& SzCntV) {
  //TCnCom::GetWccCnt(Graph, SzCntV);  IAssertR(SzCntV.Len() == 1, "Graph is not connected.");
  TIntPr64V EdgeV;
  GetEdgeBridges(Graph, EdgeV);
  if (EdgeV.Empty()) { SzCntV.Clr(false); return; }
  PUNGraph TmpG = TUNGraph::New();
  *TmpG = *Graph;
  for (int64 e = 0; e < EdgeV.Len(); e++) {
    TmpG->DelEdge(EdgeV[e].Val1, EdgeV[e].Val2);  }
  TCnComV CnComV;  GetWccs(TmpG, CnComV);
  IAssert(CnComV.Len() >= 2);
  const TInt64V& MxWcc = CnComV[0].NIdV;
  TInt64Set MxCcSet(MxWcc.Len());
  for (int64 i = 0; i < MxWcc.Len(); i++) { 
    MxCcSet.AddKey(MxWcc[i]); }
  // create new graph: bridges not touching MxCc of G with no bridges
  for (int64 e = 0; e < EdgeV.Len(); e++) {
    if (! MxCcSet.IsKey(EdgeV[e].Val1) &&  ! MxCcSet.IsKey(EdgeV[e].Val2)) {
      TmpG->AddEdge(EdgeV[e].Val1, EdgeV[e].Val2); }
  }
  GetWccSzCnt(TmpG, SzCntV);
  for (int64 c = 0; c < SzCntV.Len(); c++) {
    if (SzCntV[c].Val1 == MxCcSet.Len()) { 
      SzCntV.Del(c);  break; }
  }
}

// get the node ids in 1-connected components 
void Get1CnCom(const PUNGraph& Graph, TCnComV& Cn1ComV) {
  //TCnCom::GetWccCnt(Graph, SzCntV);  IAssertR(SzCntV.Len() == 1, "Graph is not connected.");
  TIntPr64V EdgeV;
  GetEdgeBridges(Graph, EdgeV);
  if (EdgeV.Empty()) { Cn1ComV.Clr(false); return; }
  PUNGraph TmpG = TUNGraph::New();
  *TmpG = *Graph;
  for (int64 e = 0; e < EdgeV.Len(); e++) {
    TmpG->DelEdge(EdgeV[e].Val1, EdgeV[e].Val2);  }
  TCnComV CnComV;  GetWccs(TmpG, CnComV);
  IAssert(CnComV.Len() >= 2);
  const TInt64V& MxWcc = CnComV[0].NIdV;
  TInt64Set MxCcSet(MxWcc.Len());
  for (int64 i = 0; i < MxWcc.Len(); i++) { 
    MxCcSet.AddKey(MxWcc[i]); }
  // create new graph: bridges not touching MxCc of G with no bridges
  for (int64 e = 0; e < EdgeV.Len(); e++) {
    if (! MxCcSet.IsKey(EdgeV[e].Val1) &&  ! MxCcSet.IsKey(EdgeV[e].Val2)) {
      TmpG->AddEdge(EdgeV[e].Val1, EdgeV[e].Val2); }
  }
  GetWccs(TmpG, Cn1ComV);
  // remove the largest component of G
  for (int64 c = 0; c < Cn1ComV.Len(); c++) {
    if (MxCcSet.IsKey(Cn1ComV[c].NIdV[0])) {
      Cn1ComV.Del(c);  break; }
  }
}

PUNGraph GetMxBiCon(const PUNGraph& Graph, const bool& RenumberNodes) {
  TCnComV CnComV;
  GetBiCon(Graph, CnComV);
  if (CnComV.Empty()) { 
    return PUNGraph(); 
  }
  int64 CcId = 0, MxSz = 0;
  for (int64 i = 0; i < CnComV.Len(); i++) {
    if (MxSz < CnComV[i].Len()) {
      MxSz = CnComV[i].Len();  
      CcId=i; 
    }
  }
  return TSnap::GetSubGraph(Graph, CnComV[CcId](), RenumberNodes);
}

} // namespace TSnap
