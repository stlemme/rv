// LoopHint: 0, LaunchCode: fooA
extern "C" void
foo(float* A) {
  for (unsigned i = 0; i < 296298; ++i) {
    if (i < 8) {
      A[i] = 42.0;
    }
  }
}