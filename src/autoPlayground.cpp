#include "autoPlayground.hpp"
#include "window.hpp"

AutoPlayground::AutoPlayground(QTextEdit* output) : outputStream(output) {
    file = "helloWorld.py";
    filepath = absolutePath();//"../../src/data/";
    size_t pos = filepath.find_last_of("/", filepath.length()-1);
    pos = filepath.find_last_of("/", pos-1);
    pos = filepath.find_last_of("/", pos-1);
    filepath = filepath.substr(0, pos) + "/src/data/";
    
    wfilepath = std::wstring(filepath.begin(), filepath.end());
    *outputStream << filepath;
    
    const wchar_t* program = L"Embedded_python";
    Py_SetProgramName(program);
    
    argv = new wchar_t*[1];
    argv[0] = new wchar_t[wfilepath.length()+1];
    
    for (int i = 0; i < wfilepath.length(); i++)
        argv[0][i] = wfilepath[i];
    argv[0][ wfilepath.length()] = L'\0';
    
    Py_Initialize();
}

AutoPlayground::~AutoPlayground(){
    if (Py_FinalizeEx() < 0)
        *outputStream << "Fatal python error\n";
    delete[] argv[0];
    delete argv;
}

void AutoPlayground::runScript(){
    if( !Py_IsInitialized() ) {
        *outputStream << "Unable to initialize Python interpreter.\n";
        return;
    }
    PySys_SetArgv(1, argv);
    script = fopen((filepath + file).c_str(), "r");
    if (script){
        *outputStream << "Running script file\n";
        PyRun_SimpleFileEx(script, (filepath + file).c_str(), 1);
        fclose(script);
    }
    else{
        *outputStream << "Error: Couldn't run script\n";
    }
}
