typedef TPair<TStr, TAttrType> TAttrPr;
typedef TVec<TAttrPr, int64> TAttrPrV;

class TAttr {
private:
  // Sparse Attributes Name Mapping
  TStrIntPr64H AttrNameToId;
  TIntIntStrPr64H AttrIdToName;

  TIntPrInt64H IntAttrVals;
  TIntPrFlt64H FltAttrVals;
  TIntPrStr64H StrAttrVals;
private:
  int64 GetIdVSAttr(const TInt64& AttrId, const TAttrType Type, TInt64V& IdV) const;
public:
  TAttr() : AttrNameToId(), AttrIdToName(), IntAttrVals(),
    FltAttrVals(), StrAttrVals() { }
  TAttr(const TAttr& Attrs) : AttrNameToId(Attrs.AttrNameToId),
    AttrIdToName(Attrs.AttrIdToName), IntAttrVals(Attrs.IntAttrVals),
    FltAttrVals(Attrs.FltAttrVals), StrAttrVals(Attrs.StrAttrVals) { }
  /// Constructor for loading attributes from a (binary) stream SIn.
  TAttr(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  /// Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }
  /// Clears the contents of the attribute map.
  void Clr() { AttrNameToId.Clr(); AttrIdToName.Clr(); IntAttrVals.Clr(); FltAttrVals.Clr(); StrAttrVals.Clr(); }

  /// Returns the amount of memory used by sparse attributes.
  size_t GetMemUsed() const { return AttrNameToId.GetMemUsed() + AttrIdToName.GetMemUsed() + IntAttrVals.GetMemUsed() + FltAttrVals.GetMemUsed() + StrAttrVals.GetMemUsed(); }

  /// Add Int attribute with name \c AttrName for the given id \c Id.
  int64 AddSAttrDat(const TInt64& Id, const TStr& AttrName, const TInt64& Val);
  /// Add Int attribute with attribute id \c AttrId for the given id \c Id.
  int64 AddSAttrDat(const TInt64& Id, const TInt64& AttrId, const TInt64& Val);

  /// Add Flt attribute with name \c AttrName for the given id \c Id.
  int64 AddSAttrDat(const TInt64& Id, const TStr& AttrName, const TFlt& Val);
  /// Add Flt attribute with attribute id \c AttrId for the given id \c Id.
  int64 AddSAttrDat(const TInt64& Id, const TInt64& AttrId, const TFlt& Val);

  /// Add Str attribute with name \c AttrName for the given id \c Id.
  int64 AddSAttrDat(const TInt64& Id, const TStr& AttrName, const TStr& Val);
  /// Add Str attribute with attribute id \c AttrId for the given id \c Id.
  int64 AddSAttrDat(const TInt64& Id, const TInt64& AttrId, const TStr& Val);

  /// Get Int attribute with name \c AttrName for the given id \c Id.
  int64 GetSAttrDat(const TInt64& Id, const TStr& AttrName, TInt64& ValX) const;
  /// Get Int attribute with attribute id \c AttrId for the given id \c Id.
  int64 GetSAttrDat(const TInt64& Id, const TInt64& AttrId, TInt64& ValX) const;

  /// Get Flt attribute with name \c AttrName for the given id \c Id.
  int64 GetSAttrDat(const TInt64& Id, const TStr& AttrName, TFlt& ValX) const;
  /// Get Flt attribute with attribute id \c AttrId for the given id \c Id.
  int64 GetSAttrDat(const TInt64& Id, const TInt64& AttrId, TFlt& ValX) const;

  /// Get Str attribute with name \c AttrName for the given id \c Id.
  int64 GetSAttrDat(const TInt64& Id, const TStr& AttrName, TStr& ValX) const;
  /// Get Str attribute with attribute id \c AttrId for the given id \c Id.
  int64 GetSAttrDat(const TInt64& Id, const TInt64& AttrId, TStr& ValX) const;

  /// Delete attribute with name \c AttrName for the given id \c Id.
  int64 DelSAttrDat(const TInt64& Id, const TStr& AttrName); 
  /// Delete attribute with attribute id \c AttrId for the given id \c Id.
  int64 DelSAttrDat(const TInt64& Id, const TInt64& AttrId);

  /// Delete all attributes for the given id \c Id.
  void DelSAttrId(const TInt64& Id);

  /// Get a list of all attributes of type \c AttrType for the given id \c Id.
  void GetSAttrV(const TInt64& Id, const TAttrType AttrType, TAttrPrV& AttrV) const;

  /// Get a list of all ids that have an attribute with name \c AttrName.
  int64 GetIdVSAttr(const TStr& AttrName, TInt64V& IdV) const;
  /// Get a list of all ids that have an attribute with attribute id \c AttrId.
  int64 GetIdVSAttr(const TInt64& AttrId, TInt64V& IdV) const;

  /// Adds a mapping for an attribute with name \c Name and type \c AttrType.
  int64 AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt64& AttrIdX);

  /// Given the attribute name \c Name, get the attribute id.
  int64 GetSAttrId(const TStr& Name, TInt64& AttrIdX, TAttrType& AttrTypeX) const;
  /// Given the attribute id \c AttrId, get the attribute name.
  int64 GetSAttrName(const TInt64& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;
};


class TAttrPair {
private:
  TStrIntPr64H AttrNameToId;
  TIntIntStrPr64H AttrIdToName;

  TIntIntPrPrInt64H IntAttrVals;
  TIntIntPrPrFlt64H FltAttrVals;
  TIntIntPrPrStr64H StrAttrVals;
private:
  int64 GetIdVSAttr(const TInt64& AttrId, const TAttrType Type, TIntPr64V& IdV) const;
public:
  TAttrPair() : AttrNameToId(), AttrIdToName(), IntAttrVals(),
    FltAttrVals(), StrAttrVals() { }
  TAttrPair(const TAttrPair& Attrs) : AttrNameToId(Attrs.AttrNameToId),
    AttrIdToName(Attrs.AttrIdToName), IntAttrVals(Attrs.IntAttrVals),
    FltAttrVals(Attrs.FltAttrVals), StrAttrVals(Attrs.StrAttrVals) { }
  /// Constructor for loading attributes from a (binary) stream SIn.
  TAttrPair(TSIn& SIn) : AttrNameToId(SIn), AttrIdToName(SIn),
    IntAttrVals(SIn), FltAttrVals(SIn), StrAttrVals(SIn) { }
  /// Saves the attributes to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    AttrNameToId.Save(SOut); AttrIdToName.Save(SOut);
    IntAttrVals.Save(SOut); FltAttrVals.Save(SOut); StrAttrVals.Save(SOut); }
  /// Clears the contents of the attribute map.
  void Clr() { AttrNameToId.Clr(); AttrIdToName.Clr(); IntAttrVals.Clr(); FltAttrVals.Clr(); StrAttrVals.Clr(); }

  /// Add Int attribute with name \c AttrName for the given id \c Id.
  int64 AddSAttrDat(const TInt64Pr& Id, const TStr& AttrName, const TInt64& Val);
  /// Add Int attribute with attribute id \c AttrId for the given id \c Id.
  int64 AddSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, const TInt64& Val);

  /// Add Flt attribute with name \c AttrName for the given id \c Id.
  int64 AddSAttrDat(const TInt64Pr& Id, const TStr& AttrName, const TFlt& Val);
  /// Add Flt attribute with attribute id \c AttrId for the given id \c Id.
  int64 AddSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, const TFlt& Val);

  /// Add Str attribute with name \c AttrName for the given id \c Id.
  int64 AddSAttrDat(const TInt64Pr& Id, const TStr& AttrName, const TStr& Val);
  /// Add Str attribute with attribute id \c AttrId for the given id \c Id.
  int64 AddSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, const TStr& Val);

  /// Get Int attribute with name \c AttrName for the given id \c Id.
  int64 GetSAttrDat(const TInt64Pr& Id, const TStr& AttrName, TInt64& ValX) const;
  /// Get Int attribute with attribute id \c AttrId for the given id \c Id.
  int64 GetSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, TInt64& ValX) const;

  /// Get Flt attribute with name \c AttrName for the given id \c Id.
  int64 GetSAttrDat(const TInt64Pr& Id, const TStr& AttrName, TFlt& ValX) const;
  /// Get Flt attribute with attribute id \c AttrId for the given id \c Id.
  int64 GetSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, TFlt& ValX) const;

  /// Get Str attribute with name \c AttrName for the given id \c Id.
  int64 GetSAttrDat(const TInt64Pr& Id, const TStr& AttrName, TStr& ValX) const;
  /// Get Str attribute with attribute id \c AttrId for the given id \c Id.
  int64 GetSAttrDat(const TInt64Pr& Id, const TInt64& AttrId, TStr& ValX) const;

  /// Delete attribute with name \c AttrName for the given id \c Id.
  int64 DelSAttrDat(const TInt64Pr& Id, const TStr& AttrName);
  /// Delete attribute with attribute id \c AttrId for the given id \c Id.
  int64 DelSAttrDat(const TInt64Pr& Id, const TInt64& AttrId);

  /// Delete all attributes for the given id \c Id.
  void DelSAttrId(const TInt64Pr& Id);

  /// Get a list of all attributes of the given type \c AttrType for the given id \c Id.
  void GetSAttrV(const TInt64Pr& Id, const TAttrType AttrType, TAttrPrV& AttrV) const;

  /// Get a list of all ids that have an attribute with name \c AttrName.
  int64 GetIdVSAttr(const TStr& AttrName, TIntPr64V& IdV) const;
  /// Get a list of all ids that have an attribute with attribute id \c AttrId.
  int64 GetIdVSAttr(const TInt64& AttrId, TIntPr64V& IdV) const;

  /// Adds a mapping for an attribute with name Name and type \c AttrType.
  int64 AddSAttr(const TStr& Name, const TAttrType& AttrType, TInt64& AttrIdX);

  /// Given the attribute name \c Name, get the attribute id and type.
  int64 GetSAttrId(const TStr& Name, TInt64& AttrIdX, TAttrType& AttrTypeX) const;
  /// Given the attribute id \c AttrId, get the attribute name and type.
  int64 GetSAttrName(const TInt64& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;
};
