#include <gtest/gtest.h>

#include "Snap.h"
#include <sstream>
#include <iostream> // TODO (millimat): remove

TEST(multimodal, AddNbrType) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  TStr64V M1Names;
  TStr64V M2Names;
  M1.GetCrossNetNames(M1Names);
  EXPECT_EQ(M1Names.Len(), 1);
  EXPECT_EQ(M1Names[0].CStr(), CrossName.CStr());
  M2.GetCrossNetNames(M2Names);
  EXPECT_EQ(M2Names.Len(), 1);
  EXPECT_EQ(M2Names[0].CStr(), CrossName.CStr());

  EXPECT_EQ(Net->GetModeId(ModeName1), C1.GetMode1());
  EXPECT_EQ(Net->GetModeId(ModeName2), C1.GetMode2());
}

TEST(multimodal, CheckNeighborsDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int64 EId = C1.AddEdge(0, 0);
  TInt64V InNeighbors;
  TInt64V OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);
}

TEST(multimodal, CheckNeighborsSameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);
}

TEST(multimodal, CheckNeighborsSameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);
}


TEST(multimodal, DelEdgeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int64 EId = C1.AddEdge(0, 0);
  TInt64V InNeighbors;
  TInt64V OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.DelEdge(EId);
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelEdgeSameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.DelEdge(EId);
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());\
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelEdgeSameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.DelEdge(EId);
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());\
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode1Directed) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int64 EId = C1.AddEdge(0, 0);
  TInt64V InNeighbors;
  TInt64V OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.DelNode(0);

  OutNeighbors.Clr();
  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode1SameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  EXPECT_EQ(1, C1.GetEdges());

  M1.DelNode(0);

  EXPECT_EQ(1, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode1SameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.DelNode(0);

  EXPECT_EQ(1, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}


TEST(multimodal, DelNode2Directed) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int64 EId = C1.AddEdge(0, 0);
  TInt64V InNeighbors;
  TInt64V OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M2.DelNode(0);

  OutNeighbors.Clr();
  EXPECT_EQ(0, M2.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode2SameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  EXPECT_EQ(1, C1.GetEdges());

  M1.DelNode(1);

  EXPECT_EQ(1, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelNode2SameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.DelNode(1);

  EXPECT_EQ(1, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}


TEST(multimodal, DelCrossNetSameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  C1.AddEdge(0, 1);

  TStr64V InNeighbors;
  TStr64V OutNeighbors;

  M1.IntVAttrNameNI(0, InNeighbors);
  M1.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(1, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(1, InNeighbors);
  M1.IntVAttrNameNI(1, OutNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(1, OutNeighbors.Len());

  Net->DelCrossNet(CrossName);

  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(0, InNeighbors);
  M1.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(1, InNeighbors);
  M1.IntVAttrNameNI(1, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelCrossNetSameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  C1.AddEdge(0, 1);

  TStr64V InNeighbors;
  TStr64V OutNeighbors;

  M1.IntVAttrNameNI(0, InNeighbors);
  M1.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(2, InNeighbors.Len());
  EXPECT_EQ(2, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(1, InNeighbors);
  M1.IntVAttrNameNI(1, OutNeighbors);
  EXPECT_EQ(2, InNeighbors.Len());
  EXPECT_EQ(2, OutNeighbors.Len());

  Net->DelCrossNet(CrossName);

  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(0, InNeighbors);
  M1.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(1, InNeighbors);
  M1.IntVAttrNameNI(1, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelCrossNetDifModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName, false);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  C1.AddEdge(0, 0);
  
  TStr64V InNeighbors;
  TStr64V OutNeighbors;

  M1.IntVAttrNameNI(0, InNeighbors);
  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(1, OutNeighbors.Len());

  Net->DelCrossNet(CrossName);

  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(0, InNeighbors);
  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelCrossNetDifModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName, true);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  C1.AddEdge(0, 0);
  
  TStr64V InNeighbors;
  TStr64V OutNeighbors;

  M1.IntVAttrNameNI(0, InNeighbors);
  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(1, OutNeighbors.Len());

  Net->DelCrossNet(CrossName);

  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());

  InNeighbors.Clr();
  OutNeighbors.Clr();

  M1.IntVAttrNameNI(0, InNeighbors);
  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, DelModeNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());

  Net->DelModeNet(ModeName1);

  EXPECT_EQ(Net->BegModeNetI(), Net->EndModeNetI());
}

TEST(multimodal, DelModeNetWithCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName, true);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  
  M1.AddNode(0);
  M2.AddNode(0);
  C1.AddEdge(0, 0);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_NE(Net->BegCrossNetI(), Net->EndCrossNetI());

  Net->DelModeNet(ModeName1);

  TStr64V OutNeighbors;

  M2.IntVAttrNameNI(0, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());

  TMMNet::TModeNetI it = Net->BegModeNetI();
  it++;

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_EQ(it, Net->EndModeNetI());
  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());
}

TEST(multimodal, DelModeNetWithSameModeCrossNet) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  Net->AddModeNet(ModeName1);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName1, CrossName, true);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  
  M1.AddNode(0);
  M1.AddNode(1);
  C1.AddEdge(0, 1);

  EXPECT_NE(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_NE(Net->BegCrossNetI(), Net->EndCrossNetI());

  Net->DelModeNet(ModeName1);

  EXPECT_EQ(Net->BegModeNetI(), Net->EndModeNetI());
  EXPECT_EQ(Net->BegCrossNetI(), Net->EndCrossNetI());
}

TEST(multimodal, ClrCrossDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int64 EId = C1.AddEdge(0, 0);
  TInt64V InNeighbors;
  TInt64V OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.Clr();
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(0, InNeighbors.Len());
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrCrossSameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.Clr();
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());\
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrCrossSameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  C1.Clr();
  EXPECT_EQ(0, C1.GetEdges());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());\
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrMode1Directed) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int64 EId = C1.AddEdge(0, 0);
  TInt64V InNeighbors;
  TInt64V OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.Clr();

  OutNeighbors.Clr();
  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrMode1SameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  EXPECT_EQ(1, C1.GetEdges());

  M1.Clr();

  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
}

TEST(multimodal, ClrMode1SameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.Clr();

  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
}


TEST(multimodal, ClrMode2Directed) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName1("TestMode1");
  TStr ModeName2("TestMode2");
  Net->AddModeNet(ModeName1);
  Net->AddModeNet(ModeName2);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName1, ModeName2, CrossName);
  TModeNet& M1 = Net->GetModeNetByName(ModeName1);
  TModeNet& M2 = Net->GetModeNetByName(ModeName2);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);

  M1.AddNode(0);
  M2.AddNode(0);
  int64 EId = C1.AddEdge(0, 0);
  TInt64V InNeighbors;
  TInt64V OutNeighbors;

  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M2.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M2.Clr();

  OutNeighbors.Clr();
  EXPECT_EQ(0, M2.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors);
  EXPECT_EQ(0, OutNeighbors.Len());
}

TEST(multimodal, ClrMode2SameModeDirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, true);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);


  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(0, OutNeighbors.Len());

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(0, InNeighbors.Len());
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  EXPECT_EQ(1, C1.GetEdges());

  M1.Clr();

  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
}

TEST(multimodal, ClrMode2SameModeUndirected) {
  PMMNet Net;
  Net = TMMNet::New();
  TStr ModeName("TestMode");
  Net->AddModeNet(ModeName);
  TStr CrossName("TestCross");
  Net->AddCrossNet(ModeName, ModeName, CrossName, false);

  TModeNet& M1 = Net->GetModeNetByName(ModeName);
  TCrossNet& C1 = Net->GetCrossNetByName(CrossName);
  M1.AddNode(0);
  M1.AddNode(1);
  int64 EId = C1.AddEdge(0, 1);

  TInt64V InNeighbors;
  TInt64V OutNeighbors;
  M1.GetNeighborsByCrossNet(0, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(0, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  InNeighbors.Clr();
  OutNeighbors.Clr();
  M1.GetNeighborsByCrossNet(1, CrossName, InNeighbors, true);
  EXPECT_EQ(1, InNeighbors.Len());
  EXPECT_EQ(EId, InNeighbors[0]);
  M1.GetNeighborsByCrossNet(1, CrossName, OutNeighbors, false);
  EXPECT_EQ(1, OutNeighbors.Len());
  EXPECT_EQ(EId, OutNeighbors[0]);

  M1.Clr();

  EXPECT_EQ(0, M1.GetNodes());
  EXPECT_EQ(0, C1.GetEdges());
}

TEST(multimodal, GetSubgraph) {
  int64 NNodes = 1000;
  int64 NEdges = 1000;

  PMMNet Graph;
  Graph = PMMNet::New();

  // Add mode2
  TStr TestMode1("TestMode1");
  Graph->AddModeNet(TestMode1);
  TInt64 TestModeId1 = Graph->GetModeId(TestMode1);
  TStr TestMode2("TestMode2");
  Graph->AddModeNet(TestMode2);
  TInt64 TestModeId2 = Graph->GetModeId(TestMode2);

  // Add crossnets
  TStr TestCross1("TestCross1");
  Graph->AddCrossNet(TestMode1, TestMode1, TestCross1, true);
  Graph->GetCrossId(TestCross1);
  TStr TestCross2("TestCross2");
  Graph->AddCrossNet(TestModeId1, TestModeId1, TestCross2, false);
  TInt64 TestCrossId2 = Graph->GetCrossId(TestCross2);
  TStr TestCross3("TestCross3");
  Graph->AddCrossNet(TestMode1, TestMode2, TestCross3, true);
  TStr TestCross4("TestCross4");
  Graph->AddCrossNet(TestModeId1, TestModeId2, TestCross4, false);
  TInt64 TestCrossId4 = Graph->GetCrossId(TestCross4);

  // Add Nodes
  TModeNet& ModeNet1 = Graph->GetModeNetByName(TestMode1);
  TModeNet& ModeNet2 = Graph->GetModeNetById(TestModeId2);
  for (int64 i=0; i < NNodes; i++) {
    ModeNet1.AddNode(i);
    ModeNet2.AddNode(i*2);
  }

  // Add edges
  TCrossNet& CrossNet1 = Graph->GetCrossNetByName(TestCross1);
  TCrossNet& CrossNet2 = Graph->GetCrossNetById(TestCrossId2);
  TCrossNet& CrossNet3 = Graph->GetCrossNetByName(TestCross3);
  TCrossNet& CrossNet4 = Graph->GetCrossNetById(TestCrossId4);
  for (int64 i=0; i < NEdges; i++) {
    CrossNet1.AddEdge(i, (i+1)%NNodes, i);
    CrossNet2.AddEdge((i+5)%NNodes, i, i);
    CrossNet3.AddEdge(i, (i%NNodes)*2, i);
    CrossNet4.AddEdge((i+5)%NNodes, (i%NNodes)*2, i);
  }

  // Get subgraph
  TStr64V CrossNets;
  CrossNets.Add(TestCross1);
  PMMNet Subgraph = Graph->GetSubgraphByCrossNet(CrossNets);
  EXPECT_EQ(1, Subgraph->GetModeNets());
  EXPECT_EQ(1, Subgraph->GetCrossNets());
  TModeNet& M1 = Subgraph->GetModeNetByName(TestMode1);
  
  // Get neighbor types
  TStr64V M1Names;
  M1.GetCrossNetNames(M1Names);
  EXPECT_EQ(1, M1Names.Len());
  
  // Get Neighbors for node 0
  TInt64V Neighbors;
  M1.GetNeighborsByCrossNet(0, TestCross1, Neighbors);
  EXPECT_EQ(1, Neighbors.Len());
}

TEST(multimodal, ToNetwork) {
    int64 NNodes = 1000;
  int64 NEdges = 1000;

  PMMNet Graph;
  Graph = PMMNet::New();

  // Add mode2
  TStr TestMode1("TestMode1");
  Graph->AddModeNet(TestMode1);
  TInt64 TestModeId1 = Graph->GetModeId(TestMode1);
  TStr TestMode2("TestMode2");
  Graph->AddModeNet(TestMode2);
  TInt64 TestModeId2 = Graph->GetModeId(TestMode2);

  // Add crossnets
  TStr TestCross1("TestCross1");
  Graph->AddCrossNet(TestMode1, TestMode1, TestCross1, true);
  TInt64 TestCrossId1 = Graph->GetCrossId(TestCross1);
  TStr TestCross2("TestCross2");
  Graph->AddCrossNet(TestModeId1, TestModeId1, TestCross2, false);
  TInt64 TestCrossId2 = Graph->GetCrossId(TestCross2);
  TStr TestCross3("TestCross3");
  Graph->AddCrossNet(TestMode1, TestMode2, TestCross3, true);
  TInt64 TestCrossId3 = Graph->GetCrossId(TestCross3);
  TStr TestCross4("TestCross4");
  Graph->AddCrossNet(TestModeId1, TestModeId2, TestCross4, false);
  TInt64 TestCrossId4 = Graph->GetCrossId(TestCross4);

  // Add Nodes
  TModeNet& ModeNet1 = Graph->GetModeNetByName(TestMode1);
  TModeNet& ModeNet2 = Graph->GetModeNetById(TestModeId2);
  for (int64 i=0; i < NNodes; i++) {
    ModeNet1.AddNode(i);
    ModeNet2.AddNode(i*2);
  }

  // Add edges
  TCrossNet& CrossNet1 = Graph->GetCrossNetByName(TestCross1);
  TCrossNet& CrossNet2 = Graph->GetCrossNetById(TestCrossId2);
  TCrossNet& CrossNet3 = Graph->GetCrossNetByName(TestCross3);
  TCrossNet& CrossNet4 = Graph->GetCrossNetById(TestCrossId4);
  for (int64 i=0; i < NEdges; i++) {
    CrossNet1.AddEdge(i, (i+1)%NNodes, i);
    CrossNet2.AddEdge((i+5)%NNodes, i, i);
    CrossNet3.AddEdge(i, (i%NNodes)*2, i);
    CrossNet4.AddEdge((i+5)%NNodes, (i%NNodes)*2, i);
  }

  // Convert to TNEANet
  TInt64V CrossNetIds;
  CrossNetIds.Add(TestCrossId1);
  CrossNetIds.Add(TestCrossId2);
  CrossNetIds.Add(TestCrossId3);
  CrossNetIds.Add(TestCrossId4);
  TVec<TTriple<TInt64, TStr, TStr>, int64> NodeAttrMapping; //Triples of (ModeId, OldAttrName, NewAttrName)
  TVec<TTriple<TInt64, TStr, TStr>, int64> EdgeAttrMapping; //Triples of (CrossId, OldAttrName, NewAttrName)
  PNEANet Net = Graph->ToNetwork(CrossNetIds, NodeAttrMapping, EdgeAttrMapping);
  EXPECT_EQ(NNodes*2, Net->GetNodes());
  EXPECT_EQ(NEdges*6, Net->GetEdges()); //undirected has 2*NEdges edges, one in each direction
}


static const char *kIntANameN = "TEST_INTN",  *kFltANameN = "TEST_FLTN",
  *kStrANameN = "TEST_STRN", *kIntVANameN = "TEST_INTVN", 
  *kIntSANameN = "TEST_SINTN", *kFltSANameN = "TEST_SFLTN", 
  *kStrSANameN = "TEST_SSTRN", *kIntANameE = "TEST_INTE",  
  *kFltANameE = "TEST_FLTE",  *kStrANameE = "TEST_STRE", 
  *kIntVANameE = "TEST_INTVE", *kIntSANameE = "TEST_SINTE", 
  *kFltSANameE = "TEST_SFLTE", *kStrSANameE = "TEST_SSTRE";

static const TInt64 int_default = 1000;
static const TFlt flt_default = 2000.0;
static const char *str_default = "3000";

static const int kNModes = 3;
static const int kNNodes = 30;

static void setup_copytests(PMMNet& mmnet, PMMNet& mmnet2) {
  mmnet = TMMNet::New();
  mmnet2 = TMMNet::New();
  // add modes and nodes
  for(int i = 0; i < kNModes; i++) {
    std::ostringstream oss; oss << "Mode " << i;
    TStr modename = oss.str().c_str();
    mmnet->AddModeNet(modename);
    for(int j = 0; j < kNNodes; j++) {
      mmnet->GetModeNetByName(modename).AddNode();
    }
  }

  // add dense and sparse node attributes to one mode and assign values to nodes
  TModeNet& mode = mmnet->GetModeNetById(0);
  mode.AddIntAttrN(kIntANameN, int_default);
  mode.AddFltAttrN(kFltANameN, flt_default);
  mode.AddStrAttrN(kStrANameN, str_default);
  mode.AddIntVAttrN(kIntVANameN);
  for(TInt64 i = 0; i < kNNodes - 1; i++) {
    mode.AddIntAttrDatN(i, int_default + i, kIntANameN);
    mode.AddSAttrDatN(i, kIntSANameN, (TInt64)(int_default + i));
    mode.AddFltAttrDatN(i, flt_default + i, kFltANameN);
    mode.AddSAttrDatN(i, kFltSANameN, (TFlt)(flt_default - i));
    std::ostringstream strattr(str_default), strattr_sparse(str_default);
    strattr << " - " << i; strattr_sparse << " - " << i << "S";
    mode.AddStrAttrDatN(i, strattr.str().c_str(), kStrANameN);
    mode.AddSAttrDatN(i, kStrSANameN, strattr_sparse.str().c_str());
    for(TInt64 j = 0; j < 5; j++) mode.AppendIntVAttrDatN(i, j, kIntVANameN);                        
  }

  // add edge attributes to a mode, add a (meaningless) edge to mode and assign sparse attrs to it
  mode.AddIntAttrE(kIntANameE);
  mode.AddFltAttrE(kFltANameE);
  mode.AddStrAttrE(kStrANameE);
  mode.AddIntVAttrE(kIntVANameE);
  mode.AddEdge(0, 0, 0);
  mode.AddSAttrDatE(0, kIntSANameE, int_default);
  mode.AddSAttrDatE(0, kFltSANameE, flt_default);
  mode.AddSAttrDatE(0, kStrSANameE, str_default);
  
  // add crossnets
  for(int i = 0; i < kNModes; i++) {
    std::ostringstream oss1; oss1 << "Mode " << i;
    TStr modename1 = oss1.str().c_str();
    // inter-mode: three per pair, one per direction and one undirected
    for(int j = 0; j < i; j++) {
      std::ostringstream oss2; oss2 << "Mode " << j;
      TStr modename2 = oss2.str().c_str();
      std::ostringstream oss1to2, oss2to1, ossboth;
      oss1to2 << i << "->" << j;
      oss2to1 << i << "<-" << j;
      ossboth << i << "<->" << j;      
      mmnet->AddCrossNet(modename1, modename2, oss1to2.str().c_str(), true);
      mmnet->AddCrossNet(modename2, modename1, oss2to1.str().c_str(), true);
      mmnet->AddCrossNet(modename1, modename2, ossboth.str().c_str(), false);
    }
    // intra-mode: one directed, one undirected
    std::ostringstream selfdir, selfundir;
    selfdir << i << "dir"; selfundir << i << "undir";
    mmnet->AddCrossNet(modename1, modename1, selfdir.str().c_str(), true);
    mmnet->AddCrossNet(modename1, modename1, selfdir.str().c_str(), false);
  }

  int kMagicPrime = 7;
  // add edges and edge attributes to all crossnets
  for(TInt64 i = 0; i < mmnet->GetCrossNets(); i++) {
    TCrossNet& CN = mmnet->GetCrossNetById(i);
    CN.AddIntAttrE(kIntANameE, int_default);
    CN.AddFltAttrE(kFltANameE, flt_default);
    CN.AddStrAttrE(kStrANameE, str_default);
    for(TInt64 j = 0; j < kNNodes; j++) {
      for(TInt64 k = 0; k < kNNodes; k++) {
        TInt64 EId = i * mmnet->GetCrossNets() * kNNodes + j * kNNodes + k;
        if(EId % kMagicPrime == 0) {
          CN.AddEdge(j, k, EId);
          CN.AddIntAttrDatE(EId, int_default + j * kNNodes + k, kIntANameE);
          CN.AddFltAttrDatE(EId, flt_default + j * kNNodes + k, kFltANameE);
          std::ostringstream strattr(str_default);
          strattr << " - " << int_default + j * kNNodes + k;
          CN.AddStrAttrDatE(EId, strattr.str().c_str(), kStrANameE);
        }
      }
    }
  }
}

static void GetCNAttrNames(TStr64V& Names, const PMMNet& mmnet, TModeNet& mode) {
  Names = TStr64V();
  TStr64V CNNames; mode.GetCrossNetNames(CNNames);
  for(int64 i = 0; i < CNNames.Len(); i++) {
    TStr name = CNNames[i];
    TCrossNet& origCN = mmnet->GetCrossNetByName(name);
    if(origCN.GetMode1() == origCN.GetMode2() && origCN.IsDirected()) { // attrs are name:SRC and name:DST
      Names.Add(name + (TStr)":SRC");
      Names.Add(name + (TStr)":DST");
    } else {
      Names.Add(name);
    }
  }
}

TEST(multimodal, CopyModeWithoutNodes) {
  PMMNet mmnet, mmnet2;
  setup_copytests(mmnet, mmnet2);
  TModeNet& mode = mmnet->GetModeNetById(0);
  
  // invoke method, ensure basic information about TMMNet and nodeless modenet holds
  TMMNet::CopyModeWithoutNodes(mmnet, mmnet2, 0);
  EXPECT_EQ(0, mmnet2->GetModeId("Mode 0"));
  EXPECT_EQ(1, mmnet2->GetModeNets()); 
  TModeNet& modecopy = mmnet2->GetModeNetById(0);
  EXPECT_EQ(0, modecopy.GetNodes());
  EXPECT_EQ(0, modecopy.GetEdges());

  // Ensure dense node attr names preserved but dense edge attrs and crossnet attrs deleted
  EXPECT_TRUE(modecopy.IsIntAttrN(kIntANameN));
  EXPECT_TRUE(modecopy.IsFltAttrN(kFltANameN));
  EXPECT_TRUE(modecopy.IsStrAttrN(kStrANameN));
  EXPECT_TRUE(modecopy.IsIntVAttrN(kIntVANameN));
  EXPECT_FALSE(modecopy.IsIntAttrE(kIntANameE));
  EXPECT_FALSE(modecopy.IsFltAttrE(kFltANameE));
  EXPECT_FALSE(modecopy.IsStrAttrE(kStrANameE));
  EXPECT_FALSE(modecopy.IsIntVAttrE(kIntVANameE));
  TStr64V CNAttrNames;
  GetCNAttrNames(CNAttrNames, mmnet, mode);
  for(int i = 0; i < CNAttrNames.Len(); i++) EXPECT_FALSE(modecopy.IsIntVAttrN(CNAttrNames[i]));

  // Ensure all sparse attrs deleted and node default attrs preserved; temporarily add nodes/edge 
  // back to test directly
  TInt64 GarbageInt;
  TFlt GarbageFlt;
  TStr GarbageStr;
  for(TInt64 i = 0; i < kNNodes; i++) {
    modecopy.AddNode(i);
    EXPECT_EQ(-1, modecopy.GetSAttrDatN(i, kIntSANameN, GarbageInt));
    EXPECT_EQ(-1, modecopy.GetSAttrDatN(i, kFltSANameN, GarbageFlt));
    EXPECT_EQ(-1, modecopy.GetSAttrDatN(i, kStrSANameN, GarbageStr));
    EXPECT_EQ(int_default, modecopy.GetIntAttrDatN(i, kIntANameN));
    EXPECT_EQ(flt_default, modecopy.GetFltAttrDatN(i, kFltANameN));
    EXPECT_EQ((TStr)str_default, modecopy.GetStrAttrDatN(i, kStrANameN));
    modecopy.DelNode(i);
  }
  modecopy.AddNode(0); modecopy.AddEdge(0,0,0);
  EXPECT_EQ(-1, modecopy.GetSAttrDatE(0, kIntSANameE, GarbageInt));
  EXPECT_EQ(-1, modecopy.GetSAttrDatE(0, kFltSANameE, GarbageFlt));
  EXPECT_EQ(-1, modecopy.GetSAttrDatE(0, kStrSANameE, GarbageStr));
  modecopy.DelEdge(0); modecopy.DelNode(0);
}

TEST(multimodal, CopyNodesWithoutNeighbors) {
  PMMNet mmnet, mmnet2;
  setup_copytests(mmnet, mmnet2);
  TMMNet::CopyModeWithoutNodes(mmnet, mmnet2, 0);
  TModeNet& mode = mmnet->GetModeNetById(0), &modecopy = mmnet2->GetModeNetById(0);

  TInt64V ToCopy, ToIgnore;
  // Copy half the nodes from mode 0 to its counterpart. Check that exactly the nodes we want are copied
  for(TInt64 i = 0; i < kNNodes/2; i++) ToIgnore.Add(i);
  for(TInt64 i = kNNodes/2; i < kNNodes; i++) ToCopy.Add(i);
  TModeNet::CopyNodesWithoutNeighbors(mode, modecopy, ToCopy);
  for(TInt64V::TIter nit = ToCopy.BegI(); nit < ToCopy.EndI(); nit++) EXPECT_TRUE(modecopy.IsNode(*nit));
  for(TInt64V::TIter nit = ToIgnore.BegI(); nit < ToIgnore.EndI(); nit++) EXPECT_FALSE(modecopy.IsNode(*nit));
  EXPECT_EQ(ToCopy.Len(), modecopy.GetNodes());
  EXPECT_EQ(0, modecopy.GetEdges());
  
  // Check that all non-crossnet attribute values are copied
  for(TInt64V::TIter nit = ToCopy.BegI(); nit < ToCopy.EndI(); nit++) {
    TInt64 NId = *nit;
    EXPECT_EQ(mode.GetIntAttrDatN(NId, kIntANameN), modecopy.GetIntAttrDatN(NId, kIntANameN));
    EXPECT_EQ(mode.GetFltAttrDatN(NId, kFltANameN), modecopy.GetFltAttrDatN(NId, kFltANameN));
    EXPECT_EQ(mode.GetStrAttrDatN(NId, kStrANameN), modecopy.GetStrAttrDatN(NId, kStrANameN));
    EXPECT_EQ(mode.GetIntVAttrDatN(NId, kIntVANameN), modecopy.GetIntVAttrDatN(NId, kIntVANameN));    
 
    TInt64 Int1, Int2;
    TFlt Flt1, Flt2;
    TStr Str1, Str2;
    mode.GetSAttrDatN(NId, (TStr)kIntSANameN, Int1); modecopy.GetSAttrDatN(NId, (TStr)kIntSANameN, Int2);
    EXPECT_EQ(Int1, Int2);
    mode.GetSAttrDatN(NId, (TStr)kFltSANameN, Flt1); modecopy.GetSAttrDatN(NId, (TStr)kFltSANameN, Flt2);
    EXPECT_EQ(Flt1, Flt2);
    mode.GetSAttrDatN(NId, (TStr)kStrSANameN, Str1); modecopy.GetSAttrDatN(NId, (TStr)kStrSANameN, Str2);
    EXPECT_EQ(Str1, Str2);
  }

  // Check crossnet attrs still deleted
  TStr64V CNAttrNames;
  GetCNAttrNames(CNAttrNames, mmnet, mode);
  for(int i = 0; i < CNAttrNames.Len(); i++) EXPECT_FALSE(modecopy.IsIntVAttrN(CNAttrNames[i]));
}

TEST(multimodal, CopyCrossNetWithoutEdges) {
  PMMNet mmnet, mmnet2;
  setup_copytests(mmnet, mmnet2);
  TInt64V ToCopy; for(TInt64 i = 0; i < kNNodes; i++) ToCopy.Add(i);
  for(int i = 0; i < kNModes; i++) { 
    TMMNet::CopyModeWithoutNodes(mmnet, mmnet2, i);
    TModeNet::CopyNodesWithoutNeighbors(mmnet->GetModeNetById(i), mmnet2->GetModeNetById(i), ToCopy);
  }
  
  // Copy all crossnets. Verify that they contain no edges, but that crossedge attribute names
  // and defaults hold.
  for(int i = 0; i < mmnet->GetCrossNets(); i++) {
    TMMNet::CopyCrossNetWithoutEdges(mmnet, mmnet2, i);
    TCrossNet& CN2 = mmnet2->GetCrossNetById(i);
    EXPECT_EQ(0, CN2.GetEdges());
    EXPECT_TRUE(CN2.IsIntAttrE(kIntANameE));
    EXPECT_TRUE(CN2.IsFltAttrE(kFltANameE));
    EXPECT_TRUE(CN2.IsStrAttrE(kStrANameE));
    
    CN2.AddEdge(0, 0, 0);
    EXPECT_EQ(int_default, CN2.GetIntAttrDatE(0, kIntANameE));
    EXPECT_EQ(flt_default, CN2.GetFltAttrDatE(0, kFltANameE));
    EXPECT_EQ((TStr)str_default, CN2.GetStrAttrDatE(0, kStrANameE));    
  }
  EXPECT_EQ(mmnet->GetCrossNets(), mmnet2->GetCrossNets());

  // Verify that all crossnet attribute names are properly copied.
  for(int i = 0; i < mmnet->GetModeNets(); i++) {
    TModeNet& mode = mmnet->GetModeNetById(i), & modecopy = mmnet2->GetModeNetById(i);
    TStr64V CNAttrNames; GetCNAttrNames(CNAttrNames, mmnet, mode);
    for(int j = 0; j < CNAttrNames.Len(); j++) EXPECT_TRUE(modecopy.IsIntVAttrN(CNAttrNames[j]));
  }
}
