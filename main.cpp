#include<stdio.h>
#include<stdlib.h>
#include "xray.h"

FILE *g_stdout = NULL;

class A {
public:
    A() {};
    void add() {
        int i = 0;
        for(;i<0xffffff;i++);
        m_count++;
    };
private:
    int m_count;
};

int count = 0;
void one_unit_of_work()
{
    int i = 0;
    for(;i<0xffffff;i++) {
        count++;
    }
}

void func4(void)
{
    one_unit_of_work();
}
void func3(void)
{
    one_unit_of_work();
    func4();
}

void func1(void)
{
    one_unit_of_work();
    func3();
}

void func2(void)
{
    one_unit_of_work();
}

int main(int argc, char **argv)
{
    struct XRayTraceCapture *xray = XRayInit(4, 1024, 2, argv[1]);
    XRayStartFrame(xray);
    one_unit_of_work();
    one_unit_of_work();
    A a;
    a.add();
    for (int i= 0; i < 10; i++) {
      one_unit_of_work();
    }
    func1();
    func2();
    XRayEndFrame(xray);
    XRaySaveReport(xray, argv[2], atof(argv[3]), 0);
    XRayStartFrame(xray);
    for (int i= 0; i < 10; i++) {
      one_unit_of_work();
    }
    XRayEndFrame(xray);
    XRaySaveReport(xray, argv[2], atof(argv[3]), 0);
    return 0;
}
