#include "stdafx.h"
#include <time.h>

void printConversionTime(clock_t t0, clock_t t1, TInt64 nnodes, TInt64 nedges,
			 const TStr &ConversionTypeSrc, const TStr &ConversionTypeDst) {
    printf("Convert %s->%s (%s nodes, %s edges): %f\n",
	   ConversionTypeSrc.CStr(),
	   ConversionTypeDst.CStr(),
  	   TInt64::GetStr(nnodes).CStr(),
  	   TInt64::GetStr(nedges).CStr(),	   
  	   ((float)t1-t0)/CLOCKS_PER_SEC);
}


template <class PGraph>
PGraph LoadGraph(const TStr &InFNm, const TStr &InBinFNm) {
  clock_t t0 = clock();    
  PGraph G;
  // If Binary File provided use that instead
  if (InBinFNm.Len() != 0) {
    TFIn GFIn(InBinFNm);
    G = PGraph::TObj::Load(GFIn);
  } 
  else {
    G = TSnap::LoadEdgeList<PGraph>(InFNm);
  }
  clock_t t1 = clock();
  printf("Load Graph (%s nodes, %s edges): %f\n",
	 TUInt64::GetStr(G->GetNodes()).CStr(),
	 TUInt64::GetStr(G->GetEdges()).CStr(),
	 ((float)t1-t0)/CLOCKS_PER_SEC);
  return G;
}



int main(int argc, char* argv[]) {

  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("ragm. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input edgelist file name");
  const TStr InBinFNm = Env.GetIfArgPrefixStr("-b:", "", "Input graph binary file name");

  // Arguments specifying conversion types
  const TStr InGraphType = Env.GetIfArgPrefixStr("-s:", "TUNGraph", "Specify Graph type to convert from: TTable, TUNGraph, TNGraph, or TNEANet.");
  const TStr OutGraphType = Env.GetIfArgPrefixStr("-d:", "TNGraph", "Specify Graph type to convert to: TUNGraph, TNGraph, TNEANet, or TTable.");

  const bool FromTable = (InGraphType == "TTable") ? true : false;
  const bool FromTUNGraph = (InGraphType == "TUNGraph") ? true : false;
  const bool FromTNGraph = (InGraphType == "TNGraph") ? true : false;
  const bool FromTNEANet = (InGraphType == "TNEANet") ? true : false;
  
  const bool ToTUNGraph = (OutGraphType == "TUNGraph") ? true : false;
  const bool ToTNGraph = (OutGraphType == "TNGraph") ? true : false;
  const bool ToTNEANet = (OutGraphType == "TNEANet") ? true : false;
  const bool ToTTable = (OutGraphType == "TTable") ? true : false;
  const bool ToTNGraphMP = (OutGraphType == "TNGraphMP") ? true : false;
  const bool ToTNEANetMP = (OutGraphType == "TNEANetMP") ? true : false;

  if (FromTable) {
    TTableContext Context = TTableContext();

    Schema s = Schema();
    s.Add(TPair<TStr, TAttrType>("src", atInt));
    s.Add(TPair<TStr, TAttrType>("dst", atInt));

    clock_t t_table0 = clock();
    PTable T = TTable::LoadSS(s, InFNm, &Context, '\t');
    clock_t t_table1 = clock();
    printf("Load Table: %f\n", ((float)t_table1-t_table0)/CLOCKS_PER_SEC);

    TStr64V OrderBy;
    OrderBy.Add("src");
    OrderBy.Add("dst");
                                                                        
    if (ToTUNGraph) {
      // get UNGraph
      // ToGraph has errors when converting to TUNGraph
      clock_t t0 = clock();
      PUNGraph H = TSnap::ToGraph<PUNGraph>(T, "src", "dst", aaMin);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType); 
    }
    // TODO: ADD the other conversions for ToGraph (TNGraph) and ToNetwork (TNEANet)
    // HERE

    if (ToTNGraphMP) {                                                                        
      // get NGraph
      clock_t t0 = clock();                                                                 
      PNGraphMP H = TSnap::ToGraphMP<PNGraphMP>(T, "src", "dst");                            
      clock_t t1 = clock();                                                                 
      H->IsOk(false);                                                                       
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType); 
    }                                                                                       
    if (ToTNEANetMP) {                                                                        
      // get NEANetMP
      clock_t t0 = clock();                    
      PNEANetMP H = TSnap::ToNetworkMP<PNEANetMP>(T, "src", "dst", aaMin);                        
      clock_t t1 = clock();                                                                 
      H->IsOk(false);                                                                       
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType); 
    }                                                                 
    if (ToTUNGraph) {
      // get UNGraph
      clock_t t0 = clock();
      PUNGraph H = TSnap::ConvertGraphTable<PUNGraph>(T, OrderBy);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTNGraph) {
      // get NGraph
      clock_t t0 = clock();
      PNGraph H = TSnap::ConvertGraphTable<PNGraph>(T, OrderBy);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTNEANet) {
      // get NEANet
      clock_t t0 = clock();
      PNEANet H = TSnap::ConvertGraphTable<PNEANet>(T, OrderBy);  
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
  }
  else if (FromTUNGraph) {

    PUNGraph G = LoadGraph<PUNGraph>(InFNm, InBinFNm);

    if (ToTUNGraph) {
      // get UNGraph
      clock_t t0 = clock();
      PUNGraph H = TSnap::ConvertGraphFast<PUNGraph>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTNGraph) {
      // get NGraph
      clock_t t0 = clock();
      PNGraph H = TSnap::ConvertGraphFast<PNGraph>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTNEANet) {
      // get NEANet
      clock_t t0 = clock();
      //PNEANet H = TSnap::ConvertMultiGraph<PNEANet>(G);
      PNEANet H = TSnap::ConvertGraphFast<PNEANet>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTTable) {
      // get Table
      clock_t t0 = clock();
      PTable T = TSnap::ToTable(G);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, G->GetNodes(), T->GetNumRows(), InGraphType, OutGraphType);
    }
  } else if (FromTNGraph) {
 
    PNGraph G = LoadGraph<PNGraph>(InFNm, InBinFNm);

    if (ToTUNGraph) {
      // get UNGraph
      clock_t t0 = clock();
      PUNGraph H = TSnap::ConvertGraphFast<PUNGraph>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTNGraph) {
      // get NGraph
      clock_t t0 = clock();
      PNGraph H = TSnap::ConvertGraphFast<PNGraph>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTNEANet) {
      // get NEANet
      clock_t t0 = clock();
      PNEANet H = TSnap::ConvertGraphFast<PNEANet>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTTable) {
      // get Table
      clock_t t0 = clock();
      PTable T = TSnap::ToTable(G);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, G->GetNodes(), T->GetNumRows(), InGraphType, OutGraphType);
    }
  } else if (FromTNEANet) {

    PNEANet G = LoadGraph<PNEANet>(InFNm, InBinFNm);

    if (ToTUNGraph) {
      // get UNGraph
      clock_t t0 = clock();
      PUNGraph H = TSnap::ConvertGraphFast<PUNGraph>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTNGraph) {
      // get NGraph
      clock_t t0 = clock();
      PNGraph H = TSnap::ConvertGraphFast<PNGraph>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTNEANet) {
      // get NEANet
      clock_t t0 = clock();
      PNEANet H = TSnap::ConvertGraphFast<PNEANet>(G);
      clock_t t1 = clock();
      H->IsOk(false);
      printConversionTime(t0, t1, H->GetNodes(), H->GetEdges(), InGraphType, OutGraphType);
    }
    if (ToTTable) {
      // get Table
      clock_t t0 = clock();
      PTable T = TSnap::ToTable(G);
      clock_t t1 = clock();
      //H->IsOk(false);
      printConversionTime(t0, t1, G->GetNodes(), T->GetNumRows(), InGraphType, OutGraphType);
    }
  }
  return 0;
}
