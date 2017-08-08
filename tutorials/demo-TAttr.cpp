#include "Snap.h"

// Test attribute functionality
void ManipulateSparseAttributes() {
  TAttr Attrs;

  // Add mappings for three attributes
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  printf("Added attribute %s with id %s\n", IntAttr.CStr(),
            TInt64::GetStr(IntId.Val).CStr());

  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  printf("Added attribute %s with id %s\n", FltAttr.CStr(),
            TInt64::GetStr(FltId.Val).CStr());

  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);
  printf("Added attribute %s with id %s\n", StrAttr.CStr(),
            TInt64::GetStr(StrId.Val).CStr());

  // Add values for attributes to id NId.
  TInt NId(0);
  TInt IntVal(5);
  Attrs.AddSAttrDat(NId, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(NId, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(NId, StrId, StrVal);

  // Get values for the attributes for id NId.
  TInt64 IntVal2;
  Attrs.GetSAttrDat(NId, IntId, IntVal2);
  printf("Node %d has attribute, with id %s, with value %s.\n", NId.Val,
          TInt64::GetStr(IntId.Val).CStr(), TInt64::GetStr(IntVal2.Val).CStr());
  TFlt FltVal2;
  Attrs.GetSAttrDat(NId, FltAttr, FltVal2);
  printf("Node %d has attribute, with id %s, with value %f.\n", NId.Val,
          TInt64::GetStr(FltId.Val).CStr(), FltVal2.Val);
  TStr StrVal2;
  Attrs.GetSAttrDat(NId, StrId, StrVal2);
  printf("Node %d has attribute, with id %s, with value %s.\n", NId.Val,
          TInt64::GetStr(StrId.Val).CStr(), StrVal2.CStr());

  // Get list of attributes for id NId.
  TAttrPrV AttrV;
  Attrs.GetSAttrV(NId, atInt, AttrV);
  printf("ID %d has %s int attributes.\n", NId.Val,
            TInt64::GetStr(AttrV.Len()).CStr());
  Attrs.GetSAttrV(NId, atFlt, AttrV);
  printf("ID %d has %s flt attributes.\n", NId.Val,
            TInt64::GetStr(AttrV.Len()).CStr());
  Attrs.GetSAttrV(NId, atStr, AttrV);
  printf("ID %d has %s str attributes.\n", NId.Val,
            TInt64::GetStr(AttrV.Len()).CStr());
  //Attrs.GetSAttrV(NId, atAny, AttrV);
  //printf("ID %d has %d attributes.\n", NId.Val, AttrV.Len());

  // Delete all attributes for id NId (use either name or id).
  Attrs.DelSAttrDat(NId, IntAttr);
  Attrs.DelSAttrDat(NId, FltId);
  Attrs.DelSAttrDat(NId, StrAttr);

  //Get all ids with given attribute
  for (int i = 1; i <= 10; i++) {
    TInt Id(i);
    Attrs.AddSAttrDat(Id, IntId, IntVal);
    if (i%2 == 0) {
      Attrs.AddSAttrDat(Id, FltId, FltVal);
    }
  }
  Attrs.AddSAttrDat(NId, StrId, StrVal);

  TInt64V IdV;
  Attrs.GetIdVSAttr(IntAttr, IdV);
  printf("%s ids have attribute with name %s\n",
            TInt64::GetStr(IdV.Len()).CStr(), IntAttr.CStr());

  Attrs.GetIdVSAttr(FltAttr, IdV);
  printf("%s ids have attribute with name %s\n",
            TInt64::GetStr(IdV.Len()).CStr(), FltAttr.CStr());

  Attrs.GetIdVSAttr(StrAttr, IdV);
  printf("%s ids have attribute with name %s\n",
            TInt64::GetStr(IdV.Len()).CStr(), StrAttr.CStr());
}

// Test pair sparse attribute functionality
void ManipulateSparseAttributesPair() {
  TAttrPair Attrs;
  int SrcNId = 0;
  int DstNId = 1;
  TInt64Pr EId(SrcNId, DstNId);

  // Add mappings for three attributes
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  printf("Added attribute %s with id %s\n", IntAttr.CStr(),
            TInt64::GetStr(IntId.Val).CStr());

  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  printf("Added attribute %s with id %s\n", FltAttr.CStr(),
            TInt64::GetStr(FltId.Val).CStr());

  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);
  printf("Added attribute %s with id %s\n", StrAttr.CStr(),
            TInt64::GetStr(StrId.Val).CStr());

  // Add values for attributes to id NId.
  TInt IntVal(5);
  Attrs.AddSAttrDat(EId, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(EId, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(EId, StrId, StrVal);

  // Get values for the attributes for id NId.
  TInt64 IntVal2;
  Attrs.GetSAttrDat(EId, IntId, IntVal2);
  printf("ID (%d, %d) has attribute, with id %s, with value %s.\n",
            SrcNId, DstNId, TInt64::GetStr(IntId.Val).CStr(),
            TInt64::GetStr(IntVal2.Val).CStr());
  TFlt FltVal2;
  Attrs.GetSAttrDat(EId, FltAttr, FltVal2);
  printf("ID (%d, %d) has attribute, with id %s, with value %f.\n",
            SrcNId, DstNId, TInt64::GetStr(FltId.Val).CStr(), FltVal2.Val);
  TStr StrVal2;
  Attrs.GetSAttrDat(EId, StrId, StrVal2);
  printf("ID (%d, %d) has attribute, with id %s, with value %s.\n",
            SrcNId, DstNId, TInt64::GetStr(StrId.Val).CStr(), StrVal2.CStr());

  // Get list of attributes for id NId.
  TAttrPrV AttrV;
  Attrs.GetSAttrV(EId, atInt, AttrV);
  printf("ID (%d, %d) has %s int attributes.\n", SrcNId, DstNId,
            TInt64::GetStr(AttrV.Len()).CStr());
  Attrs.GetSAttrV(EId, atFlt, AttrV);
  printf("ID (%d, %d) has %s flt attributes.\n", SrcNId, DstNId,
            TInt64::GetStr(AttrV.Len()).CStr());
  Attrs.GetSAttrV(EId, atStr, AttrV);
  printf("ID (%d, %d) has %s str attributes.\n", SrcNId, DstNId,
            TInt64::GetStr(AttrV.Len()).CStr());
  //Attrs.GetSAttrV(EId, atAny, AttrV);
  //printf("ID (%d, %d) has %d attributes.\n", SrcNId, DstNId, AttrV.Len());

  // Delete all attributes for id NId (use either name or id).
  Attrs.DelSAttrDat(EId, IntAttr);
  Attrs.DelSAttrDat(EId, FltId);
  Attrs.DelSAttrDat(EId, StrAttr);

  //Get all ids with given attribute
  for (int i = 2; i < 12; i++) {
    TInt64Pr EId2(i-1, i);
    Attrs.AddSAttrDat(EId2, IntId, IntVal);
    if (i%2 == 0) {
      Attrs.AddSAttrDat(EId2, FltId, FltVal);
    }
  }
  Attrs.AddSAttrDat(EId, StrId, StrVal);

  TIntPr64V IdV;
  Attrs.GetIdVSAttr(IntAttr, IdV);
  printf("%s ids have attribute with name %s\n",
            TInt64::GetStr(IdV.Len()).CStr(), IntAttr.CStr());

  Attrs.GetIdVSAttr(FltAttr, IdV);
  printf("%s ids have attribute with name %s\n",
            TInt64::GetStr(IdV.Len()).CStr(), FltAttr.CStr());

  Attrs.GetIdVSAttr(StrAttr, IdV);
  printf("%s ids have attribute with name %s\n",
            TInt64::GetStr(IdV.Len()).CStr(), StrAttr.CStr());
}

int main(int argc, char* argv[]) {
  ManipulateSparseAttributes();
  ManipulateSparseAttributesPair();
}
