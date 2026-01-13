/**
 * SJS ECS Module - Entity Component System
 * 
 * A simple, data-oriented ECS for games. Entities are just IDs, components are data,
 * and systems process entities with specific component combinations.
 * 
 * This is NOT a full-featured ECS like Flecs or Bevy. It's the bare minimum to be useful.
 * Perfect for learning and small-to-medium games.
 * 
 * Features:
 * - Entity creation/destruction with ID reuse
 * - Component add/remove/get
 * - Component queries (get all entities with X, Y, Z components)
 * - System registration and execution
 * - Entity pools (use with slab allocator for performance)
 * 
 * Integration:
 * - Uses SJS pub/sub for entity events
 * - Uses SJS metrics for ECS stats
 * - Uses SJS logger for debugging
 * 
 * @module ecs
 */

(function (global) {
    'use strict';

    const ecs = {
        name: 'ecs',
        version: '1.0.0',
        dependencies: []
    };

    // ===== STATE =====
    let nextEntityId = 1;
    let entities = new Set();           // Set of active entity IDs
    let freeIds = [];                   // Recycled entity IDs
    let components = {};                // { componentName: Map(entityId -> componentData) }
    let systems = [];                   // Array of { name, query, fn, enabled, priority }
    let tags = {};                      // { tagName: Set(entityId) }
    let config = {
        poolSize: 1000,                 // Pre-allocate entity IDs
        emitEvents: true,
        trackStats: true
    };
    let stats = {
        entities: 0,
        components: 0,
        systems: 0
    };

    // ===== ENTITIES =====
    /**
     * Create an entity
     * @param {Object} componentData - { ComponentName: data, ... }
     * @returns {number} Entity ID
     */
    ecs.create = function (componentData = {}) {
        // Reuse freed ID or allocate new one
        const id = freeIds.length > 0 ? freeIds.pop() : nextEntityId++;
        entities.add(id);

        // Add components
        for (const [name, data] of Object.entries(componentData)) {
            ecs.add(id, name, data);
        }

        stats.entities++;
        emit('ecs:create', { id, components: Object.keys(componentData) });
        return id;
    };

    /**
     * Destroy an entity (removes all components, recycles ID)
     * @param {number} id - Entity ID
     */
    ecs.destroy = function (id) {
        if (!entities.has(id)) return;

        // Remove all components
        for (const name in components) {
            if (components[name].has(id)) {
                components[name].delete(id);
            }
        }

        // Remove all tags
        for (const tag in tags) {
            tags[tag].delete(id);
        }

        entities.delete(id);
        freeIds.push(id);
        stats.entities--;

        emit('ecs:destroy', { id });
    };

    /**
     * Check if entity exists
     */
    ecs.exists = function (id) {
        return entities.has(id);
    };

    /**
     * Destroy all entities
     */
    ecs.clear = function () {
        for (const id of entities) {
            ecs.destroy(id);
        }
    };

    // ===== COMPONENTS =====
    /**
     * Register a component type (optional, but recommended for clarity)
     * @param {string} name - Component name
     * @param {Object} defaultData - Default component data
     */
    ecs.registerComponent = function (name, defaultData = {}) {
        if (!components[name]) {
            components[name] = new Map();
        }
        components[name]._default = defaultData;
        log(`Registered component: ${name}`);
    };

    /**
     * Add a component to an entity
     * @param {number} id - Entity ID
     * @param {string} name - Component name
     * @param {Object} data - Component data
     */
    ecs.add = function (id, name, data = {}) {
        if (!entities.has(id)) {
            console.warn(`[ecs] Entity ${id} does not exist`);
            return;
        }

        if (!components[name]) {
            components[name] = new Map();
        }

        // Merge with default if registered
        const defaultData = components[name]._default || {};
        const componentData = { ...defaultData, ...data };

        components[name].set(id, componentData);
        stats.components++;

        emit('ecs:add', { id, component: name });
    };

    /**
     * Remove a component from an entity
     * @param {number} id - Entity ID
     * @param {string} name - Component name
     */
    ecs.remove = function (id, name) {
        if (components[name]) {
            const had = components[name].delete(id);
            if (had) {
                stats.components--;
                emit('ecs:remove', { id, component: name });
            }
        }
    };

    /**
     * Get a component from an entity
     * @param {number} id - Entity ID
     * @param {string} name - Component name
     * @returns {Object|undefined} Component data
     */
    ecs.get = function (id, name) {
        return components[name] ? components[name].get(id) : undefined;
    };

    /**
     * Check if entity has a component
     */
    ecs.has = function (id, name) {
        return components[name] ? components[name].has(id) : false;
    };

    /**
     * Check if entity has ALL listed components
     */
    ecs.hasAll = function (id, ...names) {
        return names.every(name => ecs.has(id, name));
    };

    /**
     * Check if entity has ANY listed components
     */
    ecs.hasAny = function (id, ...names) {
        return names.some(name => ecs.has(id, name));
    };

    // ===== QUERIES =====
    /**
     * Query entities with specific components
     * @param {...string} componentNames - Component names to match
     * @returns {number[]} Array of entity IDs
     */
    ecs.query = function (...componentNames) {
        if (componentNames.length === 0) {
            return Array.from(entities);
        }

        // Start with entities that have the first component
        const firstComponent = componentNames[0];
        if (!components[firstComponent]) return [];

        const candidates = Array.from(components[firstComponent].keys());

        // Filter candidates that have ALL components
        return candidates.filter(id =>
            componentNames.every(name => components[name] && components[name].has(id))
        );
    };

    /**
     * Iterate over entities with specific components
     * Callback receives (entityId, ...componentData)
     * 
     * @param {string[]} componentNames - Component names
     * @param {Function} callback - (id, comp1, comp2, ...) => void
     */
    ecs.each = function (componentNames, callback) {
        const ids = ecs.query(...componentNames);
        for (const id of ids) {
            const componentData = componentNames.map(name => ecs.get(id, name));
            callback(id, ...componentData);
        }
    };

    // ===== TAGS (components without data) =====
    /**
     * Add a tag to an entity (lightweight marker)
     */
    ecs.tag = function (id, tagName) {
        if (!tags[tagName]) tags[tagName] = new Set();
        tags[tagName].add(id);
    };

    /**
     * Remove a tag from an entity
     */
    ecs.untag = function (id, tagName) {
        if (tags[tagName]) tags[tagName].delete(id);
    };

    /**
     * Check if entity has a tag
     */
    ecs.hasTag = function (id, tagName) {
        return tags[tagName] ? tags[tagName].has(id) : false;
    };

    /**
     * Get all entities with a tag
     */
    ecs.queryTag = function (tagName) {
        return tags[tagName] ? Array.from(tags[tagName]) : [];
    };

    // ===== SYSTEMS =====
    /**
     * Register a system
     * @param {string} name - System name
     * @param {string[]} query - Component names this system processes
     * @param {Function} fn - System function (dt, entities) => void
     * @param {Object} options - { priority, enabled }
     */
    ecs.registerSystem = function (name, query, fn, options = {}) {
        const system = {
            name,
            query,
            fn,
            enabled: options.enabled !== false,
            priority: options.priority || 0
        };

        systems.push(system);
        systems.sort((a, b) => b.priority - a.priority); // Higher priority = runs first

        stats.systems++;
        log(`Registered system: ${name}`, { query, priority: system.priority });
    };

    /**
     * Enable/disable a system
     */
    ecs.setSystemEnabled = function (name, enabled) {
        const system = systems.find(s => s.name === name);
        if (system) system.enabled = enabled;
    };

    /**
     * Run all enabled systems
     * @param {number} dt - Delta time (seconds)
     */
    ecs.update = function (dt) {
        for (const system of systems) {
            if (!system.enabled) continue;

            const entities = ecs.query(...system.query);
            system.fn(dt, entities);
        }

        emit('ecs:update', { dt, systems: systems.filter(s => s.enabled).length });
    };

    /**
     * Run a specific system
     */
    ecs.runSystem = function (name, dt) {
        const system = systems.find(s => s.name === name);
        if (!system || !system.enabled) return;

        const entities = ecs.query(...system.query);
        system.fn(dt, entities);
    };

    // ===== PREFABS (entity templates) =====
    const prefabs = {};

    /**
     * Define a prefab (entity template)
     * @param {string} name - Prefab name
     * @param {Object} components - Component data
     */
    ecs.definePrefab = function (name, components) {
        prefabs[name] = components;
    };

    /**
     * Spawn an entity from a prefab
     * @param {string} name - Prefab name
     * @param {Object} overrides - Override component data
     * @returns {number} Entity ID
     */
    ecs.spawn = function (name, overrides = {}) {
        const prefab = prefabs[name];
        if (!prefab) {
            console.warn(`[ecs] Prefab not found: ${name}`);
            return -1;
        }

        const components = { ...prefab, ...overrides };
        return ecs.create(components);
    };

    // ===== UTILITIES =====
    ecs.getEntities = () => Array.from(entities);
    ecs.getComponents = () => Object.keys(components);
    ecs.getSystems = () => systems.map(s => ({ name: s.name, enabled: s.enabled }));
    ecs.getStats = () => ({ ...stats });

    /**
     * Debug: dump entity info
     */
    ecs.inspect = function (id) {
        if (!entities.has(id)) {
            console.log(`Entity ${id} does not exist`);
            return;
        }

        const entityComponents = {};
        for (const [name, map] of Object.entries(components)) {
            if (map.has(id)) {
                entityComponents[name] = map.get(id);
            }
        }

        const entityTags = [];
        for (const [tagName, tagSet] of Object.entries(tags)) {
            if (tagSet.has(id)) {
                entityTags.push(tagName);
            }
        }

        console.log(`Entity ${id}:`, {
            components: entityComponents,
            tags: entityTags
        });
    };

    // ===== HELPER FUNCTIONS =====
    function emit(event, data) {
        if (!config.emitEvents) return;
        if (global.SJS && global.SJS.pubSub) {
            global.SJS.pubSub.publish(event, data);
        }
    }

    function log(message, data) {
        if (global.SJS && global.SJS.logger) {
            global.SJS.logger.debug('[ecs] ' + message, data);
        }
    }

    // ===== EXPORT =====
    ecs.configure = function (options) {
        Object.assign(config, options);
    };

    ecs.getConfig = function () {
        return { ...config };
    };

    // UMD export
    if (typeof module !== 'undefined' && module.exports) {
        module.exports = ecs;
    } else if (typeof define === 'function' && define.amd) {
        define([], function () { return ecs; });
    } else {
        global.SJS = global.SJS || {};
        global.SJS.ecs = ecs;
    }

})(typeof globalThis !== 'undefined' ? globalThis : this);
