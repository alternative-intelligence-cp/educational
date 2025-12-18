#include "snippet_manager.h"
#include <fstream>
#include <sstream>
#include <random>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

SnippetManager::SnippetManager(const std::string& storage_path) 
    : storage_path_(storage_path) {
    fs::create_directories(storage_path_);
}

std::string SnippetManager::generate_id() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    const char* hex = "0123456789abcdef";
    std::string id;
    for (int i = 0; i < 16; i++) {
        id += hex[dis(gen)];
    }
    return id;
}

std::string SnippetManager::get_snippet_path(const std::string& id) {
    return storage_path_ + "/" + id + ".json";
}

std::string SnippetManager::escape_json(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c;
        }
    }
    return escaped;
}

std::vector<std::string> SnippetManager::parse_tags(const std::string& tags_str) {
    std::vector<std::string> tags;
    std::stringstream ss(tags_str);
    std::string tag;
    while (std::getline(ss, tag, ',')) {
        // Trim whitespace
        tag.erase(0, tag.find_first_not_of(" \t"));
        tag.erase(tag.find_last_not_of(" \t") + 1);
        if (!tag.empty()) {
            tags.push_back(tag);
        }
    }
    return tags;
}

std::string SnippetManager::snippet_to_json(const Snippet& snippet) {
    std::ostringstream json;
    json << "{";
    json << "\"id\":\"" << escape_json(snippet.id) << "\",";
    json << "\"title\":\"" << escape_json(snippet.title) << "\",";
    json << "\"language\":\"" << escape_json(snippet.language) << "\",";
    json << "\"code\":\"" << escape_json(snippet.code) << "\",";
    json << "\"tags\":[";
    for (size_t i = 0; i < snippet.tags.size(); i++) {
        json << "\"" << escape_json(snippet.tags[i]) << "\"";
        if (i < snippet.tags.size() - 1) json << ",";
    }
    json << "],";
    json << "\"created\":" << snippet.created << ",";
    json << "\"modified\":" << snippet.modified;
    json << "}";
    return json.str();
}

std::string SnippetManager::snippets_to_json(const std::vector<Snippet>& snippets) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < snippets.size(); i++) {
        json << snippet_to_json(snippets[i]);
        if (i < snippets.size() - 1) json << ",";
    }
    json << "]";
    return json.str();
}

bool SnippetManager::save_snippet(const Snippet& snippet) {
    std::ofstream file(get_snippet_path(snippet.id));
    if (!file) return false;
    file << snippet_to_json(snippet);
    return true;
}

std::string SnippetManager::create_snippet(const std::string& title, const std::string& language,
                                           const std::string& code, const std::vector<std::string>& tags) {
    Snippet snippet;
    snippet.id = generate_id();
    snippet.title = title;
    snippet.language = language;
    snippet.code = code;
    snippet.tags = tags;
    snippet.created = std::time(nullptr);
    snippet.modified = snippet.created;
    
    if (!save_snippet(snippet)) {
        return "";
    }
    return snippet.id;
}

Snippet SnippetManager::get_snippet(const std::string& id) {
    Snippet snippet;
    std::ifstream file(get_snippet_path(id));
    if (!file) return snippet;
    
    // Simple JSON parsing (enough for our use case)
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    snippet.id = id;
    
    // Extract fields (naive but works for our controlled format)
    auto extract = [&](const std::string& key) -> std::string {
        size_t pos = content.find("\"" + key + "\":\"");
        if (pos == std::string::npos) return "";
        pos += key.length() + 4;
        size_t end = pos;
        while (end < content.length() && (content[end] != '"' || content[end-1] == '\\')) {
            end++;
        }
        std::string value = content.substr(pos, end - pos);
        // Unescape
        std::string unescaped;
        for (size_t i = 0; i < value.length(); i++) {
            if (value[i] == '\\' && i + 1 < value.length()) {
                switch (value[i+1]) {
                    case 'n': unescaped += '\n'; i++; break;
                    case 'r': unescaped += '\r'; i++; break;
                    case 't': unescaped += '\t'; i++; break;
                    case '"': unescaped += '"'; i++; break;
                    case '\\': unescaped += '\\'; i++; break;
                    default: unescaped += value[i];
                }
            } else {
                unescaped += value[i];
            }
        }
        return unescaped;
    };
    
    snippet.title = extract("title");
    snippet.language = extract("language");
    snippet.code = extract("code");
    
    // Extract tags array
    size_t tags_start = content.find("\"tags\":[");
    if (tags_start != std::string::npos) {
        size_t tags_end = content.find("]", tags_start);
        std::string tags_str = content.substr(tags_start + 8, tags_end - tags_start - 8);
        size_t pos = 0;
        while ((pos = tags_str.find("\"", pos)) != std::string::npos) {
            size_t end = tags_str.find("\"", pos + 1);
            if (end != std::string::npos) {
                snippet.tags.push_back(tags_str.substr(pos + 1, end - pos - 1));
                pos = end + 1;
            } else break;
        }
    }
    
    return snippet;
}

bool SnippetManager::update_snippet(const std::string& id, const std::string& title,
                                    const std::string& language, const std::string& code,
                                    const std::vector<std::string>& tags) {
    Snippet snippet = get_snippet(id);
    if (snippet.id.empty()) return false;
    
    snippet.title = title;
    snippet.language = language;
    snippet.code = code;
    snippet.tags = tags;
    snippet.modified = std::time(nullptr);
    
    return save_snippet(snippet);
}

bool SnippetManager::delete_snippet(const std::string& id) {
    return fs::remove(get_snippet_path(id));
}

std::vector<Snippet> SnippetManager::list_snippets() {
    std::vector<Snippet> snippets;
    
    for (const auto& entry : fs::directory_iterator(storage_path_)) {
        if (entry.path().extension() == ".json") {
            std::string id = entry.path().stem().string();
            Snippet snippet = get_snippet(id);
            if (!snippet.id.empty()) {
                snippets.push_back(snippet);
            }
        }
    }
    
    // Sort by modified time (newest first)
    std::sort(snippets.begin(), snippets.end(), 
              [](const Snippet& a, const Snippet& b) { return a.modified > b.modified; });
    
    return snippets;
}

std::vector<Snippet> SnippetManager::search_snippets(const std::string& query) {
    std::vector<Snippet> all = list_snippets();
    std::vector<Snippet> results;
    
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    
    for (const auto& snippet : all) {
        std::string lower_title = snippet.title;
        std::string lower_code = snippet.code;
        std::transform(lower_title.begin(), lower_title.end(), lower_title.begin(), ::tolower);
        std::transform(lower_code.begin(), lower_code.end(), lower_code.begin(), ::tolower);
        
        if (lower_title.find(lower_query) != std::string::npos ||
            lower_code.find(lower_query) != std::string::npos) {
            results.push_back(snippet);
        } else {
            // Check tags
            for (const auto& tag : snippet.tags) {
                std::string lower_tag = tag;
                std::transform(lower_tag.begin(), lower_tag.end(), lower_tag.begin(), ::tolower);
                if (lower_tag.find(lower_query) != std::string::npos) {
                    results.push_back(snippet);
                    break;
                }
            }
        }
    }
    
    return results;
}
