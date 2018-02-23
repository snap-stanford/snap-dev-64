#include "stdafx.h"
#include <time.h>

void printConversionTime(clock_t t0, clock_t t1, TUInt64 nnodes, TUInt64 nedges, const char * ConversionType) {
    printf("Convert TUNGraph->%s (%s nodes, %s edges): %f\n",
	   ConversionType,
  	   TUInt64::GetStr(nnodes).CStr(),
  	   TUInt64::GetStr(nedges).CStr(),	   
  	   ((float)t1-t0)/CLOCKS_PER_SEC);
}


int main(int argc, char* argv[]) {

  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("ragm. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  
  const bool useTwitter = Env.GetIfArgPrefixBool("-t:", false, "Run conversions on Twitter Network");

  const bool runAll = Env.GetIfArgPrefixBool("-a:", false, "Run all conversions");
  const bool runUNDir = Env.GetIfArgPrefixBool("-u:", false, "Run conversion: TUNGraph->TUNGraph");  
  const bool runDir = Env.GetIfArgPrefixBool("-d:", false, "Run conversion: TUNGraph->TNGraph");
  const bool runNet = Env.GetIfArgPrefixBool("-e:", false, "Run conversion: TUNGraph->TNEANet");


  // File input
  const char * binaryUNGraph = NULL;
  if (useTwitter) {
    binaryUNGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.graph";
    printf("Twitter Graph\n");
  } else {
    binaryUNGraph = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.graph";
    printf("Journal Graph\n");
  }
  //const char * binaryNGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.ngraph";
  //const char * binaryNEANet = "/lfs/local/0/gaspar09/networks/twitter_rv.neanet";  
  
  //const char * binaryNGraph = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.ngraph";
  //const char * binaryNEANet = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.neanet";
  PSIn SIn_p = TFIn::New(binaryUNGraph);

  clock_t t0 = clock();
  PUNGraph G = TUNGraph::Load(*SIn_p);
  clock_t t1 = clock();
  printf("Load TUNGraph (%s nodes, %s edges) Binary: %f\n",
	 TUInt64::GetStr(G->GetNodes()).CStr(),
	 TUInt64::GetStr(G->GetEdges()).CStr(),
	 ((float)t1-t0)/CLOCKS_PER_SEC);
 
  if (runAll || runUNDir) {
    // get UNGraph
    clock_t t0 = clock();
    PUNGraph H = TSnap::ConvertGraph<PUNGraph>(G);
    clock_t t1 = clock();
    printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), "TUNGraph");

  }
  if (runAll || runDir) {
    // get NGraph
    clock_t t0 = clock();
    PNGraph H = TSnap::ConvertGraph<PNGraph>(G);
    clock_t t1 = clock();
    printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), "TNGraph");
  }
  if (runAll || runNet) {
    // get NEGraph
    clock_t t0 = clock();
    PNEANet H = TSnap::ConvertGraph<PNEANet>(G);
    clock_t t1 = clock();
    printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), "TNEANet");
  }

  return 0;
}
