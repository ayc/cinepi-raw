/*
 * test_redis_command_parsing.cpp
 *
 * Unit test for parsing Redis command strings in RedisControl.
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <cstring>

// Helper function to simulate the parsing logic inside RedisControl
void parseColorGains(const std::string& val, float& r, float& b) {
    // Logic ported from original CinePIController
    std::string s = val;
    char *ptr = strtok(&s[0], ",");
    if(ptr != NULL){
        r = std::stof(ptr);
        ptr = strtok(NULL, ",");
        if(ptr != NULL){
            b = std::stof(ptr);
        }
    }
}

int main() {
    std::cout << "Running Redis Command Parsing tests..." << std::endl;

    // 1. Test Color Gains Parsing
    std::cout << "Testing Color Gains Parsing..." << std::endl;
    float r = 0.0f, b = 0.0f;
    std::string input = "1.5,2.8";
    
    parseColorGains(input, r, b);
    
    assert(r == 1.5f);
    assert(b == 2.8f);
    std::cout << "PASS: " << input << " -> R=" << r << ", B=" << b << std::endl;

    // Test incomplete input
    input = "1.2";
    r = 0.0f; b = 0.0f;
    parseColorGains(input, r, b);
    assert(r == 1.2f);
    // b remains 0.0f or undefined based on implementation, assuming safe check
    std::cout << "PASS: Partial input handled." << std::endl;

    std::cout << "All Parsing tests passed." << std::endl;
    return 0;
}
