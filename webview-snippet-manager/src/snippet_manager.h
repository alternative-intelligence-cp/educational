#pragma once

#include <string>
#include <vector>
#include <ctime>

struct Snippet {
    std::string id;
    std::string title;
    std::string language;
    std::string code;
    std::vector<std::string> tags;
    time_t created;
    time_t modified;
};

class SnippetManager {
public:
    SnippetManager(const std::string& storage_path);
    
    // CRUD operations
    std::string create_snippet(const std::string& title, const std::string& language, 
                               const std::string& code, const std::vector<std::string>& tags);
    Snippet get_snippet(const std::string& id);
    bool update_snippet(const std::string& id, const std::string& title, 
                       const std::string& language, const std::string& code,
                       const std::vector<std::string>& tags);
    bool delete_snippet(const std::string& id);
    std::vector<Snippet> list_snippets();
    std::vector<Snippet> search_snippets(const std::string& query);
    
    // Serialization helpers
    std::string snippet_to_json(const Snippet& snippet);
    std::string snippets_to_json(const std::vector<Snippet>& snippets);
    std::vector<std::string> parse_tags(const std::string& tags_str);
    
private:
    std::string storage_path_;
    std::string generate_id();
    std::string get_snippet_path(const std::string& id);
    bool save_snippet(const Snippet& snippet);
    std::string escape_json(const std::string& str);
};
