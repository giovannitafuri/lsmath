# lsmath (Light & Simple Math)

A lightweight, single-header C/C++ vector math library inspired by the DirectXMath load/store architecture.

`lsmath` separates calculation types from memory storage types to ensure maximum cache density and SIMD alignment:
- **`LSMVECTOR`:** 4-float calculation vector union (`x, y, z, w` or `data[4]`) aligned for fast CPU register manipulation.
- **`LSMMATRIX`:** 4x4 calculation matrix union (`r[4]` row vectors or `m[4][4]` / `data[16]`) in row-major layout.
- **`LSMFLOAT2` / `LSMFLOAT3` / `LSMFLOAT4` / `LSMFLOAT4X4`:** Compact storage types for flat arrays and Structure of Arrays (SoA) component pools.

---

## Features

- **Single-Header:** Just drop `LSMath.h` into your project.
- **Zero Dependencies:** Pure C99 with standard library types (`<stdint.h>`, `<string.h>`, `<math.h>`).
- **Zero Allocations:** Pure POD structs passed by value or reference.
- **C & C++ Dual Support:** Native C inline functions, with optional C++ operator overloading (`+`, `-`, `*`, `/`, `+=`, `-=`, `*=`, `/=`) enabled automatically under `__cplusplus`.
- **Hardware SIMD Acceleration:** 128-bit hardware acceleration for x86/x64 (`__m128` SSE/SSE2) and ARM (`float32x4_t` NEON), with automatic pure C99 scalar fallback.
- **DirectXMath Load/Store Pattern:** Explicit memory staging avoids alignment penalties and implicit conversions.

---

## Configuration

`lsmath` supports both Row-Major and Column-Major matrix conventions via a compile-time macro:

| Macro | Value | Description |
| :--- | :---: | :--- |
| `LSM_ROW_MAJOR` | `0` | **(Default)** Row-major matrices (DirectX / HLSL style). Row vectors (`v * m`), `r[4]` rows. |
| `LSM_COLUMN_MAJOR` | `1` | Column-major matrices (OpenGL / GLSL style). Column vectors (`m * v`), `c[4]` columns. |

To switch to Column-Major, define `MATRIX_ORDER` (or `LSM_MATRIX_ORDER`) before including `LSMath.h`:
```c
#define MATRIX_ORDER 1 // or pass -DMATRIX_ORDER=1 to compiler
#include "LSMath.h"
```

To disable SIMD and force scalar fallback, define `LSM_NO_SIMD`:
```c
#define LSM_NO_SIMD 1
#include "LSMath.h"
```

---

## Quick Example

### In C++:
```cpp
#include "LSMath.h"

// 1. Compact storage in memory (e.g. in an entity array)
LSMFLOAT2 pos_storage = { 10.0f, 20.0f };
LSMFLOAT2 vel_storage = { 1.5f, -0.5f };

// 2. Load into calculation vector
LSMVECTOR pos = LSMLoadFloat2(&pos_storage);
LSMVECTOR vel = LSMLoadFloat2(&vel_storage);

// 3. Vector arithmetic
float dt = 1.0f / 60.0f;
pos = pos + vel * dt;

// 4. Store back to compact memory
pos_storage = LSMStoreFloat2(pos);
```

### In Pure C:
```c
#include "LSMath.h"

LSMFLOAT3 data = { 1.0f, 2.0f, 3.0f };
LSMVECTOR v1 = LSMVectorZero();
LSMVECTOR v2 = LSMLoadFloat3(&data);

LSMVECTOR result = LSMVectorAdd(v1, LSMVectorScale(v2, 2.0f));
float dot = LSMVectorDot(v1, v2);
```

---

## API Summary

| Operation | C Function | C++ Operator |
| :--- | :--- | :---: |
| **Load Vector** | `LSMLoadFloat2`, `LSMLoadFloat3`, `LSMLoadFloat4` | — |
| **Store Vector** | `LSMStoreFloat2`, `LSMStoreFloat3`, `LSMStoreFloat4` | — |
| **Set Vector** | `LSMVectorSet(x, y, z, w)` | — |
| **Zero Vector** | `LSMVectorZero()` | — |
| **Negation** | `LSMVectorNegate(v)` | `-v` |
| **Scaling** | `LSMVectorScale(v, s)` | `v * s` / `s * v`, `v *= s` |
| **Addition** | `LSMVectorAdd(v1, v2)` | `v1 + v2`, `v1 += v2` |
| **Subtraction** | `LSMVectorSub(v1, v2)` | `v1 - v2`, `v1 -= v2` |
| **Hadamard Product** | `LSMVectorMul(v1, v2)` | `v1 * v2`, `v1 *= v2` |
| **Division** | `LSMVectorDiv(v1, v2)` / `LSMVectorDivScalar(v, s)` | `v1 / v2`, `v / s`, `/=` |
| **Dot Product (4D)** | `LSMVectorDot(v1, v2)` | — |
| **Dot Product (3D)** | `LSMVector3Dot(v1, v2)` | — |
| **Cross Product** | `LSMVectorCross(v1, v2)` | — |
| **Length / Magnitude** | `LSMVectorLength(v)`, `LSMVectorLengthSq(v)` | — |
| **Normalize** | `LSMVectorNormalize(v)` | — |
| **Distance** | `LSMVectorDistance(v1, v2)`, `LSMVectorDistanceSq(v1, v2)` | — |
| **Linear Interpolation** | `LSMVectorLerp(v1, v2, t)` | — |
| **Min / Max / Clamp** | `LSMVectorMin`, `LSMVectorMax`, `LSMVectorClamp` | — |
| **Load Matrix** | `LSMLoadFloat4x4(src)` | — |
| **Store Matrix** | `LSMStoreFloat4x4(m)` | — |
| **Identity / Zero Matrix** | `LSMMatrixIdentity()`, `LSMMatrixZero()` | — |
| **Transpose** | `LSMMatrixTranspose(m)` | — |
| **Matrix Multiplication** | `LSMMatrixMultiply(m1, m2)` | `m1 * m2`, `m1 *= m2` |
| **Vector Transformation** | `LSMVector4Transform(v, m)`, `LSMVector3TransformCoord(v, m)`, `LSMVector3TransformNormal(v, m)` | `v * m`, `m * v` |
| **Affine Transformations** | `LSMMatrixTranslation(x, y, z)`, `LSMMatrixScaling(x, y, z)`, `LSMMatrixRotationX/Y/Z(angle)` | — |
| **Camera View** | `LSMMatrixLookAtLH(eye, target, up)`, `LSMMatrixLookAtRH(eye, target, up)` | — |
| **Perspective Projection** | `LSMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ)`, `...RH(...)` | — |
| **Orthographic Projection** | `LSMMatrixOrthographicLH(w, h, nearZ, farZ)`, `...RH(...)` | — |

---

## Status & Roadmap

> **Note:** `lsmath` is a work-in-progress prototype exploring DirectXMath-style load/store vector math in C.

- [x] Separate storage (`LSMFLOAT2/3/4/4X4`) and calculation (`LSMVECTOR`, `LSMMATRIX`) types
- [x] Pure C99 with zero heap allocations
- [x] C++ operator overloads (`+`, `-`, `*`, `/`, `+=`, `-=`, `*=`, `/=`)
- [x] 4x4 matrices (LookAt, Perspective, Orthographic, Transformations)
- [x] Hardware SIMD intrinsics (`__m128` SSE / ARM NEON with pure C99 scalar fallback)

---

## License

`lsmath` is released under the BSD 2-Clause License. See `LICENSE` for details.
