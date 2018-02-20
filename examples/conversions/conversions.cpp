#include "stdafx.h"
#include <time.h>

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

  // File input
  const char * binaryUNGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.graph";
  //const char * binaryNGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.ngraph";
  //const char * binaryNEANet = "/lfs/local/0/gaspar09/networks/twitter_rv.neanet";

  binaryUNGraph = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.graph";
  //binaryNGraph = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.ngraph";
  //binaryNEANet = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.neanet";


  printf("Benchmark Graph Load Binary\n");

  PSIn SIn_p = TFIn::New(binaryUNGraph);

  clock_t t0 = clock();
  PGraph G = TUNGraph::Load(*SIn_p);
  clock_t t1 = clock();
  printf("Load TUNGraph (%s nodes, %s edges) Binary: %f\n",
	 TUInt64::GetStr(G->GetNodes()).CStr(),
	 TUInt64::GetStr(G->GetEdges()).CStr(),
	 ((float)t1-t0)/CLOCKS_PER_SEC);

  printf("Benchmark Graph Conversions\n");
 
  // get UNGraph
  {
    clock_t t0 = clock();
    PUNGraph UNG = TSnap::ConvertGraph<PUNGraph>(G);
    clock_t t1 = clock();
    printf("Convert TUNGraph->TUNGraph (%s nodes, %s edges): %f\n",
  	   TUInt64::GetStr(UNG->GetNodes()).CStr(),
  	   TUInt64::GetStr(UNG->GetEdges()).CStr(),	   
  	   ((float)t1-t0)/CLOCKS_PER_SEC);
  }
  
  // get NGraph
  {
    clock_t t0 = clock();
    PNGraph NG = TSnap::ConvertGraph<PNGraph>(G);
    clock_t t1 = clock();
    printf("Convert TUNGraph->TNGraph (%s nodes, %s edges) : %f\n",
  	   TUInt64::GetStr(NG->GetNodes()).CStr(),
  	   TUInt64::GetStr(NG->GetEdges()).CStr(),
  	   ((float)t1-t0)/CLOCKS_PER_SEC);
  }

  // get NEGraph
  {
    clock_t t0 = clock();
    PNEANet NEG = TSnap::ConvertGraph<PNEANet>(G);
    clock_t t1 = clock();
    printf("Convert TUNGraph->TNEANet (%s nodes, %s edges): %f\n",
	   TUInt64::GetStr(NEG->GetNodes()).CStr(),
	   TUInt64::GetStr(NEG->GetEdges()).CStr(),
	   ((float)t1-t0)/CLOCKS_PER_SEC);
  }

  return 0;
}

