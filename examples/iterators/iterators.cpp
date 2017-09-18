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

    //

    return 0;
}
