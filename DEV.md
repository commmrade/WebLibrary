## Formatting
`find . -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} +`
## Tidying
```
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .
clang-tidy -p . src/**/*.cpp
```
