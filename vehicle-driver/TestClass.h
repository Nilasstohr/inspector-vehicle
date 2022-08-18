#ifndef SRC_TESTCLASS_H_
#define SRC_TESTCLASS_H_

#include <Arduino.h>
#include "src/Logger.h"

class TestClass {
public:
	TestClass();
	void TestLoop();
    void customLogger(Logger::Level level, const char* module, const char* message);
	virtual ~TestClass();
};

#endif /* SRC_TESTCLASS_H_ */
