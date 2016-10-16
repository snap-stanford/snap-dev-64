#include "bd.h"

/////////////////////////////////////////////////
// Hash-Table-Key-Data
#pragma pack(push, 1) // pack class size
template <class TKey, class TDat, class TSizeTy = int>
class THashKeyDat{
public:
  TInt64 Next;
  TInt64 HashCd;
  TKey Key;
  TDat Dat;
public:
  THashKeyDat():
    Next(-1), HashCd(-1), Key(), Dat(){}
  THashKeyDat(const TSizeTy& _Next, const TSizeTy& _HashCd, const TKey& _Key):
    Next(_Next), HashCd(_HashCd), Key(_Key), Dat(){}
  explicit THashKeyDat(TSIn& SIn):
    Next(SIn), HashCd(SIn), Key(SIn), Dat(SIn){}
  void Save(TSOut& SOut) const {
    Next.Save(SOut); HashCd.Save(SOut); Key.Save(SOut); Dat.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  bool operator==(const THashKeyDat& HashKeyDat) const {
    if (this==&HashKeyDat || (HashCd==HashKeyDat.HashCd
      && Key==HashKeyDat.Key && Dat==HashKeyDat.Dat)){return true;}
    return false;}
  THashKeyDat& operator=(const THashKeyDat& HashKeyDat){
    if (this!=&HashKeyDat){
      Next=HashKeyDat.Next; HashCd=HashKeyDat.HashCd;
      Key=HashKeyDat.Key; Dat=HashKeyDat.Dat;}
    return *this;}
};
#pragma pack(pop)

/////////////////////////////////////////////////
// Hash-Table-Key-Data-Iterator
template<class TKey, class TDat, class TSizeTy = int>
class THashKeyDatI{
public:
  typedef THashKeyDat<TKey, TDat, TSizeTy> THKeyDat;
private:
  THKeyDat* KeyDatI;
  THKeyDat* EndI;
public:
  THashKeyDatI(): KeyDatI(NULL), EndI(NULL){}
  THashKeyDatI(const THashKeyDatI& _HashKeyDatI):
    KeyDatI(_HashKeyDatI.KeyDatI), EndI(_HashKeyDatI.EndI){}
  THashKeyDatI(const THKeyDat* _KeyDatI, const THKeyDat* _EndI):
    KeyDatI((THKeyDat*)_KeyDatI), EndI((THKeyDat*)_EndI){}

  THashKeyDatI& operator=(const THashKeyDatI& HashKeyDatI){
    KeyDatI=HashKeyDatI.KeyDatI; EndI=HashKeyDatI.EndI; return *this;}
  bool operator==(const THashKeyDatI& HashKeyDatI) const {
    return KeyDatI==HashKeyDatI.KeyDatI;}
  bool operator<(const THashKeyDatI& HashKeyDatI) const {
    return KeyDatI<HashKeyDatI.KeyDatI;}
  THashKeyDatI& operator++(int){ KeyDatI++; while (KeyDatI < EndI && KeyDatI->HashCd==-1) { KeyDatI++; } return *this; }
  THashKeyDatI& operator--(int){ do { KeyDatI--; } while (KeyDatI->HashCd==-1); return *this;}
  THKeyDat& operator*() const { return *KeyDatI; }
  THKeyDat& operator()() const { return *KeyDatI; }
  THKeyDat* operator->() const { return KeyDatI; }
  THashKeyDatI& Next(){ operator++(1); return *this; }

  /// Tests whether the iterator has been initialized.
  bool IsEmpty() const { return KeyDatI == NULL; }
  /// Tests whether the iterator is pointing to the past-end element.
  bool IsEnd() const { return EndI == KeyDatI; }

  const TKey& GetKey() const {Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1)); return KeyDatI->Key;}
  const TDat& GetDat() const {Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1)); return KeyDatI->Dat;}
  TDat& GetDat() {Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1)); return KeyDatI->Dat;}
};

/////////////////////////////////////////////////
// Default-Hash-Function
template<class TKey, class TSizeTy = int>
class TDefaultHashFunc {
public:
 static inline TSizeTy GetPrimHashCd(const TKey& Key) { return Key.GetPrimHashCd(); }
 static inline TSizeTy GetSecHashCd(const TKey& Key) { return Key.GetSecHashCd(); }
};

/////////////////////////////////////////////////
// Hash-Table
template<class TKey, class TDat, class TSizeTy = int, class THashFunc = TDefaultHashFunc<TKey, TSizeTy> >
class THash{
public:
  enum {HashPrimes=32};
  static const unsigned int HashPrimeT[HashPrimes];
public:
  typedef THashKeyDatI<TKey, TDat, TSizeTy> TIter;
private:
  typedef THashKeyDat<TKey, TDat, TSizeTy> THKeyDat;
  typedef TPair<TKey, TDat> TKeyDatP;
  TInt64V PortV;
  TVec<THKeyDat, TSizeTy> KeyDatV;
  TBool AutoSizeP;
  TInt64 FFreeKeyId, FreeKeys;
private:
  class THashKeyDatCmp {
  public:
    const THash<TKey, TDat, TSizeTy, THashFunc>& Hash;
    bool CmpKey, Asc;
    THashKeyDatCmp(THash<TKey, TDat, TSizeTy, THashFunc>& _Hash, const bool& _CmpKey, const bool& _Asc) :
      Hash(_Hash), CmpKey(_CmpKey), Asc(_Asc) { }
    bool operator () (const TSizeTy& KeyId1, const TSizeTy& KeyId2) const {
      if (CmpKey) {
        if (Asc) { return Hash.GetKey(KeyId1) < Hash.GetKey(KeyId2); }
        else { return Hash.GetKey(KeyId2) < Hash.GetKey(KeyId1); } }
      else {
        if (Asc) { return Hash[KeyId1] < Hash[KeyId2]; }
        else { return Hash[KeyId2] < Hash[KeyId1]; } } }
  };
private:
  THKeyDat& GetHashKeyDat(const TSizeTy& KeyId){
    THKeyDat& KeyDat=KeyDatV[KeyId];
    Assert(KeyDat.HashCd!=-1); return KeyDat;}
  const THKeyDat& GetHashKeyDat(const TSizeTy& KeyId) const {
    const THKeyDat& KeyDat=KeyDatV[KeyId];
    Assert(KeyDat.HashCd!=-1); return KeyDat;}
  uint GetNextPrime(const uint& Val) const;
  void Resize();
public:
  THash():
    PortV(), KeyDatV(),
    AutoSizeP(true), FFreeKeyId(-1), FreeKeys(0){}
  THash(const THash& Hash):
    PortV(Hash.PortV), KeyDatV(Hash.KeyDatV), AutoSizeP(Hash.AutoSizeP),
    FFreeKeyId(Hash.FFreeKeyId), FreeKeys(Hash.FreeKeys){}
  explicit THash(const TSizeTy& ExpectVals, const bool& _AutoSizeP=false);
  explicit THash(TSIn& SIn):
    PortV(SIn), KeyDatV(SIn),
    AutoSizeP(SIn), FFreeKeyId(SIn), FreeKeys(SIn){
    SIn.LoadCs();}
  void Load(TSIn& SIn){
    PortV.Load(SIn); KeyDatV.Load(SIn);
    AutoSizeP=TBool(SIn); FFreeKeyId=TInt64(SIn); FreeKeys=TInt64(SIn);
    SIn.LoadCs();}
  void Save(TSOut& SOut) const {
    PortV.Save(SOut); KeyDatV.Save(SOut);
    AutoSizeP.Save(SOut); FFreeKeyId.Save(SOut); FreeKeys.Save(SOut);
    SOut.SaveCs();}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm);

  THash& operator=(const THash& Hash){
    if (this!=&Hash){
      PortV=Hash.PortV; KeyDatV=Hash.KeyDatV; AutoSizeP=Hash.AutoSizeP;
      FFreeKeyId=Hash.FFreeKeyId; FreeKeys=Hash.FreeKeys;}
    return *this;}
  bool operator==(const THash& Hash) const; //J: zdaj tak kot je treba
  bool operator < (const THash& Hash) const { Fail; return true; }
  /// The [] operator takes KeyId, use GetDat() if you need value access via the key.
  const TDat& operator[](const TSizeTy& KeyId) const {return GetHashKeyDat(KeyId).Dat;}
  TDat& operator[](const TSizeTy& KeyId){return GetHashKeyDat(KeyId).Dat;}
  TDat& operator()(const TKey& Key){return AddDat(Key);}
  ::TSize GetMemUsed() const {
    // return PortV.GetMemUsed()+KeyDatV.GetMemUsed()+sizeof(bool)+2*sizeof(int);}
      int64 MemUsed = sizeof(bool)+2*sizeof(int64);
      MemUsed += int64(PortV.Reserved()) * int64(sizeof(TInt64));
      for (TSizeTy KeyDatN = 0; KeyDatN < KeyDatV.Len(); KeyDatN++) {
          MemUsed += int64(2 * sizeof(TInt64));
          MemUsed += int64(KeyDatV[KeyDatN].Key.GetMemUsed());
          MemUsed += int64(KeyDatV[KeyDatN].Dat.GetMemUsed());
      }
      return ::TSize(MemUsed);
  }

  TIter BegI() const {
    if (Len() == 0){return TIter(KeyDatV.EndI(), KeyDatV.EndI());}
    if (IsKeyIdEqKeyN()) { return TIter(KeyDatV.BegI(), KeyDatV.EndI());}
    TSizeTy FKeyId=-1;  FNextKeyId(FKeyId);
    return TIter(KeyDatV.BegI()+FKeyId, KeyDatV.EndI()); }
  TIter EndI() const {return TIter(KeyDatV.EndI(), KeyDatV.EndI());}
  //TIter GetI(const int& KeyId) const {return TIter(&KeyDatV[KeyId], KeyDatV.EndI());}
  TIter GetI(const TKey& Key) const {return TIter(&KeyDatV[GetKeyId(Key)], KeyDatV.EndI());}

  void Gen(const TSizeTy& ExpectVals){
    PortV.Gen(GetNextPrime(ExpectVals/2)); KeyDatV.Gen(ExpectVals, 0);
    FFreeKeyId=-1; FreeKeys=0; PortV.PutAll(TInt64(-1));}

  void Clr(const bool& DoDel=true, const TSizeTy& NoDelLim=-1, const bool& ResetDat=true);
  bool Empty() const {return Len()==0;}
  TSizeTy Len() const {return KeyDatV.Len()-FreeKeys;}
  TSizeTy GetPorts() const {return PortV.Len();}
  bool IsAutoSize() const {return AutoSizeP;}
  TSizeTy GetMxKeyIds() const {return KeyDatV.Len();}
  TSizeTy GetReservedKeyIds() const {return KeyDatV.Reserved();}
  bool IsKeyIdEqKeyN() const {return FreeKeys==0;}

  TSizeTy AddKey(const TKey& Key);
  TDat& AddDatId(const TKey& Key){
	  TSizeTy KeyId=AddKey(Key); return KeyDatV[KeyId].Dat=KeyId;}
  TDat& AddDat(const TKey& Key){return KeyDatV[AddKey(Key)].Dat;}
  TDat& AddDat(const TKey& Key, const TDat& Dat){
    return KeyDatV[AddKey(Key)].Dat=Dat;}

  void DelKey(const TKey& Key);
  bool DelIfKey(const TKey& Key){
	  TSizeTy KeyId; if (IsKey(Key, KeyId)){DelKeyId(KeyId); return true;} return false;}
  void DelKeyId(const TSizeTy& KeyId){DelKey(GetKey(KeyId));}
  void DelKeyIdV(const TInt64V& KeyIdV){
    for (TSizeTy KeyIdN=0; KeyIdN<KeyIdV.Len(); KeyIdN++){DelKeyId(KeyIdV[KeyIdN]);}}

  void MarkDelKey(const TKey& Key); // marks the record as deleted - doesn't delete Dat (to avoid fragmentation)
  void MarkDelKeyId(const TSizeTy& KeyId){MarkDelKey(GetKey(KeyId));}

  const TKey& GetKey(const TSizeTy& KeyId) const { return GetHashKeyDat(KeyId).Key;}
  TSizeTy GetKeyId(const TKey& Key) const;
  /// Get an index of a random element. If the hash table has many deleted keys, this may take a long time.
  TSizeTy GetRndKeyId(TRnd& Rnd) const;
  /// Get an index of a random element. If the hash table has many deleted keys, defrag the hash table first (that's why the function is non-const).
  TSizeTy GetRndKeyId(TRnd& Rnd, const double& EmptyFrac);
  bool IsKey(const TKey& Key) const {return GetKeyId(Key)!=-1;}
  bool IsKey(const TKey& Key, TSizeTy& KeyId) const { KeyId=GetKeyId(Key); return KeyId!=-1;}
  bool IsKeyId(const TSizeTy& KeyId) const {
    return (0<=KeyId)&&(KeyId<KeyDatV.Len())&&(KeyDatV[KeyId].HashCd!=-1);}
  const TDat& GetDat(const TKey& Key) const {return KeyDatV[GetKeyId(Key)].Dat;}
  TDat& GetDat(const TKey& Key){return KeyDatV[GetKeyId(Key)].Dat;}
  TDat GetDatWithDefault(const TKey& Key, TDat DefaultValue) {
	  TSizeTy KeyId = GetKeyId(Key);
    return KeyId >= 0 ? KeyDatV[KeyId].Dat : DefaultValue;
  }
//  TKeyDatP GetKeyDat(const int& KeyId) const {
//    TKeyDat& KeyDat=GetHashKeyDat(KeyId);
//    return TKeyDatP(KeyDat.Key, KeyDat.Dat);}
  void GetKeyDat(const TSizeTy& KeyId, TKey& Key, TDat& Dat) const {
    const THKeyDat& KeyDat=GetHashKeyDat(KeyId);
    Key=KeyDat.Key; Dat=KeyDat.Dat;}
  bool IsKeyGetDat(const TKey& Key, TDat& Dat) const {TSizeTy KeyId;
    if (IsKey(Key, KeyId)){Dat=GetHashKeyDat(KeyId).Dat; return true;}
    else {return false;}}

  TSizeTy FFirstKeyId() const {return 0-1;}
  bool FNextKeyId(TSizeTy& KeyId) const;
  void GetKeyV(TVec<TKey, TSizeTy>& KeyV) const;
  void GetDatV(TVec<TDat, TSizeTy>& DatV) const;
  void GetKeyDatPrV(TVec<TPair<TKey, TDat>, TSizeTy >& KeyDatPrV) const;
  void GetDatKeyPrV(TVec<TPair<TDat, TKey>, TSizeTy >& DatKeyPrV) const;
  void GetKeyDatKdV(TVec<TKeyDat<TKey, TDat>, TSizeTy >& KeyDatKdV) const;
  void GetDatKeyKdV(TVec<TKeyDat<TDat, TKey>, TSizeTy >& DatKeyKdV) const;

  void Swap(THash& Hash);
  void Defrag();
  void Pack(){KeyDatV.Pack();}
  void Sort(const bool& CmpKey, const bool& Asc);
  void SortByKey(const bool& Asc=true) { Sort(true, Asc); }
  void SortByDat(const bool& Asc=true) { Sort(false, Asc); }
};

template<class TKey, class TDat, class TSizeTy, class THashFunc>
const unsigned int THash<TKey, TDat, TSizeTy, THashFunc>::HashPrimeT[HashPrimes]={
  3ul, 5ul, 11ul, 23ul,
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
  1610612741ul, 3221225473ul, 4294967291ul
};

template<class TKey, class TDat, class TSizeTy, class THashFunc>
uint THash<TKey, TDat, TSizeTy, THashFunc>::GetNextPrime(const uint& Val) const {
  const uint* f=(const uint*)HashPrimeT, *m, *l=(const uint*)HashPrimeT + (int)HashPrimes;
  int h, len = (int)HashPrimes;
  while (len > 0) {
    h = len >> 1;  m = f + h;
    if (*m < Val) { f = m;  f++;  len = len - h - 1; }
    else len = h;
  }
  return f == l ? *(l - 1) : *f;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::Resize(){
  // resize & initialize port vector
  //if (PortV.Len()==0){PortV.Gen(17);}
  //else {PortV.Gen(2*PortV.Len()+1);}
  if (PortV.Len()==0){
    PortV.Gen(17);
  } else if (AutoSizeP&&(KeyDatV.Len()>2*PortV.Len())){
    PortV.Gen(GetNextPrime(PortV.Len()+1));
  } else {
    return;
  }
  PortV.PutAll(TInt64(-1));
  // rehash keys
  for (TSizeTy KeyId=0; KeyId<KeyDatV.Len(); KeyId++){
    THKeyDat& KeyDat=KeyDatV[KeyId];
    if (KeyDat.HashCd!=-1){
      const TSizeTy PortN = abs(THashFunc::GetPrimHashCd(KeyDat.Key) % PortV.Len());
      KeyDat.Next=PortV[PortN];
      PortV[PortN]=KeyId;
    }
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
THash<TKey, TDat, TSizeTy, THashFunc>::THash(const TSizeTy& ExpectVals, const bool& _AutoSizeP):
  PortV(GetNextPrime(ExpectVals/2)), KeyDatV(ExpectVals, 0),
  AutoSizeP(_AutoSizeP), FFreeKeyId(-1), FreeKeys(0){
  PortV.PutAll(TInt64(-1));
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
bool THash<TKey, TDat, TSizeTy, THashFunc>::operator==(const THash& Hash) const {
  if (Len() != Hash.Len()) { return false; }
  for (TSizeTy i = FFirstKeyId(); FNextKeyId(i); ) {
    const TKey& Key = GetKey(i);
    if (! Hash.IsKey(Key)) { return false; }
    if (GetDat(Key) != Hash.GetDat(Key)) { return false; }
  }
  return true;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::Clr(const bool& DoDel, const TSizeTy& NoDelLim, const bool& ResetDat){
  if (DoDel){
    PortV.Clr(); KeyDatV.Clr();
  } else {
    PortV.PutAll(TInt64(-1));
    KeyDatV.Clr(DoDel, NoDelLim);
    if (ResetDat){KeyDatV.PutAll(THKeyDat());}
  }
  FFreeKeyId=TInt64(-1); FreeKeys=TInt64(0);
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THash<TKey, TDat, TSizeTy, THashFunc>::AddKey(const TKey& Key){
  if ((KeyDatV.Len()>2*PortV.Len())||PortV.Empty()){Resize();}
  const TSizeTy PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const TSizeTy HashCd=abs(THashFunc::GetSecHashCd(Key));
  TSizeTy PrevKeyId=-1;
  TSizeTy KeyId=PortV[PortN];
  while ((KeyId!=-1) &&
   !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
    PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}

  if (KeyId==-1){
    if (FFreeKeyId==-1){
      KeyId=KeyDatV.Add(THKeyDat(-1, HashCd, Key));
    } else {
      KeyId=FFreeKeyId; FFreeKeyId=KeyDatV[FFreeKeyId].Next; FreeKeys--;
      //KeyDatV[KeyId]=TKeyDat(-1, HashCd, Key); // slow version
      KeyDatV[KeyId].Next=-1;
      KeyDatV[KeyId].HashCd=HashCd;
      KeyDatV[KeyId].Key=Key;
      //KeyDatV[KeyId].Dat=TDat(); // already empty
    }
    if (PrevKeyId==-1){
      PortV[PortN]=KeyId;
    } else {
      KeyDatV[PrevKeyId].Next=KeyId;
    }
  }
  return KeyId;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::DelKey(const TKey& Key){
  IAssert(!PortV.Empty());
  const TSizeTy PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const TSizeTy HashCd=abs(THashFunc::GetSecHashCd(Key));
  TSizeTy PrevKeyId=-1;
  TSizeTy KeyId=PortV[PortN];

  while ((KeyId!=-1) &&
   !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
    PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}

  //IAssertR(KeyId!=-1, Key.GetStr()); //J: some classes do not provide GetStr()?
  IAssert(KeyId!=-1); //J: some classes do not provide GetStr()?
  if (PrevKeyId==-1){PortV[PortN]=KeyDatV[KeyId].Next;}
  else {KeyDatV[PrevKeyId].Next=KeyDatV[KeyId].Next;}
  KeyDatV[KeyId].Next=FFreeKeyId; FFreeKeyId=KeyId; FreeKeys++;
  KeyDatV[KeyId].HashCd=TInt64(-1);
  KeyDatV[KeyId].Key=TKey();
  KeyDatV[KeyId].Dat=TDat();
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::MarkDelKey(const TKey& Key){
  // MarkDelKey is same as Delkey except last two lines
  IAssert(!PortV.Empty());
  const TSizeTy PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const TSizeTy HashCd=abs(THashFunc::GetSecHashCd(Key));
  TSizeTy PrevKeyId=-1;
  TSizeTy KeyId=PortV[PortN];
  while ((KeyId!=-1) &&
   !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
    PrevKeyId=KeyId; KeyId=KeyDatV[KeyId].Next;}
  IAssertR(KeyId!=-1, Key.GetStr());
  if (PrevKeyId==-1){PortV[PortN]=KeyDatV[KeyId].Next;}
  else {KeyDatV[PrevKeyId].Next=KeyDatV[KeyId].Next;}
  KeyDatV[KeyId].Next=FFreeKeyId; FFreeKeyId=KeyId; FreeKeys++;
  KeyDatV[KeyId].HashCd=TInt64(-1);
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THash<TKey, TDat, TSizeTy, THashFunc>::GetRndKeyId(TRnd& Rnd) const  {
  IAssert(! Empty());
  TSizeTy KeyId = abs(Rnd.GetUniDevInt64(KeyDatV.Len()));
  while (KeyDatV[KeyId].HashCd == -1) { // if the index is empty, just try again
    KeyId = abs(Rnd.GetUniDevInt64(KeyDatV.Len())); }
  return KeyId;
}

// return random KeyId even if the hash table contains deleted keys
// defrags the table if necessary
template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THash<TKey, TDat, TSizeTy, THashFunc>::GetRndKeyId(TRnd& Rnd, const double& EmptyFrac) {
  IAssert(! Empty());
  if (FreeKeys/double(Len()+FreeKeys) > EmptyFrac) { Defrag(); }
  TSizeTy KeyId = Rnd.GetUniDevInt64(KeyDatV.Len());
  while (KeyDatV[KeyId].HashCd == -1) { // if the index is empty, just try again
    KeyId = Rnd.GetUniDevInt64(KeyDatV.Len());
  }
  return KeyId;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THash<TKey, TDat, TSizeTy, THashFunc>::GetKeyId(const TKey& Key) const {
  if (PortV.Empty()){return -1;}
  const TSizeTy PortN=abs(THashFunc::GetPrimHashCd(Key)%PortV.Len());
  const TSizeTy HashCd=abs(THashFunc::GetSecHashCd(Key));
  TSizeTy KeyId=PortV[PortN];
  while ((KeyId!=-1) &&
   !((KeyDatV[KeyId].HashCd==HashCd) && (KeyDatV[KeyId].Key==Key))){
    KeyId=KeyDatV[KeyId].Next;}
  return KeyId;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
bool THash<TKey, TDat, TSizeTy, THashFunc>::FNextKeyId(TSizeTy& KeyId) const {
  do {KeyId++;} while ((KeyId<KeyDatV.Len())&&(KeyDatV[KeyId].HashCd==-1));
  return KeyId<KeyDatV.Len();
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::GetKeyV(TVec<TKey, TSizeTy>& KeyV) const {
  KeyV.Gen(Len(), 0);
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    KeyV.Add(GetKey(KeyId));}
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::GetDatV(TVec<TDat, TSizeTy>& DatV) const {
  DatV.Gen(Len(), 0);
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    DatV.Add(GetHashKeyDat(KeyId).Dat);}
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::GetKeyDatPrV(TVec<TPair<TKey, TDat>, TSizeTy >& KeyDatPrV) const {
  KeyDatPrV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    KeyDatPrV.Add(TPair<TKey, TDat>(Key, Dat));
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::GetDatKeyPrV(TVec<TPair<TDat, TKey>, TSizeTy >& DatKeyPrV) const {
  DatKeyPrV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    DatKeyPrV.Add(TPair<TDat, TKey>(Dat, Key));
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::GetKeyDatKdV(TVec<TKeyDat<TKey, TDat>, TSizeTy >& KeyDatKdV) const {
  KeyDatKdV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    KeyDatKdV.Add(TKeyDat<TKey, TDat>(Key, Dat));
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::GetDatKeyKdV(TVec<TKeyDat<TDat, TKey>, TSizeTy >& DatKeyKdV) const {
  DatKeyKdV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    DatKeyKdV.Add(TKeyDat<TDat, TKey>(Dat, Key));
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::Swap(THash& Hash) {
  if (this!=&Hash){
    PortV.Swap(Hash.PortV);
    KeyDatV.Swap(Hash.KeyDatV);
    ::Swap(AutoSizeP, Hash.AutoSizeP);
    ::Swap(FFreeKeyId, Hash.FFreeKeyId);
    ::Swap(FreeKeys, Hash.FreeKeys);
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::Defrag(){
  if (!IsKeyIdEqKeyN()){
    THash<TKey, TDat, TSizeTy, THashFunc> Hash(PortV.Len());
    TSizeTy KeyId=FFirstKeyId(); TKey Key; TDat Dat;
    while (FNextKeyId(KeyId)){
      GetKeyDat(KeyId, Key, Dat);
      Hash.AddDat(Key, Dat);
    }
    Pack();
    operator=(Hash);
    IAssert(IsKeyIdEqKeyN());
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THash<TKey, TDat, TSizeTy, THashFunc>::Sort(const bool& CmpKey, const bool& Asc) {
  IAssertR(IsKeyIdEqKeyN(), "THash::Sort only works when table has no deleted keys.");
  TInt64V TargV(Len()), MapV(Len()), StateV(Len());
  for (TSizeTy i = 0; i < TargV.Len(); i++) {
    TargV[i] = i; MapV[i] = i; StateV[i] = i;
  }
  // sort KeyIds
  THashKeyDatCmp HashCmp(*this, CmpKey, Asc);
  TargV.SortCmp(HashCmp);
  // now sort the update vector
  THashKeyDat<TKey, TDat, TSizeTy> Tmp;
  for (TSizeTy i = 0; i < TargV.Len()-1; i++) {
    const TSizeTy SrcPos = MapV[TargV[i]];
    const TSizeTy Loc = i;
    // swap data
    Tmp = KeyDatV[SrcPos];
    KeyDatV[SrcPos] = KeyDatV[Loc];
    KeyDatV[Loc] = Tmp;
    // swap keys
    MapV[StateV[i]] = SrcPos;
    StateV.Swap(Loc, SrcPos);
  }
  for (TSizeTy i = 0; i < TargV.Len(); i++) {
    MapV[TargV[i]] = i; }
  for (TSizeTy p = 0; p < PortV.Len(); p++) {
    if (PortV[p] != -1) {
      PortV[p] = MapV[PortV[p]]; } }
  for (TSizeTy i = 0; i < KeyDatV.Len(); i++) {
    if (KeyDatV[i].Next != -1) {
      KeyDatV[i].Next = MapV[KeyDatV[i].Next]; }
  }
}

/////////////////////////////////////////////////
// Common-Hash-Types
typedef THash<TCh, TCh> TChChH;
typedef THash<TChTr, TInt> TChTrIntH;
typedef THash<TInt, TInt> TIntH;
typedef THash<TUInt64, TInt> TUInt64H;
typedef THash<TInt, TBool> TIntBoolH;
typedef THash<TInt, TInt> TIntIntH;
typedef THash<TInt, TUInt64> TIntUInt64H;
typedef THash<TInt, TIntFltPr> TIntIntFltPrH;
typedef THash<TInt, TIntV> TIntIntVH;
typedef THash<TInt, TIntH> TIntIntHH;
typedef THash<TInt, TFlt> TIntFltH;
typedef THash<TInt, TFltPr> TIntFltPrH;
typedef THash<TInt, TFltTr> TIntFltTrH;
typedef THash<TInt, TFltV> TIntFltVH;
typedef THash<TInt, TStr> TIntStrH;
typedef THash<TInt, TStrV> TIntStrVH;
typedef THash<TInt, TStrPrV> TIntStrPrVH;
typedef THash<TInt, TIntPr> TIntIntPrH;
typedef THash<TInt, TIntPrV> TIntIntPrVH;
typedef THash<TInt, TIntStrPr> TIntIntStrPrH;
typedef THash<TInt, TIntVFltVPr> TIntIntVFltVPrH;
typedef THash<TUInt64, TStrV> TUInt64StrVH;
typedef THash<TIntPr, TInt> TIntPrIntH;
typedef THash<TIntPr, TIntPr> TIntPrH;
typedef THash<TIntPr, TIntV> TIntPrIntVH;
typedef THash<TIntPr, TIntPrV> TIntPrIntPrVH;
typedef THash<TIntTr, TInt> TIntTrIntH;
typedef THash<TIntV, TInt> TIntVIntH;
typedef THash<TUInt, TUInt> TUIntH;
typedef THash<TIntPr, TInt> TIntPrIntH;
typedef THash<TIntPr, TIntV> TIntPrIntVH;
typedef THash<TIntPr, TFlt> TIntPrFltH;
typedef THash<TIntTr, TFlt> TIntTrFltH;
typedef THash<TIntPr, TStr> TIntPrStrH;
typedef THash<TIntPr, TStrV> TIntPrStrVH;
typedef THash<TIntStrPr, TInt> TIntStrPrIntH;
typedef THash<TIntIntPrPr, TInt> TIntIntPrPrIntH;
typedef THash<TIntIntPrPr, TFlt> TIntIntPrPrFltH;
typedef THash<TIntIntPrPr, TStr> TIntIntPrPrStrH;
typedef THash<TFlt, TFlt> TFltFltH;
typedef THash<TStr, TInt> TStrH;
typedef THash<TStr, TBool> TStrBoolH;
typedef THash<TStr, TInt> TStrIntH;
typedef THash<TStr, TIntPr> TStrIntPrH;
typedef THash<TStr, TIntV> TStrIntVH;
typedef THash<TStr, TUInt64> TStrUInt64H;
typedef THash<TStr, TUInt64V> TStrUInt64VH;
typedef THash<TStr, TIntPrV> TStrIntPrVH;
typedef THash<TStr, TFlt> TStrFltH;
typedef THash<TStr, TFltV> TStrFltVH;
typedef THash<TStr, TStr> TStrStrH;
typedef THash<TStr, TStrPr> TStrStrPrH;
typedef THash<TStr, TStrV> TStrStrVH;
typedef THash<TStr, TStrPrV> TStrStrPrVH;
typedef THash<TStr, TStrKdV> TStrStrKdVH;
typedef THash<TStr, TIntFltPr> TStrIntFltPrH;
typedef THash<TStr, TStrIntPrV> TStrStrIntPrVH;
typedef THash<TStr, TStrIntKdV> TStrStrIntKdVH;
typedef THash<TDbStr, TInt> TDbStrIntH;
typedef THash<TDbStr, TStr> TDbStrStrH;
typedef THash<TStrPr, TBool> TStrPrBoolH;
typedef THash<TStrPr, TInt> TStrPrIntH;
typedef THash<TStrPr, TFlt> TStrPrFltH;
typedef THash<TStrPr, TStr> TStrPrStrH;
typedef THash<TStrPr, TStrV> TStrPrStrVH;
typedef THash<TStrTr, TInt> TStrTrIntH;
typedef THash<TStrIntPr, TInt> TStrIntPrIntH;
typedef THash<TStrV, TInt> TStrVH;
typedef THash<TStrV, TInt> TStrVIntH;
typedef THash<TStrV, TIntV> TStrVIntVH;
typedef THash<TStrV, TStr> TStrVStrH;
typedef THash<TStrV, TStrV> TStrVStrVH;

/////////////////////////////////////////////////
// Hash-Pointer
template <class TKey, class TDat, class TSizeTy = int>
class PHash{
private:
  TCRef CRef;
public:
  THash<TKey, TDat, TSizeTy> H;
public:
  PHash<TKey, TDat, TSizeTy>(): H(){}
  static TPt<PHash<TKey, TDat, TSizeTy> > New(){
    return new PHash<TKey, TDat, TSizeTy>();}
  PHash<TKey, TDat, TSizeTy>(const TSizeTy& MxVals, const TSizeTy& Vals): H(MxVals, Vals){}
  static TPt<PHash<TKey, TDat, TSizeTy> > New(const TSizeTy& MxVals, const TSizeTy& Vals){
    return new PHash<TKey, TDat, TSizeTy>(MxVals, Vals);}
  PHash<TKey, TDat, TSizeTy>(const THash<TKey, TDat, TSizeTy>& _V): H(_V){}
  static TPt<PHash<TKey, TDat, TSizeTy> > New(const THash<TKey, TDat, TSizeTy>& H){
    return new PHash<TKey, TDat, TSizeTy>(H);}
  explicit PHash<TKey, TDat, TSizeTy>(TSIn& SIn): H(SIn){}
  static TPt<PHash<TKey, TDat, TSizeTy> > Load(TSIn& SIn){return new PHash<TKey, TDat, TSizeTy>(SIn);}
  void Save(TSOut& SOut) const {H.Save(SOut);}

  PHash<TKey, TDat, TSizeTy>& operator=(const PHash<TKey, TDat, TSizeTy>& Vec){
    if (this!=&Vec){H=Vec.H;} return *this;}
  bool operator==(const PHash<TKey, TDat, TSizeTy>& Vec) const {return H==Vec.H;}
  bool operator<(const PHash<TKey, TDat, TSizeTy>& Vec) const {return H<Vec.H;}

  friend class TPt<PHash<TKey, TDat, TSizeTy> >;
};

/////////////////////////////////////////////////
// Big-String-Pool (holds up to 2 giga strings, storage overhead is 8(4) bytes per string)
//J: have to put it here since it uses TVec (can't be in dt.h)
ClassTP(TBigStrPool, PBigStrPool)//{
private:
  TSize MxBfL, BfL;
  uint64 GrowBy;
  char *Bf;
  TVec<TSize, int64> IdOffV; // string ID to offset
private:
  void Resize(TSize _MxBfL);
public:
  TBigStrPool(TSize MxBfLen = 0, uint64 _GrowBy = 16*1024*1024);
  TBigStrPool(TSIn& SIn, bool LoadCompact = true);
  TBigStrPool(const TBigStrPool& Pool) : MxBfL(Pool.MxBfL), BfL(Pool.BfL), GrowBy(Pool.GrowBy) {
    Bf = (char *) malloc(Pool.MxBfL); IAssert(Bf); memcpy(Bf, Pool.Bf, Pool.BfL); }
  ~TBigStrPool() { if (Bf) free(Bf); else IAssert(MxBfL == 0);  MxBfL = 0; BfL = 0; }

  static PBigStrPool New(TSize _MxBfLen = 0, uint64 _GrowBy = 16*1024*1024) { return PBigStrPool(new TBigStrPool(_MxBfLen, _GrowBy)); }
  static PBigStrPool New(TSIn& SIn) { return new TBigStrPool(SIn); }
  static PBigStrPool New(const TStr& fileName) { PSIn SIn = TFIn::New(fileName); return new TBigStrPool(*SIn); }
  static PBigStrPool Load(TSIn& SIn, bool LoadCompacted = true) { return PBigStrPool(new TBigStrPool(SIn, LoadCompacted)); }
  void Save(TSOut& SOut) const;
  void Save(const TStr& fileName) { TFOut FOut(fileName); Save(FOut); }

  int64 GetStrs() const { return IdOffV.Len(); }
  TSize Len() const { return BfL; }
  TSize Size() const { return MxBfL; }
  bool Empty() const { return ! Len(); }
  char* operator () () const { return Bf; }
  TBigStrPool& operator = (const TBigStrPool& Pool);
  ::TSize GetMemUsed(){
  	return 4 * sizeof(int64) + IdOffV.GetMemUsed() + MxBfL;
  }

  int64 AddStr(const char *Str, uint64 Len);
  int64 AddStr(const char *Str) { return AddStr(Str, uint64(strlen(Str)) + 1); }
  int64 AddStr(const TStr& Str) { return AddStr(Str.CStr(), Str.Len() + 1); }

  TStr GetStr(const int64& StrId) const { Assert(StrId < GetStrs());
    if (StrId == 0) return TStr::GetNullStr(); else return TStr(Bf + (TSize)IdOffV[StrId]); }
  const char *GetCStr(const int64& StrId) const { Assert(StrId < GetStrs());
    if (StrId == 0) return TStr::GetNullStr().CStr(); else return (Bf + (TSize)IdOffV[StrId]); }

  TStr GetStrFromOffset(const TSize& Offset) const { Assert(Offset < BfL);
    if (Offset == 0) return TStr::GetNullStr(); else return TStr(Bf + Offset); }
  const char *GetCStrFromOffset(const TSize& Offset) const { Assert(Offset < BfL);
    if (Offset == 0) return TStr::GetNullStr().CStr(); else return Bf + Offset; }

  void Clr(bool DoDel = false) { BfL = 0; if (DoDel && Bf) { free(Bf); Bf = 0; MxBfL = 0; } }
  int Cmp(const int64& StrId, const char *Str) const { Assert(StrId < GetStrs());
    if (StrId != 0) return strcmp(Bf + (TSize)IdOffV[StrId], Str); else return strcmp("", Str); }

  static int GetPrimHashCd(const char *CStr);
  static int GetSecHashCd(const char *CStr);
  int64 GetPrimHashCd(const int64& StrId) { Assert(StrId < GetStrs());
    if (StrId != 0) return GetPrimHashCd(Bf + (TSize)IdOffV[StrId]); else return GetPrimHashCd(""); }
  int64 GetSecHashCd(const int64& StrId) { Assert(StrId < GetStrs());
    if (StrId != 0) return GetSecHashCd(Bf + (TSize)IdOffV[StrId]); else return GetSecHashCd(""); }
};

/////////////////////////////////////////////////
// String-Hash-Table
template <class TDat, class TStringPool = TStrPool, class TSizeTy = int, class THashFunc = TDefaultHashFunc<TStr> >
class TStrHash{
public:
  //typedef typename PStringPool::TObj TStringPool;
  typedef TPt<TStringPool> PStringPool;
private:
  typedef THashKeyDat<TInt64, TDat> THKeyDat;
  typedef TPair<TInt64, TDat> TKeyDatP;
  typedef TVec<THKeyDat, TSizeTy> THKeyDatV;
  TInt64V PortV;
  THKeyDatV KeyDatV;
  TBool AutoSizeP;
  TInt64 FFreeKeyId, FreeKeys;
  PStringPool Pool;
private:
  uint GetNextPrime(const uint& Val) const;
  void Resize();
  const THKeyDat& GetHashKeyDat(const TSizeTy& KeyId) const {
    const THKeyDat& KeyDat = KeyDatV[KeyId];  Assert(KeyDat.HashCd != -1);  return KeyDat; }
  THKeyDat& GetHashKeyDat(const TSizeTy& KeyId) {
    THKeyDat& KeyDat = KeyDatV[KeyId];  Assert(KeyDat.HashCd != -1);  return KeyDat; }
public:
  TStrHash(): PortV(), KeyDatV(), AutoSizeP(true), FFreeKeyId(-1), FreeKeys(0), Pool() { }
  TStrHash(const PStringPool& StrPool): PortV(), KeyDatV(), AutoSizeP(true), FFreeKeyId(-1), FreeKeys(0), Pool(StrPool) { }
  TStrHash(const TSizeTy& Ports, const bool& _AutoSizeP = false, const PStringPool& StrPool = PStringPool()) :
    PortV(Ports), KeyDatV(Ports, 0), AutoSizeP(_AutoSizeP), FFreeKeyId(-1), FreeKeys(0), Pool(StrPool) { PortV.PutAll(-1); }
  TStrHash(const TStrHash& Hash): PortV(Hash.PortV), KeyDatV(Hash.KeyDatV), AutoSizeP(Hash.AutoSizeP),
    FFreeKeyId(Hash.FFreeKeyId), FreeKeys(Hash.FreeKeys), Pool() {
      if (! Hash.Pool.Empty()) { Pool=PStringPool(new TStringPool(*Hash.Pool)); } }
  TStrHash(TSIn& SIn, bool PoolToo = true): PortV(SIn), KeyDatV(SIn), AutoSizeP(SIn), FFreeKeyId(SIn), FreeKeys(SIn){ SIn.LoadCs(); if (PoolToo) Pool = PStringPool(SIn); }

  void Load(TSIn& SIn, bool PoolToo = true) { PortV.Load(SIn); KeyDatV.Load(SIn); AutoSizeP.Load(SIn); FFreeKeyId.Load(SIn);
    FreeKeys.Load(SIn); SIn.LoadCs(); if (PoolToo) Pool = PStringPool(SIn); }
  void Save(TSOut& SOut, bool PoolToo = true) const { PortV.Save(SOut); KeyDatV.Save(SOut);
    AutoSizeP.Save(SOut); FFreeKeyId.Save(SOut); FreeKeys.Save(SOut); SOut.SaveCs(); if (PoolToo) Pool.Save(SOut); }

  void SetPool(const PStringPool& StrPool) { IAssert(Pool.Empty() || Pool->Empty()); Pool = StrPool; }
  PStringPool GetPool() const { return Pool; }

  TStrHash& operator = (const TStrHash& Hash);

  bool Empty() const {return ! Len(); }
  TSizeTy Len() const { return KeyDatV.Len() - FreeKeys; }
  TSizeTy Reserved() const { return KeyDatV.Reserved(); }
  TSizeTy GetPorts() const { return PortV.Len(); }
  bool IsAutoSize() const { return AutoSizeP; }
  TSizeTy GetMxKeyIds() const { return KeyDatV.Len(); }
  bool IsKeyIdEqKeyN() const {return ! FreeKeys; }

  TSizeTy AddKey(const char *Key);
  TSizeTy AddKey(const TStr& Key) { return AddKey(Key.CStr()); }
  TSizeTy AddKey(const TChA& Key) { return AddKey(Key.CStr()); }
  TSizeTy AddDat(const char *Key, const TDat& Dat) { const TSizeTy KeyId = AddKey(Key); KeyDatV[KeyId].Dat = Dat; return KeyId; }
  TSizeTy AddDat(const TStr& Key, const TDat& Dat) { const TSizeTy KeyId = AddKey(Key.CStr()); KeyDatV[KeyId].Dat = Dat; return KeyId; }
  TSizeTy AddDat(const TChA& Key, const TDat& Dat) { const TSizeTy KeyId = AddKey(Key.CStr()); KeyDatV[KeyId].Dat = Dat; return KeyId; }
  TDat& AddDat(const char *Key) { return KeyDatV[AddKey(Key)].Dat; }
  TDat& AddDat(const TStr& Key) { return KeyDatV[AddKey(Key.CStr())].Dat; }
  TDat& AddDat(const TChA& Key) { return KeyDatV[AddKey(Key.CStr())].Dat; }
  TDat& AddDatId(const char *Key) { const TSizeTy KeyId = AddKey(Key);  return KeyDatV[KeyId].Dat = KeyId; }
  TDat& AddDatId(const TStr& Key) { const TSizeTy KeyId = AddKey(Key.CStr());  return KeyDatV[KeyId].Dat = KeyId; }
  TDat& AddDatId(const TChA& Key) { const TSizeTy KeyId = AddKey(Key.CStr());  return KeyDatV[KeyId].Dat = KeyId; }

  const TDat& operator[](const TSizeTy& KeyId) const {return GetHashKeyDat(KeyId).Dat;}
  TDat& operator[](const TSizeTy& KeyId){return GetHashKeyDat(KeyId).Dat;}
  const TDat& operator () (const char *Key) const { return GetDat(Key);}
  //TDat& operator ()(const char *Key){return AddDat(Key);} // add if not found
  ::TSize GetMemUsed() const {
      int64 MemUsed = sizeof(bool)+2*sizeof(int64);
      MemUsed += int64(PortV.Reserved()) * int64(sizeof(TInt64));
      for (int64 KeyDatN = 0; KeyDatN < KeyDatV.Len(); KeyDatN++) {
          MemUsed += int64(2 * sizeof(TInt64));
          MemUsed += int64(KeyDatV[KeyDatN].Key.GetMemUsed());
          MemUsed += int64(KeyDatV[KeyDatN].Dat.GetMemUsed());
      }
      // printf("TStrHash: Memory used for hash table: %s\n", TUInt64::GetStr(MemUsed).CStr());
      MemUsed += 8 + Pool->GetMemUsed();
      return ::TSize(MemUsed/1000);
  }

  const TDat& GetDat(const char *Key) const { return KeyDatV[GetKeyId(Key)].Dat; }
  const TDat& GetDat(const TStr& Key) const { return GetDat(Key.CStr()); }
  TDat& GetDat(const char *Key) { return KeyDatV[GetKeyId(Key)].Dat; }
  const TDat& GetDat(const TStr& Key) { return GetDat(Key.CStr()); }
  const TDat& GetDat(const TChA& Key) { return GetDat(Key.CStr()); }
  TDat& GetDatId(const TSizeTy& KeyId) { return KeyDatV[KeyId].Dat; }
  const TDat& GetDatId(const TSizeTy& KeyId) const { return KeyDatV[KeyId].Dat; }
  void GetKeyDat(const TSizeTy& KeyId, TSizeTy& KeyO, TDat& Dat) const { const THKeyDat& KeyDat = GetHashKeyDat(KeyId); KeyO = KeyDat.Key; Dat = KeyDat.Dat; }
  void GetKeyDat(const TSizeTy& KeyId, const char*& Key, TDat& Dat) const { const THKeyDat& KeyDat = GetHashKeyDat(KeyId); Key = KeyFromOfs(KeyDat.Key); Dat = KeyDat.Dat; }
  void GetKeyDat(const TSizeTy& KeyId, TStr& Key, TDat& Dat) const { const THKeyDat& KeyDat = GetHashKeyDat(KeyId); Key = KeyFromOfs(KeyDat.Key); Dat = KeyDat.Dat;}
  void GetKeyDat(const TSizeTy& KeyId, TChA& Key, TDat& Dat) const { const THKeyDat& KeyDat = GetHashKeyDat(KeyId); Key = KeyFromOfs(KeyDat.Key); Dat = KeyDat.Dat;}

  TSizeTy GetKeyId(const char *Key) const;
  TSizeTy GetKeyId(const TStr& Key) const { return GetKeyId(Key.CStr()); }
  const char *GetKey(const TSizeTy& KeyId) const { return Pool->GetCStr(GetHashKeyDat(KeyId).Key); }
  TSizeTy GetKeyOfs(const TSizeTy& KeyId) const { return GetHashKeyDat(KeyId).Key; } // pool string id
  const char *KeyFromOfs(const TSizeTy& KeyO) const { return Pool->GetCStr(KeyO); }

  bool IsKey(const char *Key) const { return GetKeyId(Key) != -1; }
  bool IsKey(const TStr& Key) const { return GetKeyId(Key.CStr()) != -1; }
  bool IsKey(const TChA& Key) const { return GetKeyId(Key.CStr()) != -1; }
  bool IsKey(const char *Key, TSizeTy& KeyId) const { KeyId = GetKeyId(Key); return KeyId != -1; }
  bool IsKeyGetDat(const char *Key, TDat& Dat) const { const TSizeTy KeyId = GetKeyId(Key); if (KeyId != -1) { Dat = KeyDatV[KeyId].Dat; return true; } else return false; }
  bool IsKeyGetDat(const TStr& Key, TDat& Dat) const { const TSizeTy KeyId = GetKeyId(Key.CStr()); if (KeyId != -1) { Dat = KeyDatV[KeyId].Dat; return true; } else return false; }
  bool IsKeyGetDat(const TChA& Key, TDat& Dat) const { const TSizeTy KeyId = GetKeyId(Key.CStr()); if (KeyId != -1) { Dat = KeyDatV[KeyId].Dat; return true; } else return false; }
  bool IsKeyId(const TSizeTy& KeyId) const { return 0 <= KeyId && KeyId < KeyDatV.Len() && KeyDatV[KeyId].HashCd != -1; }

  TSizeTy FFirstKeyId() const {return 0-1;}
  bool FNextKeyId(TSizeTy& KeyId) const;

  void GetKeyV(TVec<TStr, TSizeTy>& KeyV) const;
  void GetStrIdV(TInt64V& StrIdV) const;
  void GetDatV(TVec<TDat, TSizeTy>& DatV) const;
  void GetKeyDatPrV(TVec<TPair<TStr, TDat>, TSizeTy >& KeyDatPrV) const;
  void GetDatKeyPrV(TVec<TPair<TDat, TStr>, TSizeTy >& DatKeyPrV) const;

  void Pack(){KeyDatV.Pack();}
};

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
uint TStrHash<TDat, TStringPool, TSizeTy,  THashFunc>::GetNextPrime(const uint& Val) const {
  uint *f = (uint *) TIntH::HashPrimeT, *m, *l = (uint *) TIntH::HashPrimeT + (int) TIntH::HashPrimes;
  int h, len = (int)TIntH::HashPrimes;
  while (len > 0) {
    h = len >> 1;  m = f + h;
    if (*m < Val) { f = m;  f++;  len = len - h - 1; }
    else len = h;
  }
  return f == l ? *(l - 1) : *f;
}

template <class TDat, class TStringPool, class TSizeTy,  class THashFunc>
void TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::Resize() {
  // resize & initialize port vector
  if (PortV.Empty()) { PortV.Gen(17);  PortV.PutAll(-1); }
  else
  if (AutoSizeP && KeyDatV.Len() > 3 * PortV.Len()) {
    const TSizeTy NxPrime = GetNextPrime(KeyDatV.Len());
    //printf("%s resize PortV: %d -> %d, Len: %d\n", GetTypeNm(*this).CStr(), PortV.Len(), NxPrime, Len());
    PortV.Gen(NxPrime);  PortV.PutAll(-1); }
  else
    return;
  // rehash keys
  const int64 NPorts = PortV.Len();
  for (int64 i = 0; i < KeyDatV.Len(); i++) {
    THKeyDat& KeyDat = KeyDatV[i];
    if (KeyDat.HashCd != -1) {
      const int64 Port = abs(THashFunc::GetPrimHashCd(Pool->GetCStr(KeyDat.Key)) % NPorts);
      KeyDat.Next = PortV[Port];
      PortV[Port] = i;
    }
  }
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
TStrHash<TDat, TStringPool, TSizeTy, THashFunc>& TStrHash<TDat, TStringPool, TSizeTy, THashFunc>:: operator = (const TStrHash& Hash) {
  if (this != &Hash) {
    PortV = Hash.PortV;
    KeyDatV = Hash.KeyDatV;
    AutoSizeP = Hash.AutoSizeP;
    FFreeKeyId = Hash.FFreeKeyId;
    FreeKeys = Hash.FreeKeys;
    if (! Hash.Pool.Empty()) Pool = PStringPool(new TStringPool(*Hash.Pool));
    else Pool = NULL;
  }
  return *this;
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
TSizeTy TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::AddKey(const char *Key) {
  if (Pool.Empty()) Pool = TStringPool::New();
  if ((AutoSizeP && KeyDatV.Len() > PortV.Len()) || PortV.Empty()) Resize();
  const TSizeTy PortN = abs(THashFunc::GetPrimHashCd(Key) % PortV.Len());
  const TSizeTy HashCd = abs(THashFunc::GetSecHashCd(Key));
  TSizeTy PrevKeyId = -1;
  TSizeTy KeyId = PortV[PortN];
  while (KeyId != -1 && ! (KeyDatV[KeyId].HashCd == HashCd && Pool->Cmp(KeyDatV[KeyId].Key, Key) == 0)) {
    PrevKeyId = KeyId;  KeyId = KeyDatV[KeyId].Next; }
  if (KeyId == -1) {
    const TSizeTy StrId = Pool->AddStr(Key);
    if (FFreeKeyId == -1) {
      KeyId = KeyDatV.Add(THKeyDat(-1, HashCd, StrId));
    } else {
      KeyId = FFreeKeyId;
      FFreeKeyId = KeyDatV[FFreeKeyId].Next;
      FreeKeys--;
      KeyDatV[KeyId] = THKeyDat(-1, HashCd, StrId);
    }
    if (PrevKeyId == -1) PortV[PortN] = KeyId;
    else KeyDatV[PrevKeyId].Next = KeyId;
  }
  return KeyId;
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
TSizeTy TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::GetKeyId(const char *Key) const {
  if (PortV.Empty()) return -1;
  const TSizeTy PortN = abs(THashFunc::GetPrimHashCd(Key) % PortV.Len());
  const TSizeTy Hc = abs(THashFunc::GetSecHashCd(Key));
  TSizeTy KeyId = PortV[PortN];
  while (KeyId != -1 && ! (KeyDatV[KeyId].HashCd == Hc && Pool->Cmp(KeyDatV[KeyId].Key, Key) == 0))
    KeyId = KeyDatV[KeyId].Next;
  return KeyId;
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
bool TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::FNextKeyId(TSizeTy& KeyId) const {
  do KeyId++; while (KeyId < KeyDatV.Len() && KeyDatV[KeyId].HashCd == -1);
  return KeyId < KeyDatV.Len();
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
void TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::GetKeyV(TVec<TStr, TSizeTy>& KeyV) const {
  KeyV.Gen(Len(), 0);
  TSizeTy KeyId = FFirstKeyId();
  while (FNextKeyId(KeyId))
    KeyV.Add(GetKey(KeyId));
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
void TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::GetStrIdV(TInt64V& StrIdV) const {
  StrIdV.Gen(Len(), 0);
  int64 KeyId = FFirstKeyId();
  while (FNextKeyId(KeyId))
    StrIdV.Add(GetKeyOfs(KeyId));
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
void TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::GetDatV(TVec<TDat, TSizeTy>& DatV) const {
  DatV.Gen(Len(), 0);
  TSizeTy KeyId = FFirstKeyId();
  while (FNextKeyId(KeyId))
    DatV.Add(GetHashKeyDat(KeyId).Dat);
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
void TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::GetKeyDatPrV(TVec<TPair<TStr, TDat>, TSizeTy >& KeyDatPrV) const {
  KeyDatPrV.Gen(Len(), 0);
  TStr Str; TDat Dat;
  TSizeTy KeyId = FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Str, Dat);
    KeyDatPrV.Add(TPair<TStr, TDat>(Str, Dat));
  }
}

template <class TDat, class TStringPool, class TSizeTy, class THashFunc>
void TStrHash<TDat, TStringPool, TSizeTy, THashFunc>::GetDatKeyPrV(TVec<TPair<TDat, TStr>, TSizeTy >& DatKeyPrV) const {
  DatKeyPrV.Gen(Len(), 0);
  TStr Str; TDat Dat;
  TSizeTy KeyId = FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Str, Dat);
    DatKeyPrV.Add(TPair<TDat, TStr>(Dat, Str));
  }
}

/////////////////////////////////////////////////
// Common-String-Hash-Types
typedef TStrHash<TInt> TStrSH;
typedef TStrHash<TInt> TStrIntSH;
typedef TStrHash<TIntV> TStrToIntVSH;

/////////////////////////////////////////////////
// Cache
template <class TKey, class TDat, class TSizeTy = int, class THashFunc = TDefaultHashFunc<TKey> >
class TCache{
private:
  typedef TLst<TKey, TSizeTy> TKeyL; typedef TLstNd<TKey>* TKeyLN;
  typedef TPair<TKeyLN, TDat> TKeyLNDatPr;
  int64 MxMemUsed;
  int64 CurMemUsed;
  THash<TKey, TKeyLNDatPr, TSizeTy, THashFunc> KeyDatH;
  TKeyL TimeKeyL;
  void* RefToBs;
  void Purge(const int64& MemToPurge);
public:
  TCache(){}
  TCache(const TCache&);
  TCache(const int64& _MxMemUsed, const int64& Ports, void* _RefToBs):
    MxMemUsed(_MxMemUsed), CurMemUsed(0),
    KeyDatH(Ports), TimeKeyL(), RefToBs(_RefToBs){}

  TCache& operator=(const TCache&);
  int64 GetMemUsed() const;
  int64 GetMxMemUsed() const { return MxMemUsed; }
  bool RefreshMemUsed();

  void Put(const TKey& Key, const TDat& Dat);
  bool Get(const TKey& Key, TDat& Dat);
  void Del(const TKey& Key, const bool& DoEventCall=true);
  void Flush();
  void FlushAndClr();
  void* FFirstKeyDat();
  bool FNextKeyDat(void*& KeyDatP, TKey& Key, TDat& Dat);

  void PutRefToBs(void* _RefToBs){RefToBs=_RefToBs;}
  void* GetRefToBs(){return RefToBs;}
};

template <class TKey, class TDat, class TSizeTy, class THashFunc>
void TCache<TKey, TDat, TSizeTy, THashFunc>::Purge(const int64& MemToPurge){
  const int64 StartMemUsed = CurMemUsed;
  while (!TimeKeyL.Empty()&&(StartMemUsed-CurMemUsed<MemToPurge)){
    TKey Key=TimeKeyL.Last()->GetVal();
    Del(Key);
  }
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
int64 TCache<TKey, TDat, TSizeTy, THashFunc>::GetMemUsed() const {
  int64 MemUsed=0;
  TSizeTy KeyId=KeyDatH.FFirstKeyId();
  while (KeyDatH.FNextKeyId(KeyId)){
    const TKey& Key=KeyDatH.GetKey(KeyId);
    const TKeyLNDatPr& KeyLNDatPr=KeyDatH[KeyId];
    TDat Dat=KeyLNDatPr.Val2;
    MemUsed+=int64(Key.GetMemUsed()+Dat->GetMemUsed());
  }
  return MemUsed;
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
bool TCache<TKey, TDat, TSizeTy, THashFunc>::RefreshMemUsed(){
  CurMemUsed=GetMemUsed();
  if (CurMemUsed>MxMemUsed){
    Purge(CurMemUsed-MxMemUsed);
    return true;
  }
  return false;
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
void TCache<TKey, TDat, TSizeTy, THashFunc>::Put(const TKey& Key, const TDat& Dat){
  TSizeTy KeyId=KeyDatH.GetKeyId(Key);
  if (KeyId==-1){
    int64 KeyDatMem=int64(Key.GetMemUsed()+Dat->GetMemUsed());
    if (CurMemUsed+KeyDatMem>MxMemUsed){Purge(KeyDatMem);}
    CurMemUsed+=KeyDatMem;
    TKeyLN KeyLN=TimeKeyL.AddFront(Key);
    TKeyLNDatPr KeyLNDatPr(KeyLN, Dat);
    KeyDatH.AddDat(Key, KeyLNDatPr);
  } else {
    TKeyLNDatPr& KeyLNDatPr=KeyDatH[KeyId];
    TKeyLN KeyLN=KeyLNDatPr.Val1;
    KeyLNDatPr.Val2=Dat;
    TimeKeyL.PutFront(KeyLN);
  }
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
bool TCache<TKey, TDat, TSizeTy, THashFunc>::Get(const TKey& Key, TDat& Dat){
  TSizeTy KeyId=KeyDatH.GetKeyId(Key);
  if (KeyId==-1){
    return false;
  } else {
    Dat=KeyDatH[KeyId].Val2;
    return true;
  }
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
void TCache<TKey, TDat, TSizeTy, THashFunc>::Del(const TKey& Key, const bool& DoEventCall){
  TSizeTy KeyId=KeyDatH.GetKeyId(Key);
  if (KeyId!=-1){
    TKeyLNDatPr& KeyLNDatPr=KeyDatH[KeyId];
    TKeyLN KeyLN=KeyLNDatPr.Val1;
    TDat& Dat=KeyLNDatPr.Val2;
    if (DoEventCall){
      Dat->OnDelFromCache(Key, RefToBs);}
    CurMemUsed-=int64(Key.GetMemUsed()+Dat->GetMemUsed());
    Dat=NULL;
    TimeKeyL.Del(KeyLN);
    KeyDatH.DelKeyId(KeyId);
  }
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
void TCache<TKey, TDat, TSizeTy, THashFunc>::Flush(){
  printf("To flush: %d\n", KeyDatH.Len());
  TSizeTy KeyId=KeyDatH.FFirstKeyId(); TSizeTy Done = 0;
  while (KeyDatH.FNextKeyId(KeyId)){
    if (Done%10000==0){printf("%d\r", Done);}
    const TKey& Key=KeyDatH.GetKey(KeyId);
    TKeyLNDatPr& KeyLNDatPr=KeyDatH[KeyId];
    TDat Dat=KeyLNDatPr.Val2;
    Dat->OnDelFromCache(Key, RefToBs);
    Done++;
  }
  printf("Done %d\n", KeyDatH.Len());
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
void TCache<TKey, TDat, TSizeTy, THashFunc>::FlushAndClr(){
  Flush();
  CurMemUsed=0;
  KeyDatH.Clr();
  TimeKeyL.Clr();
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
void* TCache<TKey, TDat, TSizeTy, THashFunc>::FFirstKeyDat(){
  return TimeKeyL.First();
}

template <class TKey, class TDat, class TSizeTy, class THashFunc>
bool TCache<TKey, TDat, TSizeTy, THashFunc>::FNextKeyDat(void*& KeyDatP, TKey& Key, TDat& Dat){
  if (KeyDatP==NULL){
    return false;
  } else {
    Key=TKeyLN(KeyDatP)->GetVal(); Dat=KeyDatH.GetDat(Key).Val2;
    KeyDatP=TKeyLN(KeyDatP)->Next(); return true;
  }
}

/////////////////////////////////////////////////
// Old-Hash-Functions

// Old-String-Hash-Function
class TStrHashF_OldGLib {
public:
  inline static int GetPrimHashCd(const char *p) {
    const int MulBy = 16;  // even older version used MulBy=2
    int HashCd = 0;
    while (*p) { HashCd = (MulBy * HashCd) + *p++; HashCd &= 0x0FFFFFFF; }
    return HashCd; }
  inline static int GetSecHashCd(const char *p) {
    const int MulBy = 16;  // even older version used MulBy=2
    int HashCd = 0;
    while (*p) { HashCd = (MulBy * HashCd) ^ *p++; HashCd &= 0x0FFFFFFF; }
    return HashCd; }
  inline static int GetPrimHashCd(const TStr& s) { return GetPrimHashCd(s.CStr()); }
  inline static int GetSecHashCd(const TStr& s) { return GetSecHashCd(s.CStr()); }
};

// Md5-Hash-Function
class TStrHashF_Md5 {
public:
  static int GetPrimHashCd(const char *p);
  static int GetSecHashCd(const char *p);
  static int GetPrimHashCd(const TStr& s);
  static int GetSecHashCd(const TStr& s);
};

// DJB-Hash-Function
class TStrHashF_DJB {
private:
  inline static unsigned int DJBHash(const char* Str, const ::TSize& Len) {
    unsigned int hash = 5381;
    for(unsigned int i = 0; i < Len; Str++, i++) {
       hash = ((hash << 5) + hash) + (*Str); }
    return hash;
  }
public:
  inline static int GetPrimHashCd(const char *p) {
    const char *r = p;  while (*r) { r++; }
    return (int) DJBHash((const char *) p, r - p) & 0x7fffffff; }
  inline static int GetSecHashCd(const char *p) {
    const char *r = p;  while (*r) { r++; }
    return (int) DJBHash((const char *) p, r - p) & 0x7fffffff; }
  inline static int GetPrimHashCd(const TStr& s) { 
    return GetPrimHashCd(s.CStr()); }
  inline static int GetSecHashCd(const TStr& s) { 
    return GetSecHashCd(s.CStr()); }
  inline static int GetPrimHashCd(const char *p, const ::TSize& Len) {
    return (int) DJBHash((const char *) p, Len) & 0x7fffffff; }
  inline static int GetSecHashCd(const char *p, const ::TSize& Len) {
    return (int) DJBHash((const char *) p, Len) & 0x7fffffff; }
};

// Old-Vector-Hash-Function
template <class TVec>
class TVecHashF_OldGLib {
public:
  static inline int GetPrimHashCd(const TVec& Vec) {
    int HashCd=0;
    for (int ValN=0; ValN<Vec.Len(); ValN++){
      HashCd+=Vec[ValN].GetPrimHashCd();}
    return abs(HashCd);
  }
  inline static int GetSecHashCd(const TVec& Vec) {
    int HashCd=0;
    for (int ValN=0; ValN<Vec.Len(); ValN++){
      HashCd+=Vec[ValN].GetSecHashCd();}
    return abs(HashCd);
  }
};
