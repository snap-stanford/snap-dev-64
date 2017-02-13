/////////////////////////////////////////////////
// Approximate Neighborhood Function.
namespace TSnap {
/// Approximate Neighborhood Function of a node: Returns the (approximate) number of nodes reachable from SrcNId in less than H hops.
/// @param SrcNId Starting node.
/// @param DistNbrsV Maps between the distance H (in hops) and the number of nodes reachable in <=H hops.
/// @param MxDist Maximum number of hops the algorithm spreads from SrcNId.
/// @param IsDir false: consider links as undirected (drop link directions).
/// @param NApprox Quality of approximation. See the ANF paper.
template <class PGraph> void GetAnf(const PGraph& Graph, const int64& SrcNId, TIntFlt64KdV& DistNbrsV, const int64& MxDist, const bool& IsDir, const int64& NApprox=32);
/// Approximate Neighborhood Function of a Graph: Returns the number of pairs of nodes reachable in less than H hops.
/// For example, DistNbrsV.GetDat(0) is the number of nodes in the graph, DistNbrsV.GetDat(1) is the number of nodes+edges and so on.
/// @param DistNbrsV Maps between the distance H (in hops) and the number of nodes reachable in <=H hops.
/// @param MxDist Maximum number of hops the algorithm spreads from SrcNId.
/// @param IsDir false: consider links as undirected (drop link directions).
/// @param NApprox Quality of approximation. See the ANF paper.
template <class PGraph> void GetAnf(const PGraph& Graph, TIntFlt64KdV& DistNbrsV, const int64& MxDist, const bool& IsDir, const int64& NApprox=32);
/// Returns a given Percentile of the shortest path length distribution of a Graph (based on a single run of ANF of approximation quality NApprox).
/// @param IsDir false: consider links as undirected (drop link directions).
template <class PGraph> double GetAnfEffDiam(const PGraph& Graph, const bool& IsDir, const double& Percentile, const int64& NApprox);
/// Returns a 90-th percentile of the shortest path length distribution of a Graph (based on a NRuns runs of ANF of approximation quality NApprox).
/// @param IsDir false: consider links as undirected (drop link directions).
template <class PGraph> double GetAnfEffDiam(const PGraph& Graph, const int64 NRuns=1, int64 NApprox=-1);
} // namespace TSnap

/////////////////////////////////////////////////
/// Approximate Neighborhood Function.
/// Implements the algorithm for computing the diameter of a given graph.
/// The method is based on the approximate counting argument by Flajolet-Martin.
/// For more details see C. R. Palmer, P. B. Gibbons and C. Faloutsos, ANF: A Fast and Scalable Tool for Data Mining in Massive Graphs, KDD 2002 (http://www.cs.cmu.edu/~christos/PUBLICATIONS/kdd02-anf.ps.gz)
/// See TSnap::TestAnf() for example of how to use the class.
template <class PGraph>
class TGraphAnf {
private:
  typedef TVec<uint64, int64> TAnfBitV;
  THash<TInt64, uint64, int64> NIdToBitPosH;  // NId to byte(!) offset in BitV
  TInt64 NApprox;                      // maintain N parallel approximations (multiple of 8)
  TInt64 NBits, MoreBits, ApproxBytes; // NBits=logNodes+MoreBits;  MoreBits: additional R bits;  ApproxBytes: Approx/8;
  PGraph Graph;
  TRnd Rnd;
private:
  UndefDefaultCopyAssign(TGraphAnf);
public:
  TGraphAnf(const PGraph& GraphPt, const int64& Approx=32, const int64& moreBits=5, const int64& RndSeed=0) :
    NApprox(Approx), MoreBits(moreBits), Graph(GraphPt), Rnd(RndSeed) { IAssert(NApprox%8==0);  IAssert(sizeof(uint)==4); }
  uint64 GetNIdOffset(const int64& NId) const { return NIdToBitPosH.GetDat(NId); }
  void InitAnfBits(TAnfBitV& BitV);
  void Union(TAnfBitV& BitV1, const uint64& NId1Offset, TAnfBitV& BitV2, const uint64& NId2Offset) const;
  double AvgLstZero(const TAnfBitV& BitV, const uint64& NIdOffset) const;
  double GetCount(const TAnfBitV& BitV, const uint64& NIdOffset) const {
    return pow(2.0, AvgLstZero(BitV, NIdOffset)) / 0.77351; }
  /// Returns the number of nodes reachable from SrcNId in less than H hops.
  /// @param SrcNId Starting node.
  /// @param DistNbrsV Maps between the distance H (in hops) and the number of nodes reachable in <=H hops.
  /// @param MxDist Maximum number of hops the algorithm spreads from SrcNId.
  /// @param IsDir false: consider links as undirected (drop link directions).
  void GetNodeAnf(const int64& SrcNId, TIntFlt64KdV& DistNbrsV, const int64& MxDist, const bool& IsDir);
  /// Returns the number of pairs of nodes reachable in less than H hops.
  /// For example, DistNbrsV.GetDat(0) is the number of nodes in the graph, DistNbrsV.GetDat(1) is the number of nodes+edges and so on.
  /// @param DistNbrsV Maps between the distance H (in hops) and the number of nodes reachable in <=H hops.
  /// @param MxDist Maximum number of hops the algorithm spreads from SrcNId.
  /// @param IsDir false: consider links as undirected (drop link directions).
  void GetGraphAnf(TIntFlt64KdV& DistNbrsV, const int64& MxDist, const bool& IsDir);
};

template <class PGraph>
void TGraphAnf<PGraph>::InitAnfBits(TAnfBitV& BitV) {
  const int64 NNodes = Graph->GetNodes();
  const int64 LogNodes = int64(ceil(TMath::Log2(NNodes)));
  ApproxBytes = NApprox / 8;
  NBits = LogNodes + MoreBits; // bits per node
  const int64 BytesPerNd = ApproxBytes * NBits; // total bytes per node
  int64 VSize = ((static_cast<int64>(NNodes) * static_cast<int64>(BytesPerNd))/sizeof(uint64)) + 1;
  IAssertR(VSize <= TInt64::Mx,
    TStr::Fmt("Your graph is too large for Approximate Neighborhood Function, %s is larger than %d",
    TUInt64::GetStr(VSize).CStr(),TInt64::Mx));
  printf("size %d\n", static_cast<int64>(VSize));
  BitV.Gen((const int64)VSize);  IAssert(BitV.BegI() != NULL);
  BitV.PutAll(0);
  int64 SetBit = 0;
  uint64 NodeOff = 0;
  uchar* BitVPt = (uchar *) BitV.BegI();
  // for each node: 1st bits of all approximations are at BitV[Offset+0], 2nd bits at BitV[Offset+NApprox/32], ...
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI != Graph->EndNI(); NI++) {
    NIdToBitPosH.AddDat(NI.GetId(), NodeOff);
    // init vertex bits
    for (int64 approx = 0; approx < NApprox; approx++) {
      const int64 RndNum = Rnd.GetUniDevInt64();
      for (SetBit = 0; (RndNum & (1<<SetBit))==0 && SetBit < NBits; SetBit++) { }
      if (SetBit >= NBits) SetBit = NBits-1;
      const int64 BitPos = ApproxBytes * SetBit + approx / 8;
      *(BitVPt + NodeOff + BitPos) |= (1<<(approx % 8)); // magically works better than code below (see anf.c)
    }
    NodeOff += BytesPerNd;
  }
}

template <class PGraph>
void TGraphAnf<PGraph>::Union(TAnfBitV& BitV1, const uint64& NId1Offset, TAnfBitV& BitV2, const uint64& NId2Offset) const {
  uchar* DstI = (uchar *) BitV1.BegI() + NId1Offset;
  uchar* SrcI = (uchar *) BitV2.BegI() + NId2Offset;
  for (int64 b=0; b < ApproxBytes*NBits; b++, DstI++, SrcI++) { *DstI |= *SrcI; }
}

// Average least zero bit position (least significant zero)
template <class PGraph>
double TGraphAnf<PGraph>::AvgLstZero(const TAnfBitV& BitV, const uint64& NIdOffset) const { //average least zero bit position (least significant zero)
  int64 approx, bit, AvgBitPos = 0;
  uchar* BitVPt = (uchar *) BitV.BegI();
  for (approx = 0; approx < NApprox; approx++) {
    for (bit = 0; bit < NBits; bit++) {
      if ((*(BitVPt+NIdOffset + ApproxBytes*bit + approx/8) & (1<<(approx%8))) == 0) break; } // found zero
    if (bit > NBits) bit = NBits;
    AvgBitPos += bit;
  }
  return AvgBitPos/double(NApprox) ;
}

template <class PGraph>
void TGraphAnf<PGraph>::GetNodeAnf(const int64& SrcNId, TIntFlt64KdV& DistNbrsV, const int64& MxDist, const bool& IsDir) {
  //const int NNodes = Graph->GetNodes();
  TAnfBitV CurBitsV, LastBitsV;
  InitAnfBits(CurBitsV);          IAssert(CurBitsV.BegI() != NULL);
  LastBitsV.Gen(CurBitsV.Len());  IAssert(LastBitsV.BegI() != NULL);
  DistNbrsV.Clr();
  DistNbrsV.Add(TIntFlt64Kd(1, Graph->GetNI(SrcNId).GetOutDeg()));
  for (int64 dist = 1; dist < (MxDist==-1 ? TInt64::Mx : MxDist); dist++) {
    memcpy(LastBitsV.BegI(), CurBitsV.BegI(), sizeof(uint64)*CurBitsV.Len()); //LastBitsV = CurBitsV;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      const uint64 NIdOffset = GetNIdOffset(NI.GetId());
      for (int64 e = 0; e < NI.GetOutDeg(); e++) {
        const uint64 NId2Offset = GetNIdOffset(NI.GetOutNId(e));
        Union(CurBitsV, NIdOffset,  LastBitsV, NId2Offset);
      }
      if (! IsDir) {
        for (int64 e = 0; e < NI.GetInDeg(); e++) {
          const uint64 NId2Offset = GetNIdOffset(NI.GetInNId(e));
          Union(CurBitsV, NIdOffset,  LastBitsV, NId2Offset);
        }
      }
    }
    DistNbrsV.Add(TIntFlt64Kd(dist, GetCount(CurBitsV, GetNIdOffset(SrcNId))));
    if (DistNbrsV.Len() > 1 && DistNbrsV.Last().Dat < 1.001*DistNbrsV[DistNbrsV.Len()-2].Dat) break; // 0.1%  change
  }
}

template <class PGraph>
void TGraphAnf<PGraph>::GetGraphAnf(TIntFlt64KdV& DistNbrsV, const int64& MxDist, const bool& IsDir) {
  TAnfBitV CurBitsV, LastBitsV;
  InitAnfBits(CurBitsV);          IAssert(CurBitsV.BegI() != NULL);
  LastBitsV.Gen(CurBitsV.Len());  IAssert(LastBitsV.BegI() != NULL);
  int64 v, e;
  double NPairs = 0.0;
  DistNbrsV.Clr();
  DistNbrsV.Add(TIntFlt64Kd(0, Graph->GetNodes()));
  //TExeTm ExeTm;
  for (int64 dist = 1; dist < (MxDist==-1 ? TInt64::Mx : MxDist); dist++) {
    //printf("ANF dist %d...", dist);  ExeTm.Tick();
    memcpy(LastBitsV.BegI(), CurBitsV.BegI(), sizeof(uint64)*CurBitsV.Len()); //LastBitsV = CurBitsV;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      const uint64 NIdOffset = GetNIdOffset(NI.GetId());
      for (e = 0; e < NI.GetOutDeg(); e++) {
        const uint64 NId2Offset = GetNIdOffset(NI.GetOutNId(e));
        Union(CurBitsV, NIdOffset,  LastBitsV, NId2Offset);
      }
      if (! IsDir) {
        for (e = 0; e < NI.GetInDeg(); e++) {
          const uint64 NId2Offset = GetNIdOffset(NI.GetInNId(e));
          Union(CurBitsV, NIdOffset,  LastBitsV, NId2Offset);
        }
      }
    }
    NPairs = 0.0;
    for (v = NIdToBitPosH.FFirstKeyId(); NIdToBitPosH.FNextKeyId(v); ) {
      NPairs += GetCount(CurBitsV, NIdToBitPosH[v]);
    }
    DistNbrsV.Add(TIntFlt64Kd(dist, NPairs));
    //printf("pairs: %g  %s\n", NPairs, ExeTm.GetTmStr());
    if (NPairs == 0) { break; }
    if (DistNbrsV.Len() > 1 && NPairs < 1.001*DistNbrsV.LastLast().Dat) { break; } // 0.1%  change
    //TGnuPlot::SaveTs(DistNbrsV, "hops.tab", "HOPS, REACHABLE PAIRS");
  }
}
/////////////////////////////////////////////////
// Approximate Neighborhood Function
namespace TSnap {

namespace TSnapDetail {
/// Helper function for computing a given Percentile of a (unnormalized) cumulative distribution function.
double CalcEffDiam(const TIntFlt64KdV& DistNbrsCdfV, const double& Percentile=0.9);
/// Helper function for computing a given Percentile of a (unnormalized) cumulative distribution function.
double CalcEffDiam(const TFlt64PrV& DistNbrsCdfV, const double& Percentile=0.9);
/// Helper function for computing a given Percentile of a (unnormalized) probability distribution function.
double CalcEffDiamPdf(const TIntFlt64KdV& DistNbrsPdfV, const double& Percentile=0.9);
/// Helper function for computing a given Percentile of a (unnormalized) probability distribution function.
double CalcEffDiamPdf(const TFlt64PrV& DistNbrsPdfV, const double& Percentile=0.9);
/// Helper function for computing the mean of a (unnormalized) probability distribution function.
double CalcAvgDiamPdf(const TIntFlt64KdV& DistNbrsPdfV);
/// Helper function for computing the mean of a (unnormalized) probability distribution function.
double CalcAvgDiamPdf(const TFlt64PrV& DistNbrsPdfV);
} // TSnapDetail

template <class PGraph>
void GetAnf(const PGraph& Graph, const int64& SrcNId, TIntFlt64KdV& DistNbrsV, const int64& MxDist, const bool& IsDir, const int64& NApprox) {
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetNodeAnf(SrcNId, DistNbrsV, MxDist, IsDir);
}

template <class PGraph>
void GetAnf(const PGraph& Graph, TIntFlt64KdV& DistNbrsV, const int64& MxDist, const bool& IsDir, const int64& NApprox) {
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetGraphAnf(DistNbrsV, MxDist, IsDir);
}

template <class PGraph>
double GetAnfEffDiam(const PGraph& Graph, const bool& IsDir, const double& Percentile, const int64& NApprox) {
  TIntFlt64KdV DistNbrsV;
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetGraphAnf(DistNbrsV, -1, IsDir);
  return TSnap::TSnapDetail::CalcEffDiam(DistNbrsV, Percentile);
}

template<class PGraph>
double GetAnfEffDiam(const PGraph& Graph, const int64 NRuns, int64 NApprox) {
  //return TSnap::GetEffDiam(Graph, IsDir, 0.9, 32);
  TMom Mom;
  if (NApprox == -1) {
    if (Graph->GetNodes() < 100000) { NApprox = 64; }
    else if (Graph->GetNodes() < 1000000) { NApprox = 32; }
    else { NApprox = 16; }
  }
  const bool IsDir = false;
  for (int64 r = 0; r < NRuns; r++) {
    Mom.Add(TSnap::GetAnfEffDiam(Graph, IsDir, 0.9, NApprox));
  }
  Mom.Def();
  return Mom.GetMean();
}

template <class PGraph> void TestAnf() {
  PGraph Graph = PGraph::TObj::New();
  //Graph:
  //  0    2 ----> 3
  //       ^       |
  //       |       |
  //       |       ^
  //  1    5 <---- 4
  for (int64 v = 0; v < 6; v++) { Graph->AddNode(v); }
  Graph->AddEdge(2, 3);
  Graph->AddEdge(3, 4);
  Graph->AddEdge(4, 5);
  Graph->AddEdge(5, 2);
  TFlt64V AnfV;
  for (int64 t = 0; t < 10; t++) {
    TGraphAnf<PGraph> Anf(Graph, 128, 5, t+1);
    TIntFlt64KdV DistToNbrsV;
    Anf.GetGraphAnf(DistToNbrsV, 5, true);
    printf("\n--seed: %d---------------------\n", t+1);
    for (int64 i = 0; i < DistToNbrsV.Len(); i++) {
      printf("dist: %d\t hops:%f\n", DistToNbrsV[i].Key(), DistToNbrsV[i].Dat());
    }
    AnfV.Add(DistToNbrsV.Last().Dat);
  }
  TMom Mom(AnfV);
  printf("-----------\nAvgAnf: %f  StDev:  %f\n", Mom.GetMean(), Mom.GetSDev());//*/
  // const int NApprox = 32;
  /*printf("\nANF vs. SAMPLE diam test (10 runs of ANF, NApprox=%d):\n", NApprox);
  //Graph = TGGen<PGraph>::GenGrid(20, 20);
  Graph = TGAlg::GetMxWcc(TGGen<PGraph>::GenRnd(1000, 10000));
  TFltV FullAnf, EffAnf;
  for (int tryn = 0; tryn < 10; tryn++) {
    FullAnf.Add(GetEffDiam(Graph, false, 1.0, NApprox));
    EffAnf.Add(GetEffDiam(Graph, false, 0.9, NApprox));
  }
  TMom FullMom(FullAnf);
  TMom AnfMom(EffAnf);
  printf("  Sample FullDiam:      %d\n", TGAlg::GetBfsFullDiam(Graph, 100, false));
  printf("  Anf    FullDiam:      %f  [%f]\n", FullMom.GetMean(), FullMom.GetSDev());
  printf("  Sample EffDiam [90%%]: %f\n", TGAlg::GetBfsEffDiam(Graph, 100, false));
  printf("  Anf    EffDiam [90%%]: %f  [%f]\n", AnfMom.GetMean(), AnfMom.GetSDev());
  // epinions
  printf("\nEpinions graph:\n");
  { typedef PNGraph PGraph;
  PGraph G = TGGen<PGraph>::GenEpinions();
  TIntFltKdV DistToPairsV;
  GetAnf(G, DistToPairsV, 50, true);
  for(int i = 0; i < DistToPairsV.Len(); i++) {
    printf("\t%d\t%f\n", DistToPairsV[i].Key, DistToPairsV[i].Dat); }
  printf("\nUndir\n");
  TAnf<PGraph>::GetAnf(G, DistToPairsV, 50, false);
  for(int j = 0; j < DistToPairsV.Len(); j++) {
    printf("\t%d\t%f\n", DistToPairsV[j].Key, DistToPairsV[j].Dat); }
  }//*/
}

} // namespace TSnap
