#ifndef LIGHT_SIMPLE_MATH_H
#define LIGHT_SIMPLE_MATH_H

#include <string.h>
#include <stdint.h>
#include <math.h>

#if !defined(LSM_NO_SIMD)
#if defined(__SSE2__) || defined(__SSE__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
#define LSM_USE_SSE 1
#include <xmmintrin.h>
#include <emmintrin.h>
#elif (defined(__ARM_NEON) || defined(__ARM_NEON__)) && (defined(__aarch64__) || defined(_M_ARM64))
#define LSM_USE_NEON 1
#include <arm_neon.h>
#endif
#endif

enum {
	LSM_X,
	LSM_Y,
	LSM_Z,
	LSM_W
};

#define LSM_ROW_MAJOR    0
#define LSM_COLUMN_MAJOR 1

#ifndef LSM_MATRIX_ORDER
#ifdef MATRIX_ORDER
#define LSM_MATRIX_ORDER MATRIX_ORDER
#else
#define LSM_MATRIX_ORDER LSM_ROW_MAJOR
#endif
#endif

/*
 *	LSMVECTOR is used as a generic 4 float vector to do calculations
 *	so user don't need to convert between vector of different sizes
 *
 *	There are specific LSMFLOAT(N) storage version with N can be 2, 3, 4
 */


typedef struct LSMFLOAT4X4 { float m[4][4]; } LSMFLOAT4X4;

typedef struct LSMFLOAT4 { float data[4]; } LSMFLOAT4;

typedef struct LSMFLOAT3 { float data[3]; } LSMFLOAT3;

typedef struct LSMFLOAT2 { float data[2]; } LSMFLOAT2;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4201)
#endif

typedef union LSMVECTOR {
#if defined(LSM_USE_SSE)
	__m128 v;
#elif defined(LSM_USE_NEON)
	float32x4_t v;
#endif
	float data[4];
	struct { float x, y, z, w; };
} LSMVECTOR;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

typedef union LSMMATRIX {
#if LSM_MATRIX_ORDER == LSM_ROW_MAJOR
	LSMVECTOR r[4];
#else
	LSMVECTOR c[4];
	LSMVECTOR r[4];
#endif
	float m[4][4];
	float data[16];
} LSMMATRIX;

/*
 * Load 2 components into a calculation vector, zeroing unused Z and W
 */
static inline LSMVECTOR LSMLoadFloat2(const LSMFLOAT2* src) {
	LSMVECTOR v;
	v.data[LSM_X] = src->data[LSM_X];
	v.data[LSM_Y] = src->data[LSM_Y];
	v.data[LSM_Z] = 0.0f;
	v.data[LSM_W] = 0.0f;

	return v;
}

/*
 * Load 3 components into a calculation vector, zeroing unused W
 */
static inline LSMVECTOR LSMLoadFloat3(const LSMFLOAT3* src) {
	LSMVECTOR v;
	v.data[LSM_X] = src->data[LSM_X];
	v.data[LSM_Y] = src->data[LSM_Y];
	v.data[LSM_Z] = src->data[LSM_Z];
	v.data[LSM_W] = 0.0f;

	return v;
}

/*
 * Load 4 components into a calculation vector
 */
static inline LSMVECTOR LSMLoadFloat4(const LSMFLOAT4* src) {
	LSMVECTOR v;
#if defined(LSM_USE_SSE)
	v.v = _mm_loadu_ps(src->data);
#elif defined(LSM_USE_NEON)
	v.v = vld1q_f32(src->data);
#else
	v.data[LSM_X] = src->data[LSM_X];
	v.data[LSM_Y] = src->data[LSM_Y];
	v.data[LSM_Z] = src->data[LSM_Z];
	v.data[LSM_W] = src->data[LSM_W];
#endif

	return v;
}

static inline LSMFLOAT4 LSMStoreFloat4(LSMVECTOR v) {
	LSMFLOAT4 storage;
#if defined(LSM_USE_SSE)
	_mm_storeu_ps(storage.data, v.v);
#elif defined(LSM_USE_NEON)
	vst1q_f32(storage.data, v.v);
#else
	storage.data[LSM_X] = v.data[LSM_X];
	storage.data[LSM_Y] = v.data[LSM_Y];
	storage.data[LSM_Z] = v.data[LSM_Z];
	storage.data[LSM_W] = v.data[LSM_W];
#endif

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
 * Load a 4x4 matrix from compact storage memory
 */
static inline LSMMATRIX LSMLoadFloat4x4(const LSMFLOAT4X4* src) {
	LSMMATRIX mat;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			mat.m[i][j] = src->m[i][j];
		}
	}

	return mat;
}

/*
 * Store a calculation matrix into compact 4x4 memory
 */
static inline LSMFLOAT4X4 LSMStoreFloat4x4(LSMMATRIX mat) {
	LSMFLOAT4X4 storage;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			storage.m[i][j] = mat.m[i][j];
		}
	}

	return storage;
}

/*
 *	Zero vector
 *	Return a vector with X, Y, Z, W set to 0.0f
 */
static inline LSMVECTOR LSMVectorZero() {
	LSMVECTOR zeroVector;
#if defined(LSM_USE_SSE)
	zeroVector.v = _mm_setzero_ps();
#elif defined(LSM_USE_NEON)
	zeroVector.v = vdupq_n_f32(0.0f);
#else
	zeroVector.data[LSM_X] = 0.0f;
	zeroVector.data[LSM_Y] = 0.0f;
	zeroVector.data[LSM_Z] = 0.0f;
	zeroVector.data[LSM_W] = 0.0f;
#endif

	return zeroVector;
}

/*
 *	Vector scaling
 *	It takes the LSMVECTOR vector and float scalar, it return a new vector
 */
static inline LSMVECTOR LSMVectorScale(LSMVECTOR v, float s) {
#if defined(LSM_USE_SSE)
	v.v = _mm_mul_ps(v.v, _mm_set1_ps(s));
	return v;
#elif defined(LSM_USE_NEON)
	v.v = vmulq_n_f32(v.v, s);
	return v;
#else
	v.data[LSM_X] *= s;
	v.data[LSM_Y] *= s;
	v.data[LSM_Z] *= s;
	v.data[LSM_W] *= s;

	return v;
#endif
}

/*
 *	Component-wise multiplication
 *	It takes the LSMVECTOR vector1 and LSMVECTOR vector2, it return a new vector
 */
static inline LSMVECTOR LSMVectorMul(LSMVECTOR v1, LSMVECTOR v2) {
#if defined(LSM_USE_SSE)
	LSMVECTOR r;
	r.v = _mm_mul_ps(v1.v, v2.v);
	return r;
#elif defined(LSM_USE_NEON)
	LSMVECTOR r;
	r.v = vmulq_f32(v1.v, v2.v);
	return r;
#else
	v1.data[LSM_X] *= v2.data[LSM_X];
	v1.data[LSM_Y] *= v2.data[LSM_Y];
	v1.data[LSM_Z] *= v2.data[LSM_Z];
	v1.data[LSM_W] *= v2.data[LSM_W];

	return v1;
#endif
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
#if defined(LSM_USE_SSE)
	__m128 v1_yzx = _mm_shuffle_ps(v1.v, v1.v, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 v2_zxy = _mm_shuffle_ps(v2.v, v2.v, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 v1_zxy = _mm_shuffle_ps(v1.v, v1.v, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 v2_yzx = _mm_shuffle_ps(v2.v, v2.v, _MM_SHUFFLE(3, 0, 2, 1));
	LSMVECTOR cross_vector;
	cross_vector.v = _mm_sub_ps(_mm_mul_ps(v1_yzx, v2_zxy), _mm_mul_ps(v1_zxy, v2_yzx));
	cross_vector.data[LSM_W] = 0.0f;

	return cross_vector;
#else
	LSMVECTOR cross_vector;
	cross_vector.data[LSM_X] =
	(v1.data[LSM_Y] * v2.data[LSM_Z]) - (v1.data[LSM_Z] * v2.data[LSM_Y]);
	cross_vector.data[LSM_Y] =
	(v1.data[LSM_Z] * v2.data[LSM_X]) - (v1.data[LSM_X] * v2.data[LSM_Z]);
	cross_vector.data[LSM_Z] =
	(v1.data[LSM_X] * v2.data[LSM_Y]) - (v1.data[LSM_Y] * v2.data[LSM_X]);
	cross_vector.data[LSM_W] = 0.0f;

	return cross_vector;
#endif
}

/*
 *	Vector Addition
 *	It takes the LSMVECTOR vector1 and LSMVECTOR vector2, it return a new vector
 */
static inline LSMVECTOR LSMVectorAdd(LSMVECTOR v1, LSMVECTOR v2) {
#if defined(LSM_USE_SSE)
	LSMVECTOR new_vector;
	new_vector.v = _mm_add_ps(v1.v, v2.v);
	return new_vector;
#elif defined(LSM_USE_NEON)
	LSMVECTOR new_vector;
	new_vector.v = vaddq_f32(v1.v, v2.v);
	return new_vector;
#else
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
#endif
}

/*
 * Vector Subtraction
 * It takes the LSMVECTOR vector1 and LSMVECTOR vector2, it return a new vector
 */
static inline LSMVECTOR LSMVectorSub(LSMVECTOR v1, LSMVECTOR v2) {
#if defined(LSM_USE_SSE)
	LSMVECTOR new_vector;
	new_vector.v = _mm_sub_ps(v1.v, v2.v);
	return new_vector;
#elif defined(LSM_USE_NEON)
	LSMVECTOR new_vector;
	new_vector.v = vsubq_f32(v1.v, v2.v);
	return new_vector;
#else
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
#endif
}

/*
 * Construct a vector directly from 4 components
 */
static inline LSMVECTOR LSMVectorSet(float x, float y, float z, float w) {
	LSMVECTOR v;
#if defined(LSM_USE_SSE)
	v.v = _mm_set_ps(w, z, y, x);
#elif defined(LSM_USE_NEON)
	float vals[4] = { x, y, z, w };
	v.v = vld1q_f32(vals);
#else
	v.data[LSM_X] = x;
	v.data[LSM_Y] = y;
	v.data[LSM_Z] = z;
	v.data[LSM_W] = w;
#endif

	return v;
}

/*
 * Invert the sign of all vector components
 */
static inline LSMVECTOR LSMVectorNegate(LSMVECTOR v) {
#if defined(LSM_USE_SSE)
	v.v = _mm_sub_ps(_mm_setzero_ps(), v.v);
	return v;
#elif defined(LSM_USE_NEON)
	v.v = vnegq_f32(v.v);
	return v;
#else
	v.data[LSM_X] = -v.data[LSM_X];
	v.data[LSM_Y] = -v.data[LSM_Y];
	v.data[LSM_Z] = -v.data[LSM_Z];
	v.data[LSM_W] = -v.data[LSM_W];

	return v;
#endif
}

/*
 * Component-wise division
 */
static inline LSMVECTOR LSMVectorDiv(LSMVECTOR v1, LSMVECTOR v2) {
#if defined(LSM_USE_SSE)
	LSMVECTOR r;
	r.v = _mm_div_ps(v1.v, v2.v);
	return r;
#elif defined(LSM_USE_NEON)
	LSMVECTOR r;
	r.v = vdivq_f32(v1.v, v2.v);
	return r;
#else
	v1.data[LSM_X] /= v2.data[LSM_X];
	v1.data[LSM_Y] /= v2.data[LSM_Y];
	v1.data[LSM_Z] /= v2.data[LSM_Z];
	v1.data[LSM_W] /= v2.data[LSM_W];

	return v1;
#endif
}

/*
 * Divide vector components by a scalar
 */
static inline LSMVECTOR LSMVectorDivScalar(LSMVECTOR v, float s) {
#if defined(LSM_USE_SSE)
	v.v = _mm_div_ps(v.v, _mm_set1_ps(s));
	return v;
#elif defined(LSM_USE_NEON)
	v.v = vdivq_f32(v.v, vdupq_n_f32(s));
	return v;
#else
	v.data[LSM_X] /= s;
	v.data[LSM_Y] /= s;
	v.data[LSM_Z] /= s;
	v.data[LSM_W] /= s;

	return v;
#endif
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
 * Normalizes 3D components and sets W to 0.0f.
 * Optimized for high performance in real-time graphics. Assumes typical finite float
 * magnitudes; extreme magnitudes (|v| > 1e19 or |v| < 1e-19) may cause lengthSq overflow/underflow.
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
#if defined(LSM_USE_SSE)
	LSMVECTOR r;
	r.v = _mm_min_ps(v1.v, v2.v);
	return r;
#elif defined(LSM_USE_NEON)
	LSMVECTOR r;
	r.v = vminq_f32(v1.v, v2.v);
	return r;
#else
	LSMVECTOR r;
	r.data[LSM_X] = (v1.data[LSM_X] < v2.data[LSM_X]) ? v1.data[LSM_X] : v2.data[LSM_X];
	r.data[LSM_Y] = (v1.data[LSM_Y] < v2.data[LSM_Y]) ? v1.data[LSM_Y] : v2.data[LSM_Y];
	r.data[LSM_Z] = (v1.data[LSM_Z] < v2.data[LSM_Z]) ? v1.data[LSM_Z] : v2.data[LSM_Z];
	r.data[LSM_W] = (v1.data[LSM_W] < v2.data[LSM_W]) ? v1.data[LSM_W] : v2.data[LSM_W];

	return r;
#endif
}

/*
 * Select maximum values component-wise
 */
static inline LSMVECTOR LSMVectorMax(LSMVECTOR v1, LSMVECTOR v2) {
#if defined(LSM_USE_SSE)
	LSMVECTOR r;
	r.v = _mm_max_ps(v1.v, v2.v);
	return r;
#elif defined(LSM_USE_NEON)
	LSMVECTOR r;
	r.v = vmaxq_f32(v1.v, v2.v);
	return r;
#else
	LSMVECTOR r;
	r.data[LSM_X] = (v1.data[LSM_X] > v2.data[LSM_X]) ? v1.data[LSM_X] : v2.data[LSM_X];
	r.data[LSM_Y] = (v1.data[LSM_Y] > v2.data[LSM_Y]) ? v1.data[LSM_Y] : v2.data[LSM_Y];
	r.data[LSM_Z] = (v1.data[LSM_Z] > v2.data[LSM_Z]) ? v1.data[LSM_Z] : v2.data[LSM_Z];
	r.data[LSM_W] = (v1.data[LSM_W] > v2.data[LSM_W]) ? v1.data[LSM_W] : v2.data[LSM_W];

	return r;
#endif
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
#if defined(LSM_USE_SSE)
	LSMVECTOR r;
	__m128 vt = _mm_set1_ps(t);
	r.v = _mm_add_ps(v1.v, _mm_mul_ps(vt, _mm_sub_ps(v2.v, v1.v)));
	return r;
#elif defined(LSM_USE_NEON)
	LSMVECTOR r;
	r.v = vaddq_f32(v1.v, vmulq_n_f32(vsubq_f32(v2.v, v1.v), t));
	return r;
#else
	LSMVECTOR r;
	r.data[LSM_X] = v1.data[LSM_X] + t * (v2.data[LSM_X] - v1.data[LSM_X]);
	r.data[LSM_Y] = v1.data[LSM_Y] + t * (v2.data[LSM_Y] - v1.data[LSM_Y]);
	r.data[LSM_Z] = v1.data[LSM_Z] + t * (v2.data[LSM_Z] - v1.data[LSM_Z]);
	r.data[LSM_W] = v1.data[LSM_W] + t * (v2.data[LSM_W] - v1.data[LSM_W]);

	return r;
#endif
}

/*
 * Return a 4x4 identity matrix
 */
static inline LSMMATRIX LSMMatrixIdentity(void) {
	LSMMATRIX mat = { 0 };
	mat.m[0][0] = 1.0f;
	mat.m[1][1] = 1.0f;
	mat.m[2][2] = 1.0f;
	mat.m[3][3] = 1.0f;

	return mat;
}

/*
 * Return a 4x4 zero matrix
 */
static inline LSMMATRIX LSMMatrixZero(void) {
	LSMMATRIX mat = { 0 };
	return mat;
}

/*
 * Transpose rows and columns of a matrix
 */
static inline LSMMATRIX LSMMatrixTranspose(LSMMATRIX mat) {
	LSMMATRIX res;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			res.m[i][j] = mat.m[j][i];
		}
	}

	return res;
}

/*
 * Multiply two 4x4 matrices (m1 * m2)
 */
static inline LSMMATRIX LSMMatrixMultiply(LSMMATRIX m1, LSMMATRIX m2) {
	LSMMATRIX res;
#if defined(LSM_USE_SSE)
#if LSM_MATRIX_ORDER == LSM_ROW_MAJOR
	for (int i = 0; i < 4; ++i) {
		__m128 e0 = _mm_set1_ps(m1.m[i][0]);
		__m128 e1 = _mm_set1_ps(m1.m[i][1]);
		__m128 e2 = _mm_set1_ps(m1.m[i][2]);
		__m128 e3 = _mm_set1_ps(m1.m[i][3]);

		res.r[i].v = _mm_add_ps(
			_mm_add_ps(_mm_mul_ps(e0, m2.r[0].v), _mm_mul_ps(e1, m2.r[1].v)),
			_mm_add_ps(_mm_mul_ps(e2, m2.r[2].v), _mm_mul_ps(e3, m2.r[3].v))
		);
	}
#else
	for (int c = 0; c < 4; ++c) {
		__m128 e0 = _mm_set1_ps(m2.m[c][0]);
		__m128 e1 = _mm_set1_ps(m2.m[c][1]);
		__m128 e2 = _mm_set1_ps(m2.m[c][2]);
		__m128 e3 = _mm_set1_ps(m2.m[c][3]);

		res.c[c].v = _mm_add_ps(
			_mm_add_ps(_mm_mul_ps(e0, m1.c[0].v), _mm_mul_ps(e1, m1.c[1].v)),
			_mm_add_ps(_mm_mul_ps(e2, m1.c[2].v), _mm_mul_ps(e3, m1.c[3].v))
		);
	}
#endif
#elif defined(LSM_USE_NEON)
#if LSM_MATRIX_ORDER == LSM_ROW_MAJOR
	for (int i = 0; i < 4; ++i) {
		float32x4_t row = vmulq_n_f32(m2.r[0].v, m1.m[i][0]);
		row = vmlaq_n_f32(row, m2.r[1].v, m1.m[i][1]);
		row = vmlaq_n_f32(row, m2.r[2].v, m1.m[i][2]);
		row = vmlaq_n_f32(row, m2.r[3].v, m1.m[i][3]);
		res.r[i].v = row;
	}
#else
	for (int c = 0; c < 4; ++c) {
		float32x4_t col = vmulq_n_f32(m1.c[0].v, m2.m[c][0]);
		col = vmlaq_n_f32(col, m1.c[1].v, m2.m[c][1]);
		col = vmlaq_n_f32(col, m1.c[2].v, m2.m[c][2]);
		col = vmlaq_n_f32(col, m1.c[3].v, m2.m[c][3]);
		res.c[c].v = col;
	}
#endif
#else
#if LSM_MATRIX_ORDER == LSM_ROW_MAJOR
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			res.m[i][j] = m1.m[i][0] * m2.m[0][j] +
			              m1.m[i][1] * m2.m[1][j] +
			              m1.m[i][2] * m2.m[2][j] +
			              m1.m[i][3] * m2.m[3][j];
		}
	}
#else
	for (int c = 0; c < 4; ++c) {
		for (int r = 0; r < 4; ++r) {
			res.m[c][r] = m1.m[0][r] * m2.m[c][0] +
			              m1.m[1][r] * m2.m[c][1] +
			              m1.m[2][r] * m2.m[c][2] +
			              m1.m[3][r] * m2.m[c][3];
		}
	}
#endif
#endif

	return res;
}

/*
 * Transform a 4D vector by a matrix (v * m in row-major, m * v in column-major)
 */
static inline LSMVECTOR LSMVector4Transform(LSMVECTOR v, LSMMATRIX m) {
	LSMVECTOR res;
#if defined(LSM_USE_SSE)
#if LSM_MATRIX_ORDER == LSM_ROW_MAJOR
	__m128 x = _mm_set1_ps(v.data[LSM_X]);
	__m128 y = _mm_set1_ps(v.data[LSM_Y]);
	__m128 z = _mm_set1_ps(v.data[LSM_Z]);
	__m128 w = _mm_set1_ps(v.data[LSM_W]);

	res.v = _mm_add_ps(
		_mm_add_ps(_mm_mul_ps(x, m.r[0].v), _mm_mul_ps(y, m.r[1].v)),
		_mm_add_ps(_mm_mul_ps(z, m.r[2].v), _mm_mul_ps(w, m.r[3].v))
	);
#else
	__m128 x = _mm_set1_ps(v.data[LSM_X]);
	__m128 y = _mm_set1_ps(v.data[LSM_Y]);
	__m128 z = _mm_set1_ps(v.data[LSM_Z]);
	__m128 w = _mm_set1_ps(v.data[LSM_W]);

	res.v = _mm_add_ps(
		_mm_add_ps(_mm_mul_ps(x, m.c[0].v), _mm_mul_ps(y, m.c[1].v)),
		_mm_add_ps(_mm_mul_ps(z, m.c[2].v), _mm_mul_ps(w, m.c[3].v))
	);
#endif
#elif defined(LSM_USE_NEON)
#if LSM_MATRIX_ORDER == LSM_ROW_MAJOR
	float32x4_t r = vmulq_n_f32(m.r[0].v, v.data[LSM_X]);
	r = vmlaq_n_f32(r, m.r[1].v, v.data[LSM_Y]);
	r = vmlaq_n_f32(r, m.r[2].v, v.data[LSM_Z]);
	r = vmlaq_n_f32(r, m.r[3].v, v.data[LSM_W]);
	res.v = r;
#else
	float32x4_t r = vmulq_n_f32(m.c[0].v, v.data[LSM_X]);
	r = vmlaq_n_f32(r, m.c[1].v, v.data[LSM_Y]);
	r = vmlaq_n_f32(r, m.c[2].v, v.data[LSM_Z]);
	r = vmlaq_n_f32(r, m.c[3].v, v.data[LSM_W]);
	res.v = r;
#endif
#else
#if LSM_MATRIX_ORDER == LSM_ROW_MAJOR
	res.data[LSM_X] = v.data[LSM_X] * m.m[0][0] + v.data[LSM_Y] * m.m[1][0] + v.data[LSM_Z] * m.m[2][0] + v.data[LSM_W] * m.m[3][0];
	res.data[LSM_Y] = v.data[LSM_X] * m.m[0][1] + v.data[LSM_Y] * m.m[1][1] + v.data[LSM_Z] * m.m[2][1] + v.data[LSM_W] * m.m[3][1];
	res.data[LSM_Z] = v.data[LSM_X] * m.m[0][2] + v.data[LSM_Y] * m.m[1][2] + v.data[LSM_Z] * m.m[2][2] + v.data[LSM_W] * m.m[3][2];
	res.data[LSM_W] = v.data[LSM_X] * m.m[0][3] + v.data[LSM_Y] * m.m[1][3] + v.data[LSM_Z] * m.m[2][3] + v.data[LSM_W] * m.m[3][3];
#else
	res.data[LSM_X] = m.m[0][0] * v.data[LSM_X] + m.m[1][0] * v.data[LSM_Y] + m.m[2][0] * v.data[LSM_Z] + m.m[3][0] * v.data[LSM_W];
	res.data[LSM_Y] = m.m[0][1] * v.data[LSM_X] + m.m[1][1] * v.data[LSM_Y] + m.m[2][1] * v.data[LSM_Z] + m.m[3][1] * v.data[LSM_W];
	res.data[LSM_Z] = m.m[0][2] * v.data[LSM_X] + m.m[1][2] * v.data[LSM_Y] + m.m[2][2] * v.data[LSM_Z] + m.m[3][2] * v.data[LSM_W];
	res.data[LSM_W] = m.m[0][3] * v.data[LSM_X] + m.m[1][3] * v.data[LSM_Y] + m.m[2][3] * v.data[LSM_Z] + m.m[3][3] * v.data[LSM_W];
#endif
#endif

	return res;
}

/*
 * Transform a 3D coordinate (w=1.0f) by a matrix and project back dividing by W
 */
static inline LSMVECTOR LSMVector3TransformCoord(LSMVECTOR v, LSMMATRIX m) {
	LSMVECTOR p = v;
	p.data[LSM_W] = 1.0f;
	LSMVECTOR res = LSMVector4Transform(p, m);

	float invW = 1.0f / res.data[LSM_W];
	res.data[LSM_X] *= invW;
	res.data[LSM_Y] *= invW;
	res.data[LSM_Z] *= invW;
	res.data[LSM_W] = 1.0f;

	return res;
}

/*
 * Transform a 3D surface normal/direction vector (w=0.0f) by a matrix, ignoring translation.
 * Assumes the matrix has uniform scale and orthogonal rotation (DirectXMath convention).
 * If uniform scale != 1.0f, the resulting normal must be re-normalized. For non-uniform
 * scaling, transform the normal using the transpose of the inverse matrix.
 */
static inline LSMVECTOR LSMVector3TransformNormal(LSMVECTOR v, LSMMATRIX m) {
	LSMVECTOR n = v;
	n.data[LSM_W] = 0.0f;
	LSMVECTOR res = LSMVector4Transform(n, m);
	res.data[LSM_W] = 0.0f;

	return res;
}

/*
 * Create a translation matrix
 */
static inline LSMMATRIX LSMMatrixTranslation(float x, float y, float z) {
	LSMMATRIX mat = LSMMatrixIdentity();
	mat.m[3][0] = x;
	mat.m[3][1] = y;
	mat.m[3][2] = z;

	return mat;
}

/*
 * Create a non-uniform scale matrix
 */
static inline LSMMATRIX LSMMatrixScaling(float x, float y, float z) {
	LSMMATRIX mat = { 0 };
	mat.m[0][0] = x;
	mat.m[1][1] = y;
	mat.m[2][2] = z;
	mat.m[3][3] = 1.0f;

	return mat;
}

/*
 * Create a rotation matrix around the X axis (angle in radians)
 */
static inline LSMMATRIX LSMMatrixRotationX(float angle) {
	float c = cosf(angle);
	float s = sinf(angle);

	LSMMATRIX mat = LSMMatrixIdentity();
	mat.m[1][1] = c;
	mat.m[1][2] = s;
	mat.m[2][1] = -s;
	mat.m[2][2] = c;

	return mat;
}

/*
 * Create a rotation matrix around the Y axis (angle in radians)
 */
static inline LSMMATRIX LSMMatrixRotationY(float angle) {
	float c = cosf(angle);
	float s = sinf(angle);

	LSMMATRIX mat = LSMMatrixIdentity();
	mat.m[0][0] = c;
	mat.m[0][2] = -s;
	mat.m[2][0] = s;
	mat.m[2][2] = c;

	return mat;
}

/*
 * Create a rotation matrix around the Z axis (angle in radians)
 */
static inline LSMMATRIX LSMMatrixRotationZ(float angle) {
	float c = cosf(angle);
	float s = sinf(angle);

	LSMMATRIX mat = LSMMatrixIdentity();
	mat.m[0][0] = c;
	mat.m[0][1] = s;
	mat.m[1][0] = -s;
	mat.m[1][1] = c;

	return mat;
}

/*
 * Left-handed view matrix (camera looking towards target)
 */
static inline LSMMATRIX LSMMatrixLookAtLH(LSMVECTOR eye, LSMVECTOR target, LSMVECTOR up) {
	LSMVECTOR zAxis = LSMVectorNormalize(LSMVectorSub(target, eye));
	LSMVECTOR xAxis = LSMVectorNormalize(LSMVectorCross(up, zAxis));
	LSMVECTOR yAxis = LSMVectorCross(zAxis, xAxis);

	LSMMATRIX mat;
	mat.m[0][0] = xAxis.data[LSM_X];
	mat.m[0][1] = yAxis.data[LSM_X];
	mat.m[0][2] = zAxis.data[LSM_X];
	mat.m[0][3] = 0.0f;

	mat.m[1][0] = xAxis.data[LSM_Y];
	mat.m[1][1] = yAxis.data[LSM_Y];
	mat.m[1][2] = zAxis.data[LSM_Y];
	mat.m[1][3] = 0.0f;

	mat.m[2][0] = xAxis.data[LSM_Z];
	mat.m[2][1] = yAxis.data[LSM_Z];
	mat.m[2][2] = zAxis.data[LSM_Z];
	mat.m[2][3] = 0.0f;

	mat.m[3][0] = -LSMVector3Dot(xAxis, eye);
	mat.m[3][1] = -LSMVector3Dot(yAxis, eye);
	mat.m[3][2] = -LSMVector3Dot(zAxis, eye);
	mat.m[3][3] = 1.0f;

	return mat;
}

/*
 * Right-handed view matrix
 */
static inline LSMMATRIX LSMMatrixLookAtRH(LSMVECTOR eye, LSMVECTOR target, LSMVECTOR up) {
	LSMVECTOR zAxis = LSMVectorNormalize(LSMVectorSub(eye, target));
	LSMVECTOR xAxis = LSMVectorNormalize(LSMVectorCross(up, zAxis));
	LSMVECTOR yAxis = LSMVectorCross(zAxis, xAxis);

	LSMMATRIX mat;
	mat.m[0][0] = xAxis.data[LSM_X];
	mat.m[0][1] = yAxis.data[LSM_X];
	mat.m[0][2] = zAxis.data[LSM_X];
	mat.m[0][3] = 0.0f;

	mat.m[1][0] = xAxis.data[LSM_Y];
	mat.m[1][1] = yAxis.data[LSM_Y];
	mat.m[1][2] = zAxis.data[LSM_Y];
	mat.m[1][3] = 0.0f;

	mat.m[2][0] = xAxis.data[LSM_Z];
	mat.m[2][1] = yAxis.data[LSM_Z];
	mat.m[2][2] = zAxis.data[LSM_Z];
	mat.m[2][3] = 0.0f;

	mat.m[3][0] = -LSMVector3Dot(xAxis, eye);
	mat.m[3][1] = -LSMVector3Dot(yAxis, eye);
	mat.m[3][2] = -LSMVector3Dot(zAxis, eye);
	mat.m[3][3] = 1.0f;

	return mat;
}

/*
 * Left-handed perspective projection matrix (depth range [0, 1])
 */
static inline LSMMATRIX LSMMatrixPerspectiveFovLH(float fovAngleY, float aspectRatio, float nearZ, float farZ) {
	float h = 1.0f / tanf(fovAngleY * 0.5f);
	float w = h / aspectRatio;
	float range = farZ - nearZ;

	LSMMATRIX mat = { 0 };
	mat.m[0][0] = w;
	mat.m[1][1] = h;
	mat.m[2][2] = farZ / range;
	mat.m[2][3] = 1.0f;
	mat.m[3][2] = -(nearZ * farZ) / range;

	return mat;
}

/*
 * Right-handed perspective projection matrix (depth range [0, 1])
 */
static inline LSMMATRIX LSMMatrixPerspectiveFovRH(float fovAngleY, float aspectRatio, float nearZ, float farZ) {
	float h = 1.0f / tanf(fovAngleY * 0.5f);
	float w = h / aspectRatio;
	float range = nearZ - farZ;

	LSMMATRIX mat = { 0 };
	mat.m[0][0] = w;
	mat.m[1][1] = h;
	mat.m[2][2] = farZ / range;
	mat.m[2][3] = -1.0f;
	mat.m[3][2] = (nearZ * farZ) / range;

	return mat;
}

/*
 * Left-handed orthographic projection matrix (depth range [0, 1])
 */
static inline LSMMATRIX LSMMatrixOrthographicLH(float viewWidth, float viewHeight, float nearZ, float farZ) {
	float range = farZ - nearZ;

	LSMMATRIX mat = { 0 };
	mat.m[0][0] = 2.0f / viewWidth;
	mat.m[1][1] = 2.0f / viewHeight;
	mat.m[2][2] = 1.0f / range;
	mat.m[3][2] = -nearZ / range;
	mat.m[3][3] = 1.0f;

	return mat;
}

/*
 * Right-handed orthographic projection matrix (depth range [0, 1])
 */
static inline LSMMATRIX LSMMatrixOrthographicRH(float viewWidth, float viewHeight, float nearZ, float farZ) {
	float range = nearZ - farZ;

	LSMMATRIX mat = { 0 };
	mat.m[0][0] = 2.0f / viewWidth;
	mat.m[1][1] = 2.0f / viewHeight;
	mat.m[2][2] = 1.0f / range;
	mat.m[3][2] = nearZ / range;
	mat.m[3][3] = 1.0f;

	return mat;
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

static inline LSMMATRIX operator*(LSMMATRIX a, LSMMATRIX b) {
	return LSMMatrixMultiply(a, b);
}

static inline LSMVECTOR operator*(LSMVECTOR v, LSMMATRIX m) {
	return LSMVector4Transform(v, m);
}

static inline LSMVECTOR operator*(LSMMATRIX m, LSMVECTOR v) {
	return LSMVector4Transform(v, m);
}

static inline LSMMATRIX& operator*=(LSMMATRIX& a, LSMMATRIX b) {
	a = LSMMatrixMultiply(a, b);
	return a;
}
#endif

#endif // !LIGHT_SIMPLE_MATH_H
