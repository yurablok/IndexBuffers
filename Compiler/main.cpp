#include "stdafx.h"
#include "clara.hpp"

#include "inbcompiler.h"


int main(int argc, char **argv)
{
    std::string inputName;
    std::string outputName;
    bool detailed = false;
    std::string outputLang;

    //bool help = true;
    const auto cli
        = clara::Opt(inputName, "input")
            ["-i"]["--input"]
            ("Specifies input IndexBuffers schema file that has .ibs format")
        | clara::Opt(outputName, "input")
            ["-o"]["--output"]
            ("Specifies output generated C++ header")
        | clara::Opt(detailed, "detailed")
            ["-d"]["--detailed"]
            ("Print parsed info");
        //| clara::Help(help);
    const auto res = cli.parse(clara::Args(argc, argv));
    if (!res)
    {
        std::cout << res.errorMessage() << std::endl;
        return 0;
    }
    if (inputName.empty())
    {
        //std::cout << "The argument -i is required!" << std::endl;
        //return 0;
        inputName = "schema.ibs";
    }
    const auto dotPos = inputName.rfind('.');
    if (dotPos == std::string::npos
        || inputName.substr(dotPos) != ".ibs")
    {
        std::cout << "Wrong input schema file format" << std::endl;
        return 0;
    }
    if (outputName.empty())
    {
        outputName = inputName.substr(0, dotPos);
        outputName += "_generated.h";
    }
    INBCompiler compiler;
    compiler.read(inputName);
    compiler.write(outputName);
    return 0;
}
