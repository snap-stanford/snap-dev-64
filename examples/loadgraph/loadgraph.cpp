#include "stdafx.h"

void printTime(PTmProfiler tm, int id) {
  printf("%s: %.2f\n", tm->GetTimerNm(id).CStr(), tm->GetTimerSec(id));
}

int main(int argc, char* argv[]) {
  //// what type of graph do you want to use?

  //typedef TPt<TNodeNet<TInt> > PGraph;
  //typedef TPt<TNodeEdgeNet<TInt, TInt> > PGraph;

  // File input
  // const char * small = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.txt";
  const char * twitter = "/lfs/local/0/gaspar09/networks/twitter_rv.txt";
  const TStr InFNm = TStr(twitter);
  
  // File output
  //const char * smallBinary = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.graph";
  // const char * twitterBinary = "/lfs/local/0/gaspar09/networks/twitter_rv.graph";
  // const TStr OutFnm = TStr(twitterBinary);
  
  printf("Benchmark Graph Loading\n");

  PTmProfiler tm = TTmProfiler::New();

  { 
    typedef PUNGraph PGraph; // undirected graph   
    int id = tm->AddTimer("Load TUNGraph text");
    tm->StartTimer(id);
    PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
    tm->StopTimer(id);
    printTime(tm, id);
    // save the undirected graph binary for conversions
    // PSOut SOut_p = TFOut::New(OutFnm);
    // G->Save(*SOut_p);
  }
  
  { 
    typedef PNGraph PGraph; // directed graph   
    int id = tm->AddTimer("Load TNGraph text");
    tm->StartTimer(id);
    PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
    tm->StopTimer(id);
    printTime(tm, id);
  }

  { 
    typedef PNEGraph PGraph; // directed graph   
    int id = tm->AddTimer("Load TNEGraph text");
    tm->StartTimer(id);
    PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
    tm->StopTimer(id);
    printTime(tm, id);
  }

  return 0;
}

