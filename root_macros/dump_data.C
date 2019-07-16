/*
 * dump_data.C
 *
 *  Created on: Sep 1, 2009
 *      Author: lacasta
 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstdlib>
#include "Data.h"
#include "utils.h"

double tdc_time(unsigned int time)
{
    unsigned short fpart = time & 0xffff;
    short ipart = (time & 0xffff0000)>>16;
    if (ipart<0)
        fpart *= -1;
    //double tt = 100.*(1. -(ipart + (fpart/65535.)));
    double tt = 100.0*(ipart + (fpart/65535.));
    return tt;
}

double get_temperature(unsigned short temp)
{
    if (temp==0)
        return 9999.;
    else
        return 0.12*temp - 39.8;
}

void dump_data(const char *fnam=0)
{
    double ped[256], noise[256];
    unsigned short beetle_header[2][16];
    EventData data;
    if (!fnam)
    {
        std::cout << "I need an input file" << std::endl;
        return;
    }
    std::ifstream ifile(fnam);
    if (!ifile)
    {
        std::cout << "Could not open " << fnam << std::endl;
        return;
    }

    /**
     * The header
     */
    time_t t0;
    int  type;
    bool new_version = false;
    std::string header;
    unsigned int ic, lheader;
    char c;
    ifile.read((char *)&t0, sizeof(time_t));
    ifile.read((char *)&type, sizeof(int));
    if (type>15)
    {
        ifile.seekg(0, std::ios::beg);
        ifile.read((char *)&t0, sizeof(int));
        ifile.read((char *)&type, sizeof(int));
    }
    ifile.read((char *)&lheader, sizeof(unsigned int));
    for (ic=0; ic<80; ic++)
    {
        ifile.read(&c, sizeof(char));
        header.append(1, c);
    }
    header = trim_str(header);
    std::cout << "HEADER:\n[" << header << "]" << std::endl;
    std::cout << "type: " << type << std::endl;

    if (header[0]!='V' && header[0]!='v')
    {
        new_version = false;
    }
    else
    {
        new_version = true;
        header = header.substr(5);
    }

    if (new_version)
        std::cout << "# This is a new version data file" << std::endl;


    /**
     * Read pedestals and noise
     */
    ifile.read((char *)ped, 256*sizeof(double));
    ifile.read((char *)noise, 256*sizeof(double));

    std::cout << "************************************\n"
              << "* Pedestals and noise              *\n"
              << "************************************\n"
              ;
    for (ic=0; ic<256; ic++)
    {
        std::cout << ped[ic] << " | " << noise[ic] << std::endl;
    }

    /**
     * Now read the data
     */
    bool quit_loop=false;
    int ievt;
    int iheader;
    unsigned int size = sizeof(EventData);
    char *block_data = 0;
    for ( iheader=0, ievt=0; 1; ievt++ )
    {
        if (new_version)
        {
            int itry;
            unsigned int code;
            quit_loop = false;

            for (itry=0; itry>=0; itry++)
            {
                ifile.read((char *)&iheader, sizeof(unsigned int));
                if (ifile.bad() || ifile.eof())
                {
                    quit_loop = true;
                    break;
                }
                code = (iheader>>16) & 0xFFFF;
                if ( code == 0xcafe )
                    break;
            }
            if (itry>0)
            {
                std::cout << "Some data was corrupted....: itry=" << itry << std::endl;
            }
            ifile.read((char *)&size, sizeof(unsigned int));
            if (ifile.bad() || ifile.eof())
            {
                quit_loop = true;
                break;
            }
            if (size==0)
            {
                std::cout << "Invalid data: try od -x4 " << fnam << std::endl;
                quit_loop = true;
                break;
            }
        }
        if (quit_loop)
            break;

        std::cout << "*******************************\n"
                  << "* Event " << ievt
                  << " code " << std::hex << iheader << std::dec << std::endl
                  << "* size : " << size << std::endl
                  << "*******************************\n";



        int block_type = iheader & 0x0fff;
        int user_data = iheader & 0x1000;
        block_data = new char[size];

        if ( block_type != 2)
        {
            ifile.read((char *)&block_data, size);
            if (ifile.bad() || ifile.eof())
                break;
        }
        switch (block_type)
        {
            case 0:
                std::cout << "*** New file block " << std::endl;
                break;
            case 1:
                std::cout << "*** Start of Run block " << std::endl;
                break;
            case 2:
                if (user_data)
                {
                    std::cout << "*** User defined data block" << std::endl;
                    ifile.read((char *)&block_data, size);
                    if (ifile.bad() || ifile.eof())
                    {
                        ifile.close();
                        exit(1);
                    }
                }
                else
                {
                    if (new_version)
                    {
                        ifile.read((char *)&data.value, sizeof(double));
                        ifile.read((char *)&data.time, sizeof(unsigned int));
                        ifile.read((char *)&data.temp, sizeof(unsigned short));
                        for (int ii=0; ii<2; ii++)
                        {
                            ifile.read((char *)&beetle_header[ii], 16*sizeof(unsigned short));
                            ifile.read((char *)&data.data[ii*128], 128*sizeof(unsigned short));
                        }
                    }
                    else
                    {
                        ifile.read((char *)&data, sizeof(EventData));
                        for (int ii=0; ii<2; ii++)
                        {
                            memset(beetle_header[ii], 0, 16*sizeof(unsigned short));
                        }
                    }
                    std::cout << "Value: " << data.value << std::endl;
                    std::cout << "TDC time: " << tdc_time(data.time) << std::endl;
                    std::cout << "temperature: " << get_temperature(data.temp) << std::endl;
                    std::cout << "Chip Headers" << std::endl;
                    for (ic=0; ic<2;ic++)
                    {
                        for (int jc=0; jc<16; jc++)
                            std::cout << std::setw(6) << beetle_header[ic][jc];

                        std::cout << std::endl;
                    }
                    std::cout << "Data;" << std::endl;
                    for (ic=0;ic<256; ic+=8)
                    {
                        int jc;
                        for (jc=0; jc<8; jc++)
                            std::cout << std::setw(6) << data.data[ic+jc];
                        std::cout << std::endl;
                    }
                }
                break;
            case 3:
                std::cout << "*** Check Point block " << std::endl;
                break;
            case 4:
                std::cout << "*** End of Run block " << std::endl;
                break;
            default:
                std::cout << "*** Invalid data block type. " << block_type << std::endl;
                break;
        }
        delete [] block_data;
    }


    ifile.close();
}

#ifndef __CINT__
int main(int argc, char **argv)
{
    dump_data(argv[1]);
}

#endif
