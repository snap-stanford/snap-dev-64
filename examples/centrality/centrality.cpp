#include "stdafx.h"

int main(int argc, char *argv[]) {
    Env = TEnv(argc, argv, TNotify::StdNotify);
    Env.PrepArgs(TStr::Fmt("Node Centrality. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
    TExeTm ExeTm;
    Try
    const TStr InFNm = Env.GetIfArgPrefixStr("-i:", argv[2], "Input un/directed graph");
    const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", argv[4], "Output file");
    const TStr OutMetaFNm = Env.GetIfArgPrefixStr("-m:", argv[6], "Output metadata file");
    printf("Loading %s...", InFNm.CStr());
    PNGraph Graph = TSnap::LoadEdgeList<PNGraph>(InFNm);
    //PNGraph Graph = TSnap::GenRndGnm<PNGraph>(10, 10);
    //TGraphViz::Plot(Graph, gvlNeato, InFNm + ".gif", InFNm, true);
    printf("nodes:%s  edges:%s\n",
           TUInt64::GetStr(Graph->GetNodes()).CStr(),
           TUInt64::GetStr(Graph->GetEdges()).CStr());
    PUNGraph UGraph = TSnap::ConvertGraph<PUNGraph>(Graph); // undirected version of the graph
    TIntFlt64H BtwH, EigH;
    TIntFlt64H PRankH;
    TIntFlt64H HubH, AuthH;
    TIntFlt64H CcfH;
    TIntFlt64H CloseH;
    printf("Computing...\n");
    printf("Treat graph as DIRECTED: ");
    printf(" PageRank... ");
    TSnap::GetPageRank_v1(Graph, PRankH, 0.85);
    printf(" Hubs&Authorities...");
    TSnap::GetHits(Graph, HubH, AuthH);
    printf("\nTreat graph as UNDIRECTED: ");
    printf(" Eigenvector...");
    TSnap::GetEigenVectorCentr(UGraph, EigH);
    printf(" Clustering...");
    TSnap::GetNodeClustCf(UGraph, CcfH);
    printf(" Betweenness (SLOW!)...");
    TSnap::GetBetweennessCentr(UGraph, BtwH, 1);
    printf(" Constraint (SLOW!)...");
    TNetConstraint <PUNGraph> NetC(UGraph, true);
    printf(" Closeness (SLOW!)...");
    for (TUNGraph::TNodeI NI = UGraph->BegNI(); NI < UGraph->EndNI(); NI++) {
        const int64 NId = NI.GetId();
        CloseH.AddDat(NId, TSnap::GetClosenessCentr<PUNGraph>(UGraph, NId, false));
    }
    printf("\nDONE! saving...");
    FILE *F = fopen(OutFNm.CStr(), "wt");
    FILE *Fmeta = fopen(OutMetaFNm.CStr(), "wt");
    fprintf(Fmeta, "# Input: %s\tOutput: %s\n", InFNm.CStr(), OutFNm.CStr());
    fprintf(Fmeta, "# Nodes: %s\tEdges: %s\n",
            TUInt64::GetStr(Graph->GetNodes()).CStr(),
            TUInt64::GetStr(Graph->GetEdges()).CStr());
    fprintf(F,
            "NodeId,Degree,Closeness,Betweennes,EigenVector,NetworkConstraint,ClusteringCoefficient,PageRank,HubScore,AuthorityScore\n");
//    fprintf(F,
//            "#NodeId\tDegree\tCloseness\tBetweennes\tEigenVector\tNetworkConstraint\tClusteringCoefficient\tPageRank\tHubScore\tAuthorityScore\n");
    for (TUNGraph::TNodeI NI = UGraph->BegNI(); NI < UGraph->EndNI(); NI++) {
        const int64 NId = NI.GetId();
        const double DegCentr = UGraph->GetNI(NId).GetDeg();
        const double CloCentr = CloseH.GetDat(NId);
        const double BtwCentr = BtwH.GetDat(NId);
        const double EigCentr = EigH.GetDat(NId);
        const double Constraint = NetC.GetNodeC(NId);
        const double ClustCf = CcfH.GetDat(NId);
        const double PgrCentr = PRankH.GetDat(NId);
        const double HubCentr = HubH.GetDat(NId);
        const double AuthCentr = AuthH.GetDat(NId);
        fprintf(F, "%lli,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", NId,
                DegCentr, CloCentr, BtwCentr, EigCentr, Constraint, ClustCf, PgrCentr, HubCentr, AuthCentr);
//        fprintf(F, "%lli\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", NId,
//                DegCentr, CloCentr, BtwCentr, EigCentr, Constraint, ClustCf, PgrCentr, HubCentr, AuthCentr);
    }
    fclose(F);
    Catch
            printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
    return 0;
}
