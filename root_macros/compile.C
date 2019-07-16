#include <TROOT.h>
#include <TSystem.h>

int SLload(char *lnam)
{
    int rc;
    if ( (rc=gSystem->Load(lnam)))
        cout << ":> " << lnam << " NOT loaded " << endl;
    else
        cout << ":> " << lnam << " loaded " << endl;

    return rc;
}
int LoadMacro(char *lnam)
{
    cout << ":> Loading " << lnam << endl;
    return  gROOT->LoadMacro(lnam);
}


void load_alibava_libs()
{
    if (LoadMacro("fix_header_C.so") )
            gSystem->Exit(1);

    if (LoadMacro("utils_cc.so") )
            gSystem->Exit(1);

    if (LoadMacro("Hit_cc.so") )
                gSystem->Exit(1);

    if (LoadMacro("Tracer_cc.so") )
        gSystem->Exit(1);

    if (LoadMacro("AsciiRoot_cc.so") )
        gSystem->Exit(1);

    if (LoadMacro("analysis_cc.so") )
        gSystem->Exit(1);
    else
    {
        init_landau();
    }

    if (LoadMacro("sin_preguntas_C.so") )
        gSystem->Exit(1);

    if (LoadMacro("dump_data_C.so"))
        gSystem->Exit(1);

}

void compile(bool quit=true)
{

    gSystem->SetFlagsOpt("-g");
    if (LoadMacro("fix_header.C+") )
            gSystem->Exit(1);

    if (LoadMacro("utils.cc+") )
            gSystem->Exit(1);

    if (LoadMacro("Hit.cc+") )
                gSystem->Exit(1);

    if (LoadMacro("Tracer.cc+") )
        gSystem->Exit(1);

    if (LoadMacro("AsciiRoot.cc+") )
        gSystem->Exit(1);

    if (LoadMacro("analysis.cc+") )
        gSystem->Exit(1);
    else
    {
        init_landau();
    }
    if (LoadMacro("sin_preguntas.C+") )
        gSystem->Exit(1);

    if (LoadMacro("dump_data.C+"))
        gSystem->Exit(1);

    if (quit)
        gSystem->Exit(0);
}
