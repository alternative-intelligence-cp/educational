#include "webview/webview.h"
#include "snippet_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Helper to load HTML file
std::string load_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "";
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

int main() {
    try {
        // Create snippet manager
        SnippetManager manager("./snippets");
        
        // Create webview
        webview::webview w(true, nullptr);
        w.set_title("Snippet Manager");
        w.set_size(1200, 800, WEBVIEW_HINT_NONE);
        
        // Bind: list all snippets
        w.bind("list_snippets", [&](const std::string& req) -> std::string {
            std::cout << "[C++] list_snippets called, req='" << req << "'" << std::endl;
            auto snippets = manager.list_snippets();
            std::string result = manager.snippets_to_json(snippets);
            std::cout << "[C++] Returning: " << result.substr(0, 100) << "..." << std::endl;
            return result;
        });
        
        // Bind: get single snippet
        w.bind("get_snippet", [&](const std::string& id_json) -> std::string {
            // Extract ID from JSON array: ["id"]
            std::string id = id_json.substr(2, id_json.length() - 4);
            auto snippet = manager.get_snippet(id);
            if (snippet.id.empty()) {
                return "{\"error\":\"Snippet not found\"}";
            }
            return manager.snippet_to_json(snippet);
        });
        
        // Bind: create snippet
        w.bind("create_snippet", [&](const std::string& params_json) -> std::string {
            // Parse params: ["title","language","code","tag1,tag2"]
            std::vector<std::string> params;
            size_t pos = 1; // Skip opening bracket
            while (pos < params_json.length()) {
                if (params_json[pos] == '"') {
                    size_t end = pos + 1;
                    std::string param;
                    while (end < params_json.length() && !(params_json[end] == '"' && params_json[end-1] != '\\')) {
                        if (params_json[end] == '\\' && end + 1 < params_json.length()) {
                            switch (params_json[end+1]) {
                                case 'n': param += '\n'; end += 2; break;
                                case 'r': param += '\r'; end += 2; break;
                                case 't': param += '\t'; end += 2; break;
                                case '"': param += '"'; end += 2; break;
                                case '\\': param += '\\'; end += 2; break;
                                default: param += params_json[end++];
                            }
                        } else {
                            param += params_json[end++];
                        }
                    }
                    params.push_back(param);
                    pos = end + 1;
                }
                pos++;
            }
            
            if (params.size() < 4) {
                return "{\"error\":\"Invalid parameters\"}";
            }
            
            auto tags = manager.parse_tags(params[3]);
            std::string id = manager.create_snippet(params[0], params[1], params[2], tags);
            
            if (id.empty()) {
                return "{\"error\":\"Failed to create snippet\"}";
            }
            return "{\"id\":\"" + id + "\"}";
        });
        
        // Bind: update snippet
        w.bind("update_snippet", [&](const std::string& params_json) -> std::string {
            // Parse params: ["id","title","language","code","tag1,tag2"]
            std::vector<std::string> params;
            size_t pos = 1;
            while (pos < params_json.length()) {
                if (params_json[pos] == '"') {
                    size_t end = pos + 1;
                    std::string param;
                    while (end < params_json.length() && !(params_json[end] == '"' && params_json[end-1] != '\\')) {
                        if (params_json[end] == '\\' && end + 1 < params_json.length()) {
                            switch (params_json[end+1]) {
                                case 'n': param += '\n'; end += 2; break;
                                case 'r': param += '\r'; end += 2; break;
                                case 't': param += '\t'; end += 2; break;
                                case '"': param += '"'; end += 2; break;
                                case '\\': param += '\\'; end += 2; break;
                                default: param += params_json[end++];
                            }
                        } else {
                            param += params_json[end++];
                        }
                    }
                    params.push_back(param);
                    pos = end + 1;
                }
                pos++;
            }
            
            if (params.size() < 5) {
                return "{\"error\":\"Invalid parameters\"}";
            }
            
            auto tags = manager.parse_tags(params[4]);
            bool success = manager.update_snippet(params[0], params[1], params[2], params[3], tags);
            
            return success ? "{\"success\":true}" : "{\"error\":\"Failed to update snippet\"}";
        });
        
        // Bind: delete snippet
        w.bind("delete_snippet", [&](const std::string& id_json) -> std::string {
            std::string id = id_json.substr(2, id_json.length() - 4);
            bool success = manager.delete_snippet(id);
            return success ? "{\"success\":true}" : "{\"error\":\"Failed to delete snippet\"}";
        });
        
        // Bind: search snippets
        w.bind("search_snippets", [&](const std::string& query_json) -> std::string {
            std::string query = query_json.substr(2, query_json.length() - 4);
            auto snippets = manager.search_snippets(query);
            return manager.snippets_to_json(snippets);
        });
        
        // Bind: logging for debugging
        w.bind("log", [](const std::string& msg_json) -> std::string {
            std::string msg = msg_json.substr(2, msg_json.length() - 4);
            std::cout << "[JS] " << msg << std::endl;
            return "{}";
        });
        
        // Load HTML
        std::string html = load_file("./frontend/index.html");
        if (html.empty()) {
            std::cerr << "Failed to load frontend/index.html" << std::endl;
            return 1;
        }
        
        w.set_html(html);
        w.run();
        
    } catch (const webview::exception& e) {
        std::cerr << "Webview error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
