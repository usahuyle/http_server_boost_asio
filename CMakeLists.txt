cmake_minimum_required(VERSION 3.22)
project(https_server)
set (CMAKE_CXX_STANDARD 11)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
set(EXECUTABLE_NAME Executable)
set(HEADERS_LIB headers_lib)
# Define the paths to your certificate and key files
set(CRT_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/server.crt")
set(KEY_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/server.key")

# Define the output C++ header file that will hold the certificate data
set(CERT_HEADER_PATH "${CMAKE_CURRENT_BINARY_DIR}/cert_data.h")

# Create a custom command to generate the C++ header
add_custom_command(
    OUTPUT ${CERT_HEADER_PATH}
    COMMAND ${CMAKE_COMMAND} -E cat ${CRT_FILE_PATH} ${KEY_FILE_PATH} | xxd -i -c > ${CERT_HEADER_PATH}
    DEPENDS ${CRT_FILE_PATH} ${KEY_FILE_PATH}
    COMMENT "Embedding cert.crt and key.key into C++ header..."
)

add_subdirectory(src)
add_subdirectory(app)
