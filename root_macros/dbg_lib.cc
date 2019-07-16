/*
 * dbg_lib.cc
 *
 *  Created on: Nov 19, 2010
 *      Author: lacasta
 */

#include <iostream>
#include "HDFRoot.h"
int main(int argc, char **argv)
{
    HDFRoot *A = new HDFRoot("data.h5");
    if (!A->valid())
    {
        std::cout << "Could not open file" << std::endl;
    }

    while (!A->read_event())
    {
        A->process_event(false);
    }

    delete A;
    return 0;
}
