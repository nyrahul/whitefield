include config.inc

TGT=libhook.so

SRC=src/nativelauncher
LDFLAGS=-fPIC -ldl -L$(BINDIR) -lwf_commline
CFLAGS=-Wall -I$(SRC) -Isrc

TARGET=$(BINDIR)/libwf_nativehook.so

SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(addprefix $(BINDIR)/,$(SRCS:.c=.o))

ifeq ($(REL),debug)
CFLAGS+=-g
endif

all: $(BINDIR)/$(SRC) $(TARGET) $(TARGET_32)

$(TARGET): $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

$(BINDIR)/%.o: %.c %.h $(GLOBAL_HDR)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

$(BINDIR)/$(SRC):
	@mkdir -p $(BINDIR)/$(SRC)

clean:
	rm -rf $(BINDIR)/$(SRC)
