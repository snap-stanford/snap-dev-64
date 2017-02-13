#include <gtest/gtest.h>

#include "Snap.h"

int64 BuildCapacityNetwork(const TStr& InFNm, PNEANet &Net, const int64& SrcColId = 0, const int64& DstColId = 1, const int64& CapColId = 2) {
  TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
  TRnd Random;
  Net.Clr();
  Net = TNEANet::New();
  int64 SrcNId, DstNId, CapVal, EId;
  int64 MaxCap = 0;
  while (Ss.Next()) {
    if (! Ss.GetInt64(SrcColId, SrcNId) || ! Ss.GetInt64(DstColId, DstNId) || ! Ss.GetInt64(CapColId, CapVal)) { continue; }
    MaxCap = MAX(CapVal, MaxCap);
    if (! Net->IsNode(SrcNId)) {
      Net->AddNode(SrcNId);
    }
    if (! Net->IsNode(DstNId)) {
      Net->AddNode(DstNId);
    }
    EId = Net->AddEdge(SrcNId, DstNId);
    Net->AddIntAttrDatE(EId, CapVal, TSnap::CapAttrName);
  }
  return MaxCap;
}

TEST(FlowTest, BasicTest) {
  PNEANet Net;
  BuildCapacityNetwork("flow/small_sample.txt", Net);
  int64 PRFlow1 = TSnap::GetMaxFlowIntPR(Net, 53, 2);
  int64 EKFlow1 = TSnap::GetMaxFlowIntEK(Net, 53, 2);
  int64 PRFlow2 = TSnap::GetMaxFlowIntPR(Net, 86, 77);
  int64 EKFlow2 = TSnap::GetMaxFlowIntEK(Net, 86, 77);
  int64 PRFlow3 = TSnap::GetMaxFlowIntPR(Net, 62, 81);
  int64 EKFlow3 = TSnap::GetMaxFlowIntEK(Net, 62, 81);
  int64 PRFlow4 = TSnap::GetMaxFlowIntPR(Net, 92, 92);
  int64 EKFlow4 = TSnap::GetMaxFlowIntEK(Net, 92, 92);
  EXPECT_EQ (PRFlow1, EKFlow1);
  EXPECT_EQ (PRFlow2, EKFlow2);
  EXPECT_EQ (PRFlow3, EKFlow3);
  EXPECT_EQ (PRFlow4, EKFlow4);
  EXPECT_EQ (PRFlow1, 1735);
  EXPECT_EQ (PRFlow2, 3959);
  EXPECT_EQ (PRFlow3, 2074);
  EXPECT_EQ (PRFlow4, 0);
}
