// Snippet Manager Frontend - Saucer Bridge Demo

let snippets = [];
let currentSnippet = null;
let isNewSnippet = false;

// Initialize the app
document.addEventListener('DOMContentLoaded', async () => {
    console.log('Snippet Manager initializing...');

    // Set up event listeners
    document.getElementById('newSnippetBtn').addEventListener('click', createNewSnippet);
    document.getElementById('saveBtn').addEventListener('click', saveSnippet);
    document.getElementById('deleteBtn').addEventListener('click', deleteSnippet);
    document.getElementById('cancelBtn').addEventListener('click', cancelEdit);
    document.getElementById('searchInput').addEventListener('input', handleSearch);

    // Load snippets from C++ backend
    await loadSnippets();

    setStatus('Ready');
});

// Load all snippets via saucer bridge
async function loadSnippets() {
    try {
        setStatus('Loading snippets...');

        // Call C++ function via saucer bridge
        const result = await saucer.exposed.list_snippets();
        snippets = JSON.parse(result);

        if (snippets.error) {
            setStatus('Error: ' + snippets.error);
            return;
        }

        renderSnippetList();
        updateSnippetCount();
        setStatus('Loaded ' + snippets.length + ' snippets');
    } catch (error) {
        console.error('Failed to load snippets:', error);
        setStatus('Error loading snippets');
    }
}

// Render the snippet list
function renderSnippetList() {
    const container = document.getElementById('snippetListContainer');

    if (snippets.length === 0) {
        container.innerHTML = '<p class="empty-state">No snippets yet. Create your first one!</p>';
        return;
    }

    container.innerHTML = snippets.map(snippet => `
        <div class="snippet-item ${currentSnippet && currentSnippet.id === snippet.id ? 'active' : ''}" 
             data-id="${snippet.id}"
             onclick="selectSnippet('${snippet.id}')">
            <div class="snippet-item-title">${escapeHtml(snippet.title)}</div>
            <div class="snippet-item-meta">
                <span class="snippet-item-language">${snippet.language}</span>
                ${snippet.tags ? `<span class="snippet-item-tags">${escapeHtml(snippet.tags)}</span>` : ''}
            </div>
        </div>
    `).join('');
}

// Select and display a snippet
async function selectSnippet(id) {
    try {
        const result = await saucer.exposed.get_snippet(id);
        const snippet = JSON.parse(result);

        if (snippet.error) {
            setStatus('Error: ' + snippet.error);
            return;
        }

        currentSnippet = snippet;
        isNewSnippet = false;

        displaySnippet(snippet);
        renderSnippetList(); // Re-render to update active state
    } catch (error) {
        console.error('Failed to get snippet:', error);
        setStatus('Error loading snippet');
    }
}

// Display snippet in editor
function displaySnippet(snippet) {
    document.getElementById('emptyEditor').classList.add('hidden');
    document.getElementById('editorContainer').classList.remove('hidden');

    document.getElementById('snippetTitle').value = snippet.title;
    document.getElementById('languageSelect').value = snippet.language;
    document.getElementById('codeEditor').value = snippet.code;
    document.getElementById('snippetTags').value = snippet.tags;
}

// Create new snippet
function createNewSnippet() {
    currentSnippet = null;
    isNewSnippet = true;

    document.getElementById('emptyEditor').classList.add('hidden');
    document.getElementById('editorContainer').classList.remove('hidden');

    document.getElementById('snippetTitle').value = '';
    document.getElementById('languageSelect').value = 'javascript';
    document.getElementById('codeEditor').value = '';
    document.getElementById('snippetTags').value = '';

    document.getElementById('snippetTitle').focus();
}

// Save snippet
async function saveSnippet() {
    const title = document.getElementById('snippetTitle').value.trim();
    const language = document.getElementById('languageSelect').value;
    const code = document.getElementById('codeEditor').value;
    const tags = document.getElementById('snippetTags').value.trim();

    if (!title) {
        alert('Please enter a title');
        return;
    }

    try {
        setStatus('Saving...');

        let result;
        if (isNewSnippet) {
            result = await saucer.exposed.create_snippet(title, language, code, tags);
        } else {
            result = await saucer.exposed.update_snippet(currentSnippet.id, title, language, code, tags);
        }

        const snippet = JSON.parse(result);

        if (snippet.error) {
            setStatus('Error: ' + snippet.error);
            alert('Failed to save: ' + snippet.error);
            return;
        }

        currentSnippet = snippet;
        isNewSnippet = false;

        await loadSnippets();
        selectSnippet(snippet.id);

        setStatus('Saved!');
    } catch (error) {
        console.error('Failed to save snippet:', error);
        setStatus('Error saving snippet');
        alert('Failed to save snippet');
    }
}

// Delete snippet
async function deleteSnippet() {
    if (!currentSnippet || isNewSnippet) {
        return;
    }

    if (!confirm(`Delete snippet "${currentSnippet.title}"?`)) {
        return;
    }

    try {
        setStatus('Deleting...');

        const result = await saucer.exposed.delete_snippet(currentSnippet.id);
        const response = JSON.parse(result);

        if (response.error) {
            setStatus('Error: ' + response.error);
            alert('Failed to delete: ' + response.error);
            return;
        }

        currentSnippet = null;

        document.getElementById('editorContainer').classList.add('hidden');
        document.getElementById('emptyEditor').classList.remove('hidden');

        await loadSnippets();
        setStatus('Deleted');
    } catch (error) {
        console.error('Failed to delete snippet:', error);
        setStatus('Error deleting snippet');
        alert('Failed to delete snippet');
    }
}

// Cancel editing
function cancelEdit() {
    currentSnippet = null;
    isNewSnippet = false;

    document.getElementById('editorContainer').classList.add('hidden');
    document.getElementById('emptyEditor').classList.remove('hidden');

    renderSnippetList();
}

// Search snippets
let searchTimeout;
async function handleSearch(event) {
    const query = event.target.value.trim();

    clearTimeout(searchTimeout);
    searchTimeout = setTimeout(async () => {
        try {
            setStatus('Searching...');

            const result = await saucer.exposed.search_snippets(query);
            snippets = JSON.parse(result);

            if (snippets.error) {
                setStatus('Error: ' + snippets.error);
                return;
            }

            renderSnippetList();
            updateSnippetCount();
            setStatus(query ? `Found ${snippets.length} results` : 'Ready');
        } catch (error) {
            console.error('Search failed:', error);
            setStatus('Search failed');
        }
    }, 300);
}

// Update snippet count in status bar
function updateSnippetCount() {
    document.getElementById('snippetCount').textContent =
        snippets.length + ' snippet' + (snippets.length !== 1 ? 's' : '');
}

// Set status message
function setStatus(message) {
    document.getElementById('statusText').textContent = message;
}

// HTML escape utility
function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Log to C++ console (for debugging)
async function logToCpp(message) {
    try {
        await saucer.exposed.log(message);
    } catch (error) {
        console.error('Failed to log to C++:', error);
    }
}

console.log('Snippet Manager frontend loaded');
