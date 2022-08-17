#include <Arduino.h>
#include <TestClass.h>
#include <src/TestQuadratureEncorder.h>

extern "C" int main(void)
{
	TestClass *t = new TestClass();
	t->TestLoop();
}
