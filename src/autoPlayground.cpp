#include "autoPlayground.hpp"

AutoPlayground::AutoPlayground() {
    const wchar_t* program = L"Embedded_python";
    Py_SetProgramName(program);
    Py_Initialize();
    FILE* script = fopen("/Users/macgod/Dev/embedPy/helloWorld.py", "r");
    if (script){
        PyRun_SimpleFile(script, "/Users/macgod/Dev/embedPyhelloWorld.py");
        fclose(script);
    }
    else
        std::cout << "Fatal file error" << std::endl;
    if (Py_FinalizeEx() < 0)
        std::cout << "Fatal python error" << std::endl;
}
