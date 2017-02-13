/////////////////////////////////////////////////
// Approximate Neighborhood Function
namespace TSnap {
namespace TSnapDetail {

// interpolate effective diameter
double CalcEffDiam(const TIntFltKd64V& DistNbrsCdfV, const double& Percentile) {
  const double EffPairs = Percentile * DistNbrsCdfV.Last().Dat;
  int64 ValN;
  for (ValN = 0; ValN < DistNbrsCdfV.Len(); ValN++) {
    if (DistNbrsCdfV[ValN].Dat() > EffPairs) {  break; }
  }
  if (ValN >= DistNbrsCdfV.Len()) return DistNbrsCdfV.Last().Key;
  if (ValN == 0) return 1;
  // interpolate
  const double DeltaNbrs = DistNbrsCdfV[ValN].Dat - DistNbrsCdfV[ValN-1].Dat;
  if (DeltaNbrs == 0) return DistNbrsCdfV[ValN].Key;
  return DistNbrsCdfV[ValN-1].Key + (EffPairs - DistNbrsCdfV[ValN-1].Dat)/DeltaNbrs;
}

double CalcEffDiam(const TFltPr64V& DistNbrsCdfV, const double& Percentile) {
  TIntFltKd64V KdV(DistNbrsCdfV.Len(), 0);
  for (int64 i = 0; i < DistNbrsCdfV.Len(); i++) {
    KdV.Add(TIntFlt64Kd(int64(DistNbrsCdfV[i].Val1()), DistNbrsCdfV[i].Val2));
  }
  return CalcEffDiam(KdV, Percentile);
}

double CalcEffDiamPdf(const TIntFltKd64V& DistNbrsPdfV, const double& Percentile) {
  TIntFltKd64V CdfV;
  TGUtil::GetCdf(DistNbrsPdfV, CdfV);
  return CalcEffDiam(CdfV, Percentile);
}

double CalcEffDiamPdf(const TFltPr64V& DistNbrsPdfV, const double& Percentile) {
  TFltPr64V CdfV;
  TGUtil::GetCdf(DistNbrsPdfV, CdfV);
  return CalcEffDiam(CdfV, Percentile);
}

double CalcAvgDiamPdf(const TIntFltKd64V& DistNbrsPdfV) {
  double Paths=0, SumLen=0;
  for (int64 i = 0; i < DistNbrsPdfV.Len(); i++) {
    SumLen += DistNbrsPdfV[i].Key * DistNbrsPdfV[i].Dat;
    Paths += DistNbrsPdfV[i].Dat;
  }
  return SumLen/Paths;
}

double CalcAvgDiamPdf(const TFltPr64V& DistNbrsPdfV) {
  double Paths=0, SumLen=0;
  for (int64 i = 0; i < DistNbrsPdfV.Len(); i++) {
    SumLen += DistNbrsPdfV[i].Val1 * DistNbrsPdfV[i].Val2;
    Paths += DistNbrsPdfV[i].Val2;
  }
  return SumLen/Paths;
}

} // namespace TSnapDetail
} // namespace TSnap

