#include <gtest/gtest.h>

#include "Snap.h"

TEST(TAttr, AddSAttr) {
  TAttr Attrs;
  TInt64 AttrId;
  int64 status = Attrs.AddSAttr("TestInt", atInt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(0, AttrId.Val);
  status = Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrId.Val);
  status = Attrs.AddSAttr("TestStr", atStr, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(2, AttrId.Val);
  //status = Attrs.AddSAttr("TestAny", atAny, AttrId);
  //EXPECT_EQ(-1, status);
}

TEST(TAttr, GetSAttrId) {
  TAttr Attrs;
  TInt64 AttrId;
  Attrs.AddSAttr("TestInt", atInt, AttrId);
  Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  Attrs.AddSAttr("TestStr", atStr, AttrId);
  TAttrType AttrType;
  int64 status = Attrs.GetSAttrId(TStr("TestInt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_EQ(0, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestFlt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_EQ(1, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestStr"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_EQ(2, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestError"), AttrId, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TAttr, GetSAttrName) {
  TAttr Attrs;
  TInt64 AttrId;
  Attrs.AddSAttr("TestInt", atInt, AttrId);
  Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  Attrs.AddSAttr("TestStr", atStr, AttrId);
  TAttrType AttrType;
  TStr Name;
  int64 status = Attrs.GetSAttrName(0, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_STREQ("TestInt", Name.CStr());
  status = Attrs.GetSAttrName(1, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_STREQ("TestFlt", Name.CStr());
  status = Attrs.GetSAttrName(2, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_STREQ("TestStr", Name.CStr());
  status = Attrs.GetSAttrName(3, Name, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TAttr, AddSAttrDat_int) {
  TAttr Attrs;
  TInt64 Val(5);
  TInt64 Id(0);
  int64 status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt64 AttrId;
  TStr AttrName("TestInt");
  Attrs.AddSAttr(AttrName, atInt, AttrId);
  TFlt ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestInt2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TAttr, AddSAttrDat_flt) {
  TAttr Attrs;
  TFlt Val(5.0);
  TInt64 Id(0);
  int64 status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt64 AttrId;
  TStr AttrName("TestFlt");
  Attrs.AddSAttr(AttrName, atFlt, AttrId);
  TInt64 ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestFlt2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TAttr, AddSAttrDat_str) {
  TAttr Attrs;
  TStr Val("5");
  TInt64 Id(0);
  int64 status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt64 AttrId;
  TStr AttrName("TestFlt");
  Attrs.AddSAttr(AttrName, atStr, AttrId);
  TInt64 ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestStr2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TAttr, GetSAttrDat_int) {
  TAttr Attrs;
  TInt64 Val;
  TInt64 AttrId(0);
  TStr AttrName("TestInt");
  TInt64 NId(0);
  int64 status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atInt, AttrId);
  TInt64 TestVal(5);
  Attrs.AddSAttrDat(NId, AttrId, TestVal);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
}

TEST(TAttr, GetSAttrDat_flt) {
  TAttr Attrs;
  TFlt Val;
  TInt64 AttrId(0);
  TStr AttrName("TestInt");
  TInt64 NId(0);
  int64 status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atFlt, AttrId);
  TFlt TestVal(5.0);
  Attrs.AddSAttrDat(NId, AttrId, TestVal);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
}

TEST(TAttr, GetSAttrDat_str) {
  TAttr Attrs;
  TStr Val;
  TInt64 AttrId(0);
  TStr AttrName("TestInt");
  TInt64 NId(0);
  int64 status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atStr, AttrId);
  TStr TestVal("5");
  Attrs.AddSAttrDat(NId, AttrId, TestVal);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
}

TEST(TAttr, DelSAttrDat) {
  TAttr Attrs;
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);
  TInt64 Id(0);
  int64 status = Attrs.DelSAttrDat(Id, IntAttr);
  EXPECT_EQ(-1, status);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(-1, status);

  TInt64 IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  status = Attrs.DelSAttrDat(Id, IntAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(-1, status);

  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  status = Attrs.DelSAttrDat(Id, FltAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  status = Attrs.DelSAttrDat(Id, FltId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, FltId);
  EXPECT_EQ(-1, status);

  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);
  status = Attrs.DelSAttrDat(Id, StrAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, StrId, StrVal);
  status = Attrs.DelSAttrDat(Id, StrId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, StrId);
  EXPECT_EQ(-1, status);
}

TEST(TAttr, GetSAttrV) {
  TAttr Attrs;
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt64 Id(0);
  TInt64 IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);

  TAttrPrV AttrV;
  Attrs.GetSAttrV(Id, atInt, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atFlt, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atStr, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  //Attrs.GetSAttrV(Id, atAny, AttrV);
  //EXPECT_EQ(3, AttrV.Len());
  //Attrs.GetSAttrV(Id, atUndef, AttrV);
  //EXPECT_EQ(0, AttrV.Len());
}
TEST(TAttr, GetIdVSAttr) {
  TAttr Attrs;
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt64 IntVal(0);
  TFlt FltVal(0);
  TStr StrVal("test");
  for (int64 i = 0; i < 10; i++) {
    TInt64 Id(i);
    Attrs.AddSAttrDat(Id, IntId, IntVal);
    if (i%2 == 0) {
      Attrs.AddSAttrDat(Id, FltId, FltVal);
    }
  }
  Attrs.AddSAttrDat(0, StrId, StrVal);

  TInt64V IdV;
  Attrs.GetIdVSAttr(IntAttr, IdV);
  EXPECT_EQ(10, IdV.Len());
  Attrs.GetIdVSAttr(IntId, IdV);
  EXPECT_EQ(10, IdV.Len());

  Attrs.GetIdVSAttr(FltAttr, IdV);
  EXPECT_EQ(5, IdV.Len());
  Attrs.GetIdVSAttr(FltId, IdV);
  EXPECT_EQ(5, IdV.Len());

  Attrs.GetIdVSAttr(StrAttr, IdV);
  EXPECT_EQ(1, IdV.Len());
  Attrs.GetIdVSAttr(StrId, IdV);
  EXPECT_EQ(1, IdV.Len());
}

TEST(TAttr, DelSAttrId) {
  TAttr Attrs;
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt64 Id(0);
  TInt64 IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);

  //TAttrPrV AttrV;
  //Attrs.GetSAttrV(Id, atAny, AttrV);
  //EXPECT_EQ(3, AttrV.Len());

  //Attrs.DelSAttrId(Id);
  //Attrs.GetSAttrV(Id, atAny, AttrV);
  //EXPECT_EQ(0, AttrV.Len());
}

TEST(TAttrPair, AddSAttr) {
  TAttrPair Attrs;
  TInt64 AttrId;
  int64 status = Attrs.AddSAttr("TestInt", atInt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(0, AttrId.Val);
  status = Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrId.Val);
  status = Attrs.AddSAttr("TestStr", atStr, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(2, AttrId.Val);
  //status = Attrs.AddSAttr("TestAny", atAny, AttrId);
  //EXPECT_EQ(-1, status);
}

TEST(TAttrPair, GetSAttrId) {
  TAttrPair Attrs;
  TInt64 AttrId;
  Attrs.AddSAttr("TestInt", atInt, AttrId);
  Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  Attrs.AddSAttr("TestStr", atStr, AttrId);
  TAttrType AttrType;
  int64 status = Attrs.GetSAttrId(TStr("TestInt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_EQ(0, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestFlt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_EQ(1, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestStr"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_EQ(2, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestError"), AttrId, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TAttrPair, GetSAttrName) {
  TAttrPair Attrs;
  TInt64 AttrId;
  Attrs.AddSAttr("TestInt", atInt, AttrId);
  Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  Attrs.AddSAttr("TestStr", atStr, AttrId);
  TAttrType AttrType;
  TStr Name;
  int64 status = Attrs.GetSAttrName(0, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_STREQ("TestInt", Name.CStr());
  status = Attrs.GetSAttrName(1, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_STREQ("TestFlt", Name.CStr());
  status = Attrs.GetSAttrName(2, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_STREQ("TestStr", Name.CStr());
  status = Attrs.GetSAttrName(3, Name, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TAttrPair, AddSAttrDat_int) {
  TAttrPair Attrs;
  TInt64 Val(5);
  TInt64Pr Id(0, 1);
  int64 status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt64 AttrId;
  TStr AttrName("TestInt");
  Attrs.AddSAttr(AttrName, atInt, AttrId);
  TFlt ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestInt2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TAttrPair, AddSAttrDat_flt) {
  TAttrPair Attrs;
  TFlt Val(5.0);
  TInt64Pr Id(0, 1);
  int status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt64 AttrId;
  TStr AttrName("TestFlt");
  Attrs.AddSAttr(AttrName, atFlt, AttrId);
  TInt64 ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestFlt2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TAttrPair, AddSAttrDat_str) {
  TAttrPair Attrs;
  TStr Val("5");
  TInt64Pr Id(0, 1);
  int64 status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt64 AttrId;
  TStr AttrName("TestFlt");
  Attrs.AddSAttr(AttrName, atStr, AttrId);
  TInt64 ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestStr2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TAttrPair, GetSAttrDat_int) {
  TAttrPair Attrs;
  TInt64 Val;
  TInt64 AttrId(0);
  TStr AttrName("TestInt");
  TInt64Pr Id(0, 1);
  int64 status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atInt, AttrId);
  TInt64 TestVal(5);
  Attrs.AddSAttrDat(Id, AttrId, TestVal);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
}

TEST(TAttrPair, GetSAttrDat_flt) {
  TAttrPair Attrs;
  TFlt Val;
  TInt64 AttrId(0);
  TStr AttrName("TestInt");
  TInt64Pr Id(0, 1);
  int64 status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atFlt, AttrId);
  TFlt TestVal(5.0);
  Attrs.AddSAttrDat(Id, AttrId, TestVal);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
}

TEST(TAttrPair, GetSAttrDat_str) {
  TAttrPair Attrs;
  TStr Val;
  TInt64 AttrId(0);
  TStr AttrName("TestInt");
  TInt64Pr Id(0, 1);
  int64 status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atStr, AttrId);
  TStr TestVal("5");
  Attrs.AddSAttrDat(Id, AttrId, TestVal);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
}

TEST(TAttrPair, DelSAttrDat) {
  TAttrPair Attrs;
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);
  TInt64Pr Id(0, 1);
  int64 status = Attrs.DelSAttrDat(Id, IntAttr);
  EXPECT_EQ(-1, status);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(-1, status);

  TInt64 IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  status = Attrs.DelSAttrDat(Id, IntAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(-1, status);

  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  status = Attrs.DelSAttrDat(Id, FltAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  status = Attrs.DelSAttrDat(Id, FltId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, FltId);
  EXPECT_EQ(-1, status);

  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);
  status = Attrs.DelSAttrDat(Id, StrAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, StrId, StrVal);
  status = Attrs.DelSAttrDat(Id, StrId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, StrId);
  EXPECT_EQ(-1, status);
}

TEST(TAttrPair, GetSAttrV) {
  TAttrPair Attrs;
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt64Pr Id(0, 1);
  TInt64 IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);

  TAttrPrV AttrV;
  Attrs.GetSAttrV(Id, atInt, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atFlt, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atStr, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  //Attrs.GetSAttrV(Id, atAny, AttrV);
  //EXPECT_EQ(3, AttrV.Len());
  //Attrs.GetSAttrV(Id, atUndef, AttrV);
  //EXPECT_EQ(0, AttrV.Len());
}
TEST(TAttrPair, GetIdVSAttr) {
  TAttrPair Attrs;
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt64 IntVal(0);
  TFlt FltVal(0);
  TStr StrVal("test");
  for (int i = 0; i < 10; i++) {
    TInt64Pr Id(i, i+1);
    Attrs.AddSAttrDat(Id, IntId, IntVal);
    if (i%2 == 0) {
      Attrs.AddSAttrDat(Id, FltId, FltVal);
    }
  }
  TInt64Pr Id(0, 1);
  Attrs.AddSAttrDat(Id, StrId, StrVal);

  TIntPr64V IdV;
  Attrs.GetIdVSAttr(IntAttr, IdV);
  EXPECT_EQ(10, IdV.Len());
  Attrs.GetIdVSAttr(IntId, IdV);
  EXPECT_EQ(10, IdV.Len());

  Attrs.GetIdVSAttr(FltAttr, IdV);
  EXPECT_EQ(5, IdV.Len());
  Attrs.GetIdVSAttr(FltId, IdV);
  EXPECT_EQ(5, IdV.Len());

  Attrs.GetIdVSAttr(StrAttr, IdV);
  EXPECT_EQ(1, IdV.Len());
  Attrs.GetIdVSAttr(StrId, IdV);
  EXPECT_EQ(1, IdV.Len());
}

TEST(TAttrPair, DelSAttrId) {
  TAttrPair Attrs;
  TStr IntAttr("TestInt");
  TInt64 IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt64 FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt64 StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt64Pr Id(0, 1);
  TInt64 IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);

  //TAttrPrV AttrV;
  //Attrs.GetSAttrV(Id, atAny, AttrV);
  //EXPECT_EQ(3, AttrV.Len());

  //Attrs.DelSAttrId(Id);
  //Attrs.GetSAttrV(Id, atAny, AttrV);
  //EXPECT_EQ(0, AttrV.Len());
}
