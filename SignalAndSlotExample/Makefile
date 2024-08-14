# Set the Qt6 installation directory
QT_DIR = /usr

# Compiler and linker flags
CXX = g++
CXXFLAGS = -std=c++17 -I$(QT_DIR)/include/aarch64-linux-gnu/qt6 -I$(QT_DIR)/include/aarch64-linux-gnu/qt6/QtWidgets
LDFLAGS = -L$(QT_DIR)/lib -lQt6Widgets -lQt6Core

# Source files
SOURCES = main.cpp
MOC_SOURCES = main.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable file
TARGET = myapp

# Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

# Generate moc files
moc: $(MOC_SOURCES)
	moc $(MOC_SOURCES) -o main.moc

clean:
	rm -f $(TARGET) $(OBJECTS) main.moc

.PHONY: all clean moc
