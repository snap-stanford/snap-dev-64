#include "Snap.h"

using namespace TSnap;

// Get connected components on directed graph that is not fully connected
void GetCnCom() {
  
  PNGraph G;
  
// Create benchmark graph
  const int NNodes = 10000;
  const int NEdges = 5000;
  G = GenRndGnm<PNGraph>(NNodes, NEdges);
  
  printf("IsConnected(G) = %s\n", (IsConnected(G) ? "True" : "False"));
  printf("IsWeaklyConnected(G) = %s\n", (IsWeaklyConn(G) ? "True" : "False"));
  
  // Get weakly connected component counts
  TIntPr64V WccSzCnt;
  GetWccSzCnt(G, WccSzCnt);
  for (int i = 0; i < WccSzCnt.Len(); i++) {
    printf("WccSzCnt[%d] = (%s, %s)\n", i,
              ToCStr(WccSzCnt[i].Val1.Val), ToCStr(WccSzCnt[i].Val2.Val));
  }

  // Get all nodes in same component
  TInt64V CnCom;
  GetNodeWcc(G, 1, CnCom);
  printf("CnCom.Len() = %s\n", ToCStr(CnCom.Len()));

  TCnComV WCnComV;
  GetWccs(G, WCnComV);
  for (int i = 0; i < WCnComV.Len(); i++) {
    printf("WCnComV[%d].Len() = %s\n", i, ToCStr(WCnComV[i].Len()));
    for (int j = 0; j < WCnComV[i].Len(); j++) {
      printf("WCnComV[%d][%d] = %s\n", i, j, ToCStr(WCnComV[i][j].Val));
    }
  }
  
  double MxWccSz = GetMxWccSz(G);
  printf("MxWccSz = %.5f\n", MxWccSz);
  
  // Get graph with largest SCC
  PNGraph GMx = GetMxWcc(G);
  printf("GMx: GetNodes() = %s, GetEdges() = %s\n",
        ToCStr(GMx->GetNodes()), ToCStr(GMx->GetEdges()));
  
  // Get strongly connected components
  TCnComV SCnComV;
  GetSccs(G, SCnComV);
  for (int i = 0; i < SCnComV.Len(); i++) {
    printf("SCnComV[%d].Len() = %s\n", i, ToCStr(SCnComV[i].Len()));
  }
  
  // Get largest bi-connected component
  PNGraph GMxBi = GetMxBiCon(G);
  printf("GMxBi: GetNodes() = %s, GetEdges() = %s\n",
          ToCStr(GMxBi->GetNodes()), ToCStr(GMxBi->GetEdges()));
}

int main(int argc, char* argv[]) {
  GetCnCom();
}

