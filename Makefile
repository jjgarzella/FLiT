PREFIX         ?= /usr

#CXX            ?= clang++
CXX            ?= g++
FFLAGS         ?=
LIBDIR         := lib
SRCDIR         := src
TARGET         ?= $(LIBDIR)/libflit.so

CPPFLAGS       += $(FFLAGS)
CPPFLAGS       += -Wuninitialized -g
CPPFLAGS       += -fPIC
CPPFLAGS       += -std=c++11
CPPFLAGS       += -Wno-shift-count-overflow
CPPFLAGS       += -Wall
CPPFLAGS       += -Wextra
CPPFLAGS       += -Werror
CPPFLAGS       += -I.

CPPFLAGS       += $(S3_REQUIRED)
LINKFLAGS      += -lm -shared

DEPFLAGS       += -MD -MF $(SRCDIR)/$*.d

SOURCE         := $(wildcard $(SRCDIR)/*.cpp)
HEADERS        += $(wildcard $(SRCDIR)/*.h)

OBJ            := $(SOURCE:.cpp=.o)
DEPS           := $(SOURCE:.cpp=.d)

# Install variables

SCRIPT_DIR     := scripts/flitcli
DATA_DIR       := data
CONFIG_DIR     := $(SCRIPT_DIR)/config
DOC_DIR        := documentation
LITMUS_TESTS   += $(wildcard litmus-tests/tests/*.cpp)
LITMUS_TESTS   += $(wildcard litmus-tests/tests/*.h)

INSTALL_FLIT_CONFIG = $(PREFIX)/share/flit/scripts/flitconfig.py

CAT            := $(if $(filter $(OS),Windows_NT),type,cat)
VERSION        := $(shell $(CAT) $(CONFIG_DIR)/version.txt)

.PHONY : all
all: $(TARGET)

.PHONY: help
help:
	@echo "FLiT is an automation and analysis tool for reproducibility of"
	@echo "floating-point algorithms with respect to compilers, architectures,"
	@echo "and compiler flags."
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  all         Compiles the target $(TARGET)"
	@echo "  help        Shows this help message and exits"
	@echo "  install     Installs FLiT.  You may override the PREFIX variable"
	@echo "              to install to a different directory.  The default"
	@echo "              PREFIX value is /usr."
	@echo '                exe: "make install PREFIX=$$HOME/installs/usr"'
	@echo "  check       Run tests for FLiT framework (requires $(TARGET))"
	@echo "  clean       Clean the intermediate build artifacts from building"
	@echo "              $(TARGET)"
	@echo "  distclean   Run clean and then also remove $(TARGET)"
	@echo "  veryclean   An alias for distclean"
	@echo

$(TARGET): $(OBJ)
	mkdir -p lib
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LINKFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp Makefile
	$(CXX) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

.PRECIOUS: src/%.d
-include $(SOURCE:%.cpp=%.d)

check: $(TARGET)
	$(MAKE) check --directory tests

.PHONY: clean
clean:
	rm -f $(OBJ)
	rm -f $(DEPS)
	$(MAKE) clean --directory tests

.PHONY: veryclean distclean
veryclean: distclean
distclean: clean
	rm -f $(TARGET)

.PHONY: install
install: $(TARGET)
	mkdir -m 0755 -p $(PREFIX)/bin
	mkdir -m 0755 -p $(PREFIX)/lib
	mkdir -m 0755 -p $(PREFIX)/include/flit
	mkdir -m 0755 -p $(PREFIX)/share/flit/scripts
	mkdir -m 0755 -p $(PREFIX)/share/flit/doc
	mkdir -m 0755 -p $(PREFIX)/share/flit/data/tests
	mkdir -m 0755 -p $(PREFIX)/share/flit/data/db
	mkdir -m 0755 -p $(PREFIX)/share/flit/config
	mkdir -m 0755 -p $(PREFIX)/share/flit/litmus-tests
	mkdir -m 0755 -p $(PREFIX)/share/flit/benchmarks
	mkdir -m 0755 -p $(PREFIX)/share/licenses/flit
	ln -sf ../share/flit/scripts/flit.py $(PREFIX)/bin/flit
	install -m 0755 $(TARGET) $(PREFIX)/lib/$(notdir $(TARGET))
	install -m 0644 $(HEADERS) $(PREFIX)/include/flit/
	install -m 0755 $(SCRIPT_DIR)/flit.py $(PREFIX)/share/flit/scripts/
	install -m 0755 $(SCRIPT_DIR)/flit_*.py $(PREFIX)/share/flit/scripts/
	install -m 0644 $(SCRIPT_DIR)/flitutil.py $(PREFIX)/share/flit/scripts/
	install -m 0644 $(SCRIPT_DIR)/flitelf.py $(PREFIX)/share/flit/scripts/
	install -m 0644 $(SCRIPT_DIR)/README.md $(PREFIX)/share/flit/scripts/
	install -m 0644 $(DOC_DIR)/*.md $(PREFIX)/share/flit/doc/
	install -m 0644 $(DATA_DIR)/Makefile.in $(PREFIX)/share/flit/data/
	install -m 0644 $(DATA_DIR)/Makefile_bisect_binary.in $(PREFIX)/share/flit/data/
	install -m 0644 $(DATA_DIR)/custom.mk $(PREFIX)/share/flit/data/
	install -m 0644 $(DATA_DIR)/main.cpp $(PREFIX)/share/flit/data/
	install -m 0644 $(DATA_DIR)/tests/Empty.cpp $(PREFIX)/share/flit/data/tests/
	install -m 0644 $(DATA_DIR)/db/tables-sqlite.sql $(PREFIX)/share/flit/data/db/
	install -m 0644 $(CONFIG_DIR)/version.txt $(PREFIX)/share/flit/config/
	install -m 0644 $(CONFIG_DIR)/flit-default.toml.in $(PREFIX)/share/flit/config/
	install -m 0644 $(LITMUS_TESTS) $(PREFIX)/share/flit/litmus-tests/
	install -m 0644 LICENSE $(PREFIX)/share/licenses/flit/
	cp -r benchmarks/* $(PREFIX)/share/flit/benchmarks/
	@echo "Generating $(INSTALL_FLIT_CONFIG)"
	@# Make the flitconfig.py script specifying this installation information
	@echo "'''"                                                                  > $(INSTALL_FLIT_CONFIG)
	@echo "Contains paths and other configurations for the flit installation."  >> $(INSTALL_FLIT_CONFIG)
	@echo "This particular file was autogenerated at the time of installation." >> $(INSTALL_FLIT_CONFIG)
	@echo "This is the file that allows installations to work from any prefix." >> $(INSTALL_FLIT_CONFIG)
	@echo "'''"                                                                 >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "import os"                                                           >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "all = ["                                                             >> $(INSTALL_FLIT_CONFIG)
	@echo "    'version',"                                                      >> $(INSTALL_FLIT_CONFIG)
	@echo "    'script_dir',"                                                   >> $(INSTALL_FLIT_CONFIG)
	@echo "    'doc_dir',"                                                      >> $(INSTALL_FLIT_CONFIG)
	@echo "    'lib_dir',"                                                      >> $(INSTALL_FLIT_CONFIG)
	@echo "    'include_dir',"                                                  >> $(INSTALL_FLIT_CONFIG)
	@echo "    'config_dir',"                                                   >> $(INSTALL_FLIT_CONFIG)
	@echo "    'data_dir',"                                                     >> $(INSTALL_FLIT_CONFIG)
	@echo "    'litmus_test_dir',"                                              >> $(INSTALL_FLIT_CONFIG)
	@echo "    ]"                                                               >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "# flit scripts"                                                      >> $(INSTALL_FLIT_CONFIG)
	@echo "script_dir = '$(abspath $(PREFIX))/share/flit/scripts'"              >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "# flit documentation"                                                >> $(INSTALL_FLIT_CONFIG)
	@echo "doc_dir = '$(abspath $(PREFIX))/share/flit/doc'"                     >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "# compiled libflit.so"                                               >> $(INSTALL_FLIT_CONFIG)
	@echo "lib_dir = '$(abspath $(PREFIX))/lib'"                                >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "# flit C++ include files, primarily flit.h"                          >> $(INSTALL_FLIT_CONFIG)
	@echo "include_dir = '$(abspath $(PREFIX))/include/flit'"                   >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "# default configuration for flit init"                               >> $(INSTALL_FLIT_CONFIG)
	@echo "config_dir = '$(abspath $(PREFIX))/share/flit/config'"               >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "# current version"                                                   >> $(INSTALL_FLIT_CONFIG)
	@echo "with open(os.path.join(config_dir, 'version.txt'), 'r') as version_file:" >> $(INSTALL_FLIT_CONFIG)
	@echo "    version = version_file.read().strip()"                           >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "# default data files such as Makefile.in and main.cpp"               >> $(INSTALL_FLIT_CONFIG)
	@echo "data_dir = '$(abspath $(PREFIX))/share/flit/data'"                   >> $(INSTALL_FLIT_CONFIG)
	@echo                                                                       >> $(INSTALL_FLIT_CONFIG)
	@echo "# directory containing litmus tests"                                 >> $(INSTALL_FLIT_CONFIG)
	@echo "litmus_test_dir = '$(abspath $(PREFIX))/share/flit/litmus-tests'"    >> $(INSTALL_FLIT_CONFIG)

.PHONY: uninstall
uninstall:
	rm -rf $(PREFIX)/include/flit
	rm -rf $(PREFIX)/share/flit
	rm -rf $(PREFIX)/share/licenses/flit
	rm -f $(PREFIX)/bin/flit
	rm -f $(PREFIX)/lib/$(notdir $(TARGET))
	-rmdir --ignore-fail-on-non-empty $(PREFIX)/include
	-rmdir --ignore-fail-on-non-empty $(PREFIX)/share/licenses
	-rmdir --ignore-fail-on-non-empty $(PREFIX)/share
	-rmdir --ignore-fail-on-non-empty $(PREFIX)/bin
	-rmdir --ignore-fail-on-non-empty $(PREFIX)/lib
	-rmdir --ignore-fail-on-non-empty $(PREFIX)
