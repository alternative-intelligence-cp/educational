/*
 * Safe Compiler - Defensive Type Handling
 * 
 * This is the FIXED version with ARIA-026 Bug #1 resolved.
 *
 * WHAT IT DOES:
 * - Parses simple type annotations
 * - Maps known types to LLVM types
 * - THROWS ERROR for unknown types (FIX!)
 *
 * WHY IT'S SAFE:
 * - No guessing about types
 * - Explicit error message for user
 * - Compilation stops before corruption can occur
 * - Forces programmer to either:
 *   a) Define the type properly, or
 *   b) Report compiler bug if type should be known
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
    
    // ✅ FIX: This function throws error for unknown types
    LLVMType mapType(const std::string& type_name) {
        std::cout << "[TYPE MAPPING] Mapping type: " << type_name << std::endl;
        
        auto it = known_types_.find(type_name);
        if (it != known_types_.end()) {
            std::cout << "[TYPE MAPPING] Found known type: " << type_name 
                      << " (" << it->second.size_bytes << " bytes)" << std::endl;
            return it->second;
        }
        
        // ✅✅✅ THE FIX: Explicit error instead of guessing! ✅✅✅
        std::cerr << "\n[ERROR] =====================================" << std::endl;
        std::cerr << "[ERROR] Internal Compiler Error" << std::endl;
        std::cerr << "[ERROR] =====================================" << std::endl;
        std::cerr << "[ERROR] Unknown type: '" << type_name << "'" << std::endl;
        std::cerr << "[ERROR] " << std::endl;
        std::cerr << "[ERROR] This is a bug in the compiler, not your code." << std::endl;
        std::cerr << "[ERROR] The compiler doesn't know how to handle this type." << std::endl;
        std::cerr << "[ERROR] " << std::endl;
        std::cerr << "[ERROR] Please report this issue with your source code." << std::endl;
        std::cerr << "[ERROR] =====================================" << std::endl;
        
        throw std::runtime_error("Unknown type: " + type_name);
    }
    
    void compileVariable(const std::string& var_name, const std::string& type_name) {
        std::cout << "\n[CODEGEN] Compiling variable: " << var_name 
                  << " : " << type_name << std::endl;
        
        // Map the type (will throw if unknown)
        LLVMType llvm_type = mapType(type_name);
        
        // Allocate stack space based on mapped type
        std::cout << "[ALLOCA] Allocating " << llvm_type.size_bytes 
                  << " bytes for variable '" << var_name << "'" << std::endl;
        
        // If we got here, type is known and size is correct
        std::cout << "[CODEGEN] Variable compiled successfully" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "SAFE COMPILER - Defensive Type Handling" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        SimpleCompiler compiler;
        
        // Simulate compiling: let cmd: MotorCommand = ...
        std::cout << "\n[PARSER] Parsed: let cmd: MotorCommand = { ... }" << std::endl;
        compiler.compileVariable("cmd", "MotorCommand");
        
        std::cout << "\n[SUCCESS] Compilation completed" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n[COMPILATION FAILED]" << std::endl;
        std::cerr << "Reason: " << e.what() << std::endl;
        std::cerr << "\nThe compiler refused to guess about unknown types." << std::endl;
        std::cerr << "This prevented potential stack corruption." << std::endl;
        std::cerr << "\n✅ Defensive behavior working correctly! ✅" << std::endl;
        return 1;
    }
}
