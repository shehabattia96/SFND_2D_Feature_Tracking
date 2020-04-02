// test for DataFrameCircularBuffer class
#include <assert.h>
#include <iostream>
#include "../src/dataStructures.h"

// Should throw error "DataFrameCircularBuffer: buffer is empty, nothing to read."
void test_readFromAnEmptyBuffer() {
    DataFrameCircularBuffer circularBuffer(2);
    try {
        DataFrame dataFrameThatShouldNotExist = circularBuffer.readFromBuffer();
    } catch (std::string err) {
        const std::string expectedError = "DataFrameCircularBuffer: buffer is empty, nothing to read.";
        std::cout << "actual error is " << err << std::endl;
        assert(expectedError.compare(err) == 0);
    }
    std::cout << "test_readFromAnEmptyBuffer test passed" << std::endl;
}

// Should throw error "DataFrameCircularBuffer: buffer full, will not add another item."
void test_writeToAFullBuffer() {
    DataFrameCircularBuffer circularBuffer(2);
    DataFrame dataFrame1 = DataFrame(0);
    DataFrame dataFrame2 = DataFrame(1);
    DataFrame dataFrame3 = DataFrame(2);
    bool addedTwoDataFrames = false;
    try {
        circularBuffer.writeToBuffer(dataFrame1);
        circularBuffer.writeToBuffer(dataFrame2);
        addedTwoDataFrames = true;
        circularBuffer.writeToBuffer(dataFrame3); // should throw exception here
        throw std::string("Failed test, an expected error should have been thrown.");
    } catch (std::string err) {
        const std::string expectedError = "DataFrameCircularBuffer: buffer full, will not add another item.";
        std::cout << "actual error is " << err << std::endl;
        assert(addedTwoDataFrames == true);
        assert(expectedError.compare(err) == 0);
    }
    std::cout << "test_writeToAFullBuffer test passed" << std::endl;
}

// Should write two frames, read the first one and verify its index is 0. Write a third frame, read the next index and verify its index is 1. Write a fourth frame, read the next index and verify its index is 2. Write a fifth frame, read the next index and verify its index is 3. Read the last index and verify its index is 4. Read the empty buffer and catch an exception.
void test_writeAndReadToBuffer() {
    DataFrameCircularBuffer circularBuffer(2);
    DataFrame dataFrame1 = DataFrame(0);
    DataFrame dataFrame2 = DataFrame(1);
    DataFrame dataFrame3 = DataFrame(2);
    DataFrame dataFrame4 = DataFrame(3);
    DataFrame dataFrame5 = DataFrame(4);
    
    // Should write two frames, read the first one and verify its index is 0.
    circularBuffer.writeToBuffer(dataFrame1);
    circularBuffer.writeToBuffer(dataFrame2);
    DataFrame dataFrame1_read = circularBuffer.readFromBuffer();
    assert(dataFrame1_read.imageIndex == 0); // make sure the first item in the buffer is the dataFrame with index 0

    // Write a third frame, read the next index and verify its index is 1.
    circularBuffer.writeToBuffer(dataFrame3);
    DataFrame dataFrame2_read = circularBuffer.readFromBuffer();
    assert(dataFrame2_read.imageIndex == 1); // make sure the first item in the buffer is the dataFrame with index 1

    // Write a fourth frame, read the next index and verify its index is 2.
    circularBuffer.writeToBuffer(dataFrame4);
    DataFrame dataFrame3_read = circularBuffer.readFromBuffer();
    assert(dataFrame3_read.imageIndex == 2); // make sure the first item in the buffer is the dataFrame with index 2

    // Write a fifth frame, read the next index and verify its index is 3.
    circularBuffer.writeToBuffer(dataFrame5);
    DataFrame dataFrame4_read = circularBuffer.readFromBuffer();
    assert(dataFrame4_read.imageIndex == 3); // make sure the first item in the buffer is the dataFrame with index 3

    // Read the last index and verify its index is 4.
    DataFrame dataFrame5_read = circularBuffer.readFromBuffer();
    assert(dataFrame5_read.imageIndex == 4); // make sure the first item in the buffer is the dataFrame with index 4
    
    // Read the empty buffer and catch an exception.
    try {
        DataFrame dataFrameThatShouldNotExist = circularBuffer.readFromBuffer();
    } catch (std::string err) {
        const std::string expectedError = "DataFrameCircularBuffer: buffer is empty, nothing to read.";
        std::cout << "actual error is " << err << std::endl;
        assert(expectedError.compare(err) == 0);
    }
    std::cout << "test_writeAndReadToBuffer test passed" << std::endl;
}

int main ()
{
    try {
    std::cout << "Starting test test_readFromAnEmptyBuffer." << std::endl;
    test_readFromAnEmptyBuffer();
    std::cout << "Finished test test_readFromAnEmptyBuffer." << std::endl;

    std::cout << "Starting test test_writeToAFullBuffer." << std::endl;
    test_writeToAFullBuffer();
    std::cout << "Finished test test_writeToAFullBuffer." << std::endl;
    
    std::cout << "Starting test test_writeAndReadToBuffer." << std::endl;
    test_writeAndReadToBuffer();
    std::cout << "Finished test test_writeAndReadToBuffer." << std::endl;
    } catch (...) {
        std::cerr << "A test assertion failed." << std::endl;
    }
}