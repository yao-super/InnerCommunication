#
# Source files
#
# Here is where you can add the name of your local classes.
#

FILE (GLOB ALL_SOURCES "*.cpp" "*.c")
FILE (GLOB ALL_INCLUDES "*.hpp" "*.h")

SET (ALL_SRCS 
	 ${ALL_SOURCES}
	 ${ALL_INCLUDES}
)

# For QT moc
QT4_WRAP_CPP (FOO_MOC_OUTFILES vacMessageTransfer.h)

# Create the uwiCMonitorTabSensors C++ library
ADD_LIBRARY (vacMessageTransfer STATIC
			 ${ALL_SRCS} 
			 ${FOO_MOC_OUTFILES}
)

# Add the extra link-libraries
TARGET_LINK_LIBRARIES (vacMessageTransfer
					   ${QT_LIBRARIES}
)

SET_TARGET_PROPERTIES(vacMessageTransfer PROPERTIES FOLDER "vacClass")

# The installation path for the link-library
INSTALL_TARGETS (/lib vacMessageTransfer)

# The installation path for the include-files
INSTALL (FILES ${ALL_INCLUDES} DESTINATION include)

 