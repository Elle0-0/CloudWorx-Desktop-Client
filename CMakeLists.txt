cmake_minimum_required(VERSION 3.5)

project(CloudWorx VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CloudWorx
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        loginpage.h loginpage.cpp loginpage.ui
        signuppage.h signuppage.cpp signuppage.ui
        fileuploadpage.h fileuploadpage.cpp fileuploadpage.ui
        Images.qrc
        argon2id_utils.h argon2id_utils.cpp
        filepasswordpage.h filepasswordpage.cpp filepasswordpage.ui
        envelopeencryption.h envelopeencryption.cpp
        envelopeencryptionmanager.h envelopeencryptionmanager.cpp
        dashboard.h dashboard.cpp dashboard.ui
        decryptfilepage.h decryptfilepage.cpp decryptfilepage.ui
        sharefilepage.h sharefilepage.cpp sharefilepage.ui
        models/filemodel.h
        models/filelistmodel.h
        network/authapi.h network/authapi.cpp
        models/UserRegisterModel.h
        models/UserLoginModel.h
        network/filesapi.h network/filesapi.cpp
        utils/ApiHelper.h
        utils/keygenutils.h utils/keygenutils.cpp
        keygenpage.h keygenpage.cpp keygenpage.ui
        cryptography/hybridencryptionmanager.h cryptography/hybridencryptionmanager.cpp
        network/sharesapi.h network/sharesapi.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CloudWorx APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CloudWorx SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CloudWorx
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CloudWorx PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CloudWorx)
endif()
set_target_properties(CloudWorx PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CloudWorx
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(LIBSODIUM_DIR "${CMAKE_SOURCE_DIR}/libs/libsodium")

include_directories(${LIBSODIUM_DIR}/include)
link_directories(${LIBSODIUM_DIR}/lib)

target_link_libraries(CloudWorx PRIVATE ${LIBSODIUM_DIR}/lib/libsodium.a)

# set(LIBCURL_DIR "${CMAKE_SOURCE_DIR}/libs/curl")

# include_directories(${LIBCURL_DIR}/include)
# link_directories(${LIBCURL_DIR}/lib)

# target_link_libraries(CloudWorx PRIVATE ${LIBCURL_DIR}/lib/libcurl.a)

set(OPENSSL_USE_STATIC_LIBS TRUE)

set(OPENSSL_ROOT_DIR "${CMAKE_SOURCE_DIR}/libs/OpenSSLStaticLibs")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARIES
    "${OPENSSL_ROOT_DIR}/lib/libssl.a"
    "${OPENSSL_ROOT_DIR}/lib/libcrypto.a"
)
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")

# Manually set include and libs
target_include_directories(CloudWorx PRIVATE "${OPENSSL_INCLUDE_DIR}")

target_link_libraries(CloudWorx PRIVATE
    "${OPENSSL_ROOT_DIR}/lib/libssl.a"
    "${OPENSSL_ROOT_DIR}/lib/libcrypto.a"
    -lws2_32 -lcrypt32 -lgdi32
)


set(LIBCURL_DIR "${CMAKE_SOURCE_DIR}/libs/curl")
target_include_directories(CloudWorx PRIVATE "${LIBCURL_DIR}/include")
target_link_libraries(CloudWorx PRIVATE "${LIBCURL_DIR}/lib/libcurl.dll.a")


include_directories("C:/Program Files/OpenSSL-Win64/include")


# if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(OPENSSL_SSL_LIB "${CMAKE_SOURCE_DIR}/libs/libssl.lib")
#     set(OPENSSL_CRYPTO_LIB "${CMAKE_SOURCE_DIR}/libs/libcrypto.lib")
# else()
#     set(OPENSSL_SSL_LIB "${CMAKE_SOURCE_DIR}/libs/libssl.lib")
#     set(OPENSSL_CRYPTO_LIB "${CMAKE_SOURCE_DIR}/libs/libcrypto.lib")
# endif()


# target_link_libraries(CloudWorx PRIVATE
#     "${OPENSSL_SSL_LIB}"
#     "${OPENSSL_CRYPTO_LIB}"
# )

# set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
# find_package(OpenSSL REQUIRED)


# target_include_directories(CloudWorx PRIVATE ${OPENSSL_INCLUDE_DIR})
# target_link_libraries(CloudWorx PRIVATE ${OPENSSL_LIBRARIES})



file(COPY ${CMAKE_SOURCE_DIR}/certs/cacert.pem DESTINATION ${CMAKE_BINARY_DIR}/certs)



if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CloudWorx)
endif()
