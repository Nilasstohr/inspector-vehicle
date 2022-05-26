#include <Arduino.h>
#include <src/TestClass.h>

extern "C" int main(void)
{
	TestClass *t = new TestClass();
	t->TestLoop();
}
