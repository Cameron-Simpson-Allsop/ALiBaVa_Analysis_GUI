#include <cstdio>
#include <cstring>
#include <ctime>


bool dump_header(const char *fnam)
{
    bool new_version=false;
    FILE *pf = fopen(fnam,"r");
    if (!pf)
    {
        printf("Could not open %s\n",fnam);
        return false;
    }
    time_t t0;
    int type;
    int lheader;
    char header[128];
    sprintf(header,"*****");
    fread(&t0,sizeof(time_t),1,pf);
    fread(&type, sizeof(int), 1, pf);
    fread(&lheader, sizeof(int),1, pf);
    fread(header, 80,1,pf);
    if (header[0]!='V' && header[0]!='v')
        new_version = false;
    else
        new_version = true;

    printf("time: %s type %d header: %s\n",ctime(&t0), type, header);
    fclose(pf);

    return new_version;
}


void fix_header(const char *fnam, int type, const char *hdr=0)
{
    const unsigned int lheader=80;
    char header[lheader];
    printf("Current header:\n");
    bool new_version = dump_header(fnam);

    FILE *pf = fopen(fnam,"r+");
    if (!pf)
    {
        printf("Could not open %s\n",fnam);
        return;
    }
    printf("seek rc=%d\n",fseek(pf, sizeof(time_t), SEEK_SET));
    printf("fwrite rc=%d\n",fwrite(&type,sizeof(int),1,pf));
    if (hdr)
    {
        printf("fwrite rc=%d\n",fwrite(&lheader,sizeof(int),1,pf));
        memset(header, 0, lheader);
        if (new_version)
        {
            int len = strlen("V1.0|");
            int mxlen = lheader - len;
            strcpy(header, "V1.0|");
            memcpy(header+len, hdr, mxlen);
        }
        else
            strcpy(header,hdr);

        printf("fwrite rc=%d\n",fwrite(hdr,lheader,1, pf));
    }
    fclose(pf);

    printf ("New header:\n");
    dump_header(fnam);
}
