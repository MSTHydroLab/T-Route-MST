#ifndef HY_CATCHMENTAREA_H
#define HY_CATCHMENTAREA_H

#include "HY_CatchmentRealization.hpp"

#include "GM_Object.hpp"

#include "StreamHandler.hpp"
#include "FileStreamHandler.hpp"
#include "GenericDataProvider.hpp"
#include <mutex>


class HY_CatchmentArea : public HY_CatchmentRealization, public GM_Object
{
    public:

    HY_CatchmentArea();
    HY_CatchmentArea(utils::StreamHandler output_stream);
    //HY_CatchmentArea(forcing_params forcing_config, utils::StreamHandler output_stream); //TODO not sure I like this pattern
    void set_output_stream(std::string file_path){output = utils::FileStreamHandler(file_path.c_str());}
    
    //------------------------------------------------------------------------------------
    // Function: write_output
    // Purpose: Writes a single line of output and immediately flushes it.
    //          The use of std::endl ensures both a newline and a flush operation,
    //          which is critical when T-route relies on
    //          reading complete outputs during simulations.
    // Update  : Modified to use std::endl instead of '\n' to ensure immediate flushing.
    // Updated by: Md Saiduzzaman
    //------------------------------------------------------------------------------------

    // Atomic, line-based write with immediate flush (no header_once needed)
    // Atomic, line-based write with immediate flush (C++14-friendly)
    void write_output(const std::string& out) {
      static std::mutex m;                 // serialize writes to avoid interleaving
      std::lock_guard<std::mutex> lock(m);
      output << out << std::endl;          // newline + flush via manipulator
    }


    virtual ~HY_CatchmentArea();

    protected:

    polygon_t bounds;

    utils::StreamHandler output;

    private:
};

#endif // HY_CATCHMENTAREA_H
