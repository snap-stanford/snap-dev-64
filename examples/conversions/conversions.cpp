#include "stdafx.h"
#include <time.h>

void printConversionTime(clock_t t0, clock_t t1, TUInt64 nnodes, TUInt64 nedges,
			 const char * ConversionTypeSrc, const char * ConversionTypeDst) {
    printf("Convert %s->%s (%s nodes, %s edges): %f\n",
	   ConversionTypeSrc,
	   ConversionTypeDst,
  	   TUInt64::GetStr(nnodes).CStr(),
  	   TUInt64::GetStr(nedges).CStr(),	   
  	   ((float)t1-t0)/CLOCKS_PER_SEC);
}


int main(int argc, char* argv[]) {

  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("ragm. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  
  const bool useTwitter = Env.GetIfArgPrefixBool("-t:", false, "Run conversions on Twitter Network");
  const bool fromDir =  Env.GetIfArgPrefixBool("-sd:", false, "Convert from TNGraph to ...");
  const bool fromTable =  Env.GetIfArgPrefixBool("-st:", false, "Convert from TTable to ...");

  const bool runAll = Env.GetIfArgPrefixBool("-a:", false, "Run all conversions");
  const bool runUNDir = Env.GetIfArgPrefixBool("-u:", false, "Conversion to: TUNGraph");  
  const bool runDir = Env.GetIfArgPrefixBool("-d:", false, "Conversion to: TNGraph");
  const bool runNet = Env.GetIfArgPrefixBool("-e:", false, "Conversion to: TNEANet");
  

  // File input
  const char * binaryUNGraph = NULL;
  const char * binaryNGraph = NULL;
  //const char * binaryNEANet = NULL;
  const char * tabletxt = NULL;

  if (useTwitter) {
    binaryUNGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.graph";
    binaryNGraph = "/lfs/local/0/gaspar09/networks/twitter_rv.ngraph";
    //binaryNEANet = "/lfs/local/0/gaspar09/networks/twitter_rv.neanet"; 
    tabletxt = "/lfs/local/0/gaspar09/networks/twitter_rv.txt";
    printf("Twitter Graph\n");
  } else {
    binaryUNGraph = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.graph";
    binaryNGraph = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.ngraph";
    //binaryNEANet = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.neanet";
    tabletxt = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.txt";
    printf("Journal Graph\n");
  }

  if (fromTable) {
    const char * GraphTypeSrc = "TTable";
    TTableContext Context = TTableContext();

    Schema s = Schema();
    s.Add(TPair<TStr, TAttrType>("col1", atInt));
    s.Add(TPair<TStr, TAttrType>("col2", atInt));

    PTable T = TTable::LoadSS(s, tabletxt, &Context, '\t');
    TStr64V OrderBy;
    OrderBy.Add("col1");
    OrderBy.Add("col2");

    if (runAll || runUNDir) {
      // get UNGraph
      clock_t t0 = clock();
      PUNGraph H = TSnap::ConvertGraphTable<PUNGraph>(T, OrderBy);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TUNGraph");
    }
    if (runAll || runDir) {
      // get NGraph
      clock_t t0 = clock();
      PNGraph H = TSnap::ConvertGraphTable<PNGraph>(T, OrderBy);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TNGraph");
    }
    if (runAll || runNet) {
      // get NEANet
      clock_t t0 = clock();
      PNEANet H = TSnap::ConvertGraphTable<PNEANet>(T, OrderBy);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TNEANet");
    }

  }
  else if (! fromDir) {
    const char * GraphTypeSrc = "TUNGraph";
    PSIn FInPt = TFIn::New(binaryUNGraph);
    clock_t t0 = clock();
    PUNGraph G = TUNGraph::Load(*FInPt);
    clock_t t1 = clock();
    printf("Load TUNGraph (%s nodes, %s edges) Binary: %f\n",
	   TUInt64::GetStr(G->GetNodes()).CStr(),
	   TUInt64::GetStr(G->GetEdges()).CStr(),
	   ((float)t1-t0)/CLOCKS_PER_SEC);
    
    if (runAll || runUNDir) {
      // get UNGraph
      clock_t t0 = clock();
      PUNGraph H = TSnap::ConvertGraphFast<PUNGraph>(G);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TUNGraph");
    }
    if (runAll || runDir) {
      // get NGraph
      clock_t t0 = clock();
      PNGraph H = TSnap::ConvertGraphFast<PNGraph>(G);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TNGraph");
    }
    if (runAll || runNet) {
      // get NEANet
      clock_t t0 = clock();
      //PNEANet H = TSnap::ConvertMultiGraph<PNEANet>(G);
      PNEANet H = TSnap::ConvertGraphFast<PNEANet>(G);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TNEANet");
    }
  } else {
    // G is an instance of TNGraph
    const char * GraphTypeSrc = "TNGraph";
    PSIn FInPt = TFIn::New(binaryNGraph);
    clock_t t0 = clock();
    PNGraph G = TNGraph::Load(*FInPt);
    clock_t t1 = clock();
    printf("Load TNGraph (%s nodes, %s edges) Binary: %f\n",
	   TUInt64::GetStr(G->GetNodes()).CStr(),
	   TUInt64::GetStr(G->GetEdges()).CStr(),
	   ((float)t1-t0)/CLOCKS_PER_SEC);
    
    if (runAll || runUNDir) {
      // get UNGraph
      clock_t t0 = clock();
      PUNGraph H = TSnap::ConvertGraphFast<PUNGraph>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TUNGraph");
    }
    if (runAll || runDir) {
      // get NGraph
      clock_t t0 = clock();
      PNGraph H = TSnap::ConvertGraphFast<PNGraph>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TNGraph");
    }
    if (runAll || runNet) {
      // get NEANet
      clock_t t0 = clock();
      PNEANet H = TSnap::ConvertMultiGraph<PNEANet>(G);
      clock_t t1 = clock();
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), GraphTypeSrc, "TNEANet");
    }
  }
  return 0;
}
