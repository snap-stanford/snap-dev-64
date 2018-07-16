//#//////////////////////////////////////////////
// Graph Statistics
// statistics of a single snapshot of a graph
class TGStat;
typedef TPt<TGStat> PGStat;
typedef TVec<PGStat> TGStatV;

// statistics of a sequence of graph snapshots
class TGStatVec;
typedef TPt<TGStatVec> PGStatVec;

//#//////////////////////////////////////////////
// Statistics of a Sigle Graph
// Scalar statistics of the graph
typedef enum TGStatVal_ {
  gsvNone, gsvIndex, gsvTime, gsvNodes, gsvZeroNodes, gsvNonZNodes, gsvSrcNodes, gsvDstNodes,
  gsvEdges, gsvUniqEdges, gsvBiDirEdges,
  gsvWccNodes, gsvWccSrcNodes, gsvWccDstNodes, gsvWccEdges, gsvWccUniqEdges, gsvWccBiDirEdges,
  gsvSccNodes, gsvSccEdges,gsvBccNodes, gsvBccEdges,
  gsvFullDiam, gsvEffDiam, gsvEffWccDiam, gsvFullWccDiam,
  gsvFullDiamDev, gsvEffDiamDev, gsvEffWccDiamDev, gsvFullWccDiamDev, // diameter+variance
  gsvClustCf, gsvOpenTriads, gsvClosedTriads, gsvWccSize, gsvSccSize, gsvBccSize,
  gsvMx
} TGStatVal;

// Distribution statistics of the graph
typedef enum TGStatDistr_ {
  gsdUndef=100, gsdInDeg, gsdOutDeg, gsdWcc, gsdScc,
  gsdHops, gsdWccHops, gsdSngVal, gsdSngVec, gsdClustCf,
  gsdTriadPart, // triad participation
  gsdMx,
} TGStatDistr;

//#//////////////////////////////////////////////
/// Statistics of a Graph Snapshot
class TGStat {
public:
  static int64 NDiamRuns;
  static int64 TakeSngVals;
  typedef TQuad<TStr, TStr, TStr, TGpScaleTy> TPlotInfo; // file prefix, x label, y label, scale
public:
  class TCmpByVal {
  private:
    TGStatVal ValCmp;
    bool SortAsc;
  public:
    TCmpByVal(TGStatVal SortBy, bool Asc) : ValCmp(SortBy), SortAsc(Asc) { }
    bool operator () (const TGStat& GS1, const TGStat& GS2) const;
    bool operator () (const PGStat& GS1, const PGStat& GS2) const;
  };
private:
  static const TFltPr64V EmptyV;
  TCRef CRef;
public:
  TSecTm Time;
  TStr GraphNm;
  TIntFlt64H ValStatH; // scalar statistics
  THash<TInt64, TFltPr64V, int64> DistrStatH; // distribution statistics
public:
  TGStat(const TSecTm& GraphTm = TSecTm(), const TStr& GraphName=TStr());
  TGStat(const PNGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(), const TStr& GraphName=TStr());
  TGStat(const PUNGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(), const TStr& GraphName=TStr());
  TGStat(const PNEGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(), const TStr& GraphName=TStr());
  template <class PGraph> TGStat(const PGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(), const TStr& GraphName=TStr()) {
    TakeStat(Graph, Time, StatFSet, GraphName); }
  TGStat(const TGStat& GStat);
  TGStat(TSIn& SIn);
  void Save(TSOut& SOut) const;
  static PGStat New(const TSecTm& Time=TSecTm(), const TStr& GraphName=TStr()) {
    return new TGStat(Time, GraphName); }
  static PGStat New(const PNGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(),
    const TStr& GraphNm=TStr()) { return new TGStat(Graph, Time, StatFSet, GraphNm); }
  static PGStat New(const PUNGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(),
    const TStr& GraphNm=TStr()) { return new TGStat(Graph, Time, StatFSet, GraphNm); }
  static PGStat New(const PNEGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(),
    const TStr& GraphNm=TStr()) { return new TGStat(Graph, Time, StatFSet, GraphNm); }
  template <class PGraph> PGStat New(const PGraph& Graph, const TSecTm& Time, TFSet StatFSet=TFSet(),
    const TStr& GraphNm=TStr()) { return new TGStat(Graph, Time, StatFSet, GraphNm); }
  static PGStat Load(TSIn& SIn) { return new TGStat(SIn); }
  PGStat Clone() const { return new TGStat(*this); }
  TGStat& operator = (const TGStat& GStat);
  bool operator == (const TGStat& GStat) const;
  bool operator < (const TGStat& GStat) const;

  int GetYear() const { return Time.GetYearN(); }
  int GetMonth() const { return Time.GetMonthN(); }
  int GetDay() const { return Time.GetDayN(); }
  int GetHour() const { return Time.GetHourN(); }
  int GetMin() const { return Time.GetMinN(); }
  int GetSec() const { return Time.GetSecN(); }
  TStr GetTmStr() const { return Time.GetStr(); }
  void SetTm(const TSecTm& GraphTm) { Time = GraphTm; }
  TStr GetNm() const { return GraphNm; }
  void SetNm(const TStr& GraphName) { GraphNm=GraphName; }
  int GetTime(const TTmUnit& TimeUnit) const { return Time.GetInUnits(TimeUnit); }

  int64 GetVals() const { return ValStatH.Len(); }
  bool HasVal(const TGStatVal& StatVal) const;
  double GetVal(const TGStatVal& StatVal) const;
  void SetVal(const TGStatVal& StatVal, const double& Val);
  int64 GetDistrs() const { return DistrStatH.Len(); }
  bool HasDistr(const TGStatDistr& Distr) const { return DistrStatH.IsKey(Distr); }
  const TFltPr64V& GetDistr(const TGStatDistr& Distr) const;
  void GetDistr(const TGStatDistr& Distr, TFltPr64V& FltPrV) const;
  void SetDistr(const TGStatDistr& Distr, const TFltPr64V& FltPrV);

  int64 GetNodes() const { return (int64) GetVal(gsvNodes); }
  int64 GetEdges() const { return (int64) GetVal(gsvEdges); }

  void TakeStat(const PNGraph& Graph, const TSecTm& Time, TFSet StatFSet, const TStr& GraphName);
  void TakeStat(const PUNGraph& Graph, const TSecTm& Time, TFSet StatFSet, const TStr& GraphName);
  template <class PGraph> void TakeStat(const PGraph& Graph, const TSecTm& Time, TFSet StatFSet, const TStr& GraphName);
  template <class PGraph> void TakeBasicStat(const PGraph& Graph, const bool& IsMxWcc=false);
  template <class PGraph> void TakeBasicStat(const PGraph& Graph, TFSet FSet, const bool& IsMxWcc=false);
  template <class PGraph> void TakeSccStat(const PGraph& Graph, TFSet StatFSet);
  template <class PGraph> void TakeBccStat(const PGraph& Graph, TFSet StatFSet);
  template <class PGraph> void TakeDegDistr(const PGraph& Graph);
  template <class PGraph> void TakeDegDistr(const PGraph& Graph, TFSet StatFSet);
  template <class PGraph> void TakeDiam(const PGraph& Graph, const bool& IsMxWcc=false);
  template <class PGraph> void TakeDiam(const PGraph& Graph, TFSet StatFSet, const bool& IsMxWcc=false);
  template <class PGraph> void TakeConnComp(const PGraph& Graph);
  template <class PGraph> void TakeConnComp(const PGraph& Graph, TFSet StatFSet);
  template <class PGraph> void TakeClustCf(const PGraph& Graph, const int64& SampleNodes=-1);
  template <class PGraph> void TakeTriadPart(const PGraph& Graph);
  void TakeSpectral(const PNGraph& Graph, const int64 _TakeSngVals = -1);
  void TakeSpectral(const PNGraph& Graph, TFSet StatFSet, int64 _TakeSngVals = -1);

  void Plot(const TGStatDistr& Distr, const TStr& FNmPref, TStr Desc=TStr(), bool PowerFit=false) const;
  void Plot(const TFSet& FSet, const TStr& FNmPref, const TStr& Desc=TStr(), bool PowerFit=false) const;
  void PlotAll(const TStr& FNmPref, TStr Desc=TStr(), bool PowerFit=false) const;
  void DumpValStat();

  void AvgGStat(const PGStatVec& GStatVec, const bool& ClipAt1=false);
  void AvgGStat(const TGStatV& GStatV, const bool& ClipAt1=false);

  // take graph statistics (see TTakeGStat)
  static TStr GetDistrStr(const TGStatDistr& Distr);
  static TStr GetValStr(const TGStatVal& Val);
  static TPlotInfo GetPlotInfo(const TGStatVal& Val);
  static TPlotInfo GetPlotInfo(const TGStatDistr& Distr);
  static TFSet NoStat();
  static TFSet BasicStat();
  static TFSet DegDStat();
  static TFSet NoDiamStat();
  static TFSet NoDistrStat();
  static TFSet NoSvdStat();
  static TFSet AllStat();

  friend class TCmpByVal;
  friend class TPt<TGStat>;
};

//#//////////////////////////////////////////////
/// Graph Statistics Sequence
class TGStatVec {
public:
  static uint64 MinNodesEdges;
private:
  TCRef CRef;
  TTmUnit TmUnit;
  TFSet StatFSet;
  TGStatV GStatV; // each snapshot (TVec<PGStat>)
public:
  TGStatVec(const TTmUnit& _TmUnit=tmu1Sec);
  TGStatVec(const TTmUnit& _TmUnit, const TFSet& TakeGrowthStat);
  TGStatVec(const TGStatVec& GStat);
  static PGStatVec New(const TTmUnit& _TmUnit=tmu1Sec);
  static PGStatVec New(const TTmUnit& _TmUnit, const TFSet& TakeGrowthStat);
  static PGStatVec Load(TSIn& SIn) { return new TGStatVec(SIn); }
  TGStatVec(TSIn& SIn);
  void Save(TSOut& SOut) const;
  TGStatVec& operator = (const TGStatVec& GStat);

  PGStat Add();
  PGStat Add(const TSecTm& Time, const TStr& GraphNm=TStr());
  void Add(const PGStat& Growth) { GStatV.Add(Growth); }
  void Add(const PNGraph& Graph, const TSecTm& Time, const TStr& GraphNm=TStr());
  void Add(const PUNGraph& Graph, const TSecTm& Time, const TStr& GraphNm=TStr());
  void Add(const PNEGraph& Graph, const TSecTm& Time, const TStr& GraphNm=TStr());
  void Clr() { GStatV.Clr(); }
  void Sort(const TGStatVal& SortBy=gsvNodes, const bool& Asc=true);

  int64 Len() const { return GStatV.Len(); }
  bool Empty() const { return GStatV.Empty(); }
  PGStat operator[](const int64& ValN) const { return GStatV[ValN]; }
  PGStat At(const int64& ValN) const { return GStatV[ValN]; }
  PGStat Last() const { return GStatV.Last(); }
  const TGStatV& GetGStatV() const { return GStatV; }
  int GetTime(const int& ValN) const { return At(ValN)->GetTime(TmUnit); }

  void Del(const int64& ValN) { GStatV.Del(ValN); }
  void DelLast() { GStatV.DelLast(); }
  void DelBefore(const TSecTm& Tm);
  void DelAfter(const TSecTm& Tm);
  void DelSmallNodes(const int64& MinNodes);

  void SetTmUnit(const TTmUnit& TimeUnit) { TmUnit = TimeUnit; }
  TTmUnit GetTmUnit() const { return TmUnit; }
  void SetTakeStat(const TFSet& TakeStatSet) { StatFSet = TakeStatSet; }
  bool HasVal(const TGStatVal& Stat) const { return StatFSet.In(Stat); }
  bool HasDistr(const TGStatDistr& Stat) const { return StatFSet.In(Stat); }

  void GetValV(const TGStatVal& XVal, const TGStatVal& YVal, TFltPr64V& ValV) const;
  PGStat GetAvgGStat(const bool& ClipAt1=false);

  void Plot(const TGStatVal& XVal, const TGStatVal& YVal, const TStr& OutFNm, TStr& Desc,
    const TGpScaleTy& Scale=gpsAuto, const bool& PowerFit=false) const;
  void PlotAllVsX(const TGStatVal& XVal, const TStr& OutFNm, TStr Desc=TStr(), const TGpScaleTy& Scale=gpsAuto, const bool& PowerFit=false) const;
  void ImposeDistr(const TGStatDistr& Distr, const TStr& FNmPref, TStr Desc=TStr(), const bool& ExpBin=false,
    const bool& PowerFit=false, const TGpSeriesTy& PlotWith=gpwLinesPoints, const TStr& Style="") const;

  void SaveTxt(const TStr& FNmPref, const TStr& Desc) const;
  friend class TPt<TGStatVec>;
};

namespace TSnap {
/// Prints basic graph statistics. ##TSnap::PrintInfo
template <class PGraph> void PrintInfo(const PGraph& Graph, const TStr& Desc="", const TStr& OutFNm="", const bool& Fast=true);
}  // namespace TSnap

//#//////////////////////////////////////////////
// Implementation
template <class PGraph>
void TGStat::TakeStat(const PGraph& Graph, const TSecTm& _Time, TFSet StatFSet, const TStr& GraphName) {
  printf("**TakeStat:  G(%s, %s)\n", TInt64::GetStr(Graph->GetNodes()).CStr(), TInt64::GetStr(Graph->GetEdges()).CStr());
  TExeTm ExeTm, FullTm;
  Time = _Time;
  GraphNm = GraphName;
  if (StatFSet.In(gsvNone)) { return; }
  TakeBasicStat(Graph, false);
  TakeSccStat(Graph, StatFSet);
  TakeBccStat(Graph, StatFSet);
  if (StatFSet.In(gsdWcc)) {
    PGraph WccG = TSnap::GetMxWcc(Graph);
    TakeBasicStat(WccG, true);
    SetVal(gsvWccSize, WccG->GetNodes()/double(Graph->GetNodes()));
  }
  // degrees
  TakeDegDistr(Graph, StatFSet);
  if (StatFSet.In(gsvFullDiam) || StatFSet.In(gsvEffDiam) || StatFSet.In(gsdHops) ||
   StatFSet.In(gsvEffWccDiam) || StatFSet.In(gsdWccHops) || StatFSet.In(gsdWcc) || StatFSet.In(gsdScc) ||
   StatFSet.In(gsdClustCf) || StatFSet.In(gsvClustCf) || StatFSet.In(gsdTriadPart)) {
    PNGraph NGraph = TSnap::ConvertGraph<PNGraph>(Graph, true);
    // diameter
    TakeDiam(NGraph, StatFSet, false);
    // components
    TakeConnComp(NGraph, StatFSet);
    // spectral
    TakeSpectral(NGraph, StatFSet, -1);
    // clustering coeffient
    if (StatFSet.In(gsdClustCf) || StatFSet.In(gsvClustCf)) {
      TakeClustCf(NGraph); }
    if (StatFSet.In(gsdTriadPart)) {
      TakeTriadPart(NGraph); }
    if (StatFSet.In(gsvFullDiam) || StatFSet.In(gsvEffWccDiam)) {
      TakeDiam(TSnap::GetMxWcc(NGraph), StatFSet, true); }
    printf("**[%s]\n", FullTm.GetTmStr());
  }
}

template <class PGraph>
void TGStat::TakeBasicStat(const PGraph& Graph, const bool& IsMxWcc) {
  TakeBasicStat(Graph, TFSet() | gsvBiDirEdges | gsvWccBiDirEdges, IsMxWcc);
}

template <class PGraph>
void TGStat::TakeBasicStat(const PGraph& Graph, TFSet FSet, const bool& IsMxWcc) {
  TExeTm ExeTm;
  if (! IsMxWcc) {
    // gsvNodes, gsvZeroNodes, gsvNonZNodes, gsvSrcNodes, gsvDstNodes,
    // gsvEdges, gsvUniqEdges, gsvBiDirEdges
    printf("basic...");
    const int64 Nodes = Graph->GetNodes();
    SetVal(gsvNodes, Nodes);
    SetVal(gsvZeroNodes, TSnap::CntDegNodes(Graph, 0));
    SetVal(gsvNonZNodes, Nodes - GetVal(gsvZeroNodes));
    SetVal(gsvSrcNodes, Nodes - TSnap::CntOutDegNodes(Graph, 0));
    SetVal(gsvDstNodes, Nodes - TSnap::CntInDegNodes(Graph, 0));
    SetVal(gsvEdges, Graph->GetEdges());
    if (! Graph->HasFlag(gfMultiGraph)) { SetVal(gsvUniqEdges, Graph->GetEdges()); }
    else { SetVal(gsvUniqEdges, TSnap::CntUniqDirEdges(Graph)); }
    if (FSet.In(gsvBiDirEdges)) {
      if (Graph->HasFlag(gfDirected)) { SetVal(gsvBiDirEdges, TSnap::CntUniqBiDirEdges(Graph)); }
      else { SetVal(gsvUniqEdges, GetVal(gsvEdges)); }
    }
    printf("[%s] ", ExeTm.GetTmStr());
  } else {
    // gsvWccNodes, gsvWccSrcNodes, gsvWccDstNodes, gsvWccEdges, gsvWccUniqEdges, gsvWccBiDirEdges
    printf("basic wcc...");
    const int64 Nodes = Graph->GetNodes();
    SetVal(gsvWccNodes, Nodes);
    SetVal(gsvWccSrcNodes, Nodes - TSnap::CntOutDegNodes(Graph, 0));
    SetVal(gsvWccDstNodes, Nodes - TSnap::CntInDegNodes(Graph, 0));
    SetVal(gsvWccEdges, Graph->GetEdges());
    if (! Graph->HasFlag(gfMultiGraph)) { SetVal(gsvWccUniqEdges, Graph->GetEdges()); }
    else { SetVal(gsvWccUniqEdges, TSnap::CntUniqDirEdges(Graph)); }
    if (FSet.In(gsvBiDirEdges)) {
      if (Graph->HasFlag(gfDirected)) { SetVal(gsvWccBiDirEdges, TSnap::CntUniqBiDirEdges(Graph)); }
      else { SetVal(gsvUniqEdges, GetVal(gsvEdges)); }
    }
    printf("[%s]  ", ExeTm.GetTmStr());
  }
}

template <class PGraph>
void TGStat::TakeDegDistr(const PGraph& Graph) {
  TakeDegDistr(Graph, TFSet() | gsdInDeg | gsdOutDeg);
}

template <class PGraph>
void TGStat::TakeDegDistr(const PGraph& Graph, TFSet StatFSet) {
  TExeTm ExeTm;
  // degree distribution
  if (StatFSet.In(gsdOutDeg) || StatFSet.In(gsdOutDeg)) {
    printf("deg:"); }
  if (StatFSet.In(gsdInDeg)) {
    printf("-in");
    TFltPr64V& InDegV = DistrStatH.AddDat(gsdInDeg);
    TSnap::GetInDegCnt(Graph, InDegV);
  }
  if (StatFSet.In(gsdOutDeg)) {
    printf("-out");
    TFltPr64V& OutDegV = DistrStatH.AddDat(gsdOutDeg);
    TSnap::GetOutDegCnt(Graph, OutDegV);
  }
  if (StatFSet.In(gsdOutDeg) || StatFSet.In(gsdOutDeg)) {
    printf("[%s]  ", ExeTm.GetTmStr()); }
}

template <class PGraph>
void TGStat::TakeDiam(const PGraph& Graph, const bool& IsMxWcc) {
  TakeDiam(Graph, TFSet() | gsvFullDiam | gsvEffDiam | gsdHops |
    gsvEffWccDiam| gsdWccHops, IsMxWcc);
}

template <class PGraph>
void TGStat::TakeDiam(const PGraph& Graph, TFSet StatFSet, const bool& IsMxWcc) {
  TExeTm ExeTm;
  if (! IsMxWcc) {
    if (StatFSet.In(gsvFullDiam) || StatFSet.In(gsvEffDiam) || StatFSet.In(gsdHops)) {
      printf("anf:%sruns...", TInt64::GetStr(NDiamRuns).CStr()); }
    //bool Line=false;
    if (StatFSet.In(gsvEffDiam) || StatFSet.In(gsdHops)) {
      TMom DiamMom;  ExeTm.Tick();
      TIntFltKd64V DistNbrsV;
      for (int64 r = 0; r < NDiamRuns; r++) {
        TSnap::GetAnf(Graph, DistNbrsV, -1, false, 32);
        DiamMom.Add(TSnap::TSnapDetail::CalcEffDiam(DistNbrsV, 0.9));
        printf(".");
      }
      DiamMom.Def();
      SetVal(gsvEffDiam, DiamMom.GetMean());
      SetVal(gsvEffDiamDev, DiamMom.GetSDev());
      TFltPr64V& HopsV = DistrStatH.AddDat(gsdHops);
      HopsV.Gen(DistNbrsV.Len(), 0);
      for (int64 i = 0; i < DistNbrsV.Len(); i++) {
        HopsV.Add(TFltPr(DistNbrsV[i].Key(), DistNbrsV[i].Dat)); }
      printf("  anf-eff %.1f[%s]", DiamMom.GetMean(), ExeTm.GetTmStr());
      //Line=true;
    }
  } else {
    if (StatFSet.In(gsvEffWccDiam) || StatFSet.In(gsdWccHops)) { printf("wcc diam..."); }
    //bool Line=false;
    if (StatFSet.In(gsvFullDiam)) {
      TMom DiamMom;  ExeTm.Tick();
      for (int64 r = 0; r < NDiamRuns; r++) {
        DiamMom.Add(TSnap::GetBfsFullDiam(Graph, 1, false));
        printf("."); }
      DiamMom.Def();
      SetVal(gsvFullDiam, DiamMom.GetMean());
      SetVal(gsvFullDiamDev, DiamMom.GetSDev());
      printf("  bfs-full %g[%s]", DiamMom.GetMean(), ExeTm.GetTmStr());
      //Line=true;
    }
    if (StatFSet.In(gsvEffWccDiam) || StatFSet.In(gsdWccHops)) {
      TMom DiamMom; ExeTm.Tick();
      TIntFltKd64V DistNbrsV;
      for (int64 r = 0; r < NDiamRuns; r++) {
        TSnap::GetAnf(Graph, DistNbrsV, -1, false, 32);
        DiamMom.Add(TSnap::TSnapDetail::CalcEffDiam(DistNbrsV, 0.9));
        printf(".");
      }
      DiamMom.Def();
      SetVal(gsvEffWccDiam, DiamMom.GetMean());
      SetVal(gsvEffWccDiamDev, DiamMom.GetSDev());
      TFltPr64V& WccHopsV = DistrStatH.AddDat(gsdWccHops);
      WccHopsV.Gen(DistNbrsV.Len(), 0);
      for (int64 i = 0; i < DistNbrsV.Len(); i++) {
        WccHopsV.Add(TFltPr(DistNbrsV[i].Key(), DistNbrsV[i].Dat)); }
      printf("  anf-wcceff %.1f[%s]", DiamMom.GetMean(), ExeTm.GetTmStr());
      //Line=true;
    }
  }
}

template <class PGraph>
void TGStat::TakeConnComp(const PGraph& Graph) {
  TakeConnComp(Graph, TFSet() | gsdWcc | gsdScc);
}

template <class PGraph>
void TGStat::TakeConnComp(const PGraph& Graph, TFSet StatFSet) {
  TExeTm ExeTm;
  if (StatFSet.In(gsdWcc)) {
    printf("wcc...");
    TIntPr64V WccSzCntV1;
    TSnap::GetWccSzCnt(Graph, WccSzCntV1);
    TFltPr64V& WccSzCntV = DistrStatH.AddDat(gsdWcc);
    WccSzCntV.Gen(WccSzCntV1.Len(), 0);
    for (int64 i = 0; i < WccSzCntV1.Len(); i++)
      WccSzCntV.Add(TFltPr(WccSzCntV1[i].Val1(), WccSzCntV1[i].Val2()));
  }
  if (StatFSet.In(gsdScc)) {
    printf("scc...");
    TIntPr64V SccSzCntV1;
    TSnap::GetSccSzCnt(Graph, SccSzCntV1);
    TFltPr64V& SccSzCntV = DistrStatH.AddDat(gsdScc);
    SccSzCntV.Gen(SccSzCntV1.Len(), 0);
    for (int64 i = 0; i < SccSzCntV1.Len(); i++)
      SccSzCntV.Add(TFltPr(SccSzCntV1[i].Val1(), SccSzCntV1[i].Val2()));
  }
  if (StatFSet.In(gsdWcc) || StatFSet.In(gsdScc)) { printf("[%s]  ", ExeTm.GetTmStr()); }
}

template <class PGraph>
void TGStat::TakeSccStat(const PGraph& Graph, TFSet StatFSet) {
  TExeTm ExeTm;
  if (StatFSet.In(gsvSccNodes) || StatFSet.In(gsvSccEdges) || StatFSet.In(gsvSccSize)) {
    printf("scc...");
    PGraph SccG = TSnap::GetMxScc(Graph);
    SetVal(gsvSccNodes, SccG->GetNodes());
    SetVal(gsvSccEdges, SccG->GetEdges());
    SetVal(gsvSccSize, SccG->GetNodes()/double(Graph->GetNodes()));
    printf("[%s]  ", ExeTm.GetTmStr());
  }
}

template <class PGraph>
void TGStat::TakeBccStat(const PGraph& Graph, TFSet StatFSet) {
  TExeTm ExeTm;
  if (StatFSet.In(gsvBccNodes) || StatFSet.In(gsvBccEdges) || StatFSet.In(gsvBccSize)) {
    printf("bcc...");
    PGraph BccG = TSnap::GetMxBiCon(Graph);
    SetVal(gsvBccNodes, BccG->GetNodes());
    SetVal(gsvBccEdges, BccG->GetEdges());
    SetVal(gsvBccSize, BccG->GetNodes()/double(Graph->GetNodes()));
    printf("[%s]  ", ExeTm.GetTmStr());
  }
}

template <class PGraph>
void TGStat::TakeClustCf(const PGraph& Graph, const int64& SampleNodes) {
  TExeTm ExeTm;
  printf("clustcf...");
  TFltPr64V& ClustCfV = DistrStatH.AddDat(gsdClustCf);
  int64 Open, Close;
  const double ClustCf =  TSnap::GetClustCf(Graph, ClustCfV, Close, Open, SampleNodes);
  SetVal(gsvClustCf, ClustCf);
  SetVal(gsvOpenTriads, static_cast<double>(Open));
  SetVal(gsvClosedTriads, static_cast<double>(Close));
  printf("[%s]  ", ExeTm.GetTmStr());
}

template <class PGraph>
void TGStat::TakeTriadPart(const PGraph& Graph) {
  TExeTm ExeTm;
  printf("triadparticip...");
  TFltPr64V& TriadCntV = DistrStatH.AddDat(gsdTriadPart);
  TIntPr64V CntV;
  TSnap::GetTriadParticip(Graph, CntV);
  TriadCntV.Gen(CntV.Len(), 0);
  for (int64 i = 0; i < CntV.Len(); i++) {
    TriadCntV.Add(TFltPr(CntV[i].Val1(), CntV[i].Val2()));
  }
  printf("[%s]  ", ExeTm.GetTmStr());
}

namespace TSnap {

template <class PGraph>
void PrintInfo(const PGraph& Graph, const TStr& Desc, const TStr& OutFNm, const bool& Fast) {
  int64 BiDirEdges=0, ZeroNodes=0, ZeroInNodes=0, ZeroOutNodes=0, SelfEdges=0, NonZIODegNodes=0;
  THash<TInt64Pr, TInt64, int64> UniqDirE, UniqUnDirE;
  FILE *F = stdout;
  if (! OutFNm.Empty()) F = fopen(OutFNm.CStr(), "wt");
  if (! Desc.Empty()) { fprintf(F, "%s:", Desc.CStr()); }
  else { fprintf(F, "Graph:"); }
  for (int64 f = gfUndef; f < gfMx; f++) {
    if (HasGraphFlag(typename PGraph::TObj, TGraphFlag(f))) {
      fprintf(F, " %s", TSnap::GetFlagStr(TGraphFlag(f)).CStr()); }
  }
  // calc stat
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetDeg()==0) ZeroNodes++;
    if (NI.GetInDeg()==0) ZeroInNodes++;
    if (NI.GetOutDeg()==0) ZeroOutNodes++;
    if (NI.GetInDeg()!=0 && NI.GetOutDeg()!=0) NonZIODegNodes++;
    if (! Fast || Graph->GetNodes() < 1000) {
      const int64 NId = NI.GetId();
      for (int64 edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int64 DstNId = NI.GetOutNId(edge);
        if (Graph->IsEdge(DstNId, NId)) BiDirEdges++;
        if (NId == DstNId) SelfEdges++;
        UniqDirE.AddKey(TInt64Pr(NId, DstNId));
        UniqUnDirE.AddKey(TInt64Pr(TInt64::GetMn(NId, DstNId), TInt64::GetMx(NId, DstNId)));
      }
    }
  }
  int64 Closed=0, Open=0;
  double WccSz=0, SccSz=0;
  double EffDiam=0;
  int64 FullDiam=0;
  if (! Fast) {
    TSnap::GetTriads(Graph, Closed, Open);
    WccSz = TSnap::GetMxWccSz(Graph);
    SccSz = TSnap::GetMxSccSz(Graph);
    TSnap::GetBfsEffDiam(Graph, 100, false, EffDiam, FullDiam);
  }
  // print info
  fprintf(F, "\n");
  fprintf(F, "  Nodes:                    %s\n", TInt64::GetStr(Graph->GetNodes()).CStr());
  fprintf(F, "  Edges:                    %s\n", TInt64::GetStr(Graph->GetEdges()).CStr());
  fprintf(F, "  Zero Deg Nodes:           %s\n", TInt64::GetStr(ZeroNodes).CStr());
  fprintf(F, "  Zero InDeg Nodes:         %s\n", TInt64::GetStr(ZeroInNodes).CStr());
  fprintf(F, "  Zero OutDeg Nodes:        %s\n", TInt64::GetStr(ZeroOutNodes).CStr());
  fprintf(F, "  NonZero In-Out Deg Nodes: %s\n", TInt64::GetStr(NonZIODegNodes).CStr());

  if (! Fast) {
    fprintf(F, "  Unique directed edges:    %s\n", TInt64::GetStr(UniqDirE.Len()).CStr());
    fprintf(F, "  Unique undirected edges:  %s\n", TInt64::GetStr(UniqUnDirE.Len()).CStr());
    fprintf(F, "  Self Edges:               %s\n", TInt64::GetStr(SelfEdges).CStr());
    fprintf(F, "  BiDir Edges:              %s\n", TInt64::GetStr(BiDirEdges).CStr());
    fprintf(F, "  Closed triangles:         %s\n", TUInt64::GetStr(Closed).CStr());
    fprintf(F, "  Open triangles:           %s\n", TUInt64::GetStr(Open).CStr());
    fprintf(F, "  Frac. of closed triads:   %f\n", Closed/double(Closed+Open));
    fprintf(F, "  Connected component size: %f\n", WccSz);
    fprintf(F, "  Strong conn. comp. size:  %f\n", SccSz);
    fprintf(F, "  Approx. full diameter:    %s\n", TInt64::GetStr(FullDiam).CStr());
    fprintf(F, "  90%% effective diameter:  %f\n", EffDiam);
    //fprintf(F, "  Core\tNodes\tEdges\n");
    //for (int i  = 0; i < CNodesV.Len(); i++) {
    //  printf("  %d\t%d\t%d\n", CNodesV[i].Val1(), CNodesV[i].Val2(), CEdgesV[i].Val2());
    //}
  }
  if (! OutFNm.Empty()) { fclose(F); }
}

}  // namespace TSnap

