/*
 * DataFileRoot.h
 *
 *  Created on: Mar 5, 2014
 *      Author: lacasta
 */

#ifndef DATAFILEROOT_H_
#define DATAFILEROOT_H_

#include <vector>
#include <Data.h>
#include <Hit.h>
#include <ChanList.h>
#include <ctime>
#include <TH1.h>
#include <TH2.h>

/**
 * class DataFileRoot
 *
 * This class defines the interface to read the Alibava data from file
 * and also defines a number of tools to help analyzing the data
 */
class DataFileRoot
{
    public:
        enum ScanType { UnknownScan, Charge, Time, LaserScan };
        enum RunType
        {
            UnknownRun=0,
            Calibration=1,
            LaserSync,
            Laser,
            RadSource,
            Pedestal,
            ChargeScan,
            LastRType
        };

    protected: // This is ugly but comfortable
        static const int max_nchan=256;

        RunType _type;
        time_t _t0;
        int _nchips;
        int _chip_mask;
        int _firmware;
        ScanType _scantype;
        int _npoints;
        int _from;
        int _to;
        int _step;
        int _nevts;
        int _nchan; // current number of channels
        double _seedcut;
        double _neighcut;
        unsigned short _header[2][16];
        double _ped[max_nchan];
        double _noise[max_nchan];
        double _signal[max_nchan];
        double _sn[max_nchan];
        double _cmmd[2];
        double _cnoise[2];
        double _gain[max_nchan];
        double _average_gain;
        bool   _mask[max_nchan];
        int     _version;
        int     _polarity;
        double _t1, _t2;
        HitList _hits;
        std::vector<ChanList> chan_list;
        EventDataBlock _data;

    protected:
        void reset_data();

    public:
        void set_data(int i, unsigned short x) { _data.data[i] = x; }

    public:
        DataFileRoot(const char *nam=0, const char *pedfile=0, const char *gainfile=0);
        virtual ~DataFileRoot();

        static DataFileRoot *OpenFile(const char *nam=0, const char *pedfile=0, const char *gainfile=0);

        /**
         * These are the file operation methods that need to be
         * implemented
         */
        /// Tells if the data stream is valid.
        virtual bool valid() const =0;

        /**
         * Opens a new file.
         * \param name the path of the file to open
         *
         * The success of the operation can be checked with valid
         */
        virtual void open(const char *name)=0;

        /// Closes the file.
        virtual void close()=0;

        /// Moves the read pointer to the beginning of the file
        virtual void rewind()=0;

        /// Save the current state of the file
        virtual void save() {};

        /// Restore the last saved state of the file
        virtual void restore() { rewind(); }
        /**
         * Reads all the information about a given event.
         *
         * \returns 0 in case of success.
         */
        virtual int read_event()=0;

        /**
         *  Reads only the ASIC data of a given event
         *  \returns 0 in case of success
         */
        virtual int read_data()=0;

        virtual void check_point(int, const char *) {};
        virtual void new_file(int, const char *) {}
        virtual void start_of_run(int, const char *) {}
        virtual void end_of_run(int, const char *) {}
        virtual void new_data_block(int, const char *) {};

        // The data format version
        int version() const { return _version; }


        int polarity() const { return _polarity; }
        void polarity(int x) { _polarity = ( x<0 ? -1 : 1); }
        /*
         * Sets the number of channels and the data in the case
         * of non "standard" values. If data==0, then only the number
         * of channels is changed
         */
        void set_data(int nchan, const unsigned short *data=0);
        int nchan() const { return _nchan; }
        int type() const
        {
            return _type;
        }
        char *date() const
        {
            return ctime(&_t0);
        }
        double ped(int i) const
        {
            return _ped[i]/_gain[i];
        }
        double noise(int i) const
        {
            return _noise[i]/_gain[i];
        }
        double signal(int i) const
        {
            return _signal[i]/_gain[i];
        }

        double sn(int i) const
        {
            return _sn[i];
        }

        double get_cmmd(int i) const
        {
            return _cmmd[i];
        }

        double get_cnoise(int i) const
        {
            return _cnoise[i];
        }

        unsigned short data(int i) const
        {
            return _data.data[i];
        }
        double value() const
        {
            return _data.value;
        }
        virtual unsigned int clock_counter() const;
        virtual double time() const;
        virtual double temp() const;
        int scan_type() const { return _scantype; }
        int npts() const
        {
            return _npoints;
        }
        int from() const
        {
            return _from;
        }
        int to() const
        {
            return _to;
        }
        int step() const
        {
            return _step;
        }
        int nevts() const
        {
            return _step;
        }

        void add_hit(const Hit &h)
        {
            _hits.push_back(h);
        }
        HitList::iterator begin()
        {
            return _hits.begin();
        }
        HitList::iterator end()
        {
            return _hits.end();
        }
        int nhits() const
        {
            return _hits.size();
        }
        bool empty() const
        {
            return _hits.empty();
        }
        const Hit &hit(int i) const
        {
            return _hits[i];
        }
        void set_hit_list(const HitList &L) { _hits = L; }
        void clear()
        {
            _hits.clear();
        }

        double get_gain(int i) const
        {
            return _gain[i];
        }
        double gain() const
        {
            return _average_gain;
        }

        double seed_cut() const
        {
            return _seedcut;
        }
        double neigh_cut() const
        {
            return _neighcut;
        }
        void set_cuts(double s, double n)
        {
            _seedcut = s;
            _neighcut = n;
        }
        void set_timecut(double t1, double t2);
        bool valid_time(double time) const;

        unsigned short get_header(int ichip, int ibit) { return _header[ichip][ibit]; }

        TH1 *show_pedestals();
        TH1 *show_noise();
        virtual TH2 *compute_pedestals(int mxevts=-1, bool do_cmmd=true);
        virtual void compute_pedestals_fast(int mxevts = -1, double ped_weight=0.01, double noise_weight=0.001);


        void process_event(bool do_cmmd=true);
        void find_clusters(int ichip=-1);
        void find_clusters(ChanList &C);
        void save_pedestals(const char *fnam);
        void load_pedestals(const char *fnam, bool show=false);
        void load_gain(const char *fnam);
        void load_masking(const char *fnam);
        void spy_data(bool with_signal=false, double t0=0.0, double t1=0.0, int nevt=1);
        void common_mode();
        void common_mode(ChanList &C, bool correct=false);

        int  n_channel_list() const { return chan_list.size(); }
        void add_channel_list(const ChanList &C);
        void clear_channel_lists() { chan_list.clear(); }
        ChanList get_channel_list(int i) const { return chan_list[i]; }

        int chip_mask() const
        {
            return _chip_mask;
        }

        int firmware() const
        {
            return _firmware;
        }

        int nchips() const
        {
            return _nchips;
        }
};

// Return true if file is an ASCII text file
bool is_text(const char *);

#endif /* DATAFILEROOT_H_ */
