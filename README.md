# lsmath (Light & Simple Math)

A lightweight, single-header C/C++ vector math library inspired by the DirectXMath load/store architecture.

`lsmath` separates calculation types from memory storage types to ensure maximum cache density and SIMD alignment:
- **`LSMVECTOR`:** 4-float calculation vector union (`x, y, z, w` or `data[4]`) aligned for fast CPU register manipulation.
- **`LSMFLOAT2` / `LSMFLOAT3` / `LSMFLOAT4`:** Compact storage types for flat arrays and Structure of Arrays (SoA) component pools.

---

## Features

- **Single-Header:** Just drop `LSMath.h` into your project.
- **Zero Dependencies:** Pure C99 with standard library types (`<stdint.h>`, `<string.h>`).
- **Zero Allocations:** Pure POD structs passed by value or reference.
- **C & C++ Dual Support:** Native C inline functions, with optional C++ operator overloading (`+`, `-`, `*`) enabled automatically under `__cplusplus`.
- **DirectXMath Load/Store Pattern:** Explicit memory staging avoids alignment penalties and implicit conversions.

---

## Quick Example

### In C++:
```cpp
#include "LSMath.h"

// 1. Compact storage in memory (e.g. in an entity array)
LSMFLOAT2 pos_storage = { 10.0f, 20.0f };
LSMFLOAT2 vel_storage = { 1.5f, -0.5f };

// 2. Load into calculation vector
LSMVECTOR pos, vel;
LSMLoadVector(&pos_storage, 2, &pos);
LSMLoadVector(&vel_storage, 2, &vel);

// 3. Vector arithmetic
float dt = 1.0f / 60.0f;
pos = pos + vel * dt;

// 4. Store back to compact memory
pos_storage = LSMStoreFloat2(pos);
```

### In Pure C:
```c
#include "LSMath.h"

LSMVECTOR v1 = LSMVectorZero();
LSMVECTOR v2;
LSMLoadVector(data, 3, &v2);

LSMVECTOR result = LSMVectorAdd(v1, LSMVectorScale(v2, 2.0f));
float dot = LSMVectorDot(v1, v2);
```

---

## API Summary

| Operation | C Function | C++ Operator |
| :--- | :--- | :---: |
| **Load Vector** | `LSMLoadVector(src, num_components, dst)` | — |
| **Store Vector** | `LSMStoreFloat2`, `LSMStoreFloat3`, `LSMStoreFloat4` | — |
| **Zero Vector** | `LSMVectorZero()` | — |
| **Scaling** | `LSMVectorScale(v, s)` | `v * s` / `s * v` |
| **Addition** | `LSMVectorAdd(v1, v2)` | `v1 + v2` |
| **Subtraction** | `LSMVectorSub(v1, v2)` | `v1 - v2` |
| **Hadamard Product** | `LSMVectorMul(v1, v2)` | `v1 * v2` |
| **Dot Product** | `LSMVectorDot(v1, v2)` | — |
| **Cross Product** | `LSMVectorCross(v1, v2)` | — |

---

## License

`lsmath` is released under the BSD 2-Clause License. See `LICENSE` for details.
