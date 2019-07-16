/*
 * sin_preguntas.h
 *
 *  Created on: May 26, 2010
 *      Author: lacasta
 */

#ifndef SIN_PREGUNTAS_H_
#define SIN_PREGUNTAS_H_

#include <TGraphErrors.h>
#include <TCanvas.h>
#include <DataFileRoot.h>

double get_volt(const char *fnam);
TGraphErrors *scan_laser(const char *detector, const char *cal_file=0,
                         const char *ped_file=0, int polarity=0, bool dofit=true,
                         int mxevts=-1);
void graph2csv(TGraphErrors *gr, const char *ofilename=0);
int printCanvas(TCanvas *cnvs, const char *out, int res=300);
void sin_preguntas(DataFileRoot *A,
                   const char *data_file, const char *cal_file=0, const char *ped_file=0,
                   int polarity=0, bool dofit=true,
                   int peaktimesliceinitial=5, int peaktimeslicefinal=15);


#endif /* SIN_PREGUNTAS_H_ */
