#ifndef __DRONJSON_TESTER__
#define __DRONJSON_TESTER__

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <functional>

#define TEST_ASSERT(condition)\
	if(!(condition)){ \
		throw std::runtime_error(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": fall: " #condition );\
	}\

#define TEST_ASSERT_THROW(expr, except)\
	{\
		bool caught = false; \
		try { expr; } \
		catch (const except &) { caught = true; } \
		catch (...) {} \
		if(!caught){ \
			throw std::runtime_error(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": exception: " #except );\
		}\
	}\

using test_func = std::function<void()>;

class test_case {
public:
	test_case(test_func func, const std::string name) : _func(func), _name(name) {}
	bool run() {
		try {
			auto start = std::chrono::high_resolution_clock::now();
			_func();
			auto end = std::chrono::high_resolution_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			std::cout << "[PASS] " << _name << ", time: " << ms << ";" << std::endl;
			return true;
		}
		catch (const std::exception & e) {
			std::cout << "[FALL] " << _name << ", error: " << e.what() << std::endl;
		}
		catch (...) {
			std::cout << "[FALL] " << _name << ", error: unknown exception"<< std::endl;
		}
		return false;
	}
private:
	test_func _func;
	std::string _name;
};

class tester {
public:

	//tester() {};

	static tester & inst() {
		static tester _inst;
		return _inst;
	}
	
	void add(test_func func, const std::string name) {
		_tests.emplace_back(std::move(func), name);
	}

	void run() {
		std::cout << "Running " << _tests.size() << " tests..." << std::endl;
		size_t passed = 0; 
		size_t falled = 0;
		auto start = std::chrono::high_resolution_clock::now();
		for (auto & test : _tests) {
			if (test.run()) {
				passed++;
			}
			else {
				falled++;
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "Tests end.\n" << "Result: passed: " << passed 
										   << " falled: " << falled 
										   << " time: " << ms << " ms"<<  std::endl;
	}

private:
	std::vector<test_case> _tests;
};

#define TEST_CASE(name)\
	void name();\
	namespace { \
		struct reg_##name { \
			reg_##name() { \
				tester::inst().add(name, #name); \
			} \
		}; \
		static reg_##name _reg_##name;\
	} \
	void name()\

#define ASSERT_TOKEN_STRING(tok, str)\
		TEST_ASSERT(tok.type() == json::io::token_type::_string);\
		TEST_ASSERT(tok.string_data() == str);\

#define ASSERT_TOKEN_NUMBER(tok, num)\
		TEST_ASSERT(tok.type() == json::io::token_type::_number);\
		TEST_ASSERT(tok.double_data() == num);\

#define ASSERT_TOKEN_BOOL(tok, boolean)\
		TEST_ASSERT(tok.type() == json::io::token_type::_false || tok.type() == json::io::token_type::_true);\
		TEST_ASSERT(tok.type() == boolean);\

#endif 