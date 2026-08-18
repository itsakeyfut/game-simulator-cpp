## Summary

<!-- What this PR does and why, in 1–4 sentences. -->

## Changes

<!-- List of notable changes. For bug fixes, include root cause and fix.
     For new features, include what was added. For tests, list what was covered. -->

## Related Issues

<!-- Use the keyword that fits:
     Closes #N  — feature implementation or task completion
     Fixes #N   — bug fix
     Resolves #N — general resolution (discussion, refactor, etc.) -->

## Test Plan

- [ ] `cmake --preset debug` configures cleanly
- [ ] `cmake --build --preset debug` compiles without warnings (`GSIM_WARNINGS_AS_ERRORS=ON`)
- [ ] `ctest --preset debug` passes
- [ ] `clang-format --dry-run --Werror $(git ls-files '*.cpp' '*.hpp')` passes