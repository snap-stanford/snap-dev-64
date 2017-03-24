#ifndef NUMPY_H
#define NUMPY_H

namespace TSnap {
	/// Converts TIntV to Numpy array.
	void TIntVToNumpy(TIntV& IntV, int* IntNumpyVecOut, int n);
	/// Converts TFltV to Numpy array.
	void TFltVToNumpy(TFltV& FltV, float* FltNumpyVecOut, int n);
	/// Converts NumpyArray to TIntV
	void NumpyToTIntV(TIntV& IntV, int* IntNumpyVecIn, int n);
	/// Converts NumpyArray to TFltV
	void NumpyToTFltV(TFltV& FltV, float* FltNumpyVecIn, int n);

	/// Converts TIntV to Numpy array.
	void TInt64VToNumpy(TInt64V& IntV, int64* IntNumpyVecOut, int64 n);
	/// Converts TFltV to Numpy array.
	void TFlt64VToNumpy(TFlt64V& FltV, float* FltNumpyVecOut, int64 n);
	/// Converts NumpyArray to TIntV
	void NumpyToTInt64V(TInt64V& IntV, int64* IntNumpyVecIn, int64 n);
	/// Converts NumpyArray to TFltV
	void NumpyToTFlt64V(TFlt64V& FltV, float* FltNumpyVecIn, int64 n);
}

#endif //NUMPY_H
