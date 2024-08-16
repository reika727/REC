CXX      := /usr/local/bin/g++
CPPFLAGS := -MMD -MP
CXXFLAGS := -c -std=c++23 -Wall -Wextra
SRCDIR   := src
OBJDIR   := obj
DEPDIR   := dep
TRGDIR   := bin
SRCS     := $(wildcard $(SRCDIR)/*.cpp)
OBJS     := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.cpp=.o)))
DEPS     := $(addprefix $(DEPDIR)/, $(notdir $(SRCS:.cpp=.d)))
TARGET   := $(TRGDIR)/rec

.PHONY: all
all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS) | $(TRGDIR)
	$(CXX) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) $(DEPDIR)
	$(CXX) $(CPPFLAGS) -MF $(DEPDIR)/$(*F).d $(CXXFLAGS) -o $@ $<

$(OBJDIR) $(DEPDIR) $(TRGDIR):
	mkdir $@

.PHONY: test
test: $(TARGET)
	./$<

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(DEPDIR) $(TRGDIR)
