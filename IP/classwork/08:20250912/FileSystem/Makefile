# Makefile v3.7.0 2025-Mar-19 Jeisson Hidalgo-Cespedes ECCI-UCR CC-BY 4.0

# VAR=value  ## Overrides a variable, e.g CC=mpicc DEFS=-DGUI. See helpvars
CC=gcc#= C compiler
XC=g++#= C++ compiler
JC=javac
DEFS=#= Preprocessor symbols, e.g: DEFS=-DVERBOSE
CSTD=-std=c17#= Force compliance to a C standard, e.g: c11|gnu11|c17|c2x
XSTD=-std=c++17#= Force compliance to a C++ standard, e.g: c++11|gnu++11|c++17|c++2x
FLAG=#= Compiler flags for both C and C++
FLAGS=$(strip -Wall -Wextra $(FLAG) $(DEFS))
FLAGC=$(strip $(FLAGS) $(CSTD))
FLAGX=$(strip $(FLAGS) $(XSTD))
FLAGJ=$(strip $(FLAG))
LIBS=#= Libraries to be linked, e.g: LIBS=-lm
LINTF=-build/header_guard,-build/include_subdir,-runtime/int
LINTC=$(LINTF),-readability/casting
LINTX=$(LINTF),-build/c++11,-runtime/references
ARGS=#= Execution arguments, e.g: 'ARGS=100 "a text"'
ARGSJR=-ea
RUNPRE=#= Execution prefix e.g: for `time ./myapp` run `make RUNPRE=time`
RUNSUF=#= Execution suffix e.g: for `./myapp |sort` run `make RUNSUF=|sort`
GITUSER=$(shell getent passwd "$USER" | cut -d: -f5 | cut -d, -f1)#= Your full name e.g: "Ana Soto"
GITEMAIL=
GITTIME=10800#= Store git password in memory for this amount of seconds (default 3h)
HOST=$(subst .local,,$(shell hostname))

# Directories
RSC=assets#= Dir for assets: images, sounds... to be included with executable
BIN=bin#= Dir where executable files are generated
BUILD=build#= Dir for object files (.o|.class)
DOC=doc#= Dir where documentation output will be generated
SRC=src#= Dir containing source code to compile, e.g: DIR=experiment
TESTS=tests#= Dir where test cases are stored, e.g: TESTS=large

# If src/ dir does not exist, use current directory .
ifeq "$(wildcard $(SRC) )" ""
	SRC=.
endif

# Files
DIRS=$(shell find -L $(SRC) -type d)
ASSETS=$(wildcard $(RSC)/)
APPNAME=$(shell basename $(shell pwd))
HEADERC=$(wildcard $(DIRS:%=%/*.h))
HEADERX=$(wildcard $(DIRS:%=%/*.hpp))
SOURCEC=$(wildcard $(DIRS:%=%/*.c))
SOURCEX=$(wildcard $(DIRS:%=%/*.cpp))
SOURCEJ=$(wildcard $(DIRS:%=%/*.java))
SOURCEP=$(wildcard $(DIRS:%=%/*.py))
INPUTFC=$(strip $(HEADERC) $(SOURCEC))
INPUTFX=$(strip $(HEADERX) $(SOURCEX))
INPUTCX=$(strip $(INPUTFC) $(INPUTFX))
OBJECTC=$(SOURCEC:$(SRC)/%.c=$(BUILD)/%.o)
OBJECTX=$(SOURCEX:$(SRC)/%.cpp=$(BUILD)/%.o)
OBJECTJ=$(SOURCEJ:$(SRC)/%.java=$(BUILD)/%.class)
OBJECTS=$(strip $(OBJECTC) $(OBJECTX))
TESTINP=$(wildcard $(TESTS)/input*.txt)
INCLUDE=$(DIRS:%=-I%)
DEPENDS=$(OBJECTS:%.o=%.d)
REMOVES=$(BIN)/ $(BUILD)/ $(DOC)/
IGNORES=$(REMOVES) *.pyc .DS_Store .vscode
EXEFILE=$(BIN)/$(APPNAME)
MAINCLS=$(shell grep -lE 'static\s+void\s+main\b' $(SRC)/*.java | xargs basename -s .java)
MAKFILE:=$(lastword $(MAKEFILE_LIST))
MAKSITE=http://jeisson.work
LD=$(if $(SOURCEX),$(XC),$(CC))
ifneq ($(OBJECTS),)
	TARGETS+=$(EXEFILE)
	TESTOUT+=$(TESTINP:$(TESTS)/input%.txt=exe/output%.txt)
	OUTPUTX+=$(TESTINP:$(TESTS)/input%.txt=cppout/output%.txt)
	EXEARGS+=$(strip $(EXEFILE) $(ARGS))
	DOCTARG+=cppdoc
endif
ifneq ($(OBJECTJ),)
	TARGETS+=$(EXEFILE).jar
	TESTOUT+=$(TESTINP:$(TESTS)/input%.txt=jar/output%.txt)
	OUTPUTJ+=$(TESTINP:$(TESTS)/input%.txt=javaout/output%.txt)
	JARARGS+=$(strip java $(ARGSJR) -jar $(EXEFILE).jar $(ARGS))
	DOCTARG+=javadoc
endif
ifneq ($(SOURCEP),)
	TESTOUT+=$(TESTINP:$(TESTS)/input%.txt=py3/output%.txt)
	OUTPUTP+=$(TESTINP:$(TESTS)/input%.txt=pyout/output%.txt)
	PY3ARGS=$(strip python3 $(SOURCEP) $(ARGS))
	DOCTARG+=pydoc
endif

# Parameterized targets
ifneq ($(tc),)
TSTINP=$(shell seq -f $(TESTS)/input%03g.txt $(tc))
TSTOUT=$(shell seq -f $(TESTS)/output%03g.txt $(tc))
default: tc
else  # tc
ifneq ($(project),)
PROJECT=.gitignore readme.adoc_
PROJECT+=$(if $(filter c,$(project)),src/solution.c_)
PROJECT+=$(if $(filter cpp,$(project)),src/solution.cpp_)
PROJECT+=$(if $(filter py,$(project)),src/solution.py_)
PROJECT+=$(if $(filter java,$(project)),src/Solution.java_ src/package-info.java_)
PROJECT+=$(if $(filter design,$(project)),design/readme.adoc_ design/solution.pseudo_)
default: project
else  # project
default: debug
endif
endif

# Targets
.PHONY: all asan debug helgrind memcheck msan release tsan ubsan
.PHONY: clean gitconfig help instdeps lint project run tc test update version
all: debug test lint doc  ## Run targets: test lint doc
debug: FLAGS += -g  ## Build an executable for debugging [default]
release: FLAGS += -O3 -DNDEBUG  ## Build an optimized executable
debug release: $(TARGETS)
asan: FLAGS += -fsanitize=address -fno-omit-frame-pointer  ## Build for detecting memory leaks and invalid accesses
msan: FLAGS += -fsanitize=memory  ## Build for detecting uninitialized memory usage
msan: CC = clang
msan: XC = clang++
tsan: FLAGS += -fsanitize=thread  ## Build for detecting thread errors, e.g race conditions
ubsan: FLAGS += -fsanitize=undefined  ## Build for detecting undefined behavior
asan msan tsan ubsan: debug

-include $(MAKFILE).mk
-include *.mk $(DEPENDS)
.SECONDEXPANSION:

# C/C++ Linker call
$(EXEFILE): $(OBJECTS) | $$(@D)/.
	$(LD) $(strip $(FLAGS)) $(INCLUDE) $^ -o $@ $(LIBS)

# Compile C source file
$(BUILD)/%.o: $(SRC)/%.c | $$(@D)/.
	$(CC) -c $(FLAGC) $(INCLUDE) -MMD $< -o $@

# Compile C++ source file
$(BUILD)/%.o: $(SRC)/%.cpp | $$(@D)/.
	$(XC) -c $(FLAGX) $(INCLUDE) -MMD $< -o $@

# Java Linker call. TODO(jhc): fallible main class detection
%.jar: $(OBJECTJ) | $$(@D)/.	
	jar cfe $@ $(MAINCLS) -C $(BUILD) .
ifneq ($(ASSETS),)
	jar uf $@ -C $(ASSETS) .
endif

# Compile Java source file
.PRECIOUS: $(BUILD)/%.class
$(BUILD)/%.class: $(SRC)/%.java | $$(@D)/.
	$(strip $(JC) -classpath $(SRC) $(FLAGJ)) $< -d $(BUILD)

# Create a subdirectory if not exists
.PRECIOUS: %/.
%/.:
	mkdir -p $(dir $@)

# test:  ## Run executable against test cases in folder tests/
test: SHELL:=/bin/bash
test: $(TARGETS) $(TESTOUT)

DIFF=icdiff --no-headers
ifeq (, $(shell which $(firstword $(DIFF)) 2> /dev/null))
	DIFF=diff
endif # icdiff not installed, use diff

define run_test
$(strip $(RUNPRE) $($(1)) $(shell [ ! -z "$(2)" ] && cat $(2)) < $< $(RUNSUF))
endef

# TODO(any): Remove redundancy
exe/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt $$(wildcard $(TESTS)/args%.txt)
	-$(DIFF) $(word 2,$^) <($(call run_test,EXEARGS,$(word 3,$^))) ||:

jar/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	-$(DIFF) $(word 2,$^) <($(call run_test,JARARGS,$(word 3,$^))) ||:

py3/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	-$(DIFF) $(word 2,$^) <($(call run_test,PY3ARGS,$(word 3,$^))) ||:

# <L>out  ## Generate test case output using language L: cpp|java|py
cppout: $(TARGETS) $(OUTPUTX)
cppout/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt $$(wildcard $(TESTS)/args%.txt)
	$(call run_test,EXEARGS,$(word 3,$^)) > $(word 2,$^)

javaout: $(TARGETS) $(OUTPUTJ)
javaout/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	$(call run_test,JARARGS,$(word 3,$^)) > $(word 2,$^)

pyout: $(TARGETS) $(OUTPUTP)
pyout/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	$(call run_test,PY3ARGS,$(word 3,$^)) > $(word 2,$^)

# tc=N  ## Generates N empty test cases in tests/
.PRECIOUS: $(TESTS)/%.txt
tc: $(TSTINP) $(TSTOUT)
$(TESTS)/%.txt: | $$(@D)/.
	touch $@

# project=L  ## Create files for languages L: c|cpp|java|py|design
DOWN=wget -qO -
ifeq (, $(shell which $(firstword $(DOWN)) 2> /dev/null))
	DOWN=curl -s
endif # wget not installed, try cURL

project: $(PROJECT)
	@$(MAKE) tc=1

%_: | $$(@D)/.
	@test -f $* || ($(DOWN) $(MAKSITE)/misc/project/$* > $* && echo $*)

.PHONY: doc  ## Generate documentation from sources
doc: $(DOCTARG)
cppdoc: Doxyfile $(INPUTCX)
	doxygen

javadoc: $(SOURCEJ) | $(DOC)/java/.
	javadoc -quiet $(SOURCEJ) -d $(DOC)/java

pydoc: $(SOURCEP) | $(DOC)/python/.
	pydoc -w $(SOURCEP)
	mv *.html $(DOC)/python

Doxyfile:
	-doxygen -g -s > /dev/null
	sed -i.bak -E 's/(OUTPUT_DIRECTORY *)=/\1= $(DOC)/' $@
	sed -i.bak -E 's/(INPUT *)=/\1= $(SRC)/' $@
	sed -i.bak -E 's/(RECURSIVE *)= NO/\1= YES/' $@
	sed -i.bak -E 's/(EXTRACT_ALL *)= NO/\1= YES/' $@
	sed -i.bak -E 's/(QUIET *)= NO/\1= YES/' $@
	rm -f $@.bak

lint:  ## Check code style conformance using Cpplint
ifneq ($(INPUTFC),)
	-cpplint --filter=$(LINTC) $(INPUTFC)
endif
ifneq ($(INPUTFX),)
	-cpplint --filter=$(LINTX) $(INPUTFX)
endif
ifneq ($(SOURCEJ),)
	-checkstyle -c /google_checks.xml $(SOURCEJ)
	-checkstyle -c /sun_checks.xml $(SOURCEJ)
endif
ifneq ($(SOURCEP),)
	-pylint -sn $(SOURCEP)
endif

run: $(TARGETS)  ## Run executable using ARGS value as arguments
ifneq ($(EXEARGS),)
	$(strip $(RUNPRE) $(EXEARGS) $(RUNSUF))
endif
ifneq ($(JARARGS),)
	$(strip $(RUNPRE) $(JARARGS) $(RUNSUF))
endif
ifneq ($(PY3ARGS),)
	$(strip $(RUNPRE) $(PY3ARGS) $(RUNSUF))
endif

memcheck: $(EXEFILE)  ## Run executable for detecting memory errors with Valgrind
	valgrind -q -s --sigill-diagnostics=yes --leak-check=full $(EXEARGS)

helgrind: $(EXEFILE)  ## Run executable for detecting thread errors with Valgrind
	valgrind -q -s --sigill-diagnostics=yes --tool=helgrind $(EXEARGS)

.gitignore:  ## Generate a .gitignore file
	echo $(IGNORES) | tr " " "\n" > .gitignore

gitconfig:  ## Configure name/email. Cache password in memory for some time
	git config --global user.name "$(GITUSER)"
	git config --global user.email $(GITEMAIL)
	git config credential.helper "cache --timeout=$(GITTIME)"
	ssh-keygen -t ed25519 -C "$(HOST)" && cat ~/.ssh/id_ed25519.pub

#.NOTPARALLEL: clean
clean:  ## Remove generated directories and files
	rm -rf $(REMOVES)
ifneq ($(SOURCEP),)
	find $(SRC) -name '*.pyc' -exec rm -f {} \;
endif

# Install dependencies for Debian or RedHat based distros and macOS
ALLDEPS=checkstyle doxygen pipx
LINDEPS=$(ALLDEPS) clang graphviz python3-gpg valgrind
MACDEPS=$(ALLDEPS) cpplint icdiff openjdk
ifeq (0, $(shell which brew > /dev/null 2>&1; echo $$?)) # macOS
	INSDEPS=brew install $(MACDEPS)
else ifeq (0, $(shell which dnf > /dev/null 2>&1; echo $$?)) # RedHat-based
	INSDEPS=sudo dnf install @development-tools java-21-openjdk libasan libtsan libubsan $(LINDEPS)
	PIPDEPS=&& pipx ensurepath && pipx install icdiff && pipx install cpplint
else ifeq (0, $(shell which apt > /dev/null 2>&1; echo $$?)) # Debian-based
	INSDEPS=sudo apt install build-essential cpplint icdiff openjdk-17-jdk $(LINDEPS)
else
	INSDEPS=@>&2 echo "ERROR: Package manager not supported"
endif

instdeps:  ## Install needed packages on macOS/Debian/RedHat-based distributions
	$(INSDEPS) $(PIPDEPS)

update:  ## Update this Makefile to latest version"
	$(DOWN) $(MAKSITE)/Makefile > $(MAKFILE)

version:  ## Show this Makefile version
	@head -1 $(MAKFILE)

help:  ## Shows this help
	@echo "Usage make [-jN] [VAR=value] [targets]"
	@echo "  -jN        Compile N files simultaneously [N=1]"
	@grep -E '^\S.*##' $(MAKFILE) | perl -pe 's/^(?:\.PHONY: |# )?([\.\w=<>]+):?.*  ## (.+)/  \1	\2/' | expand -t13 | sort

helpvars:  ## List common variables to override
	@grep -E '^\S.*#=' $(MAKFILE) | perl -pe 's/^(\w+).*#= (.+)/  \1	\2/' | expand -t13 | sort
