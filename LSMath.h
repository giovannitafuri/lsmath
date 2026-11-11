#ifndef LIGHT_SIMPLE_MATH_H
#define LIGHT_SIMPLE_MATH_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

enum {
	LSM_X,
	LSM_Y,
	LSM_Z,
	LSM_W
};

enum {
	LSM_SUCCESS,
	LSM_NUM_OF_COMPONENTS_ERROR,
	LSM_POINTER_ERROR
};

/*
 *	LSMVECTOR is used as a generic 4 float vector to do calculations
 *	so user don't need to convert between vector of different sizes
 *
 *	There are specific LSMFLOAT(N) storage version with N can be 2, 3, 4
 */


typedef struct LSMFLOAT4 { float data[4]; } LSMFLOAT4;

typedef struct LSMFLOAT3 { float data[3]; } LSMFLOAT3;

typedef struct LSMFLOAT2 { float data[2]; } LSMFLOAT2;

typedef union LSMVECTOR {
	float data[4];
	struct { float x, y, z, w; };
} LSMVECTOR;

/*
 *	If an erro occurred in loading the LSMVECTOR, numOfComponents will be set to -1
 */
static inline uint8_t LSMLoadVector(void* src, uint8_t numOfComponents, LSMVECTOR* dst) {
	if (!src || !dst) return LSM_POINTER_ERROR;

	switch (numOfComponents)
	{
		case 2:
			memcpy(dst->data, src, 2 * sizeof(float));
			dst->data[LSM_Z] = 0.0f;
			dst->data[LSM_W] = 0.0f;
			break;
		case 3:
			memcpy(dst->data, src, 3 * sizeof(float));
			dst->data[LSM_W] = 0.0f;
			break;

		case 4:
			memcpy(dst->data, src, 4 * sizeof(float));
			break;

		default:
			return LSM_NUM_OF_COMPONENTS_ERROR;
	}

	return LSM_SUCCESS;
}

static inline LSMFLOAT4 LSMStoreFloat4(LSMVECTOR v) {
	LSMFLOAT4 storage;
	storage.data[LSM_X] = v.data[LSM_X];
	storage.data[LSM_Y] = v.data[LSM_Y];
	storage.data[LSM_Z] = v.data[LSM_Z];
	storage.data[LSM_W] = v.data[LSM_W];

	return storage;
}

static inline LSMFLOAT3 LSMStoreFloat3(LSMVECTOR v) {
	LSMFLOAT3 storage;
	storage.data[LSM_X] = v.data[LSM_X];
	storage.data[LSM_Y] = v.data[LSM_Y];
	storage.data[LSM_Z] = v.data[LSM_Z];

	return storage;
}

static inline LSMFLOAT2 LSMStoreFloat2(LSMVECTOR v) {
	LSMFLOAT2 storage;
	storage.data[LSM_X] = v.data[LSM_X];
	storage.data[LSM_Y] = v.data[LSM_Y];

	return storage;
}

/*
 *	Zero vector
 *	Return a vector with X, Y, Z, W set to 0.0f
 */
static inline LSMVECTOR LSMVectorZero() {
	LSMVECTOR zeroVector = { 0.0f };

	return zeroVector;
}

/*
 *	Vector scaling
 *	It takes the LSMVECTOR vector and float scalar, it return a new vector
 */
static inline LSMVECTOR LSMVectorScale(LSMVECTOR v, float s) {
	v.data[LSM_X] *= s;
	v.data[LSM_Y] *= s;
	v.data[LSM_Z] *= s;
	v.data[LSM_W] *= s;

	return v;
}

/*
 *	Component-wise multiplication
 *	It takes the LSMVECTOR vector1 and LSMVECTOR vector2, it return a new vector
 */
static inline LSMVECTOR LSMVectorMul(LSMVECTOR v1, LSMVECTOR v2) {
	v1.data[LSM_X] *= v2.data[LSM_X];
	v1.data[LSM_Y] *= v2.data[LSM_Y];
	v1.data[LSM_Z] *= v2.data[LSM_Z];
	v1.data[LSM_W] *= v2.data[LSM_W];

	return v1;
}

/*
 *	Dot Product
 *	It takes the LSMVECTOR vector1 and LSMVECTOR vector2, it return a float(scalar)
 */
static inline float LSMVectorDot(LSMVECTOR v1, LSMVECTOR v2) {
	return	(v1.data[LSM_X] * v2.data[LSM_X]) +
	(v1.data[LSM_Y] * v2.data[LSM_Y]) +
	(v1.data[LSM_Z] * v2.data[LSM_Z]) +
	(v1.data[LSM_W] * v2.data[LSM_W]);
}

/*
 *	Cross Product
 *	It takes the LSMVECTOR vector1 and LSMVECTOR vector2, it return a new vector
 */
static inline LSMVECTOR LSMVectorCross(LSMVECTOR v1, LSMVECTOR v2) {
	LSMVECTOR cross_vector;
	cross_vector.data[LSM_X] =
	(v1.data[LSM_Y] * v2.data[LSM_Z]) - (v1.data[LSM_Z] * v2.data[LSM_Y]);
	cross_vector.data[LSM_Y] =
	(v1.data[LSM_Z] * v2.data[LSM_X]) - (v1.data[LSM_X] * v2.data[LSM_Z]);
	cross_vector.data[LSM_Z] =
	(v1.data[LSM_X] * v2.data[LSM_Y]) - (v1.data[LSM_Y] * v2.data[LSM_X]);
	cross_vector.data[LSM_W] = 0.0f;

	return cross_vector;
}

/*
 *	Vector Addition
 *	It takes the LSMVECTOR vector1 and LSMVECTOR vector2, it return a new vector
 */
static inline LSMVECTOR LSMVectorAdd(LSMVECTOR v1, LSMVECTOR v2) {
	LSMVECTOR new_vector;
	new_vector.data[LSM_X] =
	(v1.data[LSM_X] + v2.data[LSM_X]);
	new_vector.data[LSM_Y] =
	(v1.data[LSM_Y] + v2.data[LSM_Y]);
	new_vector.data[LSM_Z] =
	(v1.data[LSM_Z] + v2.data[LSM_Z]);
	new_vector.data[LSM_W] =
	(v1.data[LSM_W] + v2.data[LSM_W]);

	return new_vector;
}

/*
 * Vector Subtraction
 * It takes the LSMVECTOR vector1 and LSMVECTOR vector2, it return a new vector
 */
static inline LSMVECTOR LSMVectorSub(LSMVECTOR v1, LSMVECTOR v2) {
	LSMVECTOR new_vector;
	new_vector.data[LSM_X] =
	(v1.data[LSM_X] - v2.data[LSM_X]);
	new_vector.data[LSM_Y] =
	(v1.data[LSM_Y] - v2.data[LSM_Y]);
	new_vector.data[LSM_Z] =
	(v1.data[LSM_Z] - v2.data[LSM_Z]);
	new_vector.data[LSM_W] =
	(v1.data[LSM_W] - v2.data[LSM_W]);

	return new_vector;
}

/*
 * Construct a vector directly from 4 components
 */
static inline LSMVECTOR LSMVectorSet(float x, float y, float z, float w) {
	LSMVECTOR v;
	v.data[LSM_X] = x;
	v.data[LSM_Y] = y;
	v.data[LSM_Z] = z;
	v.data[LSM_W] = w;

	return v;
}

/*
 * Invert the sign of all vector components
 */
static inline LSMVECTOR LSMVectorNegate(LSMVECTOR v) {
	v.data[LSM_X] = -v.data[LSM_X];
	v.data[LSM_Y] = -v.data[LSM_Y];
	v.data[LSM_Z] = -v.data[LSM_Z];
	v.data[LSM_W] = -v.data[LSM_W];

	return v;
}

/*
 * Component-wise division
 */
static inline LSMVECTOR LSMVectorDiv(LSMVECTOR v1, LSMVECTOR v2) {
	v1.data[LSM_X] /= v2.data[LSM_X];
	v1.data[LSM_Y] /= v2.data[LSM_Y];
	v1.data[LSM_Z] /= v2.data[LSM_Z];
	v1.data[LSM_W] /= v2.data[LSM_W];

	return v1;
}

/*
 * Divide vector components by a scalar
 */
static inline LSMVECTOR LSMVectorDivScalar(LSMVECTOR v, float s) {
	float inv = 1.0f / s;
	v.data[LSM_X] *= inv;
	v.data[LSM_Y] *= inv;
	v.data[LSM_Z] *= inv;
	v.data[LSM_W] *= inv;

	return v;
}

/*
 * 3D dot product (ignores W component)
 */
static inline float LSMVector3Dot(LSMVECTOR v1, LSMVECTOR v2) {
	return (v1.data[LSM_X] * v2.data[LSM_X]) +
	       (v1.data[LSM_Y] * v2.data[LSM_Y]) +
	       (v1.data[LSM_Z] * v2.data[LSM_Z]);
}

/*
 * Squared length of 3D components to avoid square root when comparing magnitudes
 */
static inline float LSMVectorLengthSq(LSMVECTOR v) {
	return LSMVector3Dot(v, v);
}

/*
 * Length of 3D components
 */
static inline float LSMVectorLength(LSMVECTOR v) {
	return sqrtf(LSMVectorLengthSq(v));
}

/*
 * Normalizes 3D components and sets W to 0.0f
 */
static inline LSMVECTOR LSMVectorNormalize(LSMVECTOR v) {
	float lenSq = LSMVectorLengthSq(v);
	if (lenSq > 0.0f) {
		float invLen = 1.0f / sqrtf(lenSq);
		LSMVECTOR r;
		r.data[LSM_X] = v.data[LSM_X] * invLen;
		r.data[LSM_Y] = v.data[LSM_Y] * invLen;
		r.data[LSM_Z] = v.data[LSM_Z] * invLen;
		r.data[LSM_W] = 0.0f;
		return r;
	}

	return LSMVectorZero();
}

/*
 * Squared distance between two 3D positions
 */
static inline float LSMVectorDistanceSq(LSMVECTOR v1, LSMVECTOR v2) {
	return LSMVectorLengthSq(LSMVectorSub(v1, v2));
}

/*
 * Distance between two 3D positions
 */
static inline float LSMVectorDistance(LSMVECTOR v1, LSMVECTOR v2) {
	return sqrtf(LSMVectorDistanceSq(v1, v2));
}

/*
 * Select minimum values component-wise
 */
static inline LSMVECTOR LSMVectorMin(LSMVECTOR v1, LSMVECTOR v2) {
	LSMVECTOR r;
	r.data[LSM_X] = (v1.data[LSM_X] < v2.data[LSM_X]) ? v1.data[LSM_X] : v2.data[LSM_X];
	r.data[LSM_Y] = (v1.data[LSM_Y] < v2.data[LSM_Y]) ? v1.data[LSM_Y] : v2.data[LSM_Y];
	r.data[LSM_Z] = (v1.data[LSM_Z] < v2.data[LSM_Z]) ? v1.data[LSM_Z] : v2.data[LSM_Z];
	r.data[LSM_W] = (v1.data[LSM_W] < v2.data[LSM_W]) ? v1.data[LSM_W] : v2.data[LSM_W];

	return r;
}

/*
 * Select maximum values component-wise
 */
static inline LSMVECTOR LSMVectorMax(LSMVECTOR v1, LSMVECTOR v2) {
	LSMVECTOR r;
	r.data[LSM_X] = (v1.data[LSM_X] > v2.data[LSM_X]) ? v1.data[LSM_X] : v2.data[LSM_X];
	r.data[LSM_Y] = (v1.data[LSM_Y] > v2.data[LSM_Y]) ? v1.data[LSM_Y] : v2.data[LSM_Y];
	r.data[LSM_Z] = (v1.data[LSM_Z] > v2.data[LSM_Z]) ? v1.data[LSM_Z] : v2.data[LSM_Z];
	r.data[LSM_W] = (v1.data[LSM_W] > v2.data[LSM_W]) ? v1.data[LSM_W] : v2.data[LSM_W];

	return r;
}

/*
 * Restrict vector components between min and max bounds
 */
static inline LSMVECTOR LSMVectorClamp(LSMVECTOR v, LSMVECTOR min_val, LSMVECTOR max_val) {
	return LSMVectorMin(LSMVectorMax(v, min_val), max_val);
}

/*
 * Linearly interpolate between two vectors
 */
static inline LSMVECTOR LSMVectorLerp(LSMVECTOR v1, LSMVECTOR v2, float t) {
	LSMVECTOR r;
	r.data[LSM_X] = v1.data[LSM_X] + t * (v2.data[LSM_X] - v1.data[LSM_X]);
	r.data[LSM_Y] = v1.data[LSM_Y] + t * (v2.data[LSM_Y] - v1.data[LSM_Y]);
	r.data[LSM_Z] = v1.data[LSM_Z] + t * (v2.data[LSM_Z] - v1.data[LSM_Z]);
	r.data[LSM_W] = v1.data[LSM_W] + t * (v2.data[LSM_W] - v1.data[LSM_W]);

	return r;
}

#ifdef __cplusplus
// C++ Operator Overloads that map directly to the C functions
static inline LSMVECTOR operator+(LSMVECTOR a, LSMVECTOR b) {
	return LSMVectorAdd(a, b);
}

static inline LSMVECTOR operator-(LSMVECTOR a, LSMVECTOR b) {
	return LSMVectorSub(a, b);
}

static inline LSMVECTOR operator-(LSMVECTOR a) {
	return LSMVectorNegate(a);
}

static inline LSMVECTOR operator*(LSMVECTOR a, LSMVECTOR b) {
	return LSMVectorMul(a, b);
}

static inline LSMVECTOR operator*(LSMVECTOR a, float s) {
	return LSMVectorScale(a, s);
}

static inline LSMVECTOR operator*(float s, LSMVECTOR a) {
	return LSMVectorScale(a, s);
}

static inline LSMVECTOR operator/(LSMVECTOR a, LSMVECTOR b) {
	return LSMVectorDiv(a, b);
}

static inline LSMVECTOR operator/(LSMVECTOR a, float s) {
	return LSMVectorDivScalar(a, s);
}

static inline LSMVECTOR& operator+=(LSMVECTOR& a, LSMVECTOR b) {
	a = LSMVectorAdd(a, b);
	return a;
}

static inline LSMVECTOR& operator-=(LSMVECTOR& a, LSMVECTOR b) {
	a = LSMVectorSub(a, b);
	return a;
}

static inline LSMVECTOR& operator*=(LSMVECTOR& a, LSMVECTOR b) {
	a = LSMVectorMul(a, b);
	return a;
}

static inline LSMVECTOR& operator*=(LSMVECTOR& a, float s) {
	a = LSMVectorScale(a, s);
	return a;
}

static inline LSMVECTOR& operator/=(LSMVECTOR& a, LSMVECTOR b) {
	a = LSMVectorDiv(a, b);
	return a;
}

static inline LSMVECTOR& operator/=(LSMVECTOR& a, float s) {
	a = LSMVectorDivScalar(a, s);
	return a;
}
#endif

#endif // !LIGHT_SIMPLE_MATH_H
