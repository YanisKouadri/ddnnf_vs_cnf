# Makefile adapted from Minisat: https://github.com/niklasso/minisat/blob/master/Makefile

###################################################################################################

.PHONY:	r d p sh lr ld lp lsh all clean
all:	r lr lsh

## Configurable options ###########################################################################

CXX ?= g++

# Directory to store object files, libraries, executables, and dependencies:
BUILD_DIR      ?= build

# Include debug-symbols in release builds
MUTOKSIA_RELSYM ?= 

# Sets of compile flags for different build types
MUTOKSIA_REL    ?= -O3 -D NDEBUG
MUTOKSIA_DEB    ?= -O0 -D DEBUG 
MUTOKSIA_PRF    ?= -O3 -D NDEBUG
MUTOKSIA_FPIC   ?= -fpic

ifneq "$(CXX)" "clang++"
MUTOKSIA_REL    += -flto
endif

# Target file names
MUTOKSIA      = mu-toksia#       Name of MiniSat main executable.
MUTOKSIA_SLIB = lib$(MUTOKSIA).a#  Name of MiniSat static library.
MUTOKSIA_DLIB = lib$(MUTOKSIA).so# Name of MiniSat shared library.

# Shared Library Version
SOMAJOR=2023
SOMINOR=04
SORELEASE?=.11#   Declare empty to leave out from library file name.

SAT_SOLVER ?= glucose
#SAT_SOLVER = cryptominisat

MUTOKSIA_CXXFLAGS = -I include -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS -Wall -Wno-parentheses -Wextra -Wno-unused-parameter -std=c++20
MUTOKSIA_CXXFLAGS += -D GR_IN_ST
MUTOKSIA_CXXFLAGS += -D AD_IN_ST
#MUTOKSIA_CXXFLAGS += -D CO_IN_ST
MUTOKSIA_CXXFLAGS += -D CO_IN_PR
MUTOKSIA_CXXFLAGS += -D CO_IN_SST
MUTOKSIA_CXXFLAGS += -D CO_IN_ID
MUTOKSIA_CXXFLAGS += -D ST_EXISTS_STG
#MUTOKSIA_CXXFLAGS += -D ST_EXISTS_SST
#MUTOKSIA_CXXFLAGS += -D DYNAMIC_ASSUMPS
MUTOKSIA_LDFLAGS  = -Wall -lz -flto

ifeq (Darwin,$(findstring Darwin,$(shell uname)))
	SHARED_LDFLAGS += -shared -Wl,-dylib_install_name,$(MUTOKSIA_DLIB).$(SOMAJOR)
	RELEASE_LDFLAGS +=
else
	SHARED_LDFLAGS += -shared -Wl,-soname,$(MUTOKSIA_DLIB).$(SOMAJOR)
	RELEASE_LDFLAGS += 
endif

ECHO=@echo
ifeq ($(VERB),)
VERB=@
else
VERB=
endif

HDRS = $(wildcard src/*.h)
ALL_SRCS = $(wildcard src/*.cpp)
SAT_SOLVER_SRCS = $(wildcard src/*Solver.cpp)
SRCS = $(filter-out $(SAT_SOLVER_SRCS), $(ALL_SRCS))
OBJS = $(filter-out %Main.o, $(SRCS:.cpp=.o))

GLUCOSE = lib/glucose-syrup-4.1
CRYPTOMINISAT = lib/cryptominisat-5.11.4

ifeq ($(SAT_SOLVER), glucose)
	CXXFLAGS += -D SAT_GLUCOSE -D INCREMENTAL -I$(GLUCOSE)
	ifeq (Darwin,$(findstring Darwin,$(shell uname)))
		LDFLAGS += -Wl,-rpath,'@loader_path/../../../$(GLUCOSE)/build/dynamic/lib'
	else
		LDFLAGS += -Wl,-rpath,'$$ORIGIN/../../../$(GLUCOSE)/build/dynamic/lib'
	endif
	LDFLAGS  += -L$(GLUCOSE)/build/dynamic/lib -lglucose
	OBJS     += src/GlucoseSolver.o
	SAT_SOLVER_LIB = $(GLUCOSE)/build/dynamic/lib
else ifeq ($(SAT_SOLVER), cryptominisat)
	CXXFLAGS += -D SAT_CMSAT -I$(CRYPTOMINISAT)/src
	ifeq (Darwin,$(findstring Darwin,$(shell uname)))
		LDFLAGS += -Wl,-rpath,'@loader_path/../../../$(CRYPTOMINISAT)/build/lib'
	else
		LDFLAGS += -Wl,-rpath,'$$ORIGIN/../../../$(CRYPTOMINISAT)/build/lib'
	endif
	LDFLAGS  += -L$(CRYPTOMINISAT)/build/lib -lcryptominisat5
	OBJS     += src/CryptoMiniSatSolver.o
	SAT_SOLVER_LIB = $(CRYPTOMINISAT)/build/lib
else
	$(error No SAT solver specified.)
endif

$(CRYPTOMINISAT)/build/lib:
	$(ECHO) Compiling: CryptoMiniSat
	$(VERB) mkdir -p $(CRYPTOMINISAT)/build && cd $(CRYPTOMINISAT)/build && cmake ..
	$(VERB) $(MAKE) -C $(CRYPTOMINISAT)/build

$(GLUCOSE)/build/dynamic/lib:
	$(ECHO) Compiling: Glucose
	$(VERB) $(MAKE) -C $(GLUCOSE)

r:	$(BUILD_DIR)/release/bin/$(MUTOKSIA)
d:	$(BUILD_DIR)/debug/bin/$(MUTOKSIA)
p:	$(BUILD_DIR)/profile/bin/$(MUTOKSIA)
sh:	$(BUILD_DIR)/dynamic/bin/$(MUTOKSIA)

lr:	$(BUILD_DIR)/release/lib/$(MUTOKSIA_SLIB)
ld:	$(BUILD_DIR)/debug/lib/$(MUTOKSIA_SLIB)
lp:	$(BUILD_DIR)/profile/lib/$(MUTOKSIA_SLIB)
lsh:	$(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB).$(SOMAJOR).$(SOMINOR)$(SORELEASE)

## Build-type Compile-flags:
$(BUILD_DIR)/release/%.o:			MUTOKSIA_CXXFLAGS += $(MUTOKSIA_REL) $(MUTOKSIA_RELSYM)
$(BUILD_DIR)/debug/%.o:				MUTOKSIA_CXXFLAGS += $(MUTOKSIA_DEB) -g
$(BUILD_DIR)/profile/%.o:			MUTOKSIA_CXXFLAGS += $(MUTOKSIA_PRF) -pg
$(BUILD_DIR)/dynamic/%.o:			MUTOKSIA_CXXFLAGS += $(MUTOKSIA_REL) $(MUTOKSIA_FPIC)

## Build-type Link-flags:
$(BUILD_DIR)/profile/bin/$(MUTOKSIA):		MUTOKSIA_LDFLAGS += -pg
$(BUILD_DIR)/release/bin/$(MUTOKSIA):		MUTOKSIA_LDFLAGS += $(RELEASE_LDFLAGS) $(MUTOKSIA_RELSYM)

## Executable dependencies
$(BUILD_DIR)/release/bin/$(MUTOKSIA):	 	$(BUILD_DIR)/release/src/Main.o $(BUILD_DIR)/release/lib/$(MUTOKSIA_SLIB)
$(BUILD_DIR)/debug/bin/$(MUTOKSIA):	 	$(BUILD_DIR)/debug/src/Main.o $(BUILD_DIR)/debug/lib/$(MUTOKSIA_SLIB)
$(BUILD_DIR)/profile/bin/$(MUTOKSIA):	 	$(BUILD_DIR)/profile/src/Main.o $(BUILD_DIR)/profile/lib/$(MUTOKSIA_SLIB)
# need the main-file be compiled with fpic?
$(BUILD_DIR)/dynamic/bin/$(MUTOKSIA):	 	$(BUILD_DIR)/dynamic/src/Main.o $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB)

## Library dependencies
$(BUILD_DIR)/release/lib/$(MUTOKSIA_SLIB):	$(foreach o,$(OBJS),$(BUILD_DIR)/release/$(o))
$(BUILD_DIR)/debug/lib/$(MUTOKSIA_SLIB):		$(foreach o,$(OBJS),$(BUILD_DIR)/debug/$(o))
$(BUILD_DIR)/profile/lib/$(MUTOKSIA_SLIB):	$(foreach o,$(OBJS),$(BUILD_DIR)/profile/$(o))
$(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB).$(SOMAJOR).$(SOMINOR)$(SORELEASE)\
 $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB).$(SOMAJOR)\
 $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB):	$(foreach o,$(OBJS),$(BUILD_DIR)/dynamic/$(o))

## Compile rules
$(BUILD_DIR)/release/%.o:	%.cpp
	$(ECHO) Compiling: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $(MUTOKSIA_CXXFLAGS) $(CXXFLAGS) -c -o $@ $< -MMD -MF $(BUILD_DIR)/release/$*.d

$(BUILD_DIR)/profile/%.o:	%.cpp
	$(ECHO) Compiling: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $(MUTOKSIA_CXXFLAGS) $(CXXFLAGS) -c -o $@ $< -MMD -MF $(BUILD_DIR)/profile/$*.d

$(BUILD_DIR)/debug/%.o:	%.cpp
	$(ECHO) Compiling: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $(MUTOKSIA_CXXFLAGS) $(CXXFLAGS) -c -o $@ $< -MMD -MF $(BUILD_DIR)/debug/$*.d

$(BUILD_DIR)/dynamic/%.o:	%.cpp
	$(ECHO) Compiling: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $(MUTOKSIA_CXXFLAGS) $(CXXFLAGS) -c -o $@ $< -MMD -MF $(BUILD_DIR)/dynamic/$*.d

## Linking rule
$(BUILD_DIR)/release/bin/$(MUTOKSIA) $(BUILD_DIR)/debug/bin/$(MUTOKSIA) $(BUILD_DIR)/profile/bin/$(MUTOKSIA) $(BUILD_DIR)/dynamic/bin/$(MUTOKSIA):
	$(MAKE) $(SAT_SOLVER_LIB)
	$(ECHO) Linking Binary: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $^ $(LDFLAGS) $(MUTOKSIA_LDFLAGS) -o $@

## Static Library rule
%/lib/$(MUTOKSIA_SLIB):
	$(MAKE) $(SAT_SOLVER_LIB)
	$(ECHO) Linking Static Library: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(AR) -rcs $@ $^

## Shared Library rule
$(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB).$(SOMAJOR).$(SOMINOR)$(SORELEASE)\
 $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB).$(SOMAJOR)\
 $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB):
	$(MAKE) $(SAT_SOLVER_LIB)
	$(ECHO) Linking Shared Library: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $^ $(SHARED_LDFLAGS) $(MUTOKSIA_LDFLAGS) $(LDFLAGS) -o $@
	$(VERB) ln -sf $(MUTOKSIA_DLIB).$(SOMAJOR).$(SOMINOR)$(SORELEASE) $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB).$(SOMAJOR)
	$(VERB) ln -sf $(MUTOKSIA_DLIB).$(SOMAJOR) $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB)

clean:
	rm -f $(foreach t, release debug profile dynamic, $(foreach o, $(SRCS:.cpp=.o), $(BUILD_DIR)/$t/$o)) \
          $(foreach t, release debug profile dynamic, $(foreach d, $(SRCS:.cpp=.d), $(BUILD_DIR)/$t/$d)) \
	  $(foreach t, release debug profile dynamic, $(BUILD_DIR)/$t/bin/$(MUTOKSIA)) \
	  $(foreach t, release debug profile, $(BUILD_DIR)/$t/lib/$(MUTOKSIA_SLIB)) \
	  $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB).$(SOMAJOR).$(SOMINOR)$(SORELEASE)\
	  $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB).$(SOMAJOR)\
	  $(BUILD_DIR)/dynamic/lib/$(MUTOKSIA_DLIB)
	@[ ! -d $(GLUCOSE)/build ] || { $(MAKE) -C $(GLUCOSE) clean; rm -rf $(GLUCOSE)/build; }
	@[ ! -d $(CRYPTOMINISAT)/build ] || { $(MAKE) -C $(CRYPTOMINISAT)/build clean; rm -rf $(CRYPTOMINISAT)/build; }

## Include generated dependencies
-include $(foreach s, $(SRCS:.cc=.d), $(BUILD_DIR)/release/$s)
-include $(foreach s, $(SRCS:.cc=.d), $(BUILD_DIR)/debug/$s)
-include $(foreach s, $(SRCS:.cc=.d), $(BUILD_DIR)/profile/$s)
-include $(foreach s, $(SRCS:.cc=.d), $(BUILD_DIR)/dynamic/$s)
