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

$(TARGET): $(OBJS)
	@mkdir -p $(TRGDIR)
	$(CXX) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@mkdir -p $(DEPDIR)
	$(CXX) $(CPPFLAGS) -MF $(DEPDIR)/$(*F).d $(CXXFLAGS) -o $@ $<

.PHONY: test
test: $(TARGET)
	./$<

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(DEPDIR) $(TRGDIR)
