﻿###########################################################################
#                                                                         #
#                               ctk_zoocmd_ui                             #
#                                                                         #
###########################################################################
if (OPTION_CTK_ZOOCMD_UI)

project(ctk_zoocmd_ui)
set(OUT_NAME ctk_zoocmd_ui)
set(CMAKE_DEBUG_POSTFIX "")
set(PLUGIN_export_directive "ctk_zoocmd_ui_EXPORT")

IF(QT_INCLUDES)
	include_directories(${QT_INCLUDES})
ENDIF()

IF(WIN32 AND NOT ANDROID)
	# CTK
	IF(3RD_PARTY_DIR)
		FIND_PACKAGE(CTK REQUIRED)
		include_directories(
			${CTKCore_INCLUDE_DIRS}
			${CTKPluginFramework_INCLUDE_DIRS}
		)
		link_directories("${CTK_LIBRARY_DIR}")
	ELSE()
		message(FATAL_ERROR "3RD_PARTY_DIR need to set!!!")
	ENDIF()

	include_directories("${3RD_PARTY_DIR}/include")
	link_directories("${3RD_PARTY_DIR}/lib")
	include(Find3rdPartyDependencies)
else()
	FIND_PACKAGE(CTK)
	include_directories(
		${CTKCore_INCLUDE_DIRS}
		${CTKPluginFramework_INCLUDE_DIRS}
	)

	include_directories("${3RD_PARTY_DIR}/include")
	link_directories("${3RD_PARTY_DIR}/lib")
endif()

file(GLOB CTK_PLUGIN_LIB_H "*.h")
file(GLOB CTK_PLUGIN_LIB_CPP "*.cpp")

set(PLUGIN_SRCS
	${CTK_PLUGIN_LIB_H}
	${CTK_PLUGIN_LIB_CPP}
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
	UIActivator.h
	SetupDlg.h
	ZooCmdUI.h
	ZooCmdWgt.h
	LogPrintWgt.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
	ZooCmdUI.ui
	SetupDlg.ui
	LogPrintWgt.ui
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_RESOURCES
	ZooCmdUI.qrc
	SetupDlg.qrc
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
	NAME ${PROJECT_NAME}
	EXPORT_DIRECTIVE ${PLUGIN_export_directive}
	SRCS ${PLUGIN_SRCS}
	MOC_SRCS ${PLUGIN_MOC_SRCS}
	UI_FORMS ${PLUGIN_UI_FORMS}
	RESOURCES ${PLUGIN_RESOURCES}
	TARGET_LIBRARIES ${PLUGIN_target_libraries}
	${Qt5Core_LIBRARIES}
	${Qt5Xml_LIBRARIES}
	${Qt5OpenGL_LIBRARIES}
	${Qt5Gui_LIBRARIES}
	${Qt5Widgets_LIBRARIES}
	zoo
	zooCmdLoader
)

IF (WIN32)
add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${plugin_RUNTIME_output_dir}/$<CONFIGURATION>/lib${lib_name}.dll"
        "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/plugins"
)
ENDIF()

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib/$<CONFIGURATION>)
INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>/plugins)

endif()
