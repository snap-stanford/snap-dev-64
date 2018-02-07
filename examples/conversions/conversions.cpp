#include "stdafx.h"

void printTime(PTmProfiler tm, int id) {
  printf("%s: %.2f\n", tm->GetTimerNm(id).CStr(), tm->GetTimerSec(id));
}

int main(int argc, char* argv[]) {
  //// what type of graph do you want to use?
  typedef PUNGraph PGraph; // undirected graph
  //typedef PNGraph PGraph;  //   directed graph
  //typedef PNEGraph PGraph;  //   directed multigraph
  //typedef TPt<TNodeNet<TInt> > PGraph;
  //typedef TPt<TNodeEdgeNet<TInt, TInt> > PGraph;
  
  PTmProfiler tm = TTmProfiler::New();

  // File input
  const char * smallBinary = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.graph";
  //const char * twitterBinary = "/lfs/local/0/gaspar09/networks/twitter_rv.graph";

  printf("Benchmark Graph Load Binary\n");


  PSIn SIn_p = TFIn::New(smallBinary);
  int t1 = tm->AddTimer("Load TUNGraph Binary");
  tm->StartTimer(t1);

  PGraph G = TUNGraph::Load(*SIn_p);
  tm->StopTimer(t1);
  printTime(tm, t1);
  printf("Benchmark Graph Conversions\n");
 
  // get UNGraph
  {
    int tid = tm->AddTimer("Convert TUNGraph->TUNGraph");
    tm->StartTimer(tid);
    PUNGraph UNG = TSnap::ConvertGraph<PUNGraph>(G);
    tm->StopTimer(tid);
    printTime(tm, tid);
    //IAssert(UNG->IsOk());
  }
  
  // get NGraph
  {
    int tid = tm->AddTimer("Convert TUNGraph->TNGraph");
    tm->StartTimer(tid);
    PNGraph NG = TSnap::ConvertGraph<PNGraph>(G);
    tm->StopTimer(tid);
    printTime(tm, tid);
    //IAssert(NG->IsOk());
  }

  // get NEGraph
  {
    int tid = tm->AddTimer("Convert TUNGraph->TNEGraph");
    tm->StartTimer(tid);
    PNEGraph NEG = TSnap::ConvertGraph<PNEGraph>(G);
    tm->StopTimer(tid);
    printTime(tm, tid);
    //IAssert(NEG->IsOk());
  }

  return 0;
}

