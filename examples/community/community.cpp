#include "stdafx.h"

int main(int argc, char *argv[]) {
    Env = TEnv(argc, argv, TNotify::StdNotify);
    Env.PrepArgs(
            TStr::Fmt("Network community detection. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
    TExeTm ExeTm;
    Try
    const TStr InFNm = Env.GetIfArgPrefixStr("-i:", argv[2], "Input graph (undirected graph)");
    const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", argv[4], "Output file");
    const int CmtyAlg = Env.GetIfArgPrefixInt("-a:", 2,
                                              "Algorithm: 1:Girvan-Newman, 2:Clauset-Newman-Moore, 3:Infomap");

    PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(InFNm, false);
    //PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>("../as20graph.txt", false);
    //PUNGraph Graph = TSnap::GenRndGnm<PUNGraph>(5000, 10000); // generate a random graph

    TSnap::DelSelfEdges(Graph);
    TCnComV CmtyV;
    double Q = 0.0;
    TStr CmtyAlgStr;
    if (CmtyAlg == 1) {
        CmtyAlgStr = "Girvan-Newman";
        Q = TSnap::CommunityGirvanNewman(Graph, CmtyV);
    } else if (CmtyAlg == 2) {
        CmtyAlgStr = "Clauset-Newman-Moore";
        Q = TSnap::CommunityCNM(Graph, CmtyV);
    } else if (CmtyAlg == 3) {
        CmtyAlgStr = "Infomap";
        Q = TSnap::Infomap(Graph, CmtyV);
    } else { Fail; }

    FILE *F = fopen(OutFNm.CStr(), "wt");
    fprintf(F, "# Input: %s\n", InFNm.CStr());
    fprintf(F, "# Nodes: %s    Edges: %s\n",
            TUInt64::GetStr(Graph->GetNodes()).CStr(),
            TUInt64::GetStr(Graph->GetEdges()).CStr());

    fprintf(F, "# Algoritm: %s\n", CmtyAlgStr.CStr());
    if (CmtyAlg != 3) {
        fprintf(F, "# Modularity: %f\n", Q);
    } else {
        fprintf(F, "# Average code length: %f\n", Q);
    }
    fprintf(F, "# Communities: %s\n", TInt64::GetStr(CmtyV.Len()).CStr());

    fprintf(F, "# NId\tCommunityId\n");
    for (int64 c = 0; c < CmtyV.Len(); c++) {
        for (int64 i = 0; i < CmtyV[c].Len(); i++) {
            fprintf(F, "%s\t%s\n",
                    TInt64::GetStr(CmtyV[c][i].Val).CStr(),
                    TInt64::GetStr(c).CStr());
        }
    }
    fclose(F);

    Catch
            printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
    return 0;
}
