/////////////////////////////////////////////////
// Conversion Functions
namespace TSnap {


int64 LoadModeNetToNet(PMMNet Graph, const TStr& Name, PTable Table, const TStr& NCol,
  TStr64V& NodeAttrV) {
  Graph->AddModeNet(Name);
  TModeNet& Net = Graph->GetModeNetByName(Name);
  return LoadMode(Net, Table, NCol, NodeAttrV);
}


int64 LoadMode(TModeNet& Graph, PTable Table, const TStr& NCol,
  TStr64V& NodeAttrV) {

  const TAttrType NodeType = Table->GetColType(NCol);
  const TInt64 NColIdx = Table->GetColIdx(NCol);
  for (int64 CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }
    // add src and dst nodes to graph if they are not seen earlier
    TInt64 NVal;
    if (NodeType == atFlt) {
      return -1;
    } else if (NodeType == atInt || NodeType == atStr) {
      if (NodeType == atInt) {
        NVal = (Table->IntCols)[NColIdx][CurrRowIdx];
      } else {
        NVal = (Table->StrColMaps)[NColIdx][CurrRowIdx];
        if (strlen(Table->GetContextKey(NVal)) == 0) { continue; }  //illegal value
      }
      if (!Graph.IsNode(NVal)) {Graph.AddNode(NVal); }
    }
    // Aggregate edge attributes and add to graph
    for (TInt64 i = 0; i < NodeAttrV.Len(); i++) {
      TStr ColName = NodeAttrV[i];
      TAttrType T = Table->GetColType(ColName);
      TInt64 Index = Table->GetColIdx(ColName);
      switch (T) {
        case atInt:
          Graph.AddIntAttrDatN(NVal, Table->IntCols[Index][CurrRowIdx], ColName);
          break;
        case atFlt:
          Graph.AddFltAttrDatN(NVal, Table->FltCols[Index][CurrRowIdx], ColName);
          break;
        case atStr:
          Graph.AddStrAttrDatN(NVal, Table->GetStrVal(Index, CurrRowIdx), ColName);
          break;
      }
    }
  }
  return 1;
}

int64 LoadCrossNetToNet(PMMNet Graph, const TStr& Mode1, const TStr& Mode2, const TStr& CrossName,
 PTable Table, const TStr& SrcCol, const TStr& DstCol, TStr64V& EdgeAttrV)
{
  Graph->AddCrossNet(Mode1, Mode2, CrossName);
  TCrossNet& Net = Graph->GetCrossNetByName(CrossName);
  return LoadCrossNet(Net, Table, SrcCol, DstCol, EdgeAttrV);
}


int64 LoadCrossNet(TCrossNet& Graph, PTable Table, const TStr& SrcCol, const TStr& DstCol,
  TStr64V& EdgeAttrV)
{

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));
  const TInt64 SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt64 DstColIdx = Table->GetColIdx(DstCol);

  // node values - i.e. the unique values of src/dst col
  //THashSet<TInt> IntNodeVals; // for both int and string node attr types.
  THash<TFlt, TInt64, int64> FltNodeVals;

  // make single pass over all rows in the table
  for (int64 CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }

    // add src and dst nodes to graph if they are not seen earlier
    TInt64 SVal, DVal;
    if (NodeType == atFlt) {
      return -1;
    } else if (NodeType == atInt || NodeType == atStr) {
      if (NodeType == atInt) {
        SVal = (Table->IntCols)[SrcColIdx][CurrRowIdx];
        DVal = (Table->IntCols)[DstColIdx][CurrRowIdx];
      } else {
        SVal = (Table->StrColMaps)[SrcColIdx][CurrRowIdx];
        if (strlen(Table->GetContextKey(SVal)) == 0) { continue; }  //illegal value
        DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
        if (strlen(Table->GetContextKey(DVal)) == 0) { continue; }  //illegal value
      }
    }

    // add edge and edge attributes
    if (Graph.AddEdge(SVal, DVal, CurrRowIdx) == -1) { return -1; }

    // Aggregate edge attributes and add to graph
    for (TInt64 i = 0; i < EdgeAttrV.Len(); i++) {
      TStr ColName = EdgeAttrV[i];
      TAttrType T = Table->GetColType(ColName);
      TInt64 Index = Table->GetColIdx(ColName);
      switch (T) {
        case atInt:
          Graph.AddIntAttrDatE(CurrRowIdx, Table->IntCols[Index][CurrRowIdx], ColName);
          break;
        case atFlt:
          Graph.AddFltAttrDatE(CurrRowIdx, Table->FltCols[Index][CurrRowIdx], ColName);
          break;
        case atStr:
          Graph.AddStrAttrDatE(CurrRowIdx, Table->GetStrVal(Index, CurrRowIdx), ColName);
          break;
      }
    }
  }
  return 1;
}

template <>
PTable EdgesToTable(const PNEANet& Graph) {
  PTable Table = TTable::New();

  // Add node id columns
  Table->AddIntCol("src");
  Table->SetSrcCol("src");
  Table->AddIntCol("dst");
  Table->SetDstCol("dst");

  // Add attributes columns
  TStr64V IntAttrNames;
  TStr64V FltAttrNames;
  TStr64V StrAttrNames;
  Graph->GetAttrENames(IntAttrNames, FltAttrNames, StrAttrNames);
  
  for (int64 i = 0; i < IntAttrNames.Len(); i++) {
    Table->AddIntCol(IntAttrNames[i]);
    Table->AddEdgeAttr(IntAttrNames[i]);
  }
  for (int64 i = 0; i < FltAttrNames.Len(); i++) {
    Table->AddFltCol(FltAttrNames[i]);
    Table->AddEdgeAttr(FltAttrNames[i]);
  }
  for (int64 i = 0; i < StrAttrNames.Len(); i++) {
    Table->AddStrCol(StrAttrNames[i]);
    Table->AddEdgeAttr(StrAttrNames[i]);
  }
  
  // Add graph data
  for(PNEANet::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    TTableRow TableRow;
    TableRow.AddInt(EI.GetSrcNId());
    TableRow.AddInt(EI.GetDstNId());
    
    for (int64 i = 0; i < IntAttrNames.Len(); i++) {
      TableRow.AddInt(Graph->GetIntAttrDatE(EI, IntAttrNames[i]));
    }
    for (int64 i = 0; i < FltAttrNames.Len(); i++) {
      TableRow.AddFlt(Graph->GetFltAttrDatE(EI, FltAttrNames[i]));
    }
    for (int64 i = 0; i < StrAttrNames.Len(); i++) {
      TableRow.AddStr(Graph->GetStrAttrDatE(EI, StrAttrNames[i]));
    } 
    
    // Finally add the row for edge EI to Table. 
    Table->AddRow(TableRow);
  }

  return Table;
}


template <>
PTable EdgesToTable(const TCrossNet& Graph) {
  PTable Table = TTable::New();

  // Add node id columns
  Table->AddIntCol("src");
  Table->SetSrcCol("src");
  Table->AddIntCol("dst");
  Table->SetDstCol("dst");

  // Add attributes columns
  TStr64V IntAttrNames;
  TStr64V FltAttrNames;
  TStr64V StrAttrNames;
  Graph.GetAttrENames(IntAttrNames, FltAttrNames, StrAttrNames);
  
  for (int64 i = 0; i < IntAttrNames.Len(); i++) {
    Table->AddIntCol(IntAttrNames[i]);
    Table->AddEdgeAttr(IntAttrNames[i]);
  }
  for (int64 i = 0; i < FltAttrNames.Len(); i++) {
    Table->AddFltCol(FltAttrNames[i]);
    Table->AddEdgeAttr(FltAttrNames[i]);
  }
  for (int64 i = 0; i < StrAttrNames.Len(); i++) {
    Table->AddStrCol(StrAttrNames[i]);
    Table->AddEdgeAttr(StrAttrNames[i]);
  }
  
  // Add graph data
  for(TCrossNet::TCrossEdgeI EI = Graph.BegEdgeI(); EI < Graph.EndEdgeI(); EI++) {

    TTableRow TableRow;
    TableRow.AddInt(EI.GetSrcNId());
    TableRow.AddInt(EI.GetDstNId());
    
    for (int64 i = 0; i < IntAttrNames.Len(); i++) {
      TableRow.AddInt(Graph.GetIntAttrDatE(EI, IntAttrNames[i]));
    }
    for (int64 i = 0; i < FltAttrNames.Len(); i++) {
      TableRow.AddFlt(Graph.GetFltAttrDatE(EI, FltAttrNames[i]));
    }
    for (int64 i = 0; i < StrAttrNames.Len(); i++) {
      TableRow.AddStr(Graph.GetStrAttrDatE(EI, StrAttrNames[i]));
    } 
    
    // Finally add the row for edge EI to Table. 
    Table->AddRow(TableRow);
  }

  return Table;
}


// template <>
// PTable EdgesToTable(const TCrossNet& Graph) {
//   PTable Table = TTable::New();
//   Table->AddIntCol("src");
//   Table->SetSrcCol("src");
//   Table->AddIntCol("dst");
//   Table->SetDstCol("dst");
//   for(TCrossNet::TCrossEdgeI EI = Graph.BegEdgeI(); EI < Graph.EndEdgeI(); EI++) {
//     TTableRow TableRow;
//     TableRow.AddInt(EI.GetSrcNId());
//     TableRow.AddInt(EI.GetDstNId());
//     Table->AddRow(TableRow);
//   }
//   return Table;
// }

template <>
PTable NodesToTable(const PNEANet& Graph) {
  PTable Table = TTable::New();
  Table->AddIntCol("src");
  Table->SetSrcCol("src");

  // Add attributes columns
  TStr64V IntAttrNames;
  TStr64V FltAttrNames;
  TStr64V StrAttrNames;
  Graph->GetAttrNNames(IntAttrNames, FltAttrNames, StrAttrNames);
  
  for (int64 i = 0; i < IntAttrNames.Len(); i++) {
    Table->AddIntCol(IntAttrNames[i]);
    Table->AddNodeAttr(IntAttrNames[i]);
  }
  for (int64 i = 0; i < FltAttrNames.Len(); i++) {
    Table->AddFltCol(FltAttrNames[i]);
    Table->AddNodeAttr(FltAttrNames[i]);
  }
  for (int64 i = 0; i < StrAttrNames.Len(); i++) {
    Table->AddStrCol(StrAttrNames[i]);
    Table->AddNodeAttr(StrAttrNames[i]);
  }

  // Add graph data
  for(PNEANet::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    TTableRow TableRow;
    TableRow.AddInt(NI.GetId());

    for (int64 i = 0; i < IntAttrNames.Len(); i++) {
      TableRow.AddInt(Graph->GetIntAttrDatN(NI, IntAttrNames[i]));
    }
    for (int64 i = 0; i < FltAttrNames.Len(); i++) {
      TableRow.AddFlt(Graph->GetFltAttrDatN(NI, FltAttrNames[i]));
    }
    for (int64 i = 0; i < StrAttrNames.Len(); i++) {
      TableRow.AddStr(Graph->GetStrAttrDatN(NI, StrAttrNames[i]));
    } 

    // Finally add the row for edge EI to Table. 
    Table->AddRow(TableRow);
  }
  return Table;
}

template <>
PTable NodesToTable(const TModeNet& Graph) {
  PTable Table = TTable::New();
  Table->AddIntCol("src");
  Table->SetSrcCol("src");

  // Add attributes columns
  TStr64V IntAttrNames;
  TStr64V FltAttrNames;
  TStr64V StrAttrNames;
  Graph.GetAttrNNames(IntAttrNames, FltAttrNames, StrAttrNames);
  
  for (int64 i = 0; i < IntAttrNames.Len(); i++) {
    Table->AddIntCol(IntAttrNames[i]);
    Table->AddNodeAttr(IntAttrNames[i]);
  }
  for (int64 i = 0; i < FltAttrNames.Len(); i++) {
    Table->AddFltCol(FltAttrNames[i]);
    Table->AddNodeAttr(FltAttrNames[i]);
  }
  for (int64 i = 0; i < StrAttrNames.Len(); i++) {
    Table->AddStrCol(StrAttrNames[i]);
    Table->AddNodeAttr(StrAttrNames[i]);
  }

  // Add graph data
  for(TModeNet::TNodeI NI = Graph.BegMMNI(); NI < Graph.EndMMNI(); NI++) {
    TTableRow TableRow;
    TableRow.AddInt(NI.GetId());

    for (int64 i = 0; i < IntAttrNames.Len(); i++) {
      TableRow.AddInt(Graph.GetIntAttrDatN(NI, IntAttrNames[i]));
    }
    for (int64 i = 0; i < FltAttrNames.Len(); i++) {
      TableRow.AddFlt(Graph.GetFltAttrDatN(NI, FltAttrNames[i]));
    }
    for (int64 i = 0; i < StrAttrNames.Len(); i++) {
      TableRow.AddStr(Graph.GetStrAttrDatN(NI, StrAttrNames[i]));
    } 

    // Finally add the row for edge EI to Table. 
    Table->AddRow(TableRow);
  }
  return Table;
}

}; //namespace TSnap
