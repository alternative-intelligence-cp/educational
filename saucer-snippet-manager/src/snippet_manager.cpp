#include "snippet_manager.hpp"

#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <algorithm>
#include <array>

namespace snippet {

namespace {
    // Simple JSON serialization (minimal, just for this demo)
    std::string escape_json_string(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c;
            }
        }
        return result;
    }

    std::string snippet_to_json(const Snippet& s) {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"id\": \"" << escape_json_string(s.id) << "\",\n";
        oss << "  \"title\": \"" << escape_json_string(s.title) << "\",\n";
        oss << "  \"language\": \"" << escape_json_string(s.language) << "\",\n";
        oss << "  \"code\": \"" << escape_json_string(s.code) << "\",\n";
        oss << "  \"tags\": \"" << escape_json_string(s.tags) << "\",\n";
        oss << "  \"created\": " << s.created << ",\n";
        oss << "  \"modified\": " << s.modified << "\n";
        oss << "}";
        return oss.str();
    }

    // Minimal JSON parsing (just for demo - in production use a real JSON library)
    std::expected<Snippet, std::string> json_to_snippet(const std::string& json) {
        Snippet s;
        
        // This is a very naive parser - just for demo purposes
        // In production, use a proper JSON library like nlohmann/json
        
        auto get_string_field = [&](const std::string& field) -> std::string {
            std::string search = "\"" + field + "\": \"";
            auto pos = json.find(search);
            if (pos == std::string::npos) return "";
            
            pos += search.length();
            auto end = json.find("\"", pos);
            if (end == std::string::npos) return "";
            
            std::string result = json.substr(pos, end - pos);
            
            // Unescape
            std::string unescaped;
            for (size_t i = 0; i < result.length(); ++i) {
                if (result[i] == '\\' && i + 1 < result.length()) {
                    switch (result[i + 1]) {
                        case 'n': unescaped += '\n'; ++i; break;
                        case 'r': unescaped += '\r'; ++i; break;
                        case 't': unescaped += '\t'; ++i; break;
                        case '"': unescaped += '"'; ++i; break;
                        case '\\': unescaped += '\\'; ++i; break;
                        default: unescaped += result[i];
                    }
                } else {
                    unescaped += result[i];
                }
            }
            
            return unescaped;
        };
        
        auto get_number_field = [&](const std::string& field) -> long long {
            std::string search = "\"" + field + "\": ";
            auto pos = json.find(search);
            if (pos == std::string::npos) return 0;
            
            pos += search.length();
            auto end = json.find_first_of(",\n}", pos);
            if (end == std::string::npos) return 0;
            
            try {
                return std::stoll(json.substr(pos, end - pos));
            } catch (...) {
                return 0;
            }
        };
        
        s.id = get_string_field("id");
        s.title = get_string_field("title");
        s.language = get_string_field("language");
        s.code = get_string_field("code");
        s.tags = get_string_field("tags");
        s.created = get_number_field("created");
        s.modified = get_number_field("modified");
        
        if (s.id.empty()) {
            return std::unexpected{"Invalid JSON: missing id"};
        }
        
        return s;
    }
}

SnippetManager::SnippetManager(const std::filesystem::path& storage_dir)
    : storage_dir_(storage_dir) {
    
    if (!std::filesystem::exists(storage_dir_)) {
        std::filesystem::create_directories(storage_dir_);
    }
}

std::string SnippetManager::generate_id() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    const char* chars = "0123456789abcdef";
    std::string id;
    id.reserve(16);
    
    for (int i = 0; i < 16; ++i) {
        id += chars[dis(gen)];
    }
    
    return id;
}

long long SnippetManager::current_timestamp() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

std::filesystem::path SnippetManager::get_snippet_path(const std::string& id) const {
    return storage_dir_ / (id + ".json");
}

std::expected<void, std::string> SnippetManager::save_snippet(const Snippet& snippet) {
    auto path = get_snippet_path(snippet.id);
    
    std::ofstream file(path);
    if (!file) {
        return std::unexpected{"Failed to open file for writing: " + path.string()};
    }
    
    file << snippet_to_json(snippet);
    
    if (!file) {
        return std::unexpected{"Failed to write snippet to file"};
    }
    
    return {};
}

std::expected<Snippet, std::string> SnippetManager::load_snippet(
    const std::filesystem::path& path) {
    
    std::ifstream file(path);
    if (!file) {
        return std::unexpected{"Failed to open file for reading: " + path.string()};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return json_to_snippet(buffer.str());
}

std::expected<Snippet, std::string> SnippetManager::create_snippet(
    const std::string& title,
    const std::string& language,
    const std::string& code,
    const std::string& tags) {
    
    Snippet snippet{
        .id = generate_id(),
        .title = title,
        .language = language,
        .code = code,
        .tags = tags,
        .created = current_timestamp(),
        .modified = current_timestamp()
    };
    
    auto result = save_snippet(snippet);
    if (!result) {
        return std::unexpected{result.error()};
    }
    
    return snippet;
}

std::expected<Snippet, std::string> SnippetManager::get_snippet(const std::string& id) {
    auto path = get_snippet_path(id);
    
    if (!std::filesystem::exists(path)) {
        return std::unexpected{"Snippet not found: " + id};
    }
    
    return load_snippet(path);
}

std::expected<std::vector<Snippet>, std::string> SnippetManager::list_snippets() {
    std::vector<Snippet> snippets;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(storage_dir_)) {
            if (entry.path().extension() == ".json") {
                auto snippet = load_snippet(entry.path());
                if (snippet) {
                    snippets.push_back(*snippet);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        return std::unexpected{"Failed to list snippets: " + std::string(e.what())};
    }
    
    // Sort by modified time (newest first)
    std::sort(snippets.begin(), snippets.end(),
        [](const Snippet& a, const Snippet& b) {
            return a.modified > b.modified;
        });
    
    return snippets;
}

std::expected<Snippet, std::string> SnippetManager::update_snippet(
    const std::string& id,
    const std::string& title,
    const std::string& language,
    const std::string& code,
    const std::string& tags) {
    
    auto existing = get_snippet(id);
    if (!existing) {
        return std::unexpected{existing.error()};
    }
    
    Snippet updated = *existing;
    updated.title = title;
    updated.language = language;
    updated.code = code;
    updated.tags = tags;
    updated.modified = current_timestamp();
    
    auto result = save_snippet(updated);
    if (!result) {
        return std::unexpected{result.error()};
    }
    
    return updated;
}

std::expected<bool, std::string> SnippetManager::delete_snippet(const std::string& id) {
    auto path = get_snippet_path(id);
    
    if (!std::filesystem::exists(path)) {
        return std::unexpected{"Snippet not found: " + id};
    }
    
    try {
        std::filesystem::remove(path);
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        return std::unexpected{"Failed to delete snippet: " + std::string(e.what())};
    }
}

std::expected<std::vector<Snippet>, std::string> SnippetManager::search_snippets(
    const std::string& query) {
    
    auto all_snippets = list_snippets();
    if (!all_snippets) {
        return std::unexpected{all_snippets.error()};
    }
    
    if (query.empty()) {
        return *all_snippets;
    }
    
    std::vector<Snippet> results;
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    
    for (const auto& snippet : *all_snippets) {
        std::string lower_title = snippet.title;
        std::string lower_tags = snippet.tags;
        std::string lower_code = snippet.code;
        
        std::transform(lower_title.begin(), lower_title.end(), lower_title.begin(), ::tolower);
        std::transform(lower_tags.begin(), lower_tags.end(), lower_tags.begin(), ::tolower);
        std::transform(lower_code.begin(), lower_code.end(), lower_code.begin(), ::tolower);
        
        if (lower_title.find(lower_query) != std::string::npos ||
            lower_tags.find(lower_query) != std::string::npos ||
            lower_code.find(lower_query) != std::string::npos) {
            results.push_back(snippet);
        }
    }
    
    return results;
}

std::expected<ExecutionResult, std::string> SnippetManager::execute_snippet(
    const std::string& id) {
    
    auto snippet = get_snippet(id);
    if (!snippet) {
        return std::unexpected{snippet.error()};
    }
    
    // For demo purposes, just return a placeholder
    // In production, you'd use popen() or similar to execute the code
    ExecutionResult result{
        .output = "Execution not yet implemented\nWould run: " + snippet->code,
        .error = "",
        .exit_code = 0
    };
    
    return result;
}

} // namespace snippet
