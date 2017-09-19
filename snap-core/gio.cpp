namespace TSnap {

// Reads the schema from the file (that is being parsed), and fills the SrcColId, DstColId, and the vectors with the index,
// within a given line, at which the source/destination nodes and edge attributes can be found in the file.
// The schema must have the format specified in WriteEdgeSchemaToFile.
int64 ReadEdgeSchemaFromFile(TSsParser& Ss, const char& Separator, int64& SrcColId, int64& DstColId, TStrInt64H& IntAttrEVals, TStrInt64H& FltAttrEVals, TStrInt64H& StrAttrEVals) {
  if (EDGES_START != Ss.GetFld(0)) return -1;
  for (int64 i = 1; i < Ss.GetFlds(); i++) {
    if (SRC_ID_NAME == Ss.GetFld(i)) {
      SrcColId = i-1;
      continue;
    }
    if (DST_ID_NAME == Ss.GetFld(i)) {
      DstColId = i-1;
      continue;
    }
    TStr Attr(Ss.GetFld(i));
    TStr AttrType;
    TStr AttrName;
    Attr.SplitOnCh(AttrType, ':', AttrName);
    if (AttrType == INT_TYPE_PREFIX) {
      IntAttrEVals.AddDat(AttrName, i-1);
    } else if (AttrType == FLT_TYPE_PREFIX) {
      FltAttrEVals.AddDat(AttrName, i-1);
    } else if (AttrType == STR_TYPE_PREFIX) {
      StrAttrEVals.AddDat(AttrName, i-1);
    } else {
      return -1;
    }
  }
  return 0;
}

// Reads the edges from the file being parsed and adds the nodes/edges and edge attributes
// at the positions specified by SrcColId, DstColId, IntAttrEVal, etc. to the Graph.
// Continues going through the file until it hits the sentinel line END_SENTINEL, the end of the file,
// or a schema line. Returns a bool indicating whether the current line in the TSsParser is a schema line.
bool ReadEdgesFromFile(TSsParser& Ss, const char& Separator, PNEANet& Graph, int64& SrcColId, int64& DstColId, TStrInt64H& IntAttrEVals, TStrInt64H& FltAttrEVals, TStrInt64H& StrAttrEVals) {
  int64 SrcNId, DstNId;
  while (Ss.Next()) {
    if (Ss.GetFlds() == 0) continue;
    if (END_SENTINEL == Ss.GetFld(0)) { return false; }
    if (EDGES_START == Ss.GetFld(0)) { return true; }
    if (NODES_START == Ss.GetFld(0)) { return true; }
    if (Ss.GetFld(0)[0] == '#') { continue; }
    if (! Ss.GetInt64(SrcColId, SrcNId) || ! Ss.GetInt64(DstColId, DstNId)) { continue; }
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
    int64 EId = Graph->AddEdge(SrcNId, DstNId);
    double FltAttrVal;
    for (TStrInt64H::TIter it = FltAttrEVals.BegI(); it < FltAttrEVals.EndI(); it++) {
      if (Ss.GetFlt(it.GetDat(), FltAttrVal)) {
        Graph->AddFltAttrDatE(EId, FltAttrVal, it.GetKey());
      }
    }
    int64 IntAttrVal;
    for (TStrInt64H::TIter it = IntAttrEVals.BegI(); it < IntAttrEVals.EndI(); it++) {
      if (Ss.GetInt64(it.GetDat(), IntAttrVal)) {
        Graph->AddIntAttrDatE(EId, IntAttrVal, it.GetKey());
      }
    }
    char* StrAttrVal;
    for (TStrInt64H::TIter it = StrAttrEVals.BegI(); it < StrAttrEVals.EndI(); it++) {
      StrAttrVal = Ss.GetFld(it.GetDat());
      if (NULL_VAL != StrAttrVal) {
        Graph->AddStrAttrDatE(EId, TStr(StrAttrVal), it.GetKey());
      }
    }
  }
  return false;
}


// Reads the node schema from the file, and fills the NId and the vectors with the index,
// within a given line, at which the node id and attributes can be found in the file.
// The schema must have the format specified in WriteNodeSchemaToFile.
int64 ReadNodeSchemaFromFile(TSsParser& Ss, const char& Separator, int64& NId, TStrInt64H& IntAttrNVals, TStrInt64H& FltAttrNVals, TStrInt64H& StrAttrNVals) {
  if (NODES_START != Ss.GetFld(0)) return -1;
  for (int64 i = 1; i < Ss.GetFlds(); i++) {
    if (NID_NAME == Ss.GetFld(i)) {
      NId = i-1;
      continue;
    }
    TStr Attr(Ss.GetFld(i));
    TStr AttrType;
    TStr AttrName;
    Attr.SplitOnCh(AttrType, ':', AttrName);
    if (AttrType == INT_TYPE_PREFIX) {
      IntAttrNVals.AddDat(AttrName, i-1);
    } else if (AttrType == FLT_TYPE_PREFIX) {
      FltAttrNVals.AddDat(AttrName, i-1);
    } else if (AttrType == STR_TYPE_PREFIX) {
      StrAttrNVals.AddDat(AttrName, i-1);
    } else {
      return -1;
    }
  }
  return 0;
}

// Reads the nodes from the file being parsed and adds the nodes and node attributes
// at the positions specified by NColId, IntAttrEVal, etc. to the Graph.
// Continues going through the file until it hits the sentinel line END_SENTINEL, the end of the file,
// or a schema line. Returns a bool indicating whether the current line in the TSsParser is a schema line.
bool ReadNodesFromFile(TSsParser& Ss, const char& Separator, PNEANet& Graph, int64& NColId, TStrInt64H& IntAttrNVals, TStrInt64H& FltAttrNVals, TStrInt64H& StrAttrNVals) {
  int64 NId;
  while (Ss.Next()) {
    if (Ss.GetFlds() == 0) continue;
    if (END_SENTINEL == Ss.GetFld(0)) { return false; }
    if (EDGES_START == Ss.GetFld(0)) { return true; }
    if (NODES_START == Ss.GetFld(0)) { return true; }
    if (Ss.GetFld(0)[0] == '#') { continue; }
    if (! Ss.GetInt64(NColId, NId)) { continue; }
    if (! Graph->IsNode(NId)) { Graph->AddNode(NId); }
    double FltAttrVal;
    for (TStrInt64H::TIter it = FltAttrNVals.BegI(); it < FltAttrNVals.EndI(); it++) {
      if (Ss.GetFlt(it.GetDat(), FltAttrVal)) {
        Graph->AddFltAttrDatN(NId, FltAttrVal, it.GetKey());
      }
    }
    int64 IntAttrVal;
    for (TStrInt64H::TIter it = IntAttrNVals.BegI(); it < IntAttrNVals.EndI(); it++) {
      if (Ss.GetInt64(it.GetDat(), IntAttrVal)) {
        Graph->AddIntAttrDatN(NId, IntAttrVal, it.GetKey());
      }
    }
    char* StrAttrVal;
    for (TStrInt64H::TIter it = StrAttrNVals.BegI(); it < StrAttrNVals.EndI(); it++) {
      StrAttrVal = Ss.GetFld(it.GetDat());
      if (NULL_VAL != StrAttrVal) {
        Graph->AddStrAttrDatN(NId, TStr(StrAttrVal), it.GetKey());
      }
    }
  }
  return false;
}

PNEANet LoadEdgeListNet(const TStr& InFNm, const char& Separator) {
  PNEANet Graph = PNEANet::New();
  TSsParser Ss(InFNm, Separator, true, false, false);
  bool isSchemaLine = false;

  while (isSchemaLine || Ss.Next()) {
    isSchemaLine = false;
    if (Ss.GetFlds() == 0) continue;
    if (NODES_START == Ss.GetFld(0)) {
      // Map node attribute names to column number in the file.
      TStrInt64H IntAttrNVals;
      TStrInt64H FltAttrNVals;
      TStrInt64H StrAttrNVals;
      int64 NColId = -1;
      ReadNodeSchemaFromFile(Ss, Separator, NColId, IntAttrNVals, FltAttrNVals, StrAttrNVals);
      isSchemaLine = ReadNodesFromFile(Ss, Separator, Graph, NColId, IntAttrNVals, FltAttrNVals, StrAttrNVals);
    } else if (EDGES_START == Ss.GetFld(0)) {
      // Map edge attribute names to column number in the file.
      TStrInt64H IntAttrEVals;
      TStrInt64H FltAttrEVals;
      TStrInt64H StrAttrEVals;
      int64 SrcColId = -1;
      int64 DstColId = -1;
      ReadEdgeSchemaFromFile(Ss, Separator, SrcColId, DstColId, IntAttrEVals, FltAttrEVals, StrAttrEVals);
      isSchemaLine = ReadEdgesFromFile(Ss, Separator, Graph, SrcColId, DstColId, IntAttrEVals, FltAttrEVals, StrAttrEVals);
    }
  }

  return Graph;
}

// Writes the schema out to the file, which consists of the NId and node attributes, separated by tabs.
// Node attributes are written in the format <Type>:<Name>, where Type is either Int, Flt, or Str.
void WriteNodeSchemaToFile(FILE *F, TStr64V &IntAttrNNames, TStr64V &FltAttrNNames, TStr64V &StrAttrNNames) {
  fprintf(F, "%s\t%s", NODES_START.CStr(), NID_NAME.CStr());
  for(int i = 0; i < IntAttrNNames.Len(); i++) {
    fprintf(F, "\t%s:%s", INT_TYPE_PREFIX.CStr(), IntAttrNNames[i].CStr());
  }
  for(int i = 0; i < FltAttrNNames.Len(); i++) {
    fprintf(F, "\t%s:%s", FLT_TYPE_PREFIX.CStr(), FltAttrNNames[i].CStr());
  }
  for(int i = 0; i < StrAttrNNames.Len(); i++) {
    fprintf(F, "\t%s:%s", STR_TYPE_PREFIX.CStr(), StrAttrNNames[i].CStr());
  }
  fprintf(F, "\n");
}

// Writes nodes out to the file. Each line consists of the node id followed by the
// int attributes in the order specified by the TStrV IntAttrNNames, the float attributes
// in the order specified by FltAttrNNames, and the string attributes specified by StrAttrNNames.
void WriteNodesToFile(FILE *F, const PNEANet& Graph, TStr64V &IntAttrNNames, TStr64V &FltAttrNNames, TStr64V &StrAttrNNames) {
  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    fprintf(F, "%s", TInt64::GetStr(NI.GetId()).CStr());
    for(int i = 0; i < IntAttrNNames.Len(); i++) {
      if (Graph->IsIntAttrDeletedN(NI.GetId(), IntAttrNNames[i])) {
        fprintf(F, "\t%s", NULL_VAL.CStr());
        continue;
      }
      int AttrIntVal = Graph->GetIntAttrDatN(NI.GetId(), IntAttrNNames[i]);
      fprintf(F, "\t%s", TInt64::GetStr(AttrIntVal).CStr());
    }
    for(int64 i = 0; i < FltAttrNNames.Len(); i++) {
      if (Graph->IsFltAttrDeletedN(NI.GetId(), FltAttrNNames[i])) {
        fprintf(F, "\t%s", NULL_VAL.CStr());
        continue;
      }
      double AttrFltVal = Graph->GetFltAttrDatN(NI.GetId(), FltAttrNNames[i]);
      fprintf(F, "\t%f", AttrFltVal);
    }
    for(int64 i = 0; i < StrAttrNNames.Len(); i++) {
      if (Graph->IsStrAttrDeletedN(NI.GetId(), StrAttrNNames[i])) {
        fprintf(F, "\t%s", NULL_VAL.CStr());
        continue;
      }
      const char * AttrStrVal = Graph->GetStrAttrDatN(NI.GetId(), StrAttrNNames[i]).CStr();
      fprintf(F, "\t%s", AttrStrVal);
    }
    fprintf(F, "\n");
  }
}

// Writes the schema out to the file, which consists of the SrcNId, DstNId, and edge attributes, separated by tabs.
// Edge attributes are written in the format <Type>:<Name>, where Type is either Int, Flt, or Str.
void WriteEdgeSchemaToFile(FILE *F, TStr64V &IntAttrENames, TStr64V &FltAttrENames, TStr64V &StrAttrENames) {
  fprintf(F, "%s\t%s\t%s", EDGES_START.CStr(), SRC_ID_NAME.CStr(), DST_ID_NAME.CStr());
  for(int64 i = 0; i < IntAttrENames.Len(); i++) {
    fprintf(F, "\t%s:%s", INT_TYPE_PREFIX.CStr(), IntAttrENames[i].CStr());
  }
  for(int64 i = 0; i < FltAttrENames.Len(); i++) {
    fprintf(F, "\t%s:%s", FLT_TYPE_PREFIX.CStr(), FltAttrENames[i].CStr());
  }
  for(int64 i = 0; i < StrAttrENames.Len(); i++) {
    fprintf(F, "\t%s:%s", STR_TYPE_PREFIX.CStr(), StrAttrENames[i].CStr());
  }
  fprintf(F, "\n");
}

// Writes edges out to the file. Each line consists of the SrcNId and DstNId followed by the
// int attributes in the order specified by the TStrV IntAttrENames, the float attributes
// in the order specified by FltAttrENames, and the string attributes specified by StrAttrENames.
void WriteEdgesToFile(FILE *F, const PNEANet& Graph, TStr64V &IntAttrENames, TStr64V &FltAttrENames, TStr64V &StrAttrENames) {
  for (TNEANet::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    fprintf(F, "%s\t%s", TInt64::GetStr(EI.GetSrcNId()).CStr(), TInt64::GetStr(EI.GetDstNId()).CStr());
    for(int64 i = 0; i < IntAttrENames.Len(); i++) {
      if (Graph->IsIntAttrDeletedE(EI.GetId(), IntAttrENames[i])) {
        fprintf(F, "\t%s", NULL_VAL.CStr());
        continue;
      }
      int64 AttrIntVal = Graph->GetIntAttrDatE(EI.GetId(), IntAttrENames[i]);
      fprintf(F, "\t%s", TInt64::GetStr(AttrIntVal).CStr());
    }
    for(int64 i = 0; i < FltAttrENames.Len(); i++) {
      if (Graph->IsFltAttrDeletedE(EI.GetId(), FltAttrENames[i])) {
        fprintf(F, "\t%s", NULL_VAL.CStr());
        continue;
      }
      double AttrFltVal = Graph->GetFltAttrDatE(EI.GetId(), FltAttrENames[i]);
      fprintf(F, "\t%f", AttrFltVal);
    }
    for(int64 i = 0; i < StrAttrENames.Len(); i++) {
      if (Graph->IsStrAttrDeletedE(EI.GetId(), StrAttrENames[i])) {
        fprintf(F, "\t%s", NULL_VAL.CStr());
        continue;
      }
      const char * AttrStrVal = Graph->GetStrAttrDatE(EI.GetId(), StrAttrENames[i]).CStr();
      fprintf(F, "\t%s", AttrStrVal);
    }
    fprintf(F, "\n");
  }
}

void SaveEdgeListNet(const PNEANet& Graph, const TStr& OutFNm, const TStr& Desc) {
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "# Directed network: %s \n", OutFNm.CStr());
  if (! Desc.Empty()) { fprintf(F, "# %s\n", Desc.CStr()); }
  fprintf(F, "# Nodes: %s Edges: %s\n", TInt64::GetStr(Graph->GetNodes()).CStr(), TInt64::GetStr(Graph->GetEdges()).CStr());

  TStr64V IntAttrNNames;
  TStr64V FltAttrNNames;
  TStr64V StrAttrNNames;
  Graph->GetAttrNNames(IntAttrNNames, FltAttrNNames, StrAttrNNames);
  WriteNodeSchemaToFile(F, IntAttrNNames, FltAttrNNames, StrAttrNNames);
  WriteNodesToFile(F, Graph, IntAttrNNames, FltAttrNNames, StrAttrNNames);
  fprintf(F, "%s\n", END_SENTINEL.CStr());

  TStr64V IntAttrENames;
  TStr64V FltAttrENames;
  TStr64V StrAttrENames;
  Graph->GetAttrENames(IntAttrENames, FltAttrENames, StrAttrENames);
  WriteEdgeSchemaToFile(F, IntAttrENames, FltAttrENames, StrAttrENames);
  WriteEdgesToFile(F, Graph, IntAttrENames, FltAttrENames, StrAttrENames);
  fprintf(F, "%s\n", END_SENTINEL.CStr());

  fclose(F);
}


/// For more info see ORA Network Analysis Data (http://www.casos.cs.cmu.edu/computational_tools/data2.php)
PNGraph LoadDyNet(const TStr& FNm) {
  TXmlLx XmlLx(TZipIn::IsZipFNm(FNm)?TZipIn::New(FNm):TFIn::New(FNm), xspTruncate);
  THashSet<TStr> NIdStr;
  while (XmlLx.GetSym()!=xsyEof) {
    if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="network") {
      PNGraph G = TNGraph::New();
      XmlLx.GetSym();
      while (XmlLx.TagNm=="link") {
        TStr Str1, Val1, Str2, Val2;
        XmlLx.GetArg(0, Str1, Val1);  XmlLx.GetArg(1, Str2, Val2);
        IAssert(Str1=="source" && Str2=="target");
        NIdStr.AddKey(Val1); NIdStr.AddKey(Val2);
        const int64 src=NIdStr.GetKeyId(Val1);
        const int64 dst=NIdStr.GetKeyId(Val2);
        if (! G->IsNode(src)) { G->AddNode(src); }
        if (! G->IsNode(dst)) { G->AddNode(dst); }
        G->AddEdge(src, dst);
        XmlLx.GetSym();
      }
      return G;
    }
  }
  return PNGraph();
}

/// For more info see ORA Network Analysis Data (http://www.casos.cs.cmu.edu/computational_tools/data2.php)
TVec<PNGraph> LoadDyNetGraphV(const TStr& FNm) {
  TXmlLx XmlLx(TZipIn::IsZipFNm(FNm)?TZipIn::New(FNm):TFIn::New(FNm), xspTruncate);
  TVec<PNGraph> GraphV;
  THashSet<TStr> NIdStr;
  while (XmlLx.GetSym()!=xsyEof) {
    if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="network") {
      PNGraph G = TNGraph::New();
      GraphV.Add(G);
      XmlLx.GetSym();
      while (XmlLx.TagNm=="link") {
        TStr Str1, Val1, Str2, Val2;
        XmlLx.GetArg(0, Str1, Val1);  XmlLx.GetArg(1, Str2, Val2);
        IAssert(Str1=="source" && Str2=="target");
        NIdStr.AddKey(Val1); NIdStr.AddKey(Val2);
        const int64 src=NIdStr.GetKeyId(Val1);
        const int64 dst=NIdStr.GetKeyId(Val2);
        if (! G->IsNode(src)) { G->AddNode(src); }
        if (! G->IsNode(dst)) { G->AddNode(dst); }
        G->AddEdge(src, dst);
        XmlLx.GetSym();
      }
    }
  }
  return GraphV;
}

}; // namespace TSnap
