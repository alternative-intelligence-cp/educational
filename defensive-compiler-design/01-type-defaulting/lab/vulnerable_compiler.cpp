/*
 * Vulnerable Compiler - Type Defaulting Bug
 * 
 * This is a minimal compiler demonstrating ARIA-026 Bug #1:
 * Silent defaulting of unknown types to i32.
 *
 * WHAT IT DOES:
 * - Parses simple type annotations
 * - Maps known types to LLVM types
 * - SILENTLY DEFAULTS unknown types to i32 (BUG!)
 *
 * WHAT GOES WRONG:
 * - If you use an unknown struct type (e.g., MotorCommand)
 * - Compiler thinks it's i32 (4 bytes)
 * - Allocates 4 bytes on stack
 * - Tries to store 16 bytes
 * - Stack corruption! Adjacent variables overwritten!
 */

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

// Fake LLVM types (simplified for demo)
struct LLVMType {
    std::string name;
    size_t size_bytes;
};

class SimpleCompiler {
private:
    std::map<std::string, LLVMType> known_types_;
    
public:
    SimpleCompiler() {
        // Initialize known types
        known_types_["i32"] = {"i32", 4};
        known_types_["i64"] = {"i64", 8};
        known_types_["f64"] = {"f64", 8};
        // Note: MotorCommand is NOT in this map!
    }
    
    // 💀 BUG: This function silently defaults unknown types to i32
    LLVMType mapType(const std::string& type_name) {
        std::cout << "[TYPE MAPPING] Mapping type: " << type_name << std::endl;
        
        auto it = known_types_.find(type_name);
        if (it != known_types_.end()) {
            std::cout << "[TYPE MAPPING] Found known type: " << type_name 
                      << " (" << it->second.size_bytes << " bytes)" << std::endl;
            return it->second;
        }
        
        // 💀💀💀 THE BUG: Silent default to i32! 💀💀💀
        std::cerr << "[TYPE MAPPING] WARNING: Unknown type '" << type_name 
                  << "', defaulting to i32" << std::endl;
        return {"i32", 4};  // WRONG! Guessing the type!
    }
    
    void compileVariable(const std::string& var_name, const std::string& type_name) {
        std::cout << "\n[CODEGEN] Compiling variable: " << var_name 
                  << " : " << type_name << std::endl;
        
        // Map the type
        LLVMType llvm_type = mapType(type_name);
        
        // Allocate stack space based on mapped type
        std::cout << "[ALLOCA] Allocating " << llvm_type.size_bytes 
                  << " bytes for variable '" << var_name << "'" << std::endl;
        
        // Simulate storing a MotorCommand struct (16 bytes)
        if (type_name == "MotorCommand") {
            size_t actual_size = 16;  // MotorCommand is actually 16 bytes!
            
            std::cout << "[STORE] Attempting to store " << actual_size 
                      << " bytes into " << llvm_type.size_bytes << "-byte allocation..." << std::endl;
            
            if (actual_size > llvm_type.size_bytes) {
                std::cerr << "[CORRUPTION] 💀 STACK CORRUPTION! 💀" << std::endl;
                std::cerr << "[CORRUPTION] Wrote " << actual_size << " bytes into " 
                          << llvm_type.size_bytes << "-byte allocation!" << std::endl;
                std::cerr << "[CORRUPTION] Adjacent stack variables are now corrupted!" << std::endl;
                std::cerr << "[CORRUPTION] Potential security vulnerability!" << std::endl;
                
                // Simulate crash from stack corruption
                throw std::runtime_error("Stack corruption - segmentation fault");
            }
        }
        
        std::cout << "[CODEGEN] Variable compiled successfully" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "VULNERABLE COMPILER - Type Defaulting Bug" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        SimpleCompiler compiler;
        
        // Simulate compiling: let cmd: MotorCommand = ...
        std::cout << "\n[PARSER] Parsed: let cmd: MotorCommand = { ... }" << std::endl;
        compiler.compileVariable("cmd", "MotorCommand");
        
        std::cout << "\n[SUCCESS] Compilation completed" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n[CRASH] " << e.what() << std::endl;
        std::cerr << "[CRASH] Compiler crashed due to stack corruption!" << std::endl;
        return 1;
    }
}
