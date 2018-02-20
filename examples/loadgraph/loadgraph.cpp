#include "stdafx.h"
#include <time.h>

// void printTime(int t0, int t1) {
//   printf("%s: %.2f\n", tm.GetTimerNm(id).CStr(), tm.GetTimerSec(id));
// }

int main(int argc, char* argv[]) {
  //// what type of graph do you want to use?

  //typedef TPt<TNodeNet<TInt> > PGraph;
  //typedef TPt<TNodeEdgeNet<TInt, TInt> > PGraph;

  // File input
  // const TStr InFNm = TStr("/lfs/local/0/gaspar09/networks/soc-LiveJournal1.txt");
  const TStr InFNm = TStr("/lfs/local/0/gaspar09/networks/twitter_rv.txt");
  
  // File output
  // const char * smallBinary = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.graph";
  // const char * twitterBinary = "/lfs/local/0/gaspar09/networks/twitter_rv.graph";
  // const TStr OutFnm = TStr(tmpout);
  
  printf("Benchmark Graph Loading\n");
  
  {
    typedef PNGraph PGraph; // directed graph
    // const char * binaryNGraph = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.ngraph";
    const char * binaryNGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.ngraph";
    PSIn SIn_p = TFIn::New(binaryNGraph);
    clock_t t0 = clock();
    PGraph G = TNGraph::Load(*SIn_p);
    printf("Load TNGraph Binary: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);

  }

  {
    typedef PNEANet PGraph; // neanet graph
    //const char * binaryNEANet = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.neanet";
    const char * binaryNEANet = "/lfs/local/0/gaspar09/networks/twitter_rv.neanet";
    PSIn SIn_p = TFIn::New(binaryNEANet);
    clock_t t0 = clock();
    PGraph G = TNEANet::Load(*SIn_p);
    printf("Load TNEANet Binary: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);

  }
  
  // { 
  //   typedef PUNGraph PGraph; // undirected graph    
  //   clock_t t0 = clock();
  //   PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
  //   printf("Load TUNGraph: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC); 

  //   save the undirected graph binary for conversions
  //   PSOut SOut_p = TFOut::New("/lfs/local/0/gaspar09/networks/twitter_rv.graph");
  //   clock_t t2 = clock();
  //   G->Save(*SOut_p);
  //   printf("Save Binary TUNGraph: %f\n", ((float)clock()-t2)/CLOCKS_PER_SEC); 
  // }

  // {
  //   typedef PNGraph PGraph; // directed graph
  //   clock_t t0 = clock();
  //   PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
  //   printf("Load TNGraph: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);
    
  //   // save the undirected graph binary for conversions
  //   PSOut SOut_p = TFOut::New("/lfs/local/0/gaspar09/networks/twitter_rv.ngraph");
  //   // PSOut SOut_p = TFOut::New("/lfs/local/0/gaspar09/networks/soc-LiveJournal1.ngraph");
  //   clock_t t1 = clock();
  //   G->Save(*SOut_p);
  //   printf("Save Binary TNGraph: %f\n", ((float)clock()-t1)/CLOCKS_PER_SEC);
  // }

  // {
  //   typedef PNEANet PGraph; // neanet graph
  //   clock_t t0 = clock();
  //   PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
  //   printf("Load TNEANet: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);

  //   // save the undirected graph binary for conversions
  //   PSOut SOut_p = TFOut::New("/lfs/local/0/gaspar09/networks/twitter_rv.neanet");
  //   // PSOut SOut_p = TFOut::New("/lfs/local/0/gaspar09/networks/soc-LiveJournal1.neanet");
  //   clock_t t1 = clock();
  //   G->Save(*SOut_p);
  //   printf("Save Binary TNEANet: %f\n", ((float)clock()-t1)/CLOCKS_PER_SEC);
  // }

  // { 
  //   typedef PUNGraph PGraph; // undirected graph   
  //   PSIn SIn_p = TFIn::New(twitterBinary); 
  //   clock_t t0 = clock();
  //   PGraph G = TUNGraph::Load(*SIn_p); 
  //   printf("Load Binary TUNGraph: %f\n", ((float)clock()-t0)/CLOCKS_PER_SEC);     

  //   printf("bin loaded\n");
  //   PSOut SOut_p = TFOut::New(OutFnm);
  //   clock_t t2 = clock();
  //   G->Save(*SOut_p);
  //   printf("Save Binary TUNGraph: %f\n", ((float)clock()-t2)/CLOCKS_PER_SEC); 
  // }
 
  return 0;
}

