#include "Snap.h"

#define DIRNAME "cncom"

class CnComTest { };  // For gtest highlighting

using namespace TSnap;

// Get connected components on full graph
template <class PGraph>
void GetCnComFull() {
  
  printf("\nFull Graph:\n");
  
  const int NNodes = 500;
  
  PGraph G = GenFull<PGraph>(NNodes);
  
  // Tests whether the Graph is (weakly) connected
  // Note: these are equivalent functions
  printf("IsConnected(G) = %d\n", IsConnected(G));
  printf("IsWeaklyConnected(G) = %d\n", IsWeaklyConn(G));
  
  // Get all nodes in same component
  TInt64V CnCom;
  GetNodeWcc(G, 1, CnCom);
  printf("CnCom.Len() = %s\n", TInt64::GetStr(CnCom.Len()).CStr());
  
  // Get weakly connected component counts
  TIntPr64V WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
  for (int i = 0; i < WccSzCnt.Len(); i++) {
    printf("WccSzCnt[%d] = (%s, %s)\n", i,
                TInt64::GetStr(WccSzCnt[i].Val1.Val).CStr(),
                TInt64::GetStr(WccSzCnt[i].Val2.Val).CStr());
  }
  
  // Get weakly connected components
  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  for (int i = 0; i < WCnComV.Len(); i++) {
    printf("WCnComV[%d].Len() = %s\n", i,
                TInt64::GetStr(WCnComV[i].Len()).CStr());
  }
  
  // Get strongly connected fraction -- should be 1.0
  double MxWccSz = GetMxWccSz(G);
  printf("MxWccSz = %.5f\n", MxWccSz);
  
  // Get graph with largest SCC
  PGraph GMx = GetMxWcc(G);
  printf("GMx->GetNodes() = %s, GMx->GetEdges() = %s\n",
            TUInt64::GetStr(GMx->GetNodes()).CStr(),
            TUInt64::GetStr(GMx->GetEdges()).CStr());
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %s\n", i,
              TInt64::GetStr(SCnComV[i].Len()).CStr());
  }
  
  // Get largest bi-connected component
  PGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi->GetNodes() = %s, GMxBi->GetEdges() = %s\n",
            TUInt64::GetStr(GMxBi->GetNodes()).CStr(),
            TUInt64::GetStr(GMxBi->GetEdges()).CStr());
}

// Get connected components on undirected graph that is not fully connected
void GetCnComUndirectedDisconnected() {
  
  printf("\nDisconnected Undirected Graph:\n");

  PUNGraph G;
  
  // As benchmark, this is done once and compared visually to confirm values are correct
  const int NNodes = 30;
  G = GenRndPowerLaw(NNodes, 2.5);
  
  TIntStr64H NodeLabelH;
  for (int i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlNeato, TStr::Fmt("%s/sample_cncom_unpower.png", DIRNAME), "Sample CnCom Graph", NodeLabelH);
  
  printf("IsConnected(G) = %d\n", IsConnected(G));
  printf("IsWeaklyConnected(G) = %d\n", IsWeaklyConn(G));
  
  // Get weakly connected component counts
  TIntPr64V WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
  for (int i = 0; i < WccSzCnt.Len(); i++) {
    printf("WccSzCnt[%d] = (%s, %s)\n", i,
              TInt64::GetStr(WccSzCnt[i].Val1.Val).CStr(),
              TInt64::GetStr(WccSzCnt[i].Val2.Val).CStr());
  }
  
  // Get all nodes in same component
  TInt64V CnCom;
  GetNodeWcc(G, 1, CnCom);
  printf("CnCom.Len() = %s\n", TInt64::GetStr(CnCom.Len()).CStr());
  
  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  for (int i = 0; i < WCnComV.Len(); i++) {
    printf("WCnComV[%d].Len() = %s\n", i,
              TInt64::GetStr(WCnComV[i].Len()).CStr());
    for (int j = 0; j < WCnComV[i].Len(); j++) {
      printf("WCnComV[%d][%d] = %s\n", i, j,
              TInt64::GetStr(WCnComV[i][j].Val).CStr());
    }
  }
  
  double MxWccSz = GetMxWccSz(G);
  printf("MxWccSz = %.5f\n", MxWccSz);
  
  // Get graph with largest SCC
  PUNGraph GMx = GetMxWcc(G);
  printf("GMx->GetNodes() = %s, GMx->GetEdges() = %s\n",
              TInt64::GetStr(GMx->GetNodes()).CStr(),
              TInt64::GetStr(GMx->GetEdges()).CStr());
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %s\n", i,
            TInt64::GetStr(SCnComV[i].Len()).CStr());
  }
  
  // Get largest bi-connected component
  PUNGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi->GetNodes() = %s, GMxBi->GetEdges() = %s\n",
              TInt64::GetStr(GMxBi->GetNodes()).CStr(),
              TInt64::GetStr(GMxBi->GetEdges()).CStr());
  
  // Get bi-connected size components counts
  TIntPr64V SzCntV;
  GetBiConSzCnt(G, SzCntV);
  for (int i = 0; i < SzCntV.Len(); i++) {
    printf("SzCntV[%d] = (%s, %s)\n", i,
              TInt64::GetStr(SzCntV[i].Val1.Val).CStr(),
              TInt64::GetStr(SzCntV[i].Val2.Val).CStr());
  }
  
  // Get bi-connected components
  TCnComV BiCnComV;
  GetBiCon(G, BiCnComV);
  for (int i = 0; i < BiCnComV.Len(); i++) {
    printf("BiCnComV[%d].Len() = %s\n", i,
              TInt64::GetStr(BiCnComV[i].Len()).CStr());
  }
  
  // Get articulation points of a graph
  TInt64V ArtNIdV;
  GetArtPoints(G, ArtNIdV);
  printf("ArtNIdV.Len() = %s\n", TInt64::GetStr(ArtNIdV.Len()).CStr());
  for (int i = 0; i < ArtNIdV.Len(); i++) {
    printf("ArtNIdV[%d] = %s\n", i, TInt64::GetStr(ArtNIdV[i].Val).CStr());
  }
  
  // Get edge bridges of a graph
  TIntPr64V EdgeV;
  GetEdgeBridges(G, EdgeV);
  printf("EdgeV.Len() = %s\n", TInt64::GetStr(EdgeV.Len()).CStr());
  for (int i = 0; i < EdgeV.Len(); i++) {
    printf("EdgeV[%d] = (%s, %s)\n", i,
            TInt64::GetStr(EdgeV[i].Val1.Val).CStr(),
            TInt64::GetStr(EdgeV[i].Val2.Val).CStr());
  }
  
  // Get 1-components counts
  TIntPr64V SzCnt1ComV;
  Get1CnComSzCnt(G, SzCnt1ComV);
  printf("SzCnt1ComV.Len() = %s\n", TInt64::GetStr(SzCnt1ComV.Len()).CStr());
  for (int i = 0; i < SzCnt1ComV.Len(); i++) {
    printf("SzCnt1ComV[%d] = (%s, %s)\n", i,
              TInt64::GetStr(SzCnt1ComV[i].Val1.Val).CStr(),
              TInt64::GetStr(SzCnt1ComV[i].Val2.Val).CStr());
  }
  
  // Get 1-components
  TCnComV Cn1ComV;
  Get1CnCom(G, Cn1ComV);
  for (int i = 0; i < Cn1ComV.Len(); i++) {
    printf("Cn1ComV[%d].Len() = %s\n", i,
              TInt64::GetStr(Cn1ComV[i].Len()).CStr());
  }
  
}

// Get connected components on directed graph that is not fully connected
void GetCnComDirectedDisconnected() {
  
  printf("\nDisconnected Directed Graph:\n");
  PNGraph G;
  
// Create benchmark graph
  const int NNodes = 30;
  G = GenRndGnm<PNGraph>(NNodes, NNodes);

  // Add some more random edges
  for (int i = 0; i < 10; i++) {
    TInt Src, Dst;
    do {
      Src = G->GetRndNId();
      Dst = G->GetRndNId();
    }
    while (Src == Dst || G->IsEdge(Src, Dst));
    G->AddEdge(Src, Dst);
  }

  // Add isolated component
  G->AddNode(NNodes);
  G->AddNode(NNodes+1);
  G->AddNode(NNodes+2);
  G->AddEdge(NNodes, NNodes+1);
  G->AddEdge(NNodes+1, NNodes+2);
  G->AddEdge(NNodes+2, NNodes+1);

  printf("Nodes = %s, Edges = %s\n",
        TUInt64::GetStr(G->GetNodes()).CStr(),
        TUInt64::GetStr(G->GetEdges()).CStr());

  TIntStr64H NodeLabelH;
  for (int i = 0; i < G->GetNodes(); i++) {
    NodeLabelH.AddDat(i, TStr::Fmt("%d", i));
  }
  DrawGViz(G, gvlDot, TStr::Fmt("%s/sample_cncom_ngraph.png", DIRNAME), "Sample CnCom Graph", NodeLabelH);
  
  printf("IsConnected(G) = %d\n", IsConnected(G));
  printf("IsWeaklyConnected(G) = %d\n", IsWeaklyConn(G));
  
  // Get weakly connected component counts
  TIntPr64V WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
  for (int i = 0; i < WccSzCnt.Len(); i++) {
    printf("WccSzCnt[%d] = (%s, %s)\n", i,
              TInt64::GetStr(WccSzCnt[i].Val1.Val).CStr(),
              TInt64::GetStr(WccSzCnt[i].Val2.Val).CStr());
  }
  
  // Get all nodes in same component
  TInt64V CnCom;
  GetNodeWcc(G, 1, CnCom);
  printf("CnCom.Len() = %s\n", TInt64::GetStr(CnCom.Len()).CStr());
  
  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  for (int i = 0; i < WCnComV.Len(); i++) {
    printf("WCnComV[%d].Len() = %s\n", i,
                TInt64::GetStr(WCnComV[i].Len()).CStr());
    for (int j = 0; j < WCnComV[i].Len(); j++) {
      printf("WCnComV[%d][%d] = %s\n", i, j,
                TInt64::GetStr(WCnComV[i][j].Val).CStr());
    }
  }
  
  double MxWccSz = GetMxWccSz(G);
  printf("MxWccSz = %.5f\n", MxWccSz);
  
  // Get graph with largest WCC
  PNGraph GWMx = GetMxWcc(G);
  printf("GWMx->GetNodes() = %s, GWMx->GetEdges() = %s\n",
        TUInt64::GetStr(GWMx->GetNodes()).CStr(),
        TUInt64::GetStr(GWMx->GetEdges()).CStr());
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %s\n", i,
                TInt64::GetStr(SCnComV[i].Len()).CStr());
  }
  
  double MxSccSz = GetMxSccSz(G);
  printf("MxSccSz = %.5f\n", MxSccSz);
  
  // Get graph with largest SCC
  PNGraph GSMx = GetMxScc(G);
  printf("GSMx->GetNodes() = %s, GSMx->GetEdges() = %s\n",
        TUInt64::GetStr(GSMx->GetNodes()).CStr(),
        TUInt64::GetStr(GSMx->GetEdges()).CStr());

  // Get largest bi-connected component
  PNGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi->GetNodes() = %s, GMxBi->GetEdges() = %s\n",
        TUInt64::GetStr(GMxBi->GetNodes()).CStr(),
        TUInt64::GetStr(GMxBi->GetEdges()).CStr());
}

// Get connected components on full graph
void GetCnComCompleteGraph() {
  
  GetCnComFull<PUNGraph>();
  
  GetCnComFull<PNGraph>();
  
  GetCnComFull<PNEGraph>();
  
  // Test the remaining functions (for undirected graphs only)
  const int NNodes = 500;
  
  PUNGraph G = GenFull<PUNGraph>(NNodes);
  
  // Get bi-connected size components counts
  TIntPr64V SzCntV;
  GetBiConSzCnt(G, SzCntV);
  printf("SzCntV.Len() = %s\n", TInt64::GetStr(SzCntV.Len()).CStr());
  printf("SzCntV[0] = (%s, %s)\n",
            TInt64::GetStr(SzCntV[0].Val1.Val).CStr(),
            TInt64::GetStr(SzCntV[0].Val2.Val).CStr());
  
  // Get bi-connected components
  TCnComV BiCnComV;
  GetBiCon(G, BiCnComV);
  printf("BiCnComV[0].Len() = %s\n", TInt64::GetStr(BiCnComV[0].Len()).CStr());
  printf("BiCnComV.Len() = %s\n", TInt64::GetStr(BiCnComV.Len()).CStr());
  
  // Get articulation points of a graph
  TInt64V ArtNIdV;
  GetArtPoints(G, ArtNIdV);
  printf("ArtNIdV.Len() = %s\n", TInt64::GetStr(ArtNIdV.Len()).CStr());

  // Get edge bridges of a graph
  TIntPr64V EdgeV;
  GetEdgeBridges(G, EdgeV);
  printf("EdgeV.Len() == %s\n", TInt64::GetStr(EdgeV.Len()).CStr());

  // Get 1-components counts
  TIntPr64V SzCnt1ComV;
  Get1CnComSzCnt(G, SzCnt1ComV);
  printf("SzCnt1ComV.Len() == %s\n", TInt64::GetStr(SzCnt1ComV.Len()).CStr());
  
  // Get 1-components
  TCnComV Cn1ComV;
  Get1CnCom(G, Cn1ComV);
  printf("Cn1ComV.Len() == %s\n", TInt64::GetStr(Cn1ComV.Len()).CStr());
}

int main(int argc, char* argv[]) {
  
  mkdir(DIRNAME, S_IRWXU | S_IRWXG | S_IRWXO);
  
  GetCnComCompleteGraph();
  GetCnComUndirectedDisconnected();
  GetCnComDirectedDisconnected();
}
