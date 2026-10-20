# ngen Modifications: Immediate Output Flushing for Coupled Simulations

## Next Gen Water Modeling Framework Prototype
To know more about ngen, please click this link: https://github.com/CIROH-UA/ngen/blob/ngiab/README.md

## Overview

Modified ngen to ensure immediate flushing of output data to disk by using `std::endl` instead of `'\n'`. This is critical for coupled simulations where T-Route reads catchment output files in real-time.


## Why This Change?

Without immediate flushing, output data remains in memory buffers, causing T-Route to read incomplete or stale data during coupled simulations. Using `std::endl` ensures both newline insertion and immediate buffer flush.

## Modified Files

### 1. `/ngen/include/core/catchment/HY_CatchmentArea.hpp`

**Updated Function**:
```cpp
void write_output(const std::string& out) {
    output << out << std::endl;  // std::endl adds newline AND flushes
}
```
- Changed from pass-by-value to pass-by-reference for performance
- Added `std::endl` to flush output immediately

### 2. `/ngen/include/core/Layer.hpp`

**Change**: Removed `"\n"` from output string construction (flushing now handled by `write_output`)
```cpp
std::string output = std::to_string(output_time_index) + "," + current_timestamp + "," +
                     r_c->get_output_line_for_timestep(output_time_index);  // No "\n"
r_c->write_output(output);
```

### 3. `/ngen/include/core/DomainLayer.hpp`

**Changes**: Removed `"\n"` from both header and timestep output strings
```cpp
// Header
formulation->write_output("Time Step," "Time," + formulation->get_output_header_line(","));

// Timestep data
std::string output = std::to_string(output_time_index) + "," + current_timestamp + "," +
                     formulation->get_output_line_for_timestep(output_time_index);
formulation->write_output(output);
```

### 4. `/ngen/src/core/HY_Features.cpp`

**Change**: Removed `"\n"` from header output
```cpp
formulation->write_output("Time Step," "Time," + formulation->get_output_header_line(","));
```

## Key Points

- **`std::endl` vs `'\n'`**: `std::endl` both adds a newline AND flushes the buffer; `'\n'` only adds a newline
- **Benefit**: T-Route can read catchment output files in real-time during coupled simulations

## Integration with T-Route

1. ngen hydrological models generate catchment output files (`cat-<id>.csv`)
2. Each output line is immediately flushed to disk using `std::endl`
3. T-Route reads these files in real-time for routing calculations

See T-Route readme.md for details on catchment file support.


## Author
**Md Saiduzzaman**  
Graduate Research Assistant, Department of Civil, Architectural and Environmental Engineering  
Missouri University of Science and Technology, Rolla, MO, USA   
Email: [msmg8@mst.edu](mailto:msmg8@mst.edu)

**Dr. Bong-Chul Seo**  
Assistant Professor, Department of Civil, Architectural and Environmental Engineering  
Missouri University of Science and Technology, Rolla, MO, USA  
Contact: [bongchul.seo@mst.edu](mailto:bongchul.seo@mst.edu)

