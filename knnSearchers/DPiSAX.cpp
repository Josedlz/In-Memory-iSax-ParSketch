#include <mpi.h>
#include "DPiSAX.h"


std::vector<char> serializeTimeSeries(const std::vector<float> values) {
    std::vector<char> serializedData;

    size_t size = values.size();
    serializedData.resize(sizeof(size_t) + size * sizeof(float));
    std::memcpy(serializedData.data(), &size, sizeof(size_t));

    std::memcpy(serializedData.data() + sizeof(size_t), values.data(), size * sizeof(float));

    return serializedData;
}

void broadcastSerializedQuery(std::vector<char>& serializedQuery, int rank, int size) {
    MPI_Bcast(serializedQuery.data(), serializedQuery.size(), MPI_CHAR, 0, MPI_COMM_WORLD);
}

std::vector<float> deserializeTimeSeries(std::vector<char> serializedQuery) {
    size_t size;
    std::memcpy(&size, serializedQuery.data(), sizeof(size_t));

    std::vector<float> values(size);
    std::memcpy(values.data(), serializedQuery.data() + sizeof(size_t), size * sizeof(float));

    return values;
}

std::vector<TimeSeries> gatherResults(std::vector<TimeSeries> localResults, int rank, int size) {
    std::vector<TimeSeries> globalResults;

    if (rank == 0) {
        globalResults.resize(size);
    }

    MPI_Gather(localResults.data(), localResults.size() * sizeof(TimeSeries), MPI_CHAR, globalResults.data(), localResults.size() * sizeof(TimeSeries), MPI_CHAR, 0, MPI_COMM_WORLD);

    return globalResults;
}

std::vector<TimeSeries> combineResults(const TimeSeries& q, std::vector<TimeSeries> globalResults, int k) {
    std::vector<TimeSeries> combinedResults;

    for (auto& ts : globalResults) {
        combinedResults.push_back(ts);
    }

    //I sort them according to their distance to q
    std::sort(combinedResults.begin(), combinedResults.end(), [&q](const TimeSeries& a, const TimeSeries& b) {
        return q.euclideanDist(a) < q.euclideanDist(b);
    });

    combinedResults.resize(k);

    return combinedResults;
}

DPiSAX::DPiSAX(std::vector<std::string> filePaths, int k) {
    // Initialize MPI
    this->filePaths = filePaths;
    this->k = k;

    MPI_Init(nullptr, nullptr);
}

DPiSAX::~DPiSAX() {
    MPI_Finalize();
}

std::vector<TimeSeries> DPiSAX::parallelSearch(TimeSeries q, int k) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < filePaths.size()) {
        std::cerr << "Not enough processes to run the search" << std::endl;
        return std::vector<TimeSeries>();
    }

    iSAXSearcher searcher(filePaths[rank]);

    //I need to initialize the searcher, but each initialization reads from disk, so I need to block the other processes
    MPI_Barrier(MPI_COMM_WORLD);

    searcher.initialize();

    MPI_Barrier(MPI_COMM_WORLD);

    std::vector<char> serializedQuery = serializeTimeSeries(q.getValues());

    broadcastSerializedQuery(serializedQuery, rank, size);

    std::vector<float> deserializedQuery = deserializeTimeSeries(serializedQuery);

    auto deserializedTimeSeries = TimeSeries(deserializedQuery);

    std::vector<TimeSeries> localResults = searcher.search(deserializedTimeSeries, k);

    std::vector<TimeSeries> globalResults = gatherResults(localResults, rank, size);

    return combineResults(q, globalResults, k);
}