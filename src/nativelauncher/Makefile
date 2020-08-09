include config.inc

TGT=libhook.so

SRC=src/nativelauncher
LDFLAGS=-fPIC -ldl
CFLAGS=-Wall -I$(SRC)

TARGET=$(BINDIR)/libwf_nativehook.so
#TARGET_32=$(BINDIR)/libwf_nativehook_32.so

SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(addprefix $(BINDIR)/,$(SRCS:.c=.o))
#OBJS32=$(addprefix $(BINDIR)/,$(SRCS:.c=_32.o))

ifeq ($(REL),debug)
CFLAGS+=-g
endif

#$(info $$SRCS is [${SRCS}])
#$(info $$OBJS is [${OBJS}])

all: $(BINDIR)/$(SRC) $(TARGET) $(TARGET_32)
#all: $(BINDIR)/$(SRC) $(TARGET) $(TARGET_32)

#$(TARGET_32): $(OBJS32)
#	$(CC) -shared -o $@ $(OBJS32) $(CFLAGS) $(LDFLAGS)

$(TARGET): $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

#$(BINDIR)/%_32.o: %.c %.h $(GLOBAL_HDR)
#	$(CC) -m32 -c -o $@ $< $(CFLAGS) $(LDFLAGS)

$(BINDIR)/%.o: %.c %.h $(GLOBAL_HDR)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

$(BINDIR)/$(SRC):
	@mkdir -p $(BINDIR)/$(SRC)

clean:
	rm -rf $(BINDIR)/$(SRC)
