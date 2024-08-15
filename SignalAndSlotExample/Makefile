# Define the Qt version you are using
QT_VERSION = 6

# Define the output binary name
TARGET = myapp

# Define sources
SOURCES = main.cpp

# Define Qt libraries
QT_LIBS = $(shell pkg-config --libs Qt${QT_VERSION}Core Qt${QT_VERSION}Gui Qt${QT_VERSION}Widgets)

# Define Qt include paths
QT_INCS = $(shell pkg-config --cflags Qt${QT_VERSION}Core Qt${QT_VERSION}Gui Qt${QT_VERSION}Widgets)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 $(QT_INCS)
LDFLAGS = $(QT_LIBS)

# Path to moc
MOC = /usr/lib/qt6/libexec/moc

# Automatically generated files
MOC_FILE = moc_main.cpp

# Generate the moc file from the header
$(MOC_FILE): main.cpp
	$(MOC) -i main.cpp -o $(MOC_FILE)

# Compile the moc file and the main source file
$(TARGET): $(SOURCES) $(MOC_FILE)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(MOC_FILE) -o $(TARGET) $(LDFLAGS)

# Clean up generated files
clean:
	rm -f $(TARGET) $(MOC_FILE)
