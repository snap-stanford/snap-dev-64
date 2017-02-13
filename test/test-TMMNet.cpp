#include <gtest/gtest.h>

#include "Snap.h"

TEST(TMMNet, DefaultConstructor) {
  PMMNet Net;
  Net = TMMNet::New();
}

TEST(TMMNet, AddModeNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
}

TEST(TMMNet, AddCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName2);
  TStr LinkName("TestLink");
  Net->AddCrossNet(ModeName1, ModeName2, LinkName);
}

TEST(TMMNet, GetModeId) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName2);
  TInt64 ModeId1(0);
  TInt64 ModeId2(1);
  EXPECT_EQ(ModeId1, Net->GetModeId(ModeName1));
  EXPECT_EQ(ModeId2, Net->GetModeId(ModeName2));
}

TEST(TMMNet, GetModeName) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName2);
  TInt64 ModeId1(0);
  TInt64 ModeId2(1);
  EXPECT_EQ(ModeName1, Net->GetModeName(ModeId1));
  EXPECT_EQ(ModeName2, Net->GetModeName(ModeId2));
}

TEST(TMMNet, GetCrossId) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName2);
  TInt64 LinkId1(0);
  TInt64 LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddCrossNet(ModeName1, ModeName2, LinkName1);
  TStr LinkName2("TestLink2");
  Net->AddCrossNet(ModeName1, ModeName2, LinkName2);
  EXPECT_EQ(LinkId1, Net->GetCrossId(LinkName1));
  EXPECT_EQ(LinkId2, Net->GetCrossId(LinkName2));
}

TEST(TMMNet, GetCrossName) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName2);
  TInt64 LinkId1(0);
  TInt64 LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddCrossNet(ModeName1, ModeName2, LinkName1);
  TStr LinkName2("TestLink2");
  Net->AddCrossNet(ModeName1, ModeName2, LinkName2);
  EXPECT_EQ(LinkName1, Net->GetCrossName(LinkId1));
  EXPECT_EQ(LinkName2, Net->GetCrossName(LinkId2));
}

TEST(TMMNet, GetModeNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TInt64 ModeId1(0);
  Net->AddModeNet(ModeName1);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetById(ModeId1);
  EXPECT_EQ(0, M2.GetNodes());
  M1.AddNode(0);
  EXPECT_EQ(1, M2.GetNodes());
}

TEST(TMMNet, GetCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName2);
  TInt64 LinkId1(0);
  TInt64 LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddCrossNet(ModeName1, ModeName2, LinkName1);
  TCrossNet& C1 = Net->GetCrossNetByName(LinkName1);
  TCrossNet& C2 = Net->GetCrossNetById(LinkId1);
  EXPECT_EQ(&C1, &C2);
}

TEST(TMMNet, GetBipartiteGraph) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName2);
  TInt64 LinkId1(0);
  TInt64 LinkId2(1);
  TStr LinkName1("TestLink1");
  Net->AddCrossNet(ModeName1, ModeName2, LinkName1);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C = Net->GetCrossNetByName(LinkName1);
  M1.AddNode(1);
  M1.AddNode(2);
  M2.AddNode(3);
  M2.AddNode(4);
  C.AddEdge(1,3);
  C.AddEdge(1,4);
  C.AddEdge(2,4);
  PBPGraph G = C.GetBipartiteGraph();
//	PBPGraph G = TBPGraph::New();
//	G->AddNode(1,true);
//	G->AddNode(2,true);
//	G->AddNode(3,false);
//	G->AddNode(4,false);
//	G->AddEdge(1,3);
//	G->AddEdge(1,4);
//	G->AddEdge(2,4);
  int64 s = 0;
  TBPGraph::TNodeI NI = G->BegNI();
  for(; NI < G->EndNI(); NI++){
  	//printf("%d %d \n",NI.GetId(), NI.GetDeg());
  	s += NI.GetDeg();
  }
  EXPECT_EQ(s, 3);
  G->SetIteratorSideLeft(false);
  //printf("%d ", G->IsIteratorSideLeft());
  s = 0;
  for(NI = G->BegNI(); NI < G->EndNI(); NI++){
    	//printf("%d %d \n",NI.GetId(), NI.GetDeg());
    	s += NI.GetDeg();
    }
  EXPECT_EQ(s, 3);
}

TEST(TMMNet, BegModeNetI) {
  PMMNet Net;
    Net = TMMNet::New();
    for(int64 i = 0; i < 10; i++) {
      TInt64 Id(i);
      Net->AddModeNet(Id.GetStr());
    }
    TMMNet::TModeNetI MI = Net->BegModeNetI();
    EXPECT_EQ(0, MI.GetModeId());
    TModeNet& M0 = MI.GetModeNet();
    TModeNet& M0Exp = Net->GetModeNetByName(TStr("0"));
    EXPECT_EQ(&M0Exp, &M0);
}

TEST(TMMNet, GetModeNetI) {
  PMMNet Net;
    Net = TMMNet::New();
    for(int64 i = 0; i < 10; i++) {
      TInt64 Id(i);
      Net->AddModeNet(Id.GetStr());
    }
    for(int64 i = 0; i < 10; i++) {
      TMMNet::TModeNetI MI = Net->GetModeNetI(i);
      EXPECT_EQ(i, MI.GetModeId());
    }
}

TEST(TMMNet, EndModeNetI) {
  PMMNet Net;
    Net = TMMNet::New();
    for(int64 i = 0; i < 10; i++) {
      TInt64 Id(i);
      Net->AddModeNet(Id.GetStr());
    }
    TMMNet::TModeNetI MI = Net->EndModeNetI();
    for(int64 i = 0; i < 10; i++) {
      TMMNet::TModeNetI MOther = Net->GetModeNetI(i);
      EXPECT_NE(MI, MOther);
    }
}

TEST(TMMNet, ModeNetI) {
  PMMNet Net;
    Net = TMMNet::New();
    for(int64 i = 0; i < 10; i++) {
      TInt64 Id(i);
      Net->AddModeNet(Id.GetStr());
    }
    int64 i = 0;
    TMMNet::TModeNetI MI;
    for(i = 0, MI = Net->BegModeNetI(); i < 10 && MI < Net->EndModeNetI(); i++, MI++) {
      EXPECT_EQ(i, MI.GetModeId());
    }
    EXPECT_EQ(10, i);
}

TEST(TMMNet, BegCrossNetI) {
  PMMNet Net;
    Net = TMMNet::New();
    Net->AddModeNet("0");
    for(int64 i = 1; i < 11; i++) {
      TInt64 MPrev(i-1);
      TInt64 MId(i);
      TInt64 LId(i-1);
      Net->AddModeNet(MId.GetStr());
      Net->AddModeNet(MPrev.GetStr());
      Net->AddCrossNet(MPrev, MId, LId.GetStr());
    }
    TMMNet::TCrossNetI CI = Net->BegCrossNetI();
    EXPECT_EQ(0, CI.GetCrossId());
    TCrossNet& C0 = CI.GetCrossNet();
    TCrossNet& C0Exp = Net->GetCrossNetByName(TStr("0"));
    EXPECT_EQ(&C0Exp, &C0);
}

TEST(TMMNet, GetCrossNetI) {
  PMMNet Net;
    Net = TMMNet::New();
    Net->AddModeNet("0");
    for(int64 i = 1; i < 11; i++) {
      TInt64 MPrev(i-1);
      TInt64 MId(i);
      TInt64 LId(i-1);
      Net->AddModeNet(MId.GetStr());
      Net->AddModeNet(MPrev.GetStr());
      Net->AddCrossNet(MPrev, MId, LId.GetStr());
    }
    for(int64 i = 0; i < 10; i++) {
      TMMNet::TCrossNetI CI = Net->GetCrossNetI(i);
      EXPECT_EQ(i, CI.GetCrossId());
    }
}

TEST(TMMNet, EndCrossNetI) {
  PMMNet Net;
    Net = TMMNet::New();
    Net->AddModeNet("0");
    for(int64 i = 1; i < 11; i++) {
      TInt64 MPrev(i-1);
      TInt64 MId(i);
      TInt64 LId(i-1);
      Net->AddModeNet(MId.GetStr());
      Net->AddModeNet(MPrev.GetStr());
      Net->AddCrossNet(MPrev, MId, LId.GetStr());
    }
    TMMNet::TCrossNetI CI = Net->EndCrossNetI();
    for(int64 i = 0; i < 10; i++) {
      TMMNet::TCrossNetI COther = Net->GetCrossNetI(i);
      EXPECT_NE(CI, COther);
    }
}

TEST(TMMNet, CrossNetI) {
  PMMNet Net;
    Net = TMMNet::New();
    Net->AddModeNet("0");
    for(int64 i = 1; i < 11; i++) {
      TInt64 MPrev(i-1);
      TInt64 MId(i);
      TInt64 LId(i-1);
      Net->AddModeNet(MId.GetStr());
      Net->AddModeNet(MPrev.GetStr());
      Net->AddCrossNet(MPrev, MId, LId.GetStr());
    }
    int i = 0;
    TMMNet::TCrossNetI CI;
    for(i = 0, CI = Net->BegCrossNetI(); i < 10 && CI < Net->EndCrossNetI(); i++, CI++) {
      EXPECT_EQ(i, CI.GetCrossId());
    }
    EXPECT_EQ(10, i);
}
