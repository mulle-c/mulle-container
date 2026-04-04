### 10.1.1








* assoc comparisons now honor an optional value-compare callback when checking equality between two assocs; falls back to pointer equality otherwise
* reduce exported/internal symbol surface (make map release and pointerset get helpers static) — **BREAKING** for code relying on those internal symbols
* rangemap diagnostics print pointer values as hex with proper `uintptr_t` casts for portability
* remove obsolete pointerqueue enumerator declaration
