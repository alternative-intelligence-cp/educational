-- Concept Evolution Tracker Database Schema

-- Core concept being understood
CREATE TABLE concepts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    tags TEXT  -- JSON array of tags
);

-- Snapshot of understanding at a point in time
CREATE TABLE understanding_states (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    concept_id INTEGER NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    description TEXT NOT NULL,
    confidence INTEGER CHECK(confidence >= 0 AND confidence <= 100),
    model_type TEXT CHECK(model_type IN ('technical', 'intuitive', 'hybrid', 'confused')),
    FOREIGN KEY(concept_id) REFERENCES concepts(id)
);

-- Event that caused understanding to evolve
CREATE TABLE evolution_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    concept_id INTEGER NOT NULL,
    from_state_id INTEGER,  -- NULL if first understanding
    to_state_id INTEGER NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    trigger_type TEXT NOT NULL CHECK(trigger_type IN ('context', 'conversation', 'code', 'error', 'personal_experience', 'documentation')),
    trigger_content TEXT,
    shift_description TEXT NOT NULL,
    magnitude INTEGER CHECK(magnitude >= 0 AND magnitude <= 100),
    FOREIGN KEY(concept_id) REFERENCES concepts(id),
    FOREIGN KEY(from_state_id) REFERENCES understanding_states(id),
    FOREIGN KEY(to_state_id) REFERENCES understanding_states(id)
);

-- Context that can trigger evolution
CREATE TABLE contexts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    source_type TEXT NOT NULL CHECK(source_type IN ('conversation', 'code', 'documentation', 'error_message', 'experiment')),
    content TEXT NOT NULL,
    extracted_concepts TEXT  -- JSON array of concept IDs
);

-- Link contexts to evolution events they triggered
CREATE TABLE context_triggers (
    context_id INTEGER NOT NULL,
    event_id INTEGER NOT NULL,
    PRIMARY KEY(context_id, event_id),
    FOREIGN KEY(context_id) REFERENCES contexts(id),
    FOREIGN KEY(event_id) REFERENCES evolution_events(id)
);

-- Indexes for common queries
CREATE INDEX idx_states_concept ON understanding_states(concept_id, timestamp);
CREATE INDEX idx_events_concept ON evolution_events(concept_id, timestamp);
CREATE INDEX idx_events_magnitude ON evolution_events(magnitude DESC);
CREATE INDEX idx_contexts_timestamp ON contexts(timestamp);

-- Views for common queries

-- Latest understanding of each concept
CREATE VIEW latest_understanding AS
SELECT 
    c.id as concept_id,
    c.name as concept_name,
    s.description,
    s.confidence,
    s.model_type,
    s.timestamp
FROM concepts c
JOIN understanding_states s ON c.id = s.concept_id
WHERE s.id = (
    SELECT id FROM understanding_states 
    WHERE concept_id = c.id 
    ORDER BY timestamp DESC 
    LIMIT 1
);

-- Evolution timeline (all shifts)
CREATE VIEW evolution_timeline AS
SELECT
    c.name as concept,
    e.timestamp,
    e.shift_description,
    e.magnitude,
    e.trigger_type,
    s_from.description as from_understanding,
    s_to.description as to_understanding
FROM evolution_events e
JOIN concepts c ON e.concept_id = c.id
LEFT JOIN understanding_states s_from ON e.from_state_id = s_from.id
JOIN understanding_states s_to ON e.to_state_id = s_to.id
ORDER BY e.timestamp DESC;

-- Major breakthroughs (high magnitude shifts)
CREATE VIEW breakthroughs AS
SELECT * FROM evolution_timeline
WHERE magnitude >= 70
ORDER BY timestamp DESC;
