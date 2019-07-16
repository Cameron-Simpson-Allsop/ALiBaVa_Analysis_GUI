#include <iostream>
#include <unistd.h>
#include <TH1.h>
#include <TH2.h>
#include <TROOT.h>
#include <TApplication.h>
#include "DataFileRoot.h"
#include "analysis.h"
#include "Tracer.h"
#include "sin_preguntas.h"



//class MyDataFile: public AsciiRootData
//{
//    public:
//        MyDataFile(const char *nam, const char *pedfile=0, const char *gainfile=0) :
//            AsciiRoot(nam, pedfile, gainfile)
//            {}
//        void check_point(int size, const char *);
//        void new_file(int size, const char *);
//        void start_of_run(int size, const char *);
//        void end_of_run(int size, const char *);
//};
//void MyDataFile::check_point(int size, const char *s)
//{
//    std::string txt(s, size);
//    std::cout << "check_point: " << txt << std::endl;
//}
//void MyDataFile::new_file(int size, const char *s)
//{
//    std::string txt(s, size);
//    std::cout << "new_file: " << txt << std::endl;
//}
//void MyDataFile::start_of_run(int size, const char *s)
//{
//    std::string txt(s, size);
//    std::cout << "start_of_run: " << txt << std::endl;
//}
//void MyDataFile::end_of_run(int size, const char *s)
//{
//    std::string txt(s, size);
//    std::cout << "end_of_run: " << txt << std::endl;
//}

int main(int argc, char **argv) {
    char line[1024];

    getcwd(line, 1024);
    std::cout << "CWD: " << line << std::endl;
    const char *inp_file = argv[1];
    if (!inp_file)
        inp_file = "/Users/lacasta/workspace/alibava/charge-scan.h5";

    set_debug(true);
    TApplication *app = new TApplication("theApp",&argc, argv);
    DataFileRoot *A = DataFileRoot::OpenFile(inp_file);

//    A->add_channel_list( ChanList("60-127"));
    sin_preguntas(A, 0, 0, 0,1,true,15, 25);
//    DataFileRoot *A = new MyDataFile(inp_file,0, 0);
//    int ievt;
//    for (ievt=0; A->read_event()==0 ; ievt++)
//    {
//        A->process_event();
//    }
//    std::cout << "Done...." << std::endl;
//    A->rewind();
//    sin_preguntas(A, inp_file, 0, 0, -1, true);
    delete A;
    delete app;
}
