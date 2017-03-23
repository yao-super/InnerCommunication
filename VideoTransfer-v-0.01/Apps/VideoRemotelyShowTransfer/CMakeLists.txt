set (FOO_SRCS
	 VideoRemotelyShowTransfer.cpp

)
# Generate the executable application for the current poject
ADD_EXECUTABLE (VideoRemotelyShowTransfer 
				${FOO_SRCS}
#				${FOO_MOC_H}
)

# Add the link-libraries for the current poject
TARGET_LINK_LIBRARIES (VideoRemotelyShowTransfer 
					   vacMainInterface
					   ${QT_LIBRARIES}
)
SET_TARGET_PROPERTIES(VideoRemotelyShowTransfer PROPERTIES FOLDER "Apps")
# The installation path for the application
INSTALL_TARGETS (/bin VideoRemotelyShowTransfer )

 