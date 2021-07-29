#include "autoPlayground.hpp"
#include "window.hpp"


AutoPlaygroundPanel::AutoPlaygroundPanel(QWidget* parent) : QWidget(parent) {
    file = "helloWorld.py";
    filepath = absolutePath();//"../../src/data/";
    size_t pos = filepath.find_last_of("/", filepath.length()-1);
    pos = filepath.find_last_of("/", pos-1);
    pos = filepath.find_last_of("/", pos-1);
    filepath = filepath.substr(0, pos) + "/src/data/";
    
    wfilepath = std::wstring(filepath.begin(), filepath.end());
    text << "Autoplaygroun initialized.";
    text << "Place script inside \"" + filepath + "\"\n";
    
    const wchar_t* program = L"Embedded_python";
    Py_SetProgramName(program);
    
    //argv = new wchar_t*[1];
    //argv[0] = new wchar_t[wfilepath.length()+1];
    
    //for (int i = 0; i < wfilepath.length(); i++)
    //    argv[0][i] = wfilepath[i];
    //argv[0][ wfilepath.length()] = L'\0';
    
    //myProcess = new QProcess(this);
    //myProcess->setProcessChannelMode( QProcess::ForwardedChannels );

    //connect(myProcess, &QProcess::readyReadStandardOutput, this, [this](){
    //    text << myProcess->readAllStandardOutput().toStdString();
    //    QProcess *process = qobject_cast<QProcess*>(sender());
    //    text << process->readAllStandardOutput().toStdString();
    //});
    //connect(myProcess, &QProcess::readyReadStandardError, this, [this](){
    //    text << myProcess->readAllStandardOutput().toStdString();
    //    QProcess *process = qobject_cast<QProcess*>(sender());
    //    text << process->readAllStandardOutput().toStdString();
    //});
    Py_Initialize();
    runScript();
    
}

AutoPlaygroundPanel::~AutoPlaygroundPanel(){
    if (Py_FinalizeEx() < 0)
        text << "Fatal python error\n";
    //delete[] argv[0];
    //delete argv;
}

void AutoPlaygroundPanel::run(){
    if( !Py_IsInitialized() ) {
        text << "Unable to initialize Python interpreter.\n";
        return;
    }
    
    script = fopen((filepath + file).c_str(), "r");
    if (script){
        text << "Running script file\n";
        
        size_t length;
        wchar_t** argv = new wchar_t*[2];
        length = strlen((filepath + file).c_str());
        argv[0] = Py_DecodeLocale((filepath + file).c_str(), &length);
        length = strlen((filepath + "XBTZAR.csv").c_str());
        argv[1] = Py_DecodeLocale((filepath + "XBTZAR.csv").c_str(), &length);
        PySys_SetArgv(2, argv);
        
        PyRun_SimpleFileEx(script, (filepath + file).c_str(), 0);
        fclose(script);

        text << "Script COMPLETED\n";
    }
    else{
        text << "Error: Couldn't run script\n";
    }
}

void AutoPlaygroundPanel::runScript(){
    thread = new std::thread([this]{
        //run(); //crashes when not running on a separate process
        std::system(("python3 " + filepath + "helloWorld.py " + filepath + "XBTZAR.csv").c_str());
    });
    thread->detach();
    //QString program = "python3";
    //QStringList arguments;
    //arguments << (filepath + "helloWorld.py").c_str()
      //          << (filepath + "XBTZAR.csv").c_str();

    //myProcess->start(program, arguments);
}
