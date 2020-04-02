#ifndef dataStructures_h
#define dataStructures_h

#include <vector>
#include <opencv2/core.hpp>


struct DataFrame
{ // represents the available sensor information at the same time instance
    unsigned int imageIndex; // in a real camera streaming scenario, we should handle overflow at MAX_INT
    cv::Mat cameraImg; // camera image    
    std::vector<cv::KeyPoint> keypoints; // 2D keypoints within camera image
    cv::Mat descriptors; // keypoint descriptors
    std::vector<cv::DMatch> kptMatches; // keypoint matches between previous and current frame
    DataFrame() {};
    DataFrame(unsigned int imageIndex) : imageIndex(imageIndex) {};
};

struct DataFrameCircularBuffer
{ // a circular buffer implementation. Read the README for more details.
    size_t bufferSize = 2;
    DataFrame* dataFrameArray;
    unsigned int headIndex = 0, tailIndex = 0, numberOfItemsInBuffer = 0;

    DataFrameCircularBuffer(size_t bufferSize) : bufferSize(bufferSize) {
        dataFrameArray = new DataFrame[bufferSize];
    };
    // ~DataFrameCircularBuffer() {
    //     delete dataFrameArray;
    // }

    /**
    * @param (DataFrame&) dataFrameItem - a reference to the dataFrame to store in the buffer.
    * @return void
    */
    void writeToBuffer(DataFrame& dataFrameItem) {
        if (numberOfItemsInBuffer >= bufferSize) {
            throw std::string("DataFrameCircularBuffer: buffer full, will not add another item.");
        }
        dataFrameArray[headIndex] = dataFrameItem; // copy dataFrameItem into the array.
        headIndex++;
        if (headIndex >= bufferSize) headIndex = 0;
        numberOfItemsInBuffer++;
    };

    DataFrame* getDataFrameAtHead() {
        return &dataFrameArray[headIndex];
    }

    /**
    * @return DataFrame - a copy of the first dataFrame in the buffer.
    */
    DataFrame readFromBuffer() {
        if (numberOfItemsInBuffer == 0) {
            throw std::string("DataFrameCircularBuffer: buffer is empty, nothing to read.");
        }
        DataFrame* dataFrameToReturn = &dataFrameArray[tailIndex]; // point to the tail dataFrame.
        tailIndex++;
        if (tailIndex >= bufferSize) tailIndex = 0;
        numberOfItemsInBuffer--;
        return *dataFrameToReturn;
    };
};


#endif /* dataStructures_h */
