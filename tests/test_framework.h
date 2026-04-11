#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

/**
 * ============================================================================
 * SIMPLE TEST FRAMEWORK
 * ============================================================================
 * 
 * Framework minimalista de testing para pruebas unitarias.
 * 
 * EJEMPLO DE USO:
 * 
 * TEST(CameraSystem, "Rotation updates correctly") {
 *     CameraSystem cam;
 *     cam.rotate(45.0f);
 *     ASSERT_EQUAL(cam.getRotation(), 45.0f, "Rotation should be 45");
 * }
 * 
 * int main() {
 *     TestRunner::run();
 *     return 0;
 * }
 */

class TestRunner {
public:
    struct TestCase {
        std::string suite;
        std::string name;
        std::function<void()> fn;
    };

    static std::vector<TestCase>& getTests() {
        static std::vector<TestCase> tests;
        return tests;
    }

    static void registerTest(const std::string& suite, const std::string& name, 
                            std::function<void()> fn) {
        getTests().push_back({suite, name, fn});
    }

    static int run() {
        const auto& tests = getTests();
        int passed = 0;
        int failed = 0;

        std::cout << "\n=== Running Tests ===\n" << std::endl;

        for (const auto& test : tests) {
            std::cout << "[TEST] " << test.suite << " :: " << test.name << " ... ";
            try {
                test.fn();
                std::cout << "✓ PASSED" << std::endl;
                passed++;
            } catch (const std::string& error) {
                std::cout << "✗ FAILED" << std::endl;
                std::cout << "       Error: " << error << std::endl;
                failed++;
            } catch (...) {
                std::cout << "✗ FAILED (Unknown error)" << std::endl;
                failed++;
            }
        }

        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Total:  " << (passed + failed) << std::endl;
        std::cout << "Rate:   " << (passed * 100 / (passed + failed)) << "%" << std::endl;

        return failed == 0 ? 0 : 1;
    }
};

// Macro para registrar tests
#define TEST(Suite, Name) \
    void test_##Suite##_##Name(); \
    static struct { \
        test_##Suite##_##Name##_Registrar() { \
            TestRunner::registerTest(#Suite, Name, test_##Suite##_##Name); \
        } \
    } test_##Suite##_##Name##_instance; \
    void test_##Suite##_##Name()

// Macros para assertions
#define ASSERT_TRUE(condition, message) \
    if (!(condition)) throw std::string(message)

#define ASSERT_FALSE(condition, message) \
    if (condition) throw std::string(message)

#define ASSERT_EQUAL(actual, expected, message) \
    if (actual != expected) throw std::string(message)

#define ASSERT_NOT_EQUAL(actual, expected, message) \
    if (actual == expected) throw std::string(message)

#define ASSERT_THROW(expression, message) \
    try { \
        expression; \
        throw std::string(message); \
    } catch (...) { \
        /* Expected */ \
    }
