#include "stdafx.h"
#include "time.h"

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
  const char * smallBinary = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.graph";
  // const char * twitterBinary = "/lfs/local/0/gaspar09/networks/twitter_rv.graph";


  // File output
  const char * twitterBinaryNGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.ngraph";
  const char * twitterBinaryNEGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.negraph";

  printf("Benchmark Graph Load Binary\n");


  //PSIn SIn_p = TFIn::New(twitterBinary);
  PSIn SIn_p = TFIn::New(smallBinary);

  clock_t t0 = clock();
  PGraph G = TUNGraph::Load(*SIn_p);
  printf("Load TUNGraph Binary: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);

  printf("Benchmark Graph Conversions\n");
 
  // get UNGraph
  // {
  //   clock_t t0 = clock();
  //   PUNGraph UNG = TSnap::ConvertGraph<PUNGraph>(G);
  //   printf("Convert TUNGraph->TUNGraph: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);
  // }
  
  // get NGraph
  {
    clock_t t0 = clock();
    PNGraph NG = TSnap::ConvertGraph<PNGraph>(G);
    printf("Convert TUNGraph->TNGraph: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);
    
    // save Binary
    PSOut SOut_p = TFOut::New(twitterBinaryNGraph);
    clock_t t1 = clock();
    NG->Save(*SOut_p);
    printf("Save Binary TNGraph: %f\n", ((float)clock()-t1)/CLOCKS_PER_SEC);

  }

  // get NEGraph
  {
    clock_t t0 = clock();
    PNEGraph NEG = TSnap::ConvertGraph<PNEGraph>(G);
    printf("Convert TUNGraph->TNEGraph: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);

    // save Binary
    PSOut SOut_p = TFOut::New(twitterBinaryNEGraph);
    clock_t t1 = clock();
    NEG->Save(*SOut_p);
    printf("Save Binary TNEGraph: %f\n", ((float)clock()-t1)/CLOCKS_PER_SEC);

  }

  return 0;
}

