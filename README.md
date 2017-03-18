# google-xray-cpp-profiling-tool

Compare to original version, this consolidate the total time of the same function per call path together, eg: 

for this program: 
```
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
```

Before the fix, it was: 
```
Frame#        Total Ticks      Capture size    Annotations   Label
--------------------------------------------------------------------
     0             610890                21              0   @@@frame0@@@
     1             341152                10              0   @@@frame1@@@
--------------------------------------------------------------------
XRay: 2 frame(s)    31 total capture(s)


Sorted by total ticks (most expensive first):

Frame#        Total Ticks      Capture size    Annotations   Label
--------------------------------------------------------------------
     0             610890                21              0   @@@frame0@@@
     1             341152                10              0   @@@frame1@@@


====================================================================
label @@@frame0@@@

   Address          Ticks   Percent      Function    [annotation...]
--------------------------------------------------------------------
0x004015E0          40740       6.7      one_unit_of_work() 
0x004015E0          42913       7.0      one_unit_of_work() 
0x00401920          29830       4.9      A::add() 
0x004015E0          56432       9.2      one_unit_of_work() 
0x004015E0          34975       5.7      one_unit_of_work() 
0x004015E0          34669       5.7      one_unit_of_work() 
0x004015E0          34623       5.7      one_unit_of_work() 
0x004015E0          33938       5.6      one_unit_of_work() 
0x004015E0          34387       5.6      one_unit_of_work() 
0x004015E0          32517       5.3      one_unit_of_work() 
0x004015E0          33436       5.5      one_unit_of_work() 
0x004015E0          33388       5.5      one_unit_of_work() 
0x004015E0          33459       5.5      one_unit_of_work() 
0x004016C0         102163      16.7      func1() 
0x004015E0          33840       5.5       one_unit_of_work() 
0x00401680          68322      11.2       func3() 
0x004015E0          34362       5.6        one_unit_of_work() 
0x00401640          33960       5.6        func4() 
0x00401700          33418       5.5      func2() 
0x004015E0          33418       5.5       one_unit_of_work() 

====================================================================
label @@@frame1@@@

   Address          Ticks   Percent      Function    [annotation...]
--------------------------------------------------------------------
0x004015E0          35406      10.4      one_unit_of_work() 
0x004015E0          33310       9.8      one_unit_of_work() 
0x004015E0          33450       9.8      one_unit_of_work() 
0x004015E0          33564       9.8      one_unit_of_work() 
0x004015E0          33822       9.9      one_unit_of_work() 
0x004015E0          34627      10.2      one_unit_of_work() 
0x004015E0          34989      10.3      one_unit_of_work() 
0x004015E0          34345      10.1      one_unit_of_work() 
0x004015E0          34417      10.1      one_unit_of_work() 
0x004015E0          33220       9.7      one_unit_of_work() 
====================================================================

```

After the fix, it is now: 

```
Frame#        Total Ticks      Capture size    Annotations   Label
--------------------------------------------------------------------
     0             577842                12              0   @@@frame0@@@
     1             335899                 1              0   @@@frame1@@@
--------------------------------------------------------------------
XRay: 2 frame(s)    13 total capture(s)


Sorted by total ticks (most expensive first):

Frame#        Total Ticks      Capture size    Annotations   Label
--------------------------------------------------------------------
     0             577842                12              0   @@@frame0@@@
     1             335899                 1              0   @@@frame1@@@


====================================================================
label @@@frame0@@@

   Address          Ticks   Percent      Function    [annotation...]
--------------------------------------------------------------------
0x004015E0          34435       6.0      one_unit_of_work() 
0x004015E0          34747       6.0      one_unit_of_work() 
0x00401920          29981       5.2      A::add() 
0x004015E0         343517      59.4      one_unit_of_work() 
0x004016C0         101688      17.6      func1() 
0x004015E0          34255       5.9       one_unit_of_work() 
0x00401680          67433      11.7       func3() 
0x004015E0          33856       5.9        one_unit_of_work() 
0x00401640          33576       5.8        func4() 
0x00401700          33468       5.8      func2() 
0x004015E0          33467       5.8       one_unit_of_work() 

====================================================================
label @@@frame1@@@

   Address          Ticks   Percent      Function    [annotation...]
--------------------------------------------------------------------
0x004015E0         335898     100.0      one_unit_of_work() 
====================================================================

```
