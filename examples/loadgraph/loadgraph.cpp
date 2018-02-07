#include "stdafx.h"

int main(int argc, char* argv[]) {
  //// what type of graph do you want to use?

  //typedef TPt<TNodeNet<TInt> > PGraph;
  //typedef TPt<TNodeEdgeNet<TInt, TInt> > PGraph;

  // File input
  //const char * small = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.txt";
  const char * twitter = "/lfs/local/0/gaspar09/networks/twitter_rv.txt";
  const TStr InFNm = TStr(twitter);
  
  // File output
  //const char * smallBinary = "/lfs/local/0/gaspar09/networks/soc-LiveJournal1.graph";
  const char * twitterBinary = "/lfs/local/0/gaspar09/networks/twitter_rv.graph";
  const TStr OutFnm = TStr(twitterBinary);
  
  printf("Benchmark Graph Loading\n");
  
  { 
    typedef PUNGraph PGraph; // undirected graph   
    static TStopwatch * sw = TStopwatch::GetInstance();
    sw->Start(TStopwatch::TExperiment::ConstructGraph);
    PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
    sw->Stop(TStopwatch::TExperiment::ConstructGraph);
    printf("TUNGraph: %f\n", sw->Sum(TStopwatch::TExperiment::ConstructGraph));

    // save the undirected graph binary for conversions
    PSOut SOut_p = TFOut::New(OutFnm);
    G->Save(*SOut_p);
  }
  
  { 
    typedef PNGraph PGraph; // directed graph   
    static TStopwatch * sw = TStopwatch::GetInstance();
    sw->Start(TStopwatch::TExperiment::ConstructGraph);
    PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
    sw->Stop(TStopwatch::TExperiment::ConstructGraph);
    printf("TNGraph: %f\n", sw->Sum(TStopwatch::TExperiment::ConstructGraph));
  }

  { 
    typedef PNEGraph PGraph; //  directed multigraph   
    static TStopwatch * sw = TStopwatch::GetInstance();
    sw->Start(TStopwatch::TExperiment::ConstructGraph);
    PGraph G = TSnap::LoadEdgeList<PGraph>(InFNm);
    sw->Stop(TStopwatch::TExperiment::ConstructGraph);
    printf("TNEGraph: %f\n", sw->Sum(TStopwatch::TExperiment::ConstructGraph));
  }

  return 0;
}

