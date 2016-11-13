// Add functions
int64 TAttr::AddSAttrDat(const TInt64& Id, const TStr& AttrName, const TInt64& Val) {
  TInt64 AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atInt, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int64 TAttr::AddSAttrDat(const TInt64& Id, const TInt64& AttrId, const TInt64& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atInt) { return -2; }
  TInt64Pr Key(AttrId, Id);
  IntAttrVals.AddDat(Key, Val);
  return 0;
}

int64 TAttr::AddSAttrDat(const TInt64& Id, const TStr& AttrName, const TFlt& Val) {
  TInt64 AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atFlt, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int64 TAttr::AddSAttrDat(const TInt64& Id, const TInt64& AttrId, const TFlt& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atFlt) { return -2; }
  TInt64Pr Key(AttrId, Id);
  FltAttrVals.AddDat(Key, Val);
  return 0;
}

int64 TAttr::AddSAttrDat(const TInt64& Id, const TStr& AttrName, const TStr& Val) {
  TInt64 AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atStr, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int64 TAttr::AddSAttrDat(const TInt64& Id, const TInt64& AttrId, const TStr& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atStr) { return -2; }
  TInt64Pr Key(AttrId, Id);
  StrAttrVals.AddDat(Key, Val);
  return 0;
}

// Get functions
int64 TAttr::GetSAttrDat(const TInt64& Id, const TStr& AttrName, TInt64& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64 AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int64 TAttr::GetSAttrDat(const TInt64& Id, const TInt64& AttrId, TInt64& Val) const {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atInt) { return -2; }
  TInt64Pr Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    Val = IntAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

int64 TAttr::GetSAttrDat(const TInt64& Id, const TStr& AttrName, TFlt& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64 AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int64 TAttr::GetSAttrDat(const TInt64& Id, const TInt64& AttrId, TFlt& Val) const {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atFlt) { return -2; }
  TInt64Pr Key(AttrId, Id);
  if (FltAttrVals.IsKey(Key)) {
    Val = FltAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

int64 TAttr::GetSAttrDat(const TInt64& Id, const TStr& AttrName, TStr& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64 AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int64 TAttr::GetSAttrDat(const TInt64& Id, const TInt64& AttrId, TStr& Val) const {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atStr) { return -2; }
  TInt64Pr Key(AttrId, Id);
  if (StrAttrVals.IsKey(Key)) {
    Val = StrAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

// Del functions
int64 TAttr::DelSAttrDat(const TInt64& Id, const TStr& AttrName) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64 AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return DelSAttrDat(Id, AttrId);
} 
int64 TAttr::DelSAttrDat(const TInt64& Id, const TInt64& AttrId) {
  TInt64Pr Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    IntAttrVals.DelKey(Key);
    return 0;
  } else if (FltAttrVals.IsKey(Key)) {
    FltAttrVals.DelKey(Key);
    return 0;
  } else if (StrAttrVals.IsKey(Key)) {
    StrAttrVals.DelKey(Key);
    return 0;
  }
  return -1;
}

// Delete all attributes for an id
void TAttr::DelSAttrId(const TInt64& Id) {
  for (TStrIntPr64H::TIter it = AttrNameToId.BegI(); it < AttrNameToId.EndI(); it++) {
    TAttrType CurType = static_cast<TAttrType>(it.GetDat().GetVal1().Val);
    TInt64Pr AttrKey(it.GetDat().GetVal2(), Id);
    if (CurType == atInt) {
      IntAttrVals.DelIfKey(AttrKey);
    } else if (CurType == atFlt) {
      FltAttrVals.DelIfKey(AttrKey);
    } else if (CurType == atStr) {
      StrAttrVals.DelIfKey(AttrKey);
    }
  }
}

// GetNames
void TAttr::GetSAttrV(const TInt64& Id, const TAttrType AttrType, TAttrPrV& AttrV) const {
  AttrV = TAttrPrV();
  for (TStrIntPr64H::TIter it = AttrNameToId.BegI(); it < AttrNameToId.EndI(); it++) {
    TAttrType CurType = static_cast<TAttrType>(it.GetDat().GetVal1().Val);
    if (CurType == AttrType) {
      TInt64Pr AttrKey(it.GetDat().GetVal2(), Id);
      if (CurType == atInt) {
        if (IntAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }

      } else if (CurType == atFlt) {
        if (FltAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }
      } else if (CurType == atStr) {
        if (StrAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }
      }
    }
  }
}

// Get ids of all items with given attribute
int64 TAttr::GetIdVSAttr(const TInt64& AttrId, const TAttrType Type, TInt64V& IdV) const {
  if (Type == atInt) {
    for (TIntPrInt64H::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atFlt) {
    for (TIntPrFlt64H::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atStr) {
    for (TIntPrStr64H::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else {
    return -1;
  }
  return 0;
}

int64 TAttr::GetIdVSAttr(const TInt64& AttrId, TInt64V& IdV) const {
  IdV = TInt64V();
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  TInt64StrPr TypeAndName = AttrIdToName.GetDat(AttrId);
  TAttrType Type = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  return GetIdVSAttr(AttrId, Type, IdV);
}

int64 TAttr::GetIdVSAttr(const TStr& AttrName, TInt64V& IdV) const {
  IdV = TInt64V();
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64Pr TypeAndId = AttrNameToId.GetDat(AttrName);
  TAttrType Type = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  TInt64 AttrId = TypeAndId.GetVal2();
  return GetIdVSAttr(AttrId, Type, IdV);
}

// Add Attribute mapping
int64 TAttr::AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt64& AttrId) {
  if (AttrType != atInt && AttrType != atFlt && AttrType != atStr) {
    return -1; // type must be defined and can only have a single type
  }
  if (AttrNameToId.IsKey(Name)) { return -1; }
  AttrId = AttrNameToId.GetMxKeyIds();
  TInt64Pr TypeAndId(AttrType, AttrId);
  AttrNameToId.AddDat(Name, TypeAndId);
  TInt64StrPr TypeAndName(AttrType, Name);
  AttrIdToName.AddDat(AttrId, TypeAndName);
  return 0;
}

// Attribute Name to ID conversion and vice versa
int64 TAttr::GetSAttrId(const TStr& Name, TInt64& AttrId, TAttrType& AttrType) const {
  if (!AttrNameToId.IsKey(Name)) {
    return -1;
  }
  TInt64Pr TypeAndId = AttrNameToId.GetDat(Name);
  AttrType = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  AttrId = TypeAndId.GetVal2();
  return 0;
}
int64 TAttr::GetSAttrName(const TInt64& AttrId, TStr& Name, TAttrType& AttrType) const {
  if (!AttrIdToName.IsKey(AttrId)) {
    return -1;
  }
  TInt64StrPr TypeAndName = AttrIdToName.GetDat(AttrId);
  AttrType = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  Name = TypeAndName.GetVal2();
  return 0;
}



// Add functions
int64 TAttrPair::AddSAttrDat(const TInt64Pr& Id, const TStr& AttrName, const TInt64& Val) {
  TInt64 AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atInt, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int64 TAttrPair::AddSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, const TInt64& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atInt) { return -2; }
  TIntIntPr64Pr Key(AttrId, Id);
  IntAttrVals.AddDat(Key, Val);
  return 0;
}

int64 TAttrPair::AddSAttrDat(const TInt64Pr& Id, const TStr& AttrName, const TFlt& Val) {
  TInt64 AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atFlt, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int64 TAttrPair::AddSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, const TFlt& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atFlt) { return -2; }
  TIntIntPr64Pr Key(AttrId, Id);
  FltAttrVals.AddDat(Key, Val);
  return 0;
}

int64 TAttrPair::AddSAttrDat(const TInt64Pr& Id, const TStr& AttrName, const TStr& Val) {
  TInt64 AttrId;
  if (!AttrNameToId.IsKey(AttrName)) {
    AddSAttr(AttrName, atStr, AttrId);
  } else {
    AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  }
  return AddSAttrDat(Id, AttrId, Val);
} 
int64 TAttrPair::AddSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, const TStr& Val) {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atStr) { return -2; }
  TIntIntPr64Pr Key(AttrId, Id);
  StrAttrVals.AddDat(Key, Val);
  return 0;
}

// Get functions
int64 TAttrPair::GetSAttrDat(const TInt64Pr& Id, const TStr& AttrName, TInt64& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64 AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int64 TAttrPair::GetSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, TInt64& Val) const {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atInt) { return -2; }
  TIntIntPr64Pr Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    Val = IntAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

int64 TAttrPair::GetSAttrDat(const TInt64Pr& Id, const TStr& AttrName, TFlt& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64 AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int64 TAttrPair::GetSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, TFlt& Val) const {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atFlt) { return -2; }
  TIntIntPr64Pr Key(AttrId, Id);
  if (FltAttrVals.IsKey(Key)) {
    Val = FltAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

int64 TAttrPair::GetSAttrDat(const TInt64Pr& Id, const TStr& AttrName, TStr& Val) const {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64 AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return GetSAttrDat(Id, AttrId, Val);
} 
int64 TAttrPair::GetSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, TStr& Val) const {
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  if (AttrIdToName.GetDat(AttrId).GetVal1() != atStr) { return -2; }
  TIntIntPr64Pr Key(AttrId, Id);
  if (StrAttrVals.IsKey(Key)) {
    Val = StrAttrVals.GetDat(Key);
    return 0;
  }
  return -1;
}

// Del functions
int64 TAttrPair::DelSAttrDat(const TInt64Pr& Id, const TStr& AttrName) {
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64 AttrId = AttrNameToId.GetDat(AttrName).GetVal2();
  return DelSAttrDat(Id, AttrId);
} 
int64 TAttrPair::DelSAttrDat(const TInt64Pr& Id, const TInt64& AttrId) {
  TIntIntPr64Pr Key(AttrId, Id);
  if (IntAttrVals.IsKey(Key)) {
    IntAttrVals.DelKey(Key);
    return 0;
  } else if (FltAttrVals.IsKey(Key)) {
    FltAttrVals.DelKey(Key);
    return 0;
  } else if (StrAttrVals.IsKey(Key)) {
    StrAttrVals.DelKey(Key);
    return 0;
  }
  return -1;
}

// Delete all attributes for an id
void TAttrPair::DelSAttrId(const TInt64Pr& Id) {
  for (TStrIntPr64H::TIter it = AttrNameToId.BegI(); it < AttrNameToId.EndI(); it++) {
    TAttrType CurType = static_cast<TAttrType>(it.GetDat().GetVal1().Val);
    TIntIntPr64Pr AttrKey(it.GetDat().GetVal2(), Id);
    if (CurType == atInt) {
      IntAttrVals.DelIfKey(AttrKey);
    } else if (CurType == atFlt) {
      FltAttrVals.DelIfKey(AttrKey);
    } else if (CurType == atStr) {
      StrAttrVals.DelIfKey(AttrKey);
    }
  }
}

// GetNames
void TAttrPair::GetSAttrV(const TInt64Pr& Id, const TAttrType AttrType, TAttrPrV& AttrV) const {
  AttrV = TAttrPrV();
  for (TStrIntPr64H::TIter it = AttrNameToId.BegI(); it < AttrNameToId.EndI(); it++) {
    TAttrType CurType = static_cast<TAttrType>(it.GetDat().GetVal1().Val);
    if (CurType == AttrType) {
      TIntIntPr64Pr AttrKey(it.GetDat().GetVal2(), Id);
      if (CurType == atInt) {
        if (IntAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }

      } else if (CurType == atFlt) {
        if (FltAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }
      } else if (CurType == atStr) {
        if (StrAttrVals.IsKey(AttrKey)) {
          TAttrPr Dat(it.GetKey(), CurType);
          AttrV.Add(Dat);
        }
      }
    }
  }
}

// Get ids of all items with given attribute
int64 TAttrPair::GetIdVSAttr(const TInt64& AttrId, const TAttrType Type, TIntPr64V& IdV) const {
  if (Type == atInt) {
    for (TIntIntPrPrInt64H::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atFlt) {
    for (TIntIntPrPrFlt64H::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else if (Type == atStr) {
    for (TIntIntPrPrStr64H::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
      if (it.GetKey().GetVal1() == AttrId) {
        IdV.Add(it.GetKey().GetVal2());
      }
    }
  } else {
    return -1;
  }
  return 0;
}

int64 TAttrPair::GetIdVSAttr(const TStr& AttrName, TIntPr64V& IdV) const {
  IdV = TIntPr64V();
  if (!AttrNameToId.IsKey(AttrName)) { return -1; }
  TInt64Pr TypeAndId = AttrNameToId.GetDat(AttrName);
  TAttrType Type = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  TInt64 AttrId = TypeAndId.GetVal2();
  return GetIdVSAttr(AttrId, Type, IdV);
}

int64 TAttrPair::GetIdVSAttr(const TInt64& AttrId, TIntPr64V& IdV) const {
  IdV = TIntPr64V();
  if (!AttrIdToName.IsKey(AttrId)) { return -1; }
  TInt64StrPr TypeAndName = AttrIdToName.GetDat(AttrId);
  TAttrType Type = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  return GetIdVSAttr(AttrId, Type, IdV);
}

// Add Attribute mapping
int64 TAttrPair::AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt64& AttrId) {
  if (AttrType != atInt && AttrType != atFlt && AttrType != atStr) {
    return -1; // type must be defined and can only have a single type
  }
  AttrId = AttrNameToId.GetMxKeyIds();
  TInt64Pr TypeAndId(AttrType, AttrId);
  AttrNameToId.AddDat(Name, TypeAndId);
  TInt64StrPr TypeAndName(AttrType, Name);
  AttrIdToName.AddDat(AttrId, TypeAndName);
  return 0;
}

// Attribute Name to ID conversion and vice versa
int64 TAttrPair::GetSAttrId(const TStr& Name, TInt64& AttrId, TAttrType& AttrType) const {
  if (!AttrNameToId.IsKey(Name)) {
    return -1;
  }
  TInt64Pr TypeAndId = AttrNameToId.GetDat(Name);
  AttrType = static_cast<TAttrType>(TypeAndId.GetVal1().Val);
  AttrId = TypeAndId.GetVal2();
  return 0;
}

int64 TAttrPair::GetSAttrName(const TInt64& AttrId, TStr& Name, TAttrType& AttrType) const {
  if (!AttrIdToName.IsKey(AttrId)) {
    return -1;
  }
  TInt64StrPr TypeAndName = AttrIdToName.GetDat(AttrId);
  AttrType = static_cast<TAttrType>(TypeAndName.GetVal1().Val);
  Name = TypeAndName.GetVal2();
  return 0;
}
