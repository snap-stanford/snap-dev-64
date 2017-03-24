namespace TSnap {

PNGraph CascGraphSource(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt64 W) {
  // Attribute to Int mapping
  TInt64 SIdx = P->GetColIdx(C1); //Source
  TInt64 DIdx = P->GetColIdx(C2); //Dest
  TInt64 StIdx = P->GetColIdx(C3); //Start
  TInt64 DuIdx = P->GetColIdx(C4); //Duration
  TInt64V MapV;
  TStr64V SortBy;
  SortBy.Add(C1);
  P->Order(SortBy);
  TInt64V Source;
  P->ReadIntCol(C1,Source);
  PNGraph Graph = TNGraph::New();
  //Add Nodes
  for (TRowIterator RI = P->BegRI(); RI < P-> EndRI(); RI++) {
    Graph->AddNode(RI.GetRowIdx().Val);
    MapV.Add(RI.GetRowIdx());
  }
  //Add Edges
  for (TRowIterator OI = P->BegRI(); OI < P->EndRI(); OI++) {
    int64 OIdx = OI.GetRowIdx().Val;
    int64 ODest = P->GetIntValAtRowIdx(DIdx,OIdx).Val;
    int64 OStart = P->GetIntValAtRowIdx(StIdx,OIdx).Val;
    int64 ODur = P->GetIntValAtRowIdx(DuIdx,OIdx).Val;
    // Inline binary Search
    int64 val = ODest;
    int64 lo = 0;
    int64 hi = Source.Len() - 1;
    int64 index = -1;
    while (hi >= lo) {
      int64 mid = lo + (hi - lo)/2;
      if (Source.GetVal(mid) > val) { hi = mid - 1;}
      else if (Source.GetVal(mid) < val) { lo = mid + 1;}
      else { index = mid; hi = mid - 1;}
    }
    // End of binary Search
    int64 BIdx = index;
    for(int64 i = BIdx; i < Source.Len(); i++) {
      int64 InIdx = MapV.GetVal(i).Val;
      if (InIdx == OIdx) {continue;}
      int64 InSource = P->GetIntValAtRowIdx(SIdx,InIdx).Val;
      int64 InStart = P->GetIntValAtRowIdx(StIdx,InIdx).Val;
      if (InSource != ODest) { break;}
      if (InStart >= (ODur + OStart) && InStart - (ODur + OStart) <= W.Val) {
        if (!Graph->IsEdge(OIdx,InIdx)) {
          Graph->AddEdge(OIdx,InIdx);
        }
      }      
    }
  }
  return Graph;
}

PNGraph CascGraphTime(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt64 W) {
  // Attribute to Int mapping
  TInt64 SIdx = P->GetColIdx(C1); //Source
  TInt64 DIdx = P->GetColIdx(C2); //Dest
  TInt64 StIdx = P->GetColIdx(C3); //Start
  TInt64 DuIdx = P->GetColIdx(C4); //Duration
  TInt64V MapV;
  TStr64V SortBy;
  SortBy.Add(C3);
  P->Order(SortBy);
  TInt64V Start;
  P->ReadIntCol(C3,Start);
  PNGraph Graph = TNGraph::New();
  //Add Nodes
  for (TRowIterator RI = P->BegRI(); RI < P-> EndRI(); RI++) {
    Graph->AddNode(RI.GetRowIdx().Val);
    MapV.Add(RI.GetRowIdx());
  }
  //Add Edges
  for (TRowIterator OI = P->BegRI(); OI < P->EndRI(); OI++) {
    int64 OIdx = OI.GetRowIdx().Val;
    int64 ODest = P->GetIntValAtRowIdx(DIdx,OIdx).Val;
    int64 OStart = P->GetIntValAtRowIdx(StIdx,OIdx).Val;
    int64 ODur = P->GetIntValAtRowIdx(DuIdx,OIdx).Val;
    // Inline binary Search
    int64 val = OStart + ODur;
    int64 lo = 0;
    int64 hi = Start.Len() - 1;
    int64 index = -1;
    if (val >= Start.GetVal(hi)) { val = Start.GetVal(hi);}
    while (hi >= lo) {
      int64 mid = lo + (hi - lo)/2;
      if (Start.GetVal(mid) > val) {
        if ((mid-1) >= lo  &&  Start.GetVal(mid - 1) < val) {
          index = mid - 1;break;
        }
        hi = mid - 1;
      }
      else if (Start.GetVal(mid) < val) {
        if (mid + 1 <= hi  &&  Start.GetVal(mid + 1) > val) {
          index = mid;break;
        }
        lo = mid + 1;
      }
      else { index = mid; hi = mid - 1;}
    }
    // End of binary Search
    int64 BIdx = index;
    for(int64 i = BIdx; i < Start.Len(); i++) {
      int64 InIdx = MapV.GetVal(i).Val;
      if (InIdx == OIdx) {continue;}
      int64 InSource = P->GetIntValAtRowIdx(SIdx,InIdx).Val;
      int64 InStart = P->GetIntValAtRowIdx(StIdx,InIdx).Val;
      if (InStart - (ODur + OStart) > W.Val) { break;}
      if (InSource == ODest && InStart >= (ODur + OStart)) {
        if (!Graph->IsEdge(OIdx,InIdx)) {
          Graph->AddEdge(OIdx,InIdx);
        }
      }      
    }
  }
  return Graph;
}

PNGraph CascGraph(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt64 W,bool SortParam) {
  if (SortParam) {
    return CascGraphSource(P, C1, C2, C3, C4, W);
  }
  else {
    return CascGraphTime(P, C1, C2, C3, C4, W);
  }
}

void CascFind(PNGraph Graph,PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,TVec<TInt64V, int64> &TopCascVV,bool Print) {
  // Attribute to Int mapping
  TInt64 SIdx = P->GetColIdx(C1);
  TInt64 DIdx = P->GetColIdx(C2);
  TInt64 StIdx = P->GetColIdx(C3);
  TInt64 DuIdx = P->GetColIdx(C4);
  TInt64V MapV, PhyV;
  TStr64V SortBy;
  SortBy.Add(C3);
  P->Order(SortBy);
  int64 count = 0;
  for (TRowIterator RI = P->BegRI(); RI < P-> EndRI(); RI++) {
    MapV.Add(RI.GetRowIdx());
    PhyV.Add(count++);
  }
  // After sort attach with each row a rank helpful for sorting
  P->StoreIntCol("Physical",PhyV);
  TInt64 PIdx = P->GetColIdx("Physical");
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    // Check for top cascades
    if (NI.GetInDeg() != 0) { continue;}
    TInt64V CurCasc;
    TSnapQueue<TInt64> EventQ;
    THashSet<TInt64> VisitedH;
    TInt64 NId = NI.GetId();
    EventQ.Push(NId);
    VisitedH.AddKey(NId);
    CurCasc.Add(P->GetIntValAtRowIdx(PIdx,NId));
    while (! EventQ.Empty()) {
      TNGraph::TNodeI CNI = Graph->GetNI(EventQ.Top().Val); //Get Current Node
      EventQ.Pop();
      // Go over the outdegree nodes of the currernt node
      for (int64 e = 0; e < CNI.GetOutDeg(); e++) {
        TInt64 CId = CNI.GetOutNId(e);
        if ( !VisitedH.IsKey(CId)) {
          EventQ.Push(CId);
          VisitedH.AddKey(CId);
          CurCasc.Add(P->GetIntValAtRowIdx(PIdx,CId));
        }
      }
    }
    CurCasc.Sort();
    TInt64V ToAddV;
    if (Print && VisitedH.Len() > 1) {
      printf("__casacade__\t%d\n",VisitedH.Len());
    }
    for (TInt64V::TIter VI = CurCasc.BegI(); VI < CurCasc.EndI(); VI++) {
      ToAddV.Add(MapV.GetVal(VI->Val));
      if (Print && VisitedH.Len() > 1) {
        int64 PIdx = MapV.GetVal(VI->Val).Val;
        int64 PSource = P->GetIntValAtRowIdx(SIdx,PIdx).Val;
        int64 PDest = P->GetIntValAtRowIdx(DIdx,PIdx).Val;
        int64 PStart = P->GetIntValAtRowIdx(StIdx,PIdx).Val;
        int64 PDur = P->GetIntValAtRowIdx(DuIdx,PIdx).Val;
        printf("%d\t%d\t%d\t%d\t%d\n",PIdx,PSource,PDest,PStart,PDur);
      }   
    }
    if (ToAddV.Len() > 1) {
      TopCascVV.Add(ToAddV);
    }
  }
  return;
}

#ifdef USE_OPENMP
void CascFindMP(PNGraph Graph,PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,TVec<TInt64V, int64> &TopCascVV) {
  // Attribute to Int mapping
  TInt64 SIdx = P->GetColIdx(C1);
  TInt64 DIdx = P->GetColIdx(C2);
  TInt64 StIdx = P->GetColIdx(C3);
  TInt64 DuIdx = P->GetColIdx(C4);
  TInt64V MapV, PhyV;
  TStr64V SortBy;
  SortBy.Add(C3);
  P->Order(SortBy);
  int64 count = 0;
  for (TRowIterator RI = P->BegRI(); RI < P-> EndRI(); RI++) {
    MapV.Add(RI.GetRowIdx());
    PhyV.Add(count++);
  }
  P->StoreIntCol("Physical",PhyV);
  TInt64 PIdx = P->GetColIdx("Physical");
  TInt64V GNodeV;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetInDeg() == 0) { GNodeV.Add(NI.GetId()); }
  }
  TVec<TInt64V, int64> ThTopCascVV; // for each thread
  #pragma omp parallel private(ThTopCascVV) num_threads(10)
  {
    #pragma omp for schedule(dynamic,1000)
    for (int64 i = 0; i < GNodeV.Len(); i++) {
      TInt64V CurCasc;
      TSnapQueue<TInt64> EventQ;
      THashSet<TInt64> VisitedH;
      TInt64 NId = GNodeV[i];
      EventQ.Push(NId);
      VisitedH.AddKey(NId);
      CurCasc.Add(P->GetIntValAtRowIdx(PIdx,NId));
      while (! EventQ.Empty()) {
        TNGraph::TNodeI CNI = Graph->GetNI(EventQ.Top().Val); //Get Current Node
        EventQ.Pop();
        // Go over the outdegree nodes of the currernt node
        for (int64 e = 0; e < CNI.GetOutDeg(); e++) {
          TInt64 CId = CNI.GetOutNId(e);
          if ( !VisitedH.IsKey(CId)) {
            EventQ.Push(CId);
            VisitedH.AddKey(CId);
            CurCasc.Add(P->GetIntValAtRowIdx(PIdx,CId));
          }
        }
      }
      CurCasc.Sort();
      TInt64V ToAddV;
      for (TInt64V::TIter VI = CurCasc.BegI(); VI < CurCasc.EndI(); VI++) {
        ToAddV.Add(MapV.GetVal(VI->Val));
      }
      if (ToAddV.Len() > 1) { ThTopCascVV.Add(ToAddV);}  
    }
    #pragma omp critical
    {
      for (int64 j = 0; j < ThTopCascVV.Len(); j++) {
        TopCascVV.Add(ThTopCascVV[j]);
      }
    }
  }
  return;
}
#endif // USE_OPENMP
} //Namespace TSnap
