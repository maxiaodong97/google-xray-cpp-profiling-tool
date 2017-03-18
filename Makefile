CFLAGS = -g -finstrument-functions
CFLAGS += -falign-functions=32
CFLAGS += -fpermissive
CFLAGS += -Wno-format -Wno-attributes -Wno-int-to-pointer-cast
CFLAGS += -I. -I../../include/
CFLAGS += -DXRAY \
	  -DXRAY_ANNOTATE \
	  -DXRAY_DISABLE_BROWSER_INTEGRATION \
	  -DXRAY_PACK_BUFFER \
	  -DXRAY_NO_RDTSC \
	  -DLINUX

LDFLAGS = -Xlinker -Map=a.map

SOURCES = \
	browser.c \
	demangle.c \
	hashtable.c \
	main.cpp \
	parsesymbols.c \
	report.c \
	stringpool.c \
	symtable.c \
	xray.c

all:
	g++ $(CFLAGS) $(LDFLAGS) $(SOURCES) -ldl
	./a.out a.map a.report 0.1
	cat a.report

clean:
	rm -fr a.out a.map a.report
