/*
 * HDFRoot.h
 *
 *  Created on: Mar 5, 2014
 *      Author: lacasta
 */

#ifndef HDFROOT_H_
#define HDFROOT_H_

#include <DataFileRoot.h>

struct HDFRootPrivate;

class HDFRoot: public DataFileRoot
{
    private:
        HDFRootPrivate *priv;

        void next_scan_point();
    public:
        HDFRoot(const char *nam=0, const char *pedfile=0, const char *gainfile=0);
        virtual ~HDFRoot();

        bool valid() const;
        void open(const char *name);
        void close();
        void rewind();
        void save();
        void restore();
        int read_event();
        int read_data();

        unsigned int clock_counter() const;
        double time() const;
        double temp() const;
};

#endif /* HDFROOT_H_ */
