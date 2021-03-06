# Makefile for Qucs plugin
# (c) 2021 Felix Salfelder
# GPLv3+

QUCS_CONF = $(shell which qucs-conf)

ifneq ($(QUCS_CONF),)
 $(info asking $(QUCS_CONF))
 # TODO. complete the move to configure.
 QUCS_CXX      = $(shell $(QUCS_CONF) --cxx)   # c++
 QUCS_CPPFLAGS = $(shell $(QUCS_CONF) --cppflags) -DADD_VERSION
 QT_CPPFLAGS = $(shell $(QUCS_CONF) --qt-cppflags)
 QUCS_LDFLAGS  = $(shell $(QUCS_CONF) --ldflags)
 QUCS_CXXFLAGS = $(shell $(QUCS_CONF) --cxxflags)
 HAVE_QUCS=yes
else
 HAVE_QUCS=no
 $(error no qucs-conf, this will not work)
endif

include MakeList

all: ${TARGET}.so

MY_CPPFLAGS = ${QUCS_CPPFLAGS}
MY_CPPFLAGS += ${QT_CPPFLAGS}

RAW_O = ${RAW_SRCS:%.cc=%.o}

${TARGET}.so: ${RAW_O}
	${QUCS_CXX} -shared -o $@ $< ${LIBS}

%.o: %.cc
	${QUCS_CXX} -c -fPIC ${MY_CPPFLAGS} ${CPPFLAGS} ${QUCS_CXXFLAGS} ${CXXFLAGS} $< -o $@

