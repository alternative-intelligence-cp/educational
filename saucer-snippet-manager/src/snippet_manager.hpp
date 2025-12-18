#pragma once

#include <string>
#include <vector>
#include <expected>
#include <filesystem>

namespace snippet {

struct Snippet {
    std::string id;
    std::string title;
    std::string language;
    std::string code;
    std::string tags;
    long long created;
    long long modified;
};

struct ExecutionResult {
    std::string output;
    std::string error;
    int exit_code;
};

class SnippetManager {
public:
    explicit SnippetManager(const std::filesystem::path& storage_dir);

    // Snippet CRUD operations
    std::expected<Snippet, std::string> create_snippet(
        const std::string& title,
        const std::string& language,
        const std::string& code,
        const std::string& tags
    );

    std::expected<Snippet, std::string> get_snippet(const std::string& id);
    
    std::expected<std::vector<Snippet>, std::string> list_snippets();
    
    std::expected<Snippet, std::string> update_snippet(
        const std::string& id,
        const std::string& title,
        const std::string& language,
        const std::string& code,
        const std::string& tags
    );

    std::expected<bool, std::string> delete_snippet(const std::string& id);

    // Search functionality
    std::expected<std::vector<Snippet>, std::string> search_snippets(
        const std::string& query
    );

    // Code execution
    std::expected<ExecutionResult, std::string> execute_snippet(
        const std::string& id
    );

private:
    std::filesystem::path storage_dir_;
    
    std::filesystem::path get_snippet_path(const std::string& id) const;
    std::string generate_id() const;
    long long current_timestamp() const;
    
    std::expected<void, std::string> save_snippet(const Snippet& snippet);
    std::expected<Snippet, std::string> load_snippet(const std::filesystem::path& path);
};

} // namespace snippet
