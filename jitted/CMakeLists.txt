get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(translation_libs GLOBAL PROPERTY MLIR_TRANSLATION_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)

message(STATUS "dialect_libs: ${dialect_libs}")
message(STATUS "translation_libs: ${translation_libs}")

add_executable(mlir_launcher mlir_launcher.cpp)
target_include_directories(mlir_launcher PRIVATE ${MLIR_INCLUDE_DIRS})
target_link_libraries(mlir_launcher PRIVATE
    MLIRIR
    MLIRSupport
    MLIRParser
    MLIRPass
    MLIRTranslateLib
    MLIRExecutionEngine
    ${dialect_libs}
    ${translation_libs}
    ${conversion_libs}
)