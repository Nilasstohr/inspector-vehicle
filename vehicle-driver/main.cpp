#include <Arduino.h>
#include <TestClass.h>

extern "C" int main(void)
{
	TestClass *t = new TestClass();
	t->TestLoop();
}
