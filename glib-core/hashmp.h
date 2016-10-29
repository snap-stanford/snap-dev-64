#ifndef hashmp_h
#define hashmp_h

#ifdef GCC_ATOMIC

#include "bd.h"

/////////////////////////////////////////////////
// PHash-Table-Key-Data
#pragma pack(push, 1) // pack class size
template <class TKey, class TDat, class TSizeTy = int>
class THashMPKeyDat {
public:
  TInt64 HashCd;
  TKey Key;
  TDat Dat;
public:
  THashMPKeyDat():
    HashCd(-1), Key(), Dat(){}
  THashMPKeyDat(const TSizeTy& _HashCd, const TKey& _Key):
    HashCd(_HashCd), Key(_Key), Dat(){}
  explicit THashMPKeyDat(TSIn& SIn):
    HashCd(SIn), Key(SIn), Dat(SIn){}
  void Save(TSOut& SOut) const {
    HashCd.Save(SOut); Key.Save(SOut); Dat.Save(SOut);}

  bool operator==(const THashMPKeyDat& HashKeyDat) const {
    if (this==&HashKeyDat || (HashCd==HashKeyDat.HashCd
      && Key==HashKeyDat.Key && Dat==HashKeyDat.Dat)){return true;}
    return false;}
  THashMPKeyDat& operator=(const THashMPKeyDat& HashKeyDat){
    if (this!=&HashKeyDat){
      HashCd=HashKeyDat.HashCd; Key=HashKeyDat.Key;
      Dat=HashKeyDat.Dat;}
    return *this;}
};
#pragma pack(pop)

/////////////////////////////////////////////////
// PHash-Table-Key-Data-Iterator
template<class TKey, class TDat, class TSizeTy = int>
class THashMPKeyDatI{
public:
  typedef THashMPKeyDat<TKey, TDat> TPHKeyDat;
private:
  TPHKeyDat* KeyDatI;
  TPHKeyDat* EndI;
public:
  THashMPKeyDatI(): KeyDatI(NULL), EndI(NULL){}
  THashMPKeyDatI(const THashMPKeyDatI& _HashKeyDatI):
    KeyDatI(_HashKeyDatI.KeyDatI), EndI(_HashKeyDatI.EndI){}
  THashMPKeyDatI(const TPHKeyDat* _KeyDatI, const TPHKeyDat* _EndI):
    KeyDatI((TPHKeyDat*)_KeyDatI), EndI((TPHKeyDat*)_EndI){}

  THashMPKeyDatI& operator=(const THashMPKeyDatI& HashKeyDatI){
    KeyDatI=HashKeyDatI.KeyDatI; EndI=HashKeyDatI.EndI; return *this;}
  bool operator==(const THashMPKeyDatI& HashKeyDatI) const {
    return KeyDatI==HashKeyDatI.KeyDatI;}
  bool operator<(const THashMPKeyDatI& HashKeyDatI) const {
    return KeyDatI<HashKeyDatI.KeyDatI;}
  THashMPKeyDatI& operator++(int){ KeyDatI++; while (KeyDatI < EndI && KeyDatI->HashCd==-1) { KeyDatI++; } return *this; }
  THashMPKeyDatI& operator--(int){ do { KeyDatI--; } while (KeyDatI->HashCd==-1); return *this;}
  TPHKeyDat& operator*() const { return *KeyDatI; }
  TPHKeyDat& operator()() const { return *KeyDatI; }
  TPHKeyDat* operator->() const { return KeyDatI; }
  THashMPKeyDatI& Next(){ operator++(1); return *this; }

  /// Tests whether the iterator has been initialized.
  bool IsEmpty() const { return KeyDatI == NULL; }
  /// Tests whether the iterator is pointing to the past-end element.
  bool IsEnd() const { return EndI == KeyDatI; }
  
  const TKey& GetKey() const {Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1)); return KeyDatI->Key;}
  const TDat& GetDat() const {/*Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1));*/ return KeyDatI->Dat;}
  TDat& GetDat() {Assert((KeyDatI!=NULL)&&(KeyDatI->HashCd!=-1)); return KeyDatI->Dat;}
};

//#//////////////////////////////////////////////
/// Hash-Table with multiprocessing support.
template<class TKey, class TDat, class TSizeTy = int, class THashFunc = TDefaultHashFunc<TKey, TSizeTy> >
class THashMP {
public:
  enum {HashPrimes=32};
  static const unsigned int HashPrimeT[HashPrimes];
public:
  typedef THashMPKeyDatI<TKey, TDat, TSizeTy> TIter;
private:
  typedef THashMPKeyDat<TKey, TDat, TSizeTy> TPHKeyDat;
  typedef TPair<TKey, TDat> TKeyDatP;
  TVec<TPHKeyDat, int64> Table;
  TInt64 NumVals;
private:
  class THashMPKeyDatCmp {
  public:
    const THashMP<TKey, TDat, TSizeTy, THashFunc>& Hash;
    bool CmpKey, Asc;
    THashMPKeyDatCmp(THashMP<TKey, TDat, TSizeTy, THashFunc>& _Hash, const bool& _CmpKey, const bool& _Asc) :
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
  TPHKeyDat& GetPHashKeyDat(const TSizeTy& KeyId){
    TPHKeyDat& KeyDat=Table[KeyId];
    Assert(KeyDat.HashCd!=-1); return KeyDat;}
  const TPHKeyDat& GetPHashKeyDat(const TSizeTy& KeyId) const {
    const TPHKeyDat& KeyDat=Table[KeyId];
    Assert(KeyDat.HashCd!=-1); return KeyDat;}
  uint GetNextPrime(const uint& Val) const;
//  void Resize();
public:
  THashMP():
    Table(), NumVals(0){}
  THashMP(const THashMP& PHash):
    Table(PHash.Table), NumVals(PHash.NumVals){}
  explicit THashMP(const TSizeTy& ExpectVals) {
    Gen(ExpectVals);}
  explicit THashMP(TSIn& SIn):
    Table(SIn), NumVals(SIn){
    SIn.LoadCs();}
  void Load(TSIn& SIn){
    Table.Load(SIn); NumVals.Load(SIn);
    SIn.LoadCs();}
  void Save(TSOut& SOut) const {
    Table.Save(SOut); NumVals.Save(SOut);
    SOut.SaveCs();}

  THashMP& operator=(const THashMP& Hash){
    if (this!=&Hash){
      Table=Hash.Table; NumVals=Hash.NumVals;}
    return *this;}
  bool operator==(const THashMP& Hash) const; //J: zdaj tak kot je treba
//bool operator < (const THash& Hash) const { Fail; return true; }
  const TDat& operator[](const TSizeTy& KeyId) const {return GetPHashKeyDat(KeyId).Dat;}
  TDat& operator[](const TSizeTy& KeyId){return GetPHashKeyDat(KeyId).Dat;}
  TDat& operator()(const TKey& Key){return AddDat(Key);}
  ::TSize GetMemUsed() const {
    // return PortV.GetMemUsed()+KeyDatV.GetMemUsed()+sizeof(bool)+2*sizeof(int);}
      int64 MemUsed = sizeof(int64);
      for (int TableN = 0; TableN < Table.Len(); TableN++) {
          MemUsed += int64(sizeof(TInt64));
          MemUsed += int64(Table[TableN].Key.GetMemUsed());
          MemUsed += int64(Table[TableN].Dat.GetMemUsed());
      }
      return ::TSize(MemUsed);
  }

  TIter BegI() const {
    if (Len() == 0){return TIter(Table.EndI(), Table.EndI());}
    return TIter(Table.BegI(), Table.EndI());}
  TIter EndI() const {return TIter(Table.EndI(), Table.EndI());}
  //TIter GetI(const int& KeyId) const {return TIter(&KeyDatV[KeyId], KeyDatV.EndI());}
  TIter GetI(const TKey& Key) const {return TIter(&Table[GetKeyId(Key)], Table.EndI());}

  void Gen(const TSizeTy& ExpectVals){
    Table.Gen(GetNextPrime(2 * ExpectVals));}

  void Clr(const bool& DoDel=true);
  bool Empty() const {return Len()==0;}
  TSizeTy Len() const {return NumVals;}
  void SetLen(const TSizeTy& Length) {NumVals=Length;}
  TSizeTy GetMxKeyIds() const {return Table.Len();}
//bool IsKeyIdEqKeyN() const {return FreeKeys==0;}
  TSizeTy GetReservedKeyIds() const {return Table.Reserved();}

  // TODO: Non-unique keys
  TSizeTy AddKey(const TKey& Key);
  TSizeTy AddKey11(const TSizeTy& Idx, const TKey& Key, bool& Found);
  TSizeTy AddKey12(const TSizeTy& Idx, const TKey& Key, bool& Found);
  TSizeTy AddKey13(const TSizeTy& Idx, const TKey& Key);
  TSizeTy AddKey1(const TKey& Key, bool& Found);
  TSizeTy AddKey2(const TSizeTy& Idx, const TKey& Key, bool& Found);
  TDat& AddDatId(const TKey& Key){
    TSizeTy KeyId=AddKey(Key); return Table[KeyId].Dat=KeyId;}
  // TODO: Make Dat updatable
  TDat& AddDat(const TKey& Key){return Table[AddKey(Key)].Dat;}
  TDat& AddDat(const TKey& Key, const TDat& Dat){
    return Table[AddKey(Key)].Dat=Dat;}

  const TKey& GetKey(const TSizeTy& KeyId) const { return GetPHashKeyDat(KeyId).Key;}
  TSizeTy GetKeyId(const TKey& Key) const;
  /// Get an index of a random element. If the hash table has many deleted keys, this may take a long time.
  TSizeTy GetRndKeyId(TRnd& Rnd) const;
  /// Get an index of a random element. If the hash table has many deleted keys, defrag the hash table first (that's why the function is non-const).
  TSizeTy GetRndKeyId(TRnd& Rnd, const double& EmptyFrac);
  bool IsKey(const TKey& Key) const {return GetKeyId(Key)!=-1;}
  bool IsKey(const TKey& Key, TSizeTy& KeyId) const { KeyId=GetKeyId(Key); return KeyId!=-1;}
  bool IsKeyId(const TSizeTy& KeyId) const {
    return (0<=KeyId)&&(KeyId<Table.Len())&&(Table[KeyId].HashCd!=-1);}
  const TDat& GetDat(const TKey& Key) const {return Table[GetKeyId(Key)].Dat;}
  TDat& GetDat(const TKey& Key){return Table[GetKeyId(Key)].Dat;}
//  TKeyDatP GetKeyDat(const int& KeyId) const {
//    TPHKeyDat& KeyDat=GetPHashKeyDat(KeyId);
//    return TKeyDatP(KeyDat.Key, KeyDat.Dat);}
  void GetKeyDat(const TSizeTy& KeyId, TKey& Key, TDat& Dat) const {
    const TPHKeyDat& KeyDat=GetPHashKeyDat(KeyId);
    Key=KeyDat.Key; Dat=KeyDat.Dat;}
  bool IsKeyGetDat(const TKey& Key, TDat& Dat) const {TSizeTy KeyId;
    if (IsKey(Key, KeyId)){Dat=GetPHashKeyDat(KeyId).Dat; return true;}
    else {return false;}}

  TSizeTy FFirstKeyId() const {return 0-1;}
  bool FNextKeyId(TSizeTy& KeyId) const;
  void GetKeyV(TVec<TKey, TSizeTy>& KeyV) const;
  void GetDatV(TVec<TDat, TSizeTy>& DatV) const;
  void GetKeyDatPrV(TVec<TPair<TKey, TDat>, TSizeTy>& KeyDatPrV) const;
  void GetDatKeyPrV(TVec<TPair<TDat, TKey>, TSizeTy>& DatKeyPrV) const;
  void GetKeyDatKdV(TVec<TKeyDat<TKey, TDat>, TSizeTy>& KeyDatKdV) const;
  void GetDatKeyKdV(TVec<TKeyDat<TDat, TKey>, TSizeTy>& DatKeyKdV) const;

  void Swap(THashMP& Hash);
//void Defrag();
  void Pack(){Table.Pack();}
/*void Sort(const bool& CmpKey, const bool& Asc);
  void SortByKey(const bool& Asc=true) { Sort(true, Asc); }
  void SortByDat(const bool& Asc=true) { Sort(false, Asc); }*/
};
//TODO64
template<class TKey, class TDat, class TSizeTy, class THashFunc>
const unsigned int THashMP<TKey, TDat, TSizeTy, THashFunc>::HashPrimeT[HashPrimes]={
  3ul, 5ul, 11ul, 23ul,
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
  1610612741ul, 3221225473ul, 4294967291ul
};

template<class TKey, class TDat, class TSizeTy, class THashFunc>
uint THashMP<TKey, TDat, TSizeTy, THashFunc>::GetNextPrime(const uint& Val) const {
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
bool THashMP<TKey, TDat, TSizeTy, THashFunc>::operator==(const THashMP& Hash) const {
  if (Len() != Hash.Len()) { return false; }
  for (TSizeTy i = FFirstKeyId(); FNextKeyId(i); ) {
    const TKey& Key = GetKey(i);
    if (! Hash.IsKey(Key)) { return false; }
    if (GetDat(Key) != Hash.GetDat(Key)) { return false; }
  }
  return true;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::AddKey(const TKey& Key) {
  //int CurVals = __sync_fetch_and_add(&NumVals.Val, 1);
  //IAssertR(CurVals < Table.Len(), "Table must not be full");

  const TSizeTy BegTableN=abs(Key.GetPrimHashCd()%Table.Len());
  const TSizeTy HashCd=abs(Key.GetSecHashCd());

  TSizeTy TableN = BegTableN;
  while (Table[TableN].HashCd != -1 || 
    (!__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, HashCd))) {
    TableN = (TableN + 1) % Table.Len();    
  }
  Table[TableN].Key = Key;
  __sync_fetch_and_add(&NumVals.Val, 1);
  return TableN;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::AddKey11(const TSizeTy& BegTableN, const TKey& Key, bool& Found) {
  //int CurVals = __sync_fetch_and_add(&NumVals.Val, 1);
  //IAssertR(CurVals < Table.Len(), "Table must not be full");

  const TSizeTy Length = Table.Len();
  const TSizeTy HashCd=abs(Key.GetSecHashCd());

  TSizeTy TableN = BegTableN;
  Found = false;
  do {
    if (Table[TableN].HashCd.Val != -1) {
      if (Table[TableN].Key == Key) {
        Found = true;
        return TableN;
      }
    } else if (__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, HashCd)) {
      break;
    }
    
    TableN++;
    if (TableN >= Length) {
      TableN = 0;
    }
  } while (1);

  Table[TableN].Key = Key;
  return TableN;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::AddKey12(const TSizeTy& BegTableN, const TKey& Key, bool& Found) {
  //int CurVals = __sync_fetch_and_add(&NumVals.Val, 1);
  //IAssertR(CurVals < Table.Len(), "Table must not be full");

  const TSizeTy Length = Table.Len();
  //const int HashCd=abs(Key.GetSecHashCd());

  // HashCd values:
  //  -1: empty slot
  //   1: occupied slot, invalid key
  //   2: occupied slot, valid key

  TSizeTy TableN = BegTableN;
  do {
    TSizeTy HashCd = Table[TableN].HashCd.Val;
    if (HashCd == -1) {
      // an empty slot
      if (__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, 1)) {
        // an empty slot has been claimed, key is invalid
        break;
      }
    } else {
      // slot is occupied
      if (HashCd != 2) {
        // key is not yet valid, wait for a valid key
        while (!__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, 2, 2)) {
          usleep(20);
        }
      }
      if (Table[TableN].Key == Key) {
        // the key matches
        Found = true;
        return TableN;
      }
      // move to the next slot
      TableN++;
      if (TableN >= Length) {
        TableN = 0;
      }
    }
  } while (1);

  // write the key, indicate a valid key
  Table[TableN].Key = Key;
  Table[TableN].HashCd.Val = 2;

  Found = false;
  return TableN;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::AddKey13(const TSizeTy& BegTableN, const TKey& Key) {
  //int CurVals = __sync_fetch_and_add(&NumVals.Val, 1);
  //IAssertR(CurVals < Table.Len(), "Table must not be full");

  const TSizeTy Length = Table.Len();
  //const int HashCd=abs(Key.GetSecHashCd());

  // HashCd values:
  //  -1: empty slot
  //   1: occupied slot, invalid key
  //   2: occupied slot, valid key

  TSizeTy TableN = BegTableN;
  do {
    TSizeTy HashCd = Table[TableN].HashCd.Val;
    if (HashCd == -1) {
      // an empty slot
      if (__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, 1)) {
        // an empty slot has been claimed, key is invalid
        break;
      }
    } else {
      // slot is occupied, move to the next slot
      TableN++;
      if (TableN >= Length) {
        TableN = 0;
      }
    }
  } while (1);

  // write the key, indicate a valid key
  Table[TableN].Key = Key;
  Table[TableN].HashCd.Val = 2;

  return TableN;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::AddKey1(const TKey& Key, bool& Found) {
  const TSizeTy Length = Table.Len();
  //const int Length = 12582917;
  const TSizeTy BegTableN = abs(Key.GetPrimHashCd() % Length);
  const TSizeTy HashCd = abs(Key.GetSecHashCd());

  //printf("AddKey1 %5d %5d\n", Length, BegTableN);

  TSizeTy TableN = BegTableN;
  while (Table[TableN].HashCd.Val != -1) {
    if (Table[TableN].Key == Key) {
      Found = true;
      return TableN;
    }
    TableN++;
    if (TableN >= Length) {
      TableN = 0;
    }
    //(!__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, HashCd))) {
    //TableN = (TableN + 1) % Table.Len();    
  }

  NumVals.Val++;
  Found = false;
  Table[TableN].HashCd.Val = HashCd;
  Table[TableN].Key = Key;
  return TableN;

  // TODO:RS, need to set the length at the end
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::AddKey2(const TSizeTy& BegTableN, const TKey& Key, bool& Found) {
  //const int Length = 12582917;
  const TSizeTy Length = Table.Len();
  const TSizeTy HashCd = abs(Key.GetSecHashCd());

  //printf("AddKey2 %5d %5d\n", Length, BegTableN);

  TSizeTy TableN = BegTableN;
  while (Table[TableN].HashCd.Val != -1) {
    if (Table[TableN].Key == Key) {
      Found = true;
      return TableN;
    }
    TableN++;
    if (TableN >= Length) {
      TableN = 0;
    }
    //(!__sync_bool_compare_and_swap(&Table[TableN].HashCd.Val, -1, HashCd))) {
    //TableN = (TableN + 1) % Table.Len();    
  }

  //NumVals.Val++;
  Found = false;
  Table[TableN].HashCd.Val = HashCd;
  Table[TableN].Key = Key;
  return TableN;

  // TODO:RS, need to set the length at the end
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::GetKeyId(const TKey& Key) const {
  const TSizeTy BegTableN=abs(Key.GetPrimHashCd()%Table.Len());
  //const int HashCd=abs(Key.GetSecHashCd());

  TSizeTy TableN = BegTableN;
  while (Table[TableN].HashCd != -1) {
    if (Table[TableN].Key == Key) { return TableN; }
    TableN = (TableN + 1) % Table.Len();
    if (TableN == BegTableN) { return -1; }
  }

  return -1;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THashMP<TKey, TDat, TSizeTy, THashFunc>::Clr(const bool& DoDel){
  if (DoDel){
    Table.Clr();
  } else {
    Table.PutAll(TPHKeyDat());
  }
  NumVals = TInt(0);
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
bool THashMP<TKey, TDat, TSizeTy, THashFunc>::FNextKeyId(TSizeTy& KeyId) const {
  do {KeyId++;} while ((KeyId<Table.Len())&&(Table[KeyId].HashCd==-1));
  return KeyId<Table.Len();
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THashMP<TKey, TDat, TSizeTy, THashFunc>::GetKeyV(TVec<TKey, TSizeTy>& KeyV) const {
  KeyV.Gen(Len(), 0);
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    KeyV.Add(GetKey(KeyId));}
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THashMP<TKey, TDat, TSizeTy, THashFunc>::GetDatV(TVec<TDat, TSizeTy>& DatV) const {
  DatV.Gen(Len(), 0);
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    DatV.Add(GetPHashKeyDat(KeyId).Dat);}
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THashMP<TKey, TDat, TSizeTy, THashFunc>::GetKeyDatPrV(TVec<TPair<TKey, TDat>, TSizeTy>& KeyDatPrV) const {
  KeyDatPrV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    KeyDatPrV.Add(TPair<TKey, TDat>(Key, Dat));
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THashMP<TKey, TDat, TSizeTy, THashFunc>::GetDatKeyPrV(TVec<TPair<TDat, TKey>, TSizeTy>& DatKeyPrV) const {
  DatKeyPrV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    DatKeyPrV.Add(TPair<TDat, TKey>(Dat, Key));
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THashMP<TKey, TDat, TSizeTy, THashFunc>::GetKeyDatKdV(TVec<TKeyDat<TKey, TDat>, TSizeTy>& KeyDatKdV) const {
  KeyDatKdV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    KeyDatKdV.Add(TKeyDat<TKey, TDat>(Key, Dat));
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THashMP<TKey, TDat, TSizeTy, THashFunc>::GetDatKeyKdV(TVec<TKeyDat<TDat, TKey>, TSizeTy>& DatKeyKdV) const {
  DatKeyKdV.Gen(Len(), 0);
  TKey Key; TDat Dat;
  TSizeTy KeyId=FFirstKeyId();
  while (FNextKeyId(KeyId)){
    GetKeyDat(KeyId, Key, Dat);
    DatKeyKdV.Add(TKeyDat<TDat, TKey>(Dat, Key));
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
void THashMP<TKey, TDat, TSizeTy, THashFunc>::Swap(THashMP& Hash) {
  if (this!=&Hash){
    Table.Swap(Hash.Table);
    ::Swap(NumVals, Hash.NumVals);
  }
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::GetRndKeyId(TRnd& Rnd) const  {
  printf("*** ERROR *** THashMP<TKey, TDat, THashFunc>::GetRndKeyId called\n");
  return 0;
}

template<class TKey, class TDat, class TSizeTy, class THashFunc>
TSizeTy THashMP<TKey, TDat, TSizeTy, THashFunc>::GetRndKeyId(TRnd& Rnd, const double& EmptyFrac) {
  printf("*** ERROR *** THashMP<TKey, TDat, THashFunc>::GetRndKeyId called\n");
  return 0;
}

#endif // GCC_ATOMIC

#endif // hashmp_h
