#include "cube.h"

int cube(char *inputFile, char *queryFile, char *outputFile, cubeConstants &cubeCon)
{
    int k = cubeCon.k;
    int M = cubeCon.M;
    int numberOfNN = 5;
    int probes = cubeCon.probes;
    int rad;
    int w = cubeCon.w;

    int numOfLines;
    int numOfElements;
    int numOfDimensions;

    // for query file
    int numOfLinesQ;
    int numOfElementsQ;
    int numOfDimensionsQ;

    fstream inputFilePtr;
    fstream queryFilePtr;
    fstream outputFilePtr;

    vector<point> arrayOfPoints;

    int res = parsInit(inputFile, arrayOfPoints, &numOfLines, &numOfElements);

    numOfDimensions = numOfElements - 1;
    
    if (res == 1)
    {
        cout << "Error when parsing the file\n";
        return 1;
    }

    if (arrayOfPoints.empty())
    {
        cout << "Array of points is empty\n";
        return 1;
    }
    // now lets start the insertion...

    hashTableCube *hashT;
    hashT = new hashTableCube(myPow(2, k), &cubeCon, numOfDimensions);

    for (int i = 0; i < numOfLines; i++)
        hashT->insert(&arrayOfPoints[i]);


    vector<point> arrayOfQueryPoints;
    res = parsInit(inputFile, arrayOfQueryPoints, &numOfLinesQ, &numOfElementsQ);

    numOfDimensionsQ = numOfElementsQ - 1;
    if (res == 1)
    {
        return 1;
    }

    if (arrayOfQueryPoints.empty())
    {
        return 1;
    }

    vector<kNearest> list;
    vector<kNearest> listTrue;
    vector<kNearest> listR;

    list.resize(numOfLinesQ);
    listTrue.resize(numOfLinesQ);
    listR.resize(numOfLinesQ);

    ofstream outputFileStream;

    outputFileStream.open(outputFile, std::ios_base::app);
    // we have initialized an array of objects-points from query file...
    // now let's find their nearest neighbors...
    for (int i = 0; i < numOfLinesQ; i++)
    {
        initKNearest(numberOfNN, &(list[i]));
        initKNearest(numberOfNN, &(listTrue[i]));
        initKNearest(0, &(listR[i]));

        std::chrono::steady_clock::time_point beginHyper = std::chrono::steady_clock::now();
        hashT->findKNeighbors(&(arrayOfQueryPoints[i]), &list[i], probes, M);
        std::chrono::steady_clock::time_point endHyper = std::chrono::steady_clock::now();

        std::chrono::steady_clock::time_point beginTrue = std::chrono::steady_clock::now();
        hashT->findKNeighborsTrue(&(arrayOfQueryPoints[i]), &listTrue[i], probes, M);
        std::chrono::steady_clock::time_point endTrue = std::chrono::steady_clock::now();

        hashT->findNeighborsR(&(arrayOfQueryPoints[i]), &listR[i], rad, probes, M);

        outputFileStream << "Query: " << arrayOfQueryPoints[i].getKey() << endl;
        int tempId = -1;
        for (int z = 0; z < list[i].size; z++)
        {
            int flag = 0;
            if (list[i].dist[z] != MAXFLOAT)
            {
                // we have a neighbor
                outputFileStream << "Nearest neighbor-" << z + 1 << ": " << list[i].nearestPoints[z]->getKey() << endl;
                outputFileStream << "   distanceCube: " << list[i].dist[z] << endl;
                if (list[i].dist[z] != MAXFLOAT)
                {
                    flag = 1;
                    // we have a neighbor
                    outputFileStream << "   distanceTrue: " << listTrue[i].dist[z] << endl;
                }
                outputFileStream << "   tHYPER: " << list[i].vecOfTimes[z].count() << " ms" << endl;
                if (flag)
                    outputFileStream << "   tTrue: " << listTrue[i].vecOfTimes[z].count() << " ms" << endl << endl;
            }
        }

        outputFileStream << "R-nearest neigbors: " << endl;
        for (int w = 0; w < listR[i].size; w++)
        {
            outputFileStream << "   item_id: " << listR[i].nearestPoints[w]->getKey() << endl;
        }
        outputFileStream << "\n////////////////////////////////////////////////////////////\n";
    }
    outputFileStream.close();
    return 0;
}