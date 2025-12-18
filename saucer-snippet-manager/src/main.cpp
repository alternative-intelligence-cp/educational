#include <saucer/smartview.hpp>
#include <print>
#include <filesystem>

#include "snippet_manager.hpp"

// Helper to convert snippet to JSON for JS
std::string snippet_to_js_object(const snippet::Snippet& s) {
    std::ostringstream oss;
    oss << "{"
        << "\"id\":\"" << s.id << "\","
        << "\"title\":\"" << s.title << "\","
        << "\"language\":\"" << s.language << "\","
        << "\"code\":" << std::quoted(s.code) << ","
        << "\"tags\":\"" << s.tags << "\","
        << "\"created\":" << s.created << ","
        << "\"modified\":" << s.modified
        << "}";
    return oss.str();
}

coco::stray start(saucer::application *app) {
    auto window = saucer::window::create(app).value();
    auto webview = saucer::smartview<>::create({.window = window});
    
    window->set_title("Snippet Manager");
    window->set_size({.w = 1200, .h = 800});
    
    // Create snippet manager
    auto snippets_dir = std::filesystem::current_path() / "snippets";
    auto manager = std::make_shared<snippet::SnippetManager>(snippets_dir);
    
    // Expose C++ functions to JavaScript
    
    // List all snippets
    webview->expose("list_snippets", [manager]() -> coco::task<std::string> {
        auto result = manager->list_snippets();
        
        if (!result) {
            co_return std::string("{\"error\":\"") + result.error() + "\"}";
        }
        
        std::ostringstream json;
        json << "[";
        bool first = true;
        for (const auto& snippet : *result) {
            if (!first) json << ",";
            json << snippet_to_js_object(snippet);
            first = false;
        }
        json << "]";
        
        co_return json.str();
    });
    
    // Create snippet
    webview->expose("create_snippet", 
        [manager](const std::string& title, const std::string& language,
                  const std::string& code, const std::string& tags) 
        -> coco::task<std::string> {
        
        auto result = manager->create_snippet(title, language, code, tags);
        
        if (!result) {
            co_return std::string("{\"error\":\"") + result.error() + "\"}";
        }
        
        co_return snippet_to_js_object(*result);
    });
    
    // Get snippet
    webview->expose("get_snippet", [manager](const std::string& id) 
        -> coco::task<std::string> {
        
        auto result = manager->get_snippet(id);
        
        if (!result) {
            co_return std::string("{\"error\":\"") + result.error() + "\"}";
        }
        
        co_return snippet_to_js_object(*result);
    });
    
    // Update snippet
    webview->expose("update_snippet",
        [manager](const std::string& id, const std::string& title,
                  const std::string& language, const std::string& code,
                  const std::string& tags)
        -> coco::task<std::string> {
        
        auto result = manager->update_snippet(id, title, language, code, tags);
        
        if (!result) {
            co_return std::string("{\"error\":\"") + result.error() + "\"}";
        }
        
        co_return snippet_to_js_object(*result);
    });
    
    // Delete snippet
    webview->expose("delete_snippet", [manager](const std::string& id) 
        -> coco::task<std::string> {
        
        auto result = manager->delete_snippet(id);
        
        if (!result) {
            co_return std::string("{\"error\":\"") + result.error() + "\"}";
        }
        
        co_return "{\"success\":true}";
    });
    
    // Search snippets
    webview->expose("search_snippets", [manager](const std::string& query)
        -> coco::task<std::string> {
        
        auto result = manager->search_snippets(query);
        
        if (!result) {
            co_return std::string("{\"error\":\"") + result.error() + "\"}";
        }
        
        std::ostringstream json;
        json << "[";
        bool first = true;
        for (const auto& snippet : *result) {
            if (!first) json << ",";
            json << snippet_to_js_object(snippet);
            first = false;
        }
        json << "]";
        
        co_return json.str();
    });
    
    // Log message for debugging
    webview->expose("log", [](const std::string& message) {
        std::println("JS Log: {}", message);
    });
    
    // Load the frontend
    auto frontend_path = std::filesystem::current_path() / "frontend" / "index.html";
    auto url = saucer::url::from(frontend_path.string());
    
    if (!url.has_value()) {
        co_return std::println("Error: {}", url.error());
    }
    
    webview->set_url(url.value());
    window->show();
    
    co_await app->finish();
}

int main() {
    std::println("Starting Snippet Manager...");
    return saucer::application::create({.id = "snippet-manager"})->run(start);
}
