# generation de programme PVM
#  TP algorithmique parallele
#  maitrise
#  LL
#  07/10/97
# 

CC = gcc -g

# executable directory
BDIR = $(PWD)

# Programmes maitre et esclave
PROG = upper upperm uppers

#PVM_ROOT = /usr/share/pvm3
#PVM_ARCH = LINUXI386
#PVM_ROOT = /home/commun_depinfo/enseignants/lemarchand/TPPVMM1/PvmLinux/pvm3
#PVM_ARCH = LINUX64


#### do not edit #####

EXEC = $(PROG:%=$(BDIR)/%)

# EXEC = upper upperm uppers
# PVM_ROOT = /usr/share/pvm3
# PVM_ARCH = $(shell pvmgetarch)

# CFLAGS = -I$(PVM_ROOT)/include -Wall
# LDFLAGS = -L$(PVM_ROOT)/lib/$(PVM_ARCH) -lpvm3

#PVM_LIB = $(PVM_ROOT)/lib/$(PVM_ARCH)/libpvm3.a
PVM_LIB = -lpvm3
CFLAGS =  -I$(PVM_ROOT)/include -D_POSIX_C_SOURCE=2 -DEPATH=\"$(BDIR)\"
# include $(PVM_ROOT)/conf/$(PVM_ARCH).def

#### generation rules #####

all: $(EXEC)

$(BDIR)/upper: upper.o point.o
	$(CC) -o $@ $^

$(BDIR)/upperm: upperm.o point.o
	$(CC) -o $@ $^ $(PVM_LIB) $(ARCHLIB)

$(BDIR)/uppers: uppers.o point.o
	$(CC) -o $@ $^ $(PVM_LIB) $(ARCHLIB)

%.o: %.c point.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) *.o upper_hull.pdf