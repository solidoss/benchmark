// courtesy to: https://github.com/thekvs/cpp-serializers

#include <chrono>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/lexical_cast.hpp>

#include "boost/record.hpp"
#include "cereal/record.hpp"
#include "data.hpp"
#include "solid/system/log.hpp"
#include "solid_v3/record.hpp"

using namespace std;

void boost_serialization_test(size_t iterations)
{
    using namespace boost_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    to_string(r1, serialized);
    from_string(r2, serialized);

    if (r1 != r2) {
        throw std::logic_error("boost's case: deserialization failed");
    }

    std::cout << "boost: version = " << BOOST_VERSION << std::endl;
    std::cout << "boost: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized);
        from_string(r2, serialized);
    }
    auto finish   = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
                        .count();

    std::cout << "boost: time = " << duration << " milliseconds" << std::endl
              << std::endl;
}

void cereal_portable_serialization_test(size_t iterations)
{
    using namespace cereal_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    to_string(r1, serialized, true);
    from_string(r2, serialized, true);

    if (r1 != r2) {
        throw std::logic_error("cereal's case: deserialization failed");
    }

    std::cout << "cereal: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized, true);
        from_string(r2, serialized, true);
    }
    auto finish   = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
                        .count();

    std::cout << "cereal portable: time = " << duration << " milliseconds"
              << std::endl
              << std::endl;
}

void cereal_serialization_test(size_t iterations)
{
    using namespace cereal_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    to_string(r1, serialized, false);
    from_string(r2, serialized, false);

    if (r1 != r2) {
        throw std::logic_error("cereal's case: deserialization failed");
    }

    std::cout << "cereal: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized, false);
        from_string(r2, serialized, false);
    }
    auto finish   = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
                        .count();

    std::cout << "cereal: time = " << duration << " milliseconds" << std::endl
              << std::endl;
}

void solid_serialization_v3_test(size_t iterations)
{
    using namespace solid_v3_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    solid_v3_test::SerializerT   s{solid::reflection::metadata::factory};
    solid_v3_test::DeserializerT d{solid::reflection::metadata::factory};

    std::cout << "sizeof serializer: " << sizeof(s) << std::endl;
    std::cout << "sizeof deserializer: " << sizeof(d) << std::endl;

    std::string serialized;

    to_string(s,  r1, serialized);
    from_string(d, r2, serialized);

    if (r1 != r2) {
        throw std::logic_error("solid's case: deserialization failed");
    }

    std::cout << "solid: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    // char buf[256];
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(s,  r1, serialized);
        from_string(d, r2, serialized);
    }
    auto finish   = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
                        .count();

    std::cout << "solid: time = " << duration << " milliseconds" << std::endl
              << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "usage: " << argv[0]
                  << " N [boost solid_v3 cereal cerealp]";
        std::cout << std::endl
                  << std::endl;
        std::cout << "arguments: " << std::endl;
        std::cout << " N  -- number of iterations" << std::endl
                  << std::endl;
        return EXIT_SUCCESS;
    }

    size_t iterations;

    try {
        iterations = boost::lexical_cast<size_t>(argv[1]);
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        std::cerr << "First positional argument must be an integer." << std::endl;
        return EXIT_FAILURE;
    }

    std::set<std::string> names;

    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            names.insert(argv[i]);
        }
    }
#ifdef SOLID_HAS_DEBUG
    // solid::log_start(std::cerr, {".*:VIEWX"});
#endif
    std::cout << "performing " << iterations << " iterations" << std::endl
              << std::endl;

    try {

        if (names.empty() || names.find("boost") != names.end()) {
            boost_serialization_test(iterations);
        }

        if (names.empty() || names.find("solid") != names.end()) {
            solid_serialization_v3_test(iterations);
        }

        if (names.empty() || names.find("cereal") != names.end()) {
            cereal_serialization_test(iterations);
        }
        if (names.empty() || names.find("cerealp") != names.end()) {
            cereal_portable_serialization_test(iterations);
        }
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
