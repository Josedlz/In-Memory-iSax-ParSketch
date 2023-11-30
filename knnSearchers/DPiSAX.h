#ifndef ISAX_H
#define ISAX_H

#include "knnSearcher.h"
#include <mpi.h>

class DPiSAX : public knnSearcher {
private:
    // Attributes for DPiSAX, such as partitioning information, SAX parameters, etc.

public:
    DPiSAX(std::string filename) : knnSearcher(filename) {
        // Initialize MPI
        MPI_Init(nullptr, nullptr);
        // Additional DPiSAX-specific setup, like determining dataset partitions
    }

    ~DPiSAX() {
        // Finalize MPI
        MPI_Finalize();
    }

    void DPiSAX::parallelIndexBuild() {
        // MPI logic for distributing data among nodes
        int rank, size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        // 1. Sample the dataset to determine partitioning strategy
        // 2. Broadcast partitioning information to all nodes
        // 3. Each node builds a local iSAX index for its partition
    }

    void DPiSAX::parallelSearch(TimeSeries q, int k) {
        int rank, size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        // MPI-related code for performing parallel search
        // 1. Broadcast the query to all nodes
        // 2. Each node performs a local search on its iSAX index
        // 3. Gather results from all nodes
        // 4. Aggregate results to find the k-nearest neighbors
    }

};


#endif