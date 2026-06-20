# sricore

**High-performance C++ engine powering the SRI framework**

sricore is the native execution layer of [SRI](https://github.com/SRIKARNANDAGIRI/SRI). It routes goals through nine Avarana layers, enforces complexity budgets, and emits a Bindu result — all with predictable, low-latency performance suitable for production agent pipelines.

## Why C++

Routing and complexity checks run in **under 500ns** per decision. The hot path uses stack-only data structures with **zero heap allocation**, so layer transitions stay deterministic and cache-friendly.

## The 9 Avarana layers

1. Bhupura
2. 16-petal
3. 8-petal
4. 14-triangle
5. Outer10
6. Inner10
7. 8-triangle
8. Upward
9. Downward

*Bindu* is the final output stage after all layers pass.

## Build

```bash
cmake -B build && cmake --build build
```

## Quick start

```bash
./build/srichakra "analyze this task"
```

```
success: true
layers passed: 9/9
total time (us): 18
result: analyze this task
```

## Languages

C++20

## License

MIT

## Related

- [SRI](https://github.com/SRIKARNANDAGIRI/SRI) — the main framework
