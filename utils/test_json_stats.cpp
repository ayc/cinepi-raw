/*
 * test_json_stats.cpp
 *
 * Unit test for JSON stats construction.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

std::string buildStatsJson(float fps, unsigned int colorTemp, float focus, unsigned int frameCount, size_t bufferSize) {
    std::stringstream ss;
    ss << "{";
    ss << "\"fps\":" << fps << ",";
    ss << "\"color_temp\":" << colorTemp << ",";
    ss << "\"focus\":" << focus << ",";
    ss << "\"frame_count\":" << frameCount << ",";
    ss << "\"buffer_size\":" << bufferSize;
    ss << "}";
    return ss.str();
}

int main() {
    std::cout << "Running JSON Stats tests..." << std::endl;

    float fps = 24.5f;
    unsigned int colorTemp = 5600;
    float focus = 120.0f;
    unsigned int frameCount = 100;
    size_t bufferSize = 4096;

    std::string json = buildStatsJson(fps, colorTemp, focus, frameCount, bufferSize);
    std::string expected = "{\"fps\":24.5,\"color_temp\":5600,\"focus\":120,\"frame_count\":100,\"buffer_size\":4096}";

    assert(json == expected);
    std::cout << "PASS: JSON string matches expected format: " << json << std::endl;

    std::cout << "All JSON Stats tests passed." << std::endl;
    return 0;
}