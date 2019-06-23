#include "stdafx.h"
#include "clara.hpp"

#include "ibc.h"


int main(int argc, char **argv) {
    std::string inputName;
    std::string outputName;
    bool detailed = false;
    bool printVersion = false;
    bool stopOnFirstError = false;
    //bool printHelp = true;
    //std::string outputLang;

    const auto cli
        = clara::Opt(inputName, "input")
            ["-i"]["--input"]
            ("Specifies input IndexBuffers schema file that has .ibs format")
        | clara::Opt(outputName, "input")
            ["-o"]["--output"]
            ("Specifies output generated C++ header")
        | clara::Opt(detailed)
            ["-d"]["--detailed"]
            ("Print parsed info")
        | clara::Opt(printVersion)
            ["-v"]["--version"]
            ("Print IndexBuffers compiler's version")
        | clara::Opt(stopOnFirstError)
            ["--stop_on_first_error"]
            ("Stop compiler on first error");
        //| clara::Help(printHelp);
    const auto res = cli.parse(clara::Args(argc, argv));
    if (!res) {
        std::cout << res.errorMessage() << std::endl;
        return 0;
    }
    if (printVersion) {
        std::cout << "IndexBuffers Compiler v0.3-alpha" << std::endl;
    }
    if (inputName.empty()) {
        std::cout << "The argument -i is required!" << std::endl;
        return 0;
    }
    const auto dotPos = inputName.rfind('.');
    if (dotPos == std::string::npos || inputName.substr(dotPos) != ".ibs") {
        std::cout << "Wrong input schema file format" << std::endl;
        return 0;
    }
    if (outputName.empty()) {
        outputName = inputName.substr(0, dotPos);
        outputName += "_generated.h";
    }
    INBCompiler compiler;
    compiler.setStopOnFirstError(stopOnFirstError);
    compiler.read(inputName, detailed);
    compiler.write(outputName, Language::CPP);
    return 0;
}
