# Modern CMake Policy Fix
set(CMAKE_POLICY_VERSION_MINIMUM 3.5) 
cmake_minimum_required(VERSION 3.20)

project(sudo_i_am_alive VERSION 0.1.0 LANGUAGES CXX C)

# Force modern behaviors for FetchContent
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization for CachyOS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")

include(FetchContent)

# 1. nlohmann/json 
# Using the specific commit or tag that handles policies better
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# 2. llama.cpp (The deterministic heart [cite: 16, 187])
FetchContent_Declare(llama_cpp
    GIT_REPOSITORY https://github.com/ggerganov/llama.cpp.git
    GIT_TAG        master
)
set(LLAMA_STATIC ON CACHE BOOL "" FORCE)
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(llama_cpp)

# 3. FAISS (Vector Database)
FetchContent_Declare(faiss
    GIT_REPOSITORY https://github.com/facebookresearch/faiss.git
    GIT_TAG        v1.8.0
)

# FAISS için sadece ihtiyacımız olan bileşenleri aktif et, testleri kapat
set(FAISS_ENABLE_GPU OFF CACHE BOOL "" FORCE)
set(FAISS_ENABLE_PYTHON OFF CACHE BOOL "" FORCE)
set(FAISS_ENABLE_TESTS OFF CACHE BOOL "" FORCE) # BU SATIR HATAYI ÇÖZER
set(FAISS_OPT_LEVEL "avx2" CACHE STRING "" FORCE)

FetchContent_MakeAvailable(faiss)
# System dependencies for the daemon [cite: 4, 210]
find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED libsystemd)

include_directories(include ${SYSTEMD_INCLUDE_DIRS})

set(SOURCES
    src/main.cpp
    src/core/SentinelConfig.cpp
    src/sensors/TerminalMonitor.cpp
    src/memory/CognitiveRouter.cpp  # Bu satırı ekle!
    src/sensors/ClipboardMonitor.cpp # Ekle
    src/memory/MemoryManager.cpp
    src/memory/EmbeddingProcessor.cpp
    src/core/StateMachine.cpp          # BU SATIRIN EKLENDİĞİNDEN EMİN OL
    src/core/ResponseGenerator.cpp
  )

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    llama
    ggml
    faiss
    ${SYSTEMD_LIBRARIES}
    pthread
)
