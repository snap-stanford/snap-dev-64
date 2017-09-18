#include "stdafx.h"

int main(int argc, char* argv[]) {
    printf("Testing iterators:\n");

    // vectors
    TIntV IntV = TIntV::GetV(1,2,3,4,5);
    // we can iterate over vector and print out values
    for (const int Val : IntV) {
        printf("%d ", Val);
    }
    printf("\n");
    // we can access values by reference and modify them
    for (TInt& Val : IntV) {
        Val = Val * Val;
    }
    // print out updated vector
    for (const int Val : IntV) {
        printf("%d ", Val);
    }
    printf("\n");

    // hash tables
    TIntH IntH;
    for (int i = 0; i < 5; i++) {
        IntH.AddDat(i, i*i);
    }
    // we can iterate and print out keys and dats
    for (const auto& KeyDat : IntH) {
        printf("(%d -> %d) ", KeyDat.Key.Val, KeyDat.Dat.Val);
    }
    printf("\n");
    // we can access dats by reference and modify them
    for (auto& KeyDat : IntH) {
        KeyDat.Dat = KeyDat.Dat * KeyDat.Key;
    }
    // print out updated map
    for (const auto& KeyDat : IntH) {
        printf("(%d -> %d) ", KeyDat.Key.Val, KeyDat.Dat.Val);
    }
    printf("\n");

    // hash set
    TIntSet IntSet = TIntSet(TIntV::GetV(1,2,3,4,5));
    for (const auto& Key : IntSet) {
        printf("%d ", Key.Val);
    }
    printf("\n");

    // undirected graph iterator
    PUNGraph UNGraph = TUNGraph::New();
    for (int ni = 0; ni < 5; ni++) {
        UNGraph->AddNode(ni);
    }
    UNGraph->AddEdge(0, 1);
    UNGraph->AddEdge(0, 2);
    UNGraph->AddEdge(0, 3);
    UNGraph->AddEdge(3, 4);
    // we can iterate over nodes
    for (const auto& NI : UNGraph->GetNI()) {
        printf("%s:%s ", ToCStr(NI.GetId()), ToCStr(NI.GetDeg()));
    }
    printf("\n");
    for (auto& NI : UNGraph->GetNI()) {
        printf("%s:%s ", ToCStr(NI.GetId()), ToCStr(NI.GetDeg()));
        NI.SortNIdV();
    }
    printf("\n");
    // we can also iterate over edges
    for (const auto& EI : UNGraph->GetEI()) {
        printf("(%s -> %s) ", ToCStr(EI.GetSrcNId()), ToCStr(EI.GetDstNId()));
    }
    printf("\n");

    // directed graph iterator
    PNGraph NGraph = TNGraph::New();
    for (int ni = 0; ni < 5; ni++) {
        NGraph->AddNode(ni);
    }
    NGraph->AddEdge(0, 1);
    NGraph->AddEdge(1, 0);
    NGraph->AddEdge(0, 2);
    NGraph->AddEdge(0, 3);
    NGraph->AddEdge(3, 4);
    // we can iterate over nodes
    for (const auto& NI : NGraph->GetNI()) {
        printf("%s:%s ", ToCStr(NI.GetId()), ToCStr(NI.GetDeg()));
    }
    printf("\n");
    // we can also iterate over edges
    for (const auto& EI : NGraph->GetEI()) {
        printf("(%s -> %s) ", ToCStr(EI.GetSrcNId()), ToCStr(EI.GetDstNId()));
    }
    printf("\n");

    // directed multigraph iterator
    PNEGraph NEGraph = TNEGraph::New();
    for (int ni = 0; ni < 5; ni++) {
        NEGraph->AddNode(ni);
    }
    NEGraph->AddEdge(0, 1);
    NEGraph->AddEdge(0, 1);
    NEGraph->AddEdge(0, 2);
    NEGraph->AddEdge(0, 3);
    NEGraph->AddEdge(3, 4);
    // we can iterate over nodes
    for (const auto& NI : NEGraph->GetNI()) {
        printf("%s:%s ", ToCStr(NI.GetId()), ToCStr(NI.GetDeg()));
    }
    printf("\n");
    // we can also iterate over edges
    for (const auto& EI : NEGraph->GetEI()) {
        printf("(%s -> %s) ", ToCStr(EI.GetSrcNId()), ToCStr(EI.GetDstNId()));
    }
    printf("\n");

    // directed multigraph iterator
    PNEANet NEANet = TNEANet::New();
    for (int ni = 0; ni < 5; ni++) {
        NEANet->AddNode(ni);
    }
    NEANet->AddEdge(0, 1, 123);
    NEANet->AddEdge(0, 2, 234);
    NEANet->AddEdge(0, 3, 345);
    NEANet->AddEdge(3, 4, 456);
    // we can iterate over nodes
    for (const auto& NI : NEANet->GetNI()) {
        printf("%s:%s ", ToCStr(NI.GetId()), ToCStr(NI.GetDeg()));
    }
    printf("\n");
    // we can also iterate over edges
    for (const auto& EI : NEANet->GetEI()) {
        printf("(%s: %s -> %s) ", ToCStr(EI.GetId()), ToCStr(EI.GetSrcNId()), ToCStr(EI.GetDstNId()));
    }
    printf("\n");

    // table
    Schema GradeS;
    GradeS.Add(TPair<TStr,TAttrType>("Class", atStr));
    GradeS.Add(TPair<TStr,TAttrType>("Area", atStr));
    GradeS.Add(TPair<TStr,TAttrType>("Quarter", atStr));
    GradeS.Add(TPair<TStr,TAttrType>("Grade 2011", atInt));
    GradeS.Add(TPair<TStr,TAttrType>("Grade 2012", atInt));
    GradeS.Add(TPair<TStr,TAttrType>("Grade 2013", atInt));
    TInt64V RelevantCols;
    RelevantCols.Add(0); RelevantCols.Add(1); RelevantCols.Add(2);
    RelevantCols.Add(3); RelevantCols.Add(4); RelevantCols.Add(5);
    TTableContext Context;
    PTable P = TTable::LoadSS(GradeS, "../../test/table/grades.txt", &Context, RelevantCols);
    // we can iterate over the rows
    for (auto RI : P->GetRI()) {
        printf("%s: %s %s %s %s %s %s\n", ToCStr(RI.GetRowIdx()), RI.GetStrAttr("Class").CStr(),
            RI.GetStrAttr("Area").CStr(), RI.GetStrAttr("Quarter").CStr(),
            ToCStr(RI.GetIntAttr("Grade 2011")), ToCStr(RI.GetIntAttr("Grade 2011")),
            ToCStr(RI.GetIntAttr("Grade 2011")));
    }

    return 0;
}
