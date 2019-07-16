//()

#define DYNPATH "/workspace/alibava/build/install-dir/lib"
#define INCPATH "/workspace/alibava/build/install-dir/include/alibava/root"

void SLload(const char *lnam)
{
   if ( gSystem->Load(lnam) )
      cout << ":> " << lnam << " NOT loaded " << endl;
   else
      cout << ":> " << lnam << " loaded " << endl;

}
void LoadMacro(const char *lnam)
{
   cout << ":> Loading " << lnam << endl;
   gROOT->LoadMacro(lnam);

}
void rootlogon()
{
    TString home = getenv("HOME");
   	TString ss = gSystem->GetDynamicPath();
    gSystem->SetDynamicPath(ss+":"+home+DYNPATH);
    gInterpreter->AddIncludePath(home+INCPATH);
    
    std::cout << "============================================================" << std::endl;
    SLload("libAlibavaRoot.so");
    LoadMacro("read_histogram.C+");
    LoadMacro("fit_trims.C+");
    std::cout << "============================================================" << std::endl;



    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1);
    init_landau();
}
