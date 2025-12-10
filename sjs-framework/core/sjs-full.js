/*
SJS App System (Reviewed + Fixed + Enhanced)

Summary of fixes and additions (embedded as comments for easy copy/paste):
- Bootstrapper.loadJSON: added Node.js fs fallback to read JSON files when fetch/import isn't suitable.
- Loader robustness: preserved prior fallbacks; clarified warnings where module exports aren't accessible when loading via <script>.
- Storage (IndexedDB): fixed onupgradeneeded to only create object store without invalid transaction usage.
- Added a new module: http
  - A fetch-based HTTP client with:
    - baseURL, default headers, timeout, retries with backoff+jitter
    - JSON/text/arrayBuffer/blob/Response parsing
    - GET caching via the cache module (optional), and pubSub events for request/response/error
    - simple before/after interceptors
  - Methods: configure, request, get, post, put, patch, del, head
  - Events: "http:request", "http:response", "http:error"
  - Cache integration example added at the bottom.
  - FIX: avoid variable shadowing of "cache" option by renaming internal module variable to cacheModule and option to cacheFlag.
  - FIX: per-attempt timeout/abort handling (each retry gets its own AbortController and timer; user-provided AbortSignal is bridged to attempt controller).
- NEW MODULE: input
  - Keyboard + mouse listener that maintains a live set/array of currently pressed keys and mouse buttons
  - Query API: keysDown(), isKeyDown(key), buttonsDown(), isButtonDown(button), getMouse(), clear()
  - Configurable: capture phase, preventDefault, stopPropagation, ignore editable inputs, use key or code, attach to element/window
  - PubSub integration: broadcasts "input:*" events ("input:keydown", "input:keyup", "input:mousedown", "input:mouseup", "input:mousemove", "input:wheel", and pointer equivalents)
  - Browser-safety: no-ops in non-browser environments
  - Bug-avoidance in input handling:
    - Clears stuck keys on blur/visibilitychange
    - Ignores repeated keydown for held keys (repeat flag)
    - Optional dedupe of pointer-then-mouse duplicate events
- NEW MODULE: metrics
  - Lightweight metrics collector (counters, gauges, histograms, timers), with optional auto-capture of Long Tasks and Resource timings.
  - Integrations: pubSub events for changes and flushes, optional persistence via storage (namespace "metrics"), optional HTTP flush to an endpoint.
  - API:
    - configure({ enabled, defaultBuckets, persist, namespace, exportIntervalMs, httpEndpoint, httpHeaders, autoLongTask, autoResource })
    - counterInc(name, value=1, labels?)
    - gaugeSet(name, value, labels?), gaugeInc(name, delta=1, labels?), gaugeDec(name, delta=1, labels?)
    - histObserve(name, value, labels?, buckets?)
    - timeStart(name, labels?) -> token; timeEnd(tokenOrName, labels?) -> durationMs
    - timeAsync(name, fn, labels?) -> value
    - mark(name), measure(name, startMark, endMark?)
    - snapshot(reset=false)
    - reset()
    - flush()  // persists and/or POSTs snapshot when configured
    - start(), stop()  // starts or stops auto capture/flush
  - Events: "metrics:counter", "metrics:gauge", "metrics:histogram", "metrics:timer", "metrics:flush"
- NEW MODULE: router (with template engine)
  - Client-side router (history/hash/memory) + micro template engine (Underscore-style)
  - Features:
    - addRoute({ name, path, template, templateId, templateSelector, templateURL, render, load, beforeEnter, afterEnter, onLeave, title })
    - navigate(to, { replace, state, query }), back(), forward(), start(), stop()
    - build(name, params, query)
    - link interception (configurable selector, same-origin guard), popstate/hashchange handling
    - basePath, scroll behavior, active link class, trailing slash normalization
    - pubSub events: "router:before", "router:after", "router:navigate", "router:render", "router:error", "template:render"
    - Template engine:
      - router.template.compile(text, options?)  // <% %> evaluate, <%= %> interpolate, <%- %> escape
      - router.template.render(nameOrText, data)
      - router.template.register(name, text), unregister(name), has(name), fromDOM(selectorOrId), fromURL(url)
    - Integrations:
      - If SJS.config.router.autoStart is true, auto-starts in boot()
      - Optional template fetch through http module (if present)
  - Config (SJS.config.router):
      {
        "enabled": true,
        "mode": "history",               // "history" | "hash" | "memory"
        "basePath": "",                  // base path for history mode
        "root": "#app",                  // selector or HTMLElement where render() injects HTML
        "autoStart": true,               // auto-start on boot
        "linksSelector": "a[href]",      // anchor interception selector
        "linkActiveClass": "active",     // CSS class for active links
        "scrollBehavior": "auto",        // "auto" | "top" | "restore"
        "notFoundTemplate": "<h1>404</h1>", // fallback template if no route matched
        "trailingSlash": "ignore",       // "ignore" | "always" | "never"
        "emitPubSub": true,              // publish pubSub events
        "decodeParams": true             // decodeURIComponent for :params
      }
*/

/* 
Example config.json
{
  "loadManifest": "./manifest.js",
  "main": "./main.js",
  "mainArgs": ["hello from config", 42],
  "storage": {
    "driver": "auto",                  // "auto" | "idb" | "localStorage" | "fs" | "memory"
    "namespace": "app",                // default namespace
    "idbName": "SJS_KV",               // IndexedDB DB name
    "idbStore": "kv",                  // IndexedDB object store name
    "localStoragePrefix": "sjs:",      // prefix for localStorage keys
    "filePath": "./sjs_store.json",    // Node.js FS driver JSON file path
    "broadcast": true                  // Broadcast cross-context changes when available (BroadcastChannel/localStorage events)
  },
  "input": {
    "enabled": true,
    "capture": false,
    "preventDefault": false,
    "stopPropagation": false,
    "ignoreEditable": true,
    "useCode": false,
    "trackMouse": true,
    "trackPointer": true,
    "trackWheel": true,
    "dedupePointerMouse": true
  },
  "metrics": {
    "enabled": true,
    "persist": true,
    "namespace": "metrics",
    "exportIntervalMs": 60000,
    "autoLongTask": true,
    "autoResource": false,
    "httpEndpoint": null,
    "httpHeaders": { "Content-Type": "application/json" }
  },
  "router": {
    "enabled": true,
    "mode": "history",                  // "history" | "hash" | "memory"
    "basePath": "",                     // e.g. "/app"
    "root": "#app",                     // selector or HTMLElement for render target
    "autoStart": true,                  // start router automatically on boot
    "linksSelector": "a[href]",
    "linkActiveClass": "active",
    "scrollBehavior": "auto",           // "auto" | "top" | "restore"
    "notFoundTemplate": "<h1>404 - Not Found</h1>",
    "trailingSlash": "ignore",          // "ignore" | "always" | "never"
    "emitPubSub": true,
    "decodeParams": true
  }
}

Example manifest.js
export default function buildManifest(SJS, runner) {
  const m = new runner.Manifest("app");
  // Load an external script/module by URL/path (runner will dynamic import it)
  m.addEntry(new runner.ManifestEntry("lib", "./lib.js", []));
  // Or run inline functions
  m.addEntry(new runner.ManifestEntry("init", () => {
    console.log("init after lib");
  }, ["lib"]));
  return m;
}

Example main.js
export default async function main(SJS, greeting, number) {
  console.log("Main started with:", greeting, number);
  const pubSub = SJS.modules.get("pubSub");
  if (pubSub) {
    pubSub.publish("greet", { msg: `from main: ${greeting}` });
  }

  // Example: storage usage
  const storage = SJS.modules.get("storage");
  if (storage) {
    await storage.set("welcome", { greeting, number }, { ttlMs: 24 * 60 * 60 * 1000 });
    const v = await storage.get("welcome");
    console.log("Stored welcome:", v);
  }

  // Do any app startup here...
  return true;
}

How to start
- Option A (auto-boot): define globalThis.configFileSrc = "./config.json" before this script is executed.
- Option B (manual): after this script loads, call globalThis.SJS.boot("./config.json") or pass an inline object:
  SJS.boot({ loadManifest: "./manifest.js", main: "./main.js", mainArgs: ["hi", 1] });

Notes
- loadManifest can be:
  - a string path/URL to a JS module that exports a default function (SJS, runner) => Manifest, or exports { manifest }
  - a string path/URL to a JSON file of shape: { id?: string, entries: [{ id, src, depends?: string[] }] }
  - an inline JSON object with the same shape as above
- main can be:
  - a function
  - a string path/URL to a JS module exporting default or named export "main"
- mainArgs must be an array, passed to main via spread
*/

/* SJS Bootstrapper: reads config, runs manifest first, then main(...mainArgs) */
(function (env, configSrc) {
    env.SJS = env.SJS || {};
    const SJS = env.SJS;

    function isBrowser() {
        return typeof window !== "undefined" && typeof document !== "undefined";
    }
    function isNode() {
        return typeof process !== "undefined" && !!process.versions && !!process.versions.node;
    }
    function looksLikeFilePath(p) {
        return typeof p === "string" && !/^https?:\/\//i.test(p) && !/^data:/.test(p);
    }

    // FIX: loadJSON now has Node fs fallback; keeps fetch first in browsers; safe import() as last resort.
    async function loadJSON(path) {
        if (typeof path !== "string" || path.trim().length === 0) {
            throw new Error("loadJSON: invalid path");
        }
        let lastErr = null;

        // Prefer fetch in environments that have it and when URL is http(s) or data:
        if (typeof fetch === "function" && (/^https?:\/\//i.test(path) || isBrowser())) {
            try {
                const res = await fetch(path, { cache: "no-cache" });
                if (!res.ok) throw new Error(`Failed to fetch JSON: ${path} (${res.status})`);
                return await res.json();
            } catch (e) {
                lastErr = e;
            }
        }

        // Node.js: use fs for relative/absolute file paths
        if (isNode() && looksLikeFilePath(path)) {
            try {
                const fs = await import('node:fs/promises');
                const txt = await fs.readFile(path, 'utf8');
                return JSON.parse(txt);
            } catch (e) {
                lastErr = e;
            }
        }

        // As a fallback, try import(). Works for JSON only in some environments/bundlers.
        if (typeof import !== "undefined") {
            try {
                const mod = await import(/* @vite-ignore */ path);
                return mod?.default ?? mod;
            } catch (e) {
                lastErr = e;
            }
        }

        const err = new Error(`loadJSON: could not load "${path}" via fetch/fs/import in this environment`);
        if (lastErr) err.cause = lastErr;
        throw err;
    }

    // PATCH: add fallback to <script type="module"> if dynamic import() rejects (e.g., CORS or network error)
    async function loadModule(path) {
        if (typeof path !== "string" || path.trim().length === 0) {
            throw new Error("loadModule: invalid path");
        }
        if (typeof import !== "undefined") {
            try {
                return await import(/* @vite-ignore */ path);
            } catch (e) {
                if (isBrowser()) {
                    return await new Promise((resolve, reject) => {
                        const el = document.createElement("script");
                        el.type = "module";
                        el.src = path;
                        el.async = true;
                        el.onload = () => {
                            // NOTE: module exports are not available via globals. Return empty object.
                            console.warn(`loadModule: loaded "${path}" via <script type="module">; exports are not accessible to JS.`);
                            resolve({});
                        };
                        el.onerror = () => reject(new Error(`Failed to load module ${path}`));
                        document.head.appendChild(el);
                    });
                }
                throw e;
            }
        }
        if (isBrowser()) {
            return new Promise((resolve, reject) => {
                const el = document.createElement("script");
                el.type = "module";
                el.src = path;
                el.async = true;
                el.onload = () => {
                    console.warn(`loadModule: loaded "${path}" via <script type="module">; exports are not accessible to JS.`);
                    resolve({});
                };
                el.onerror = () => reject(new Error(`Failed to load module ${path}`));
                document.head.appendChild(el);
            });
        }
        throw new Error(`loadModule: cannot import "${path}" in this environment`);
    }

    function asArray(x) {
        return Array.isArray(x) ? x : (x == null ? [] : [x]);
    }

    async function buildManifestFromJSON(json, runner) {
        const m = new runner.Manifest(json.id || "manifest");
        const entries = Array.isArray(json.entries) ? json.entries : [];
        for (const e of entries) {
            if (!e || typeof e.id !== "string") {
                throw new Error("Manifest JSON: each entry must have an id");
            }
            const depends = asArray(e.depends);
            const entry = new runner.ManifestEntry(e.id, e.src, depends);
            m.addEntry(entry);
        }
        return m;
    }

    async function loadManifest(loadManifest, runner) {
        if (!loadManifest) return null;

        if (typeof loadManifest === "string") {
            if (/\.(json|jsonc)$/i.test(loadManifest)) {
                const json = await loadJSON(loadManifest);
                return buildManifestFromJSON(json, runner);
            }
            const mod = await loadModule(loadManifest);
            if (typeof mod?.default === "function") {
                return await mod.default(SJS, runner);
            }
            if (mod?.manifest) {
                return mod.manifest;
            }
            if (mod && typeof mod === "object") {
                return buildManifestFromJSON(mod.default ?? mod, runner);
            }
            throw new Error(`Manifest module "${loadManifest}" did not export a default builder or 'manifest'`);
        }

        if (typeof loadManifest === "object") {
            return buildManifestFromJSON(loadManifest, runner);
        }

        throw new Error("loadManifest: unsupported type");
    }

    async function loadMain(main) {
        if (!main) return null;

        if (typeof main === "function") return main;

        if (typeof main === "string") {
            const mod = await loadModule(main);
            return mod?.default || mod?.main || null;
        }

        return null;
    }

    async function boot(config) {
        const runner = SJS.modules?.get?.("runner");
        if (!runner) {
            throw new Error("SJS runner module is not available");
        }

        // Expose config for modules that want to auto-configure
        SJS.config = config;

        if (config.loadManifest) {
            const manifest = await loadManifest(config.loadManifest, runner);
            if (manifest) {
                await runner.run(manifest).catch((e) => {
                    console.error("Manifest run error:", e);
                });
            }
        }

        // If a storage module is present and config.storage is provided, configure it early
        try {
            const storage = SJS.modules?.get?.("storage");
            if (storage && config.storage) {
                storage.configure(config.storage);
            }
        } catch (e) {
            console.warn("storage configuration failed:", e);
        }

        // Configure input module early if present
        try {
            const input = SJS.modules?.get?.("input");
            if (input && config.input) {
                input.configure(config.input);
            }
        } catch (e) {
            console.warn("input configuration failed:", e);
        }

        // Configure metrics module early if present
        try {
            const metrics = SJS.modules?.get?.("metrics");
            if (metrics && config.metrics) {
                metrics.configure(config.metrics);
                if (config.metrics.enabled) metrics.start();
            }
        } catch (e) {
            console.warn("metrics configuration failed:", e);
        }

        // Configure router module early if present
        try {
            const router = SJS.modules?.get?.("router");
            if (router && config.router) {
                router.configure(config.router);
                if (config.router.autoStart) router.start();
            }
        } catch (e) {
            console.warn("router configuration failed:", e);
        }

        const mainFn = await loadMain(config.main);
        if (typeof mainFn !== "function") {
            throw new Error("main could not be loaded or is not a function");
        }
        const args = Array.isArray(config.mainArgs) ? config.mainArgs : [];
        return await Promise.resolve(mainFn(SJS, ...args));
    }

    SJS.boot = async function SJSBoot(configSource) {
        let configObj = null;
        if (typeof configSource === "string") {
            configObj = await loadJSON(configSource);
        } else if (configSource && typeof configSource === "object") {
            configObj = configSource;
        } else {
            throw new Error("SJS.boot: invalid config source");
        }
        return boot(configObj);
    };

    // Auto-boot if global configFileSrc is provided
    if (typeof configSrc !== "undefined" && configSrc != null) {
        SJS.boot(configSrc).catch((e) => console.error("SJS boot error:", e));
    }
})(globalThis, typeof configFileSrc !== "undefined" ? configFileSrc : undefined);

/* Module manager with fixed error messages */
(function (SJS) {
    SJS = SJS || (globalThis.SJS = {});
    const modules = {};

    function get(id) {
        if (typeof id !== 'string' || id.trim().length === 0) {
            console.error("ERROR! Invalid id.");
            return null;
        }
        if (Object.prototype.hasOwnProperty.call(modules, id)) {
            return modules[id];
        }
        console.error(`ERROR! No module with id ${id} found!`);
        return null;
    }

    function add(id, module) {
        if (
            typeof id !== 'string' || id.trim().length === 0 ||
            module == null
        ) {
            console.error("ERROR! Invalid arguments!");
            return false;
        }
        if (Object.prototype.hasOwnProperty.call(modules, id)) {
            console.error(`ERROR! Could not add module with id ${id}! A module with that id already exists!`);
            return false;
        }
        modules[id] = module;
        return true;
    }

    function list() {
        return Object.keys(modules);
    }

    SJS.modules = Object.freeze({ get, add, list });
})(globalThis.SJS);

// runner - runs scripts in order with dependency resolution
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }

    //===== HELPERS =============================================================
    function isBrowser() {
        return typeof window !== "undefined" && typeof document !== "undefined";
    }

    function runSrc(entry) {
        // Supports:
        // - function (sync or returns Promise)
        // - string URL/path: in Node/ESM via dynamic import; in browser via script tag
        const src = entry.src;

        if (typeof src === "function") {
            return Promise.resolve().then(() => src());
        }

        if (typeof src === "string") {
            if (!src) {
                return Promise.reject(new Error(`Entry ${entry.id} has empty src string`));
            }

            // Try dynamic import when available (Node/ESM or modern browsers)
            if (typeof import !== "undefined") {
                // PATCH: correctly fallback to script tag if dynamic import() rejects
                return Promise.resolve()
                    .then(() => import(/* @vite-ignore */ src))
                    .catch((err) => {
                        if (isBrowser()) {
                            return new Promise((resolve, reject) => {
                                const el = document.createElement("script");
                                el.src = src;
                                el.async = true;
                                el.onload = () => resolve(true);
                                el.onerror = () => reject(new Error(`Failed to load script ${src}`));
                                document.head.appendChild(el);
                            });
                        }
                        return Promise.reject(err);
                    });
            }

            if (isBrowser()) {
                return new Promise((resolve, reject) => {
                    const el = document.createElement("script");
                    el.src = src;
                    el.async = true;
                    el.onload = () => resolve(true);
                    el.onerror = (e) => reject(new Error(`Failed to load script ${src}`));
                    document.head.appendChild(el);
                });
            }

            return Promise.reject(new Error(`Cannot load src "${src}" in this environment.`));
        }

        return Promise.reject(new Error(`Unsupported src type for entry ${entry.id}`));
    }

    //===== API =================================================================
    function ManifestEntry(id, src, dependsArray = []) {
        if (typeof id !== "string" || id.trim().length === 0) {
            throw new Error("ManifestEntry: invalid id");
        }
        if (src == null) {
            throw new Error("ManifestEntry: src is required (function or string)");
        }
        if (!Array.isArray(dependsArray)) {
            throw new Error("ManifestEntry: dependsArray must be an array");
        }
        this.id = id;
        this.src = src; // function or string
        // Remove duplicates and normalize to ids or srcs as given.
        this.depends = Array.from(new Set(dependsArray));
        this.result = undefined;
        this.error = null;
    }

    function Manifest(id = "manifest") {
        const scripts = {};
        this.id = id;

        this.addEntry = (manifestEntry) => {
            if (!(manifestEntry instanceof ManifestEntry)) {
                throw new Error("Manifest.addEntry: argument must be a ManifestEntry");
            }
            if (scripts[manifestEntry.id]) {
                throw new Error(`Manifest.addEntry: entry with id "${manifestEntry.id}" already exists`);
            }
            scripts[manifestEntry.id] = manifestEntry;
            return true;
        };

        this.list = () => Object.keys(scripts);

        this.get = (id) => scripts[id] || null;

        // Useful for runner
        this._all = () => ({ ...scripts });

        // Allow depends referencing either ids or src strings
        this._resolveDependsToIds = () => {
            const byId = { ...scripts };
            const bySrc = {};
            for (const id of Object.keys(byId)) {
                const e = byId[id];
                if (typeof e.src === "string") {
                    bySrc[e.src] = id; // last one wins if duplicates
                }
            }
            const resolved = {};
            for (const id of Object.keys(byId)) {
                const e = byId[id];
                const depIds = e.depends.map((d) => {
                    if (byId[d]) return d;
                    if (bySrc[d]) return bySrc[d];
                    return d; // leave unresolved; runner will report
                });
                resolved[id] = { ...e, depends: depIds };
            }
            return resolved;
        };
    }

    // Topological run with async execution of ready nodes
    function run(manifest, cb = null) {
        if (!(manifest instanceof Manifest)) {
            return Promise.reject(new Error("runner.run: manifest must be a Manifest"));
        }

        const entries = manifest._resolveDependsToIds();
        const ids = Object.keys(entries);

        // Build indegree and adjacency lists
        const indegree = {};
        const adj = {};
        for (const id of ids) {
            indegree[id] = 0;
            adj[id] = [];
        }
        // Track missing deps
        const missingDeps = {};
        for (const id of ids) {
            for (const dep of entries[id].depends) {
                if (!entries[dep]) {
                    missingDeps[id] = missingDeps[id] || [];
                    missingDeps[id].push(dep);
                } else {
                    indegree[id]++;
                    adj[dep].push(id);
                }
            }
        }

        const results = {};
        const errors = {};
        const started = [];
        const completed = [];

        // Initially ready: indegree === 0 and without missing deps
        const ready = [];
        for (const id of ids) {
            if (indegree[id] === 0 && !missingDeps[id]) ready.push(id);
        }

        const inProgress = new Set();

        function maybeStart(id) {
            const entry = entries[id];
            started.push(id);
            inProgress.add(id);
            return runSrc(entry)
                .then((value) => {
                    entry.result = value;
                    results[id] = value;
                })
                .catch((err) => {
                    entry.error = err;
                    errors[id] = err;
                })
                .finally(() => {
                    inProgress.delete(id);
                    completed.push(id);
                    // Unlock neighbors if no error and indegrees drop to zero
                    for (const nx of adj[id]) {
                        indegree[nx]--;
                    }
                });
        }

        // Keep a map of whether all deps of a node have completed successfully
        function depsSatisfied(id) {
            const deps = entries[id].depends;
            for (const d of deps) {
                const depEntry = entries[d];
                if (!depEntry) return false; // missing
                if (depEntry.error) return false; // failed
                if (!completed.includes(d)) return false; // not done yet
            }
            return true;
        }

        return new Promise((resolve) => {
            const tick = () => {
                // Start all currently ready nodes whose deps are satisfied
                let startedSomething = false;

                for (const id of ids) {
                    if (indegree[id] === 0 &&
                        !missingDeps[id] &&
                        !inProgress.has(id) &&
                        !completed.includes(id) &&
                        depsSatisfied(id)) {
                        startedSomething = true;
                        maybeStart(id).then(() => {
                            // After one finishes, keep ticking
                            queueMicrotask(tick);
                        });
                    }
                }

                if (!startedSomething) {
                    // No new starts. If any are in progress, wait for them to finish.
                    if (inProgress.size > 0) {
                        return; // tick will be called on completion
                    }

                    // Determine unresolved due to cycles or missing deps or failed deps
                    const unresolved = [];
                    for (const id of ids) {
                        if (!completed.includes(id)) {
                            unresolved.push(id);
                        }
                    }

                    const summary = {
                        manifestId: manifest.id,
                        started,
                        completed,
                        results,
                        errors,
                        missingDeps,
                        unresolved
                    };

                    if (typeof cb === "function") {
                        try {
                            cb(summary);
                        } catch (e) {
                            console.error("runner callback threw:", e);
                        }
                    }

                    resolve(summary);
                }
            };

            // Prime the pump
            queueMicrotask(tick);
        });
    }

    //===== INIT ================================================================
    const IFACE = { run, Manifest, ManifestEntry };
    const MOD_ID = "runner";
    const MOD = Object.freeze(IFACE);

    //==== EXPORT MODULE ========================================================
    modules.add(MOD_ID, MOD);
})(globalThis.SJS.modules || null);

// taskManager
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }

    //===== FUNCTIONS =========================================================
    function Task(id, func, args = []) {
        if (
            typeof id !== 'string' || id.trim().length === 0 ||
            typeof func !== 'function' ||
            Array.isArray(args) === false
        ) {
            throw new Error("ERROR! Invalid arguments!");
        }
        this.id = id;
        this.func = func;
        this.args = args;
    }

    // Runs task.func(...task.args). Supports sync functions or functions that return a Promise.
    // Resolves to { id, value } as requested.
    function processTaskAsync(task) {
        return Promise.resolve()
            .then(() => task.func(...task.args))
            .then((value) => ({ id: task.id, value }));
    }

    // Sequentially process tasks and accumulate results in order.
    function asyncManager(tasks) {
        if (!Array.isArray(tasks)) {
            return Promise.reject(new Error("tasks must be an array"));
        }
        return tasks.reduce((chain, task) => {
            return chain.then((results) =>
                processTaskAsync(task).then((currentResult) => {
                    results.push(currentResult);
                    return results;
                })
            );
        }, Promise.resolve([]));
    }

    //===== INIT ==============================================================
    const IFACE = {
        asyncManager, Task
    };
    const MOD_ID = "taskManager";
    const MOD = Object.freeze(IFACE);

    //==== EXPORT MODULE ======================================================
    modules.add(MOD_ID, MOD);
})(globalThis.SJS.modules || null);

// pubSub
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }

    //===== INTERNAL STATE =====================================================
    const topics = Object.create(null); // topic -> [{ id, handler, once }]
    let seq = 0;

    //===== HELPERS ============================================================
    function isValidTopic(topic) {
        return typeof topic === 'string' && topic.trim().length > 0;
    }

    function tokenFor(topic, id) {
        return `${topic}#${id}`;
    }

    //===== API ================================================================
    function subscribe(topic, handler, options = {}) {
        if (!isValidTopic(topic) || typeof handler !== 'function') {
            console.error("ERROR! subscribe: invalid arguments.");
            return null;
        }
        const once = options.once === true;
        const id = ++seq;
        const rec = { id, handler, once };
        if (!topics[topic]) topics[topic] = [];
        topics[topic].push(rec);
        return tokenFor(topic, id); // return token to allow easy unsubscribe
    }

    function once(topic, handler) {
        return subscribe(topic, handler, { once: true });
    }

    // Unsubscribe by:
    // - token string "topic#id"
    // - or (topic, handler)
    // - or (topic) to clear all handlers for a topic
    function unsubscribe(topicOrToken, handler) {
        if (typeof topicOrToken === 'string' && topicOrToken.includes('#') && handler == null) {
            const m = topicOrToken.match(/^(.+?)#(\d+)$/);
            if (!m) {
                console.error("ERROR! unsubscribe: invalid token.");
                return false;
            }
            const [, topic, idStr] = m;
            const id = Number(idStr);
            const arr = topics[topic];
            if (!arr) return false;
            const idx = arr.findIndex(x => x.id === id);
            if (idx >= 0) {
                arr.splice(idx, 1);
                if (arr.length === 0) delete topics[topic];
                return true;
            }
            return false;
        }

        if (!isValidTopic(topicOrToken)) {
            console.error("ERROR! unsubscribe: invalid topic or token.");
            return false;
        }

        const topic = topicOrToken;
        const arr = topics[topic];
        if (!arr) return false;

        // If no handler provided, clear entire topic
        if (handler == null) {
            delete topics[topic];
            return true;
        }

        if (typeof handler !== 'function') {
            console.error("ERROR! unsubscribe: handler must be a function when provided.");
            return false;
        }

        const before = arr.length;
        for (let i = arr.length - 1; i >= 0; i--) {
            if (arr[i].handler === handler) arr.splice(i, 1);
        }
        if (arr.length === 0) delete topics[topic];
        return arr.length !== before;
    }

    function publish(topic, payload) {
        if (!isValidTopic(topic)) {
            console.error("ERROR! publish: invalid topic.");
            return 0;
        }
        const arr = topics[topic];
        if (!arr || arr.length === 0) return 0;

        const snapshot = arr.slice(); // allow mutation during dispatch
        let called = 0;

        for (const rec of snapshot) {
            try {
                rec.handler(payload, { topic, token: tokenFor(topic, rec.id) });
                called++;
            } catch (e) {
                console.error(`pubSub listener error for topic "${topic}":`, e);
            }
            if (rec.once) {
                const idx = arr.findIndex(x => x.id === rec.id);
                if (idx >= 0) arr.splice(idx, 1);
            }
        }

        if (arr.length === 0) delete topics[topic];
        return called;
    }

    async function publishAsync(topic, payload) {
        if (!isValidTopic(topic)) {
            console.error("ERROR! publishAsync: invalid topic.");
            return 0;
        }
        const arr = topics[topic];
        if (!arr || arr.length === 0) return 0;

        const snapshot = arr.slice();
        let called = 0;

        await Promise.all(snapshot.map(async (rec) => {
            try {
                const ret = rec.handler(payload, { topic, token: tokenFor(topic, rec.id) });
                called++;
                await Promise.resolve(ret);
            } catch (e) {
                console.error(`pubSub listener error for topic "${topic}":`, e);
            } finally {
                if (rec.once) {
                    const idx = arr.findIndex(x => x.id === rec.id);
                    if (idx >= 0) arr.splice(idx, 1);
                }
            }
        }));

        if (arr.length === 0) delete topics[topic];
        return called;
    }

    function list() {
        return Object.keys(topics);
    }

    function count(topic) {
        if (!isValidTopic(topic)) {
            console.error("ERROR! count: invalid topic.");
            return 0;
        }
        const arr = topics[topic];
        return arr ? arr.length : 0;
    }

    // Clear a single topic or all topics if topic is null/undefined
    function clear(topic) {
        if (topic == null) {
            for (const t of Object.keys(topics)) delete topics[t];
            return true;
        }
        if (!isValidTopic(topic)) {
            console.error("ERROR! clear: invalid topic.");
            return false;
        }
        if (topics[topic]) {
            delete topics[topic];
            return true;
        }
        return false;
    }

    //===== INIT ================================================================
    const IFACE = {
        subscribe,
        once,
        unsubscribe,
        publish,
        publishAsync,
        list,
        count,
        clear
    };
    const MOD_ID = "pubSub";
    const MOD = Object.freeze(IFACE);

    //==== EXPORT MODULE ========================================================
    modules.add(MOD_ID, MOD);
})(globalThis.SJS.modules || null);

/* NEW MODULE: storage
   Cross-context key/value storage with smart driver selection.
   - Prefers IndexedDB (browser/worker), then localStorage (browser), then Node FS (JSON file), then memory.
   - Namespaces to avoid collisions.
   - TTL support (expires automatically on read).
   - Change events published via pubSub (topic: "storage:*") and optional BroadcastChannel/localStorage events.
   API (all async unless noted):
     - configure(cfg)
     - driverName(): string
     - use(namespace): namespaced view
     - get(key)
     - set(key, value, { ttlMs }?)
     - getJSON(key) / setJSON(key, obj, { ttlMs }?)
     - has(key)
     - remove(key)
     - clear()  // clears only this namespace
     - keys()
     - entries()
     - size()
     - export()  // dumps this namespace as { key: value }
     - import(obj, { merge = true }?)
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }

    const g = globalThis;

    function isBrowser() {
        return typeof window !== "undefined" && typeof document !== "undefined";
    }
    function isWorker() {
        return typeof WorkerGlobalScope !== "undefined" && self instanceof WorkerGlobalScope;
    }
    function hasIndexedDB() {
        try { return !!g.indexedDB; } catch { return false; }
    }
    function canUseLocalStorage() {
        try {
            if (!isBrowser()) return false;
            const k = "__sjs_storage_probe__";
            window.localStorage.setItem(k, "1");
            window.localStorage.removeItem(k);
            return true;
        } catch { return false; }
    }
    function isNode() {
        return typeof process !== "undefined" && !!process.versions && !!process.versions.node;
    }
    function now() { return Date.now(); }
    function isoNow() { try { return new Date().toISOString(); } catch { return String(now()); } }

    // Envelope we store (serialize for localStorage)
    function makeEnvelope(value, ttlMs) {
        const e = { v: value, t: isoNow() };
        if (typeof ttlMs === "number" && ttlMs > 0) {
            e.e = now() + ttlMs;
        }
        return e;
    }
    function unwrapEnvelope(env) {
        if (!env || typeof env !== "object") return null;
        if (env.e != null && now() > Number(env.e)) return "__EXPIRED__";
        return env.v;
    }

    // Broadcast support
    const channelName = "SJS_STORAGE";
    let bc = null;

    // Drivers must implement async methods: getEnv, setEnv, remove, clearNS, keysNS, entriesNS, sizeNS, name
    function createMemoryDriver() {
        const map = new Map(); // key -> envelope
        return {
            name: "memory",
            async getEnv(k) { return map.get(k) ?? null; },
            async setEnv(k, env) { map.set(k, env); return true; },
            async remove(k) { map.delete(k); return true; },
            async clearNS(prefix) {
                for (const key of Array.from(map.keys())) {
                    if (key.startsWith(prefix)) map.delete(key);
                }
                return true;
            },
            async keysNS(prefix) {
                const out = [];
                for (const key of map.keys()) if (key.startsWith(prefix)) out.push(key);
                return out;
            },
            async entriesNS(prefix) {
                const out = [];
                for (const [k, env] of map.entries()) if (k.startsWith(prefix)) out.push([k, env]);
                return out;
            },
            async sizeNS(prefix) {
                let c = 0;
                for (const k of map.keys()) if (k.startsWith(prefix)) c++;
                return c;
            }
        };
    }

    function createLocalStorageDriver(prefix) {
        prefix = String(prefix || "sjs:");
        function k(ns, key) {
            return `${prefix}${ns}:${key}`;
        }
        function toNSPrefix(ns) {
            return `${prefix}${ns}:`;
        }
        return {
            name: "localStorage",
            async getEnv(compKey) {
                try {
                    const raw = window.localStorage.getItem(compKey);
                    if (raw == null) return null;
                    return JSON.parse(raw);
                } catch { return null; }
            },
            async setEnv(compKey, env) {
                try {
                    window.localStorage.setItem(compKey, JSON.stringify(env));
                    return true;
                } catch { return false; }
            },
            async remove(compKey) {
                try { window.localStorage.removeItem(compKey); return true; } catch { return false; }
            },
            async clearNS(nsPrefix) {
                try {
                    const keys = [];
                    for (let i = 0; i < window.localStorage.length; i++) {
                        const k = window.localStorage.key(i);
                        if (k && k.startsWith(nsPrefix)) keys.push(k);
                    }
                    for (const key of keys) window.localStorage.removeItem(key);
                    return true;
                } catch { return false; }
            },
            async keysNS(nsPrefix) {
                const out = [];
                try {
                    for (let i = 0; i < window.localStorage.length; i++) {
                        const k = window.localStorage.key(i);
                        if (k && k.startsWith(nsPrefix)) out.push(k);
                    }
                } catch { }
                return out;
            },
            async entriesNS(nsPrefix) {
                const out = [];
                try {
                    for (let i = 0; i < window.localStorage.length; i++) {
                        const k = window.localStorage.key(i);
                        if (k && k.startsWith(nsPrefix)) {
                            const raw = window.localStorage.getItem(k);
                            if (raw != null) {
                                try { out.push([k, JSON.parse(raw)]); } catch { }
                            }
                        }
                    }
                } catch { }
                return out;
            },
            async sizeNS(nsPrefix) {
                let c = 0;
                try {
                    for (let i = 0; i < window.localStorage.length; i++) {
                        const k = window.localStorage.key(i);
                        if (k && k.startsWith(nsPrefix)) c++;
                    }
                } catch { }
                return c;
            },
            _compose(ns, key) { return k(ns, key); },
            _nsPrefix(ns) { return toNSPrefix(ns); }
        };
    }

    function createIDBDriver(dbName, storeName) {
        dbName = dbName || "SJS_KV";
        storeName = storeName || "kv";
        let dbp = null;

        function openDB() {
            if (dbp) return dbp;
            dbp = new Promise((resolve, reject) => {
                const req = g.indexedDB.open(dbName, 1);
                // BUGFIX: avoid using db.transaction during onupgradeneeded (invalid); just create object store if missing
                req.onupgradeneeded = () => {
                    const db = req.result;
                    if (!db.objectStoreNames.contains(storeName)) {
                        db.createObjectStore(storeName, { keyPath: "k" }); // rows: { k, env }
                    }
                };
                req.onsuccess = () => resolve(req.result);
                req.onerror = () => reject(req.error || new Error("indexedDB open error"));
            });
            return dbp;
        }

        async function withStore(mode, fn) {
            const db = await openDB();
            return new Promise((resolve, reject) => {
                const tx = db.transaction(storeName, mode);
                const store = tx.objectStore(storeName);
                let done = false;
                const finish = (val, err) => {
                    if (done) return;
                    done = true;
                    err ? reject(err) : resolve(val);
                };
                tx.oncomplete = () => finish(undefined, null);
                tx.onabort = () => finish(undefined, tx.error || new Error("tx aborted"));
                tx.onerror = () => finish(undefined, tx.error || new Error("tx error"));
                Promise.resolve()
                    .then(() => fn(store))
                    .catch((e) => finish(undefined, e));
            });
        }

        return {
            name: "idb",
            async getEnv(compKey) {
                const db = await openDB();
                return new Promise((resolve, reject) => {
                    const tx = db.transaction(storeName, "readonly");
                    const store = tx.objectStore(storeName);
                    const req = store.get(compKey);
                    req.onsuccess = () => {
                        const row = req.result;
                        resolve(row ? row.env : null);
                    };
                    req.onerror = () => reject(req.error);
                });
            },
            async setEnv(compKey, env) {
                const db = await openDB();
                return new Promise((resolve, reject) => {
                    const tx = db.transaction(storeName, "readwrite");
                    const store = tx.objectStore(storeName);
                    const row = { k: compKey, env };
                    const req = store.put(row);
                    req.onsuccess = () => resolve(true);
                    req.onerror = () => reject(req.error);
                });
            },
            async remove(compKey) {
                const db = await openDB();
                return new Promise((resolve, reject) => {
                    const tx = db.transaction(storeName, "readwrite");
                    const store = tx.objectStore(storeName);
                    const req = store.delete(compKey);
                    req.onsuccess = () => resolve(true);
                    req.onerror = () => reject(req.error);
                });
            },
            async clearNS(nsPrefix) {
                const db = await openDB();
                return new Promise((resolve, reject) => {
                    const tx = db.transaction(storeName, "readwrite");
                    const store = tx.objectStore(storeName);
                    const req = store.openCursor();
                    req.onsuccess = (e) => {
                        const cursor = e.target.result;
                        if (cursor) {
                            const key = cursor.key;
                            if (String(key).startsWith(nsPrefix)) {
                                store.delete(key);
                            }
                            cursor.continue();
                        }
                    };
                    tx.oncomplete = () => resolve(true);
                    tx.onerror = () => reject(tx.error);
                    tx.onabort = () => reject(tx.error);
                });
            },
            async keysNS(nsPrefix) {
                const db = await openDB();
                return new Promise((resolve, reject) => {
                    const keys = [];
                    const tx = db.transaction(storeName, "readonly");
                    const store = tx.objectStore(storeName);
                    const req = store.openCursor();
                    req.onsuccess = (e) => {
                        const cursor = e.target.result;
                        if (cursor) {
                            const key = cursor.key;
                            if (String(key).startsWith(nsPrefix)) {
                                keys.push(key);
                            }
                            cursor.continue();
                        }
                    };
                    tx.oncomplete = () => resolve(keys);
                    tx.onerror = () => reject(tx.error);
                    tx.onabort = () => reject(tx.error);
                });
            },
            async entriesNS(nsPrefix) {
                const db = await openDB();
                return new Promise((resolve, reject) => {
                    const entries = [];
                    const tx = db.transaction(storeName, "readonly");
                    const store = tx.objectStore(storeName);
                    const req = store.openCursor();
                    req.onsuccess = (e) => {
                        const cursor = e.target.result;
                        if (cursor) {
                            const key = cursor.key;
                            if (String(key).startsWith(nsPrefix)) {
                                const row = cursor.value;
                                entries.push([key, row.env]);
                            }
                            cursor.continue();
                        }
                    };
                    tx.oncomplete = () => resolve(entries);
                    tx.onerror = () => reject(tx.error);
                    tx.onabort = () => reject(tx.error);
                });
            },
            async sizeNS(nsPrefix) {
                const keys = await this.keysNS(nsPrefix);
                return keys.length;
            }
        };
    }

    function createFSDriver(filePath) {
        let fs = null;
        let pathMod = null;
        let data = Object.create(null); // compKey -> env
        let dirty = false;
        let flushTimer = null;
        const debounceMs = 100;

        async function ensureDeps() {
            if (!fs) fs = await import('node:fs/promises');
            if (!pathMod) pathMod = await import('node:path');
        }
        async function loadFile() {
            await ensureDeps();
            try {
                const txt = await fs.readFile(filePath, 'utf8');
                const obj = JSON.parse(txt);
                if (obj && typeof obj === "object") data = obj;
            } catch (e) {
                if (e && e.code === 'ENOENT') {
                    data = Object.create(null);
                    await saveFile(); // create file
                } else {
                    console.warn("storage/fs: read error:", e);
                }
            }
        }
        async function saveFile() {
            await ensureDeps();
            try {
                const dir = pathMod.dirname(filePath);
                await fs.mkdir(dir, { recursive: true }).catch(() => { });
                const tmp = filePath + ".tmp";
                await fs.writeFile(tmp, JSON.stringify(data, null, 2), 'utf8');
                await fs.rename(tmp, filePath);
            } catch (e) {
                console.warn("storage/fs: write error:", e);
            }
        }
        function scheduleFlush() {
            dirty = true;
            if (flushTimer) return;
            flushTimer = setTimeout(async () => {
                flushTimer = null;
                if (dirty) {
                    dirty = false;
                    await saveFile();
                }
            }, debounceMs);
        }

        const api = {
            name: "fs",
            async _init() { await loadFile(); },
            async getEnv(k) { if (!fs) await loadFile(); return data[k] || null; },
            async setEnv(k, env) { data[k] = env; scheduleFlush(); return true; },
            async remove(k) { delete data[k]; scheduleFlush(); return true; },
            async clearNS(prefix) {
                for (const key of Object.keys(data)) if (key.startsWith(prefix)) delete data[key];
                scheduleFlush();
                return true;
            },
            async keysNS(prefix) {
                return Object.keys(data).filter(k => k.startsWith(prefix));
            },
            async entriesNS(prefix) {
                return Object.keys(data).filter(k => k.startsWith(prefix)).map(k => [k, data[k]]);
            },
            async sizeNS(prefix) {
                let c = 0;
                for (const k of Object.keys(data)) if (k.startsWith(prefix)) c++;
                return c;
            }
        };
        return api;
    }

    // State/config
    const defaults = {
        driver: "auto",
        namespace: "default",
        idbName: "SJS_KV",
        idbStore: "kv",
        localStoragePrefix: "sjs:",
        filePath: "./sjs_store.json",
        broadcast: true
    };

    const state = {
        cfg: { ...defaults },
        driver: null,
        composeKey(ns, key) { return `${state.cfg.namespace}:${key}`; },
        nsPrefix(ns) {
            // For drivers keyed by composed strings, we use "<ns>:"
            return `${ns}:`;
        }
    };

    function pickDriver(cfg) {
        if (cfg.driver && cfg.driver !== "auto") return cfg.driver;
        if (hasIndexedDB()) return "idb";
        if (canUseLocalStorage()) return "localStorage";
        if (isNode()) return "fs";
        return "memory";
    }

    // notify helpers
    function notifyChange(kind, ns, key, value) {
        const pubSub = modules.get("pubSub");
        const payload = { kind, ns, key, value, driver: api.driverName() };
        try {
            if (pubSub) pubSub.publish(`storage:${kind}`, payload);
            if (state.cfg.broadcast) {
                if (typeof BroadcastChannel !== "undefined") {
                    if (!bc) bc = new BroadcastChannel(channelName);
                    bc.postMessage(payload);
                } else if (canUseLocalStorage()) {
                    // generate a synthetic event via LS (other tabs will see it)
                    const k = `${state.cfg.localStoragePrefix || "sjs:"}__storage_event__`;
                    try {
                        window.localStorage.setItem(k, JSON.stringify({ ...payload, ts: now() }));
                        window.localStorage.removeItem(k);
                    } catch { /* ignore */ }
                }
            }
        } catch { /* ignore */ }
    }

    // attach listeners for external changes
    if (isBrowser() && typeof window.addEventListener === "function") {
        window.addEventListener("storage", (ev) => {
            if (!ev || !ev.key) return;
            const pubSub = modules.get("pubSub");
            if (!pubSub) return;
            const pre = (state.cfg.localStoragePrefix || "sjs:") + state.cfg.namespace + ":";
            if (ev.key.startsWith(pre)) {
                pubSub.publish("storage:external", { key: ev.key, namespace: state.cfg.namespace });
            }
        });
    }
    if (typeof BroadcastChannel !== "undefined") {
        try {
            bc = new BroadcastChannel(channelName);
            bc.onmessage = (ev) => {
                const pubSub = modules.get("pubSub");
                if (pubSub) pubSub.publish("storage:external", ev.data);
            };
        } catch { /* ignore */ }
    }

    // Core API factory (namespaced view around driver)
    function makeAPIForNamespace(ns) {
        const nsStr = String(ns || state.cfg.namespace);
        const nsPrefix = state.nsPrefix(nsStr);

        // Compose key used internally for drivers that store a single flat key space
        function comp(key) {
            return `${nsPrefix}${key}`;
        }

        async function readEnv(compKey) {
            const env = await state.driver.getEnv(compKey);
            if (env == null) return null;
            if (env && typeof env === "object" && env.e != null && now() > Number(env.e)) {
                // expired: remove
                await state.driver.remove(compKey).catch(() => { });
                return "__EXPIRED__";
            }
            return env;
        }

        return Object.freeze({
            namespace: nsStr,

            async get(key) {
                const env = await readEnv(comp(String(key)));
                if (env === "__EXPIRED__" || env == null) return null;
                return unwrapEnvelope(env);
            },

            async set(key, value, options = {}) {
                const env = makeEnvelope(value, options.ttlMs);
                const k = comp(String(key));
                const ok = await state.driver.setEnv(k, env);
                if (ok) notifyChange("set", nsStr, String(key), value);
                return ok;
            },

            async setJSON(key, obj, options = {}) {
                return this.set(key, obj, options);
            },

            async getJSON(key) {
                return this.get(key);
            },

            async has(key) {
                const env = await readEnv(comp(String(key)));
                return env !== "__EXPIRED__" && env != null;
            },

            async remove(key) {
                const k = comp(String(key));
                const ok = await state.driver.remove(k);
                if (ok) notifyChange("remove", nsStr, String(key), undefined);
                return ok;
            },

            async clear() {
                const ok = await state.driver.clearNS(nsPrefix);
                if (ok) notifyChange("clear", nsStr, null, undefined);
                return ok;
            },

            async keys() {
                const keys = await state.driver.keysNS(nsPrefix);
                return keys.map(k => k.slice(nsPrefix.length));
            },

            async entries() {
                const entries = await state.driver.entriesNS(nsPrefix);
                const out = [];
                for (const [k, env] of entries) {
                    const v = unwrapEnvelope(env);
                    if (v === "__EXPIRED__") {
                        await state.driver.remove(k).catch(() => { });
                        continue;
                    }
                    out.push([k.slice(nsPrefix.length), v]);
                }
                return out;
            },

            async size() {
                return state.driver.sizeNS(nsPrefix);
            },

            async export() {
                const obj = {};
                for (const [k, v] of await this.entries()) obj[k] = v;
                return obj;
            },

            async import(obj, { merge = true } = {}) {
                if (!obj || typeof obj !== "object") return false;
                if (!merge) await this.clear();
                const pairs = Object.entries(obj);
                for (const [k, v] of pairs) {
                    await this.set(k, v);
                }
                return true;
            }
        });
    }

    // Public API (root; includes configure and use())
    const api = {
        _initialized: false,
        _nsAPIs: Object.create(null),

        configure(cfg = {}) {
            const priorDriverName = state.driver?.name || null;
            state.cfg = { ...defaults, ...(g.SJS?.config?.storage || {}), ...cfg };

            // select driver
            const selected = state.cfg.driver === "auto" ? pickDriver(state.cfg) : state.cfg.driver;
            let driver = null;
            try {
                if (selected === "idb" && hasIndexedDB()) {
                    driver = createIDBDriver(state.cfg.idbName, state.cfg.idbStore);
                } else if (selected === "localStorage" && canUseLocalStorage()) {
                    driver = createLocalStorageDriver(state.cfg.localStoragePrefix);
                } else if (selected === "fs" && isNode()) {
                    driver = createFSDriver(state.cfg.filePath);
                } else if (selected === "memory") {
                    driver = createMemoryDriver();
                } else {
                    // fallback chain
                    if (hasIndexedDB()) driver = createIDBDriver(state.cfg.idbName, state.cfg.idbStore);
                    else if (canUseLocalStorage()) driver = createLocalStorageDriver(state.cfg.localStoragePrefix);
                    else if (isNode()) driver = createFSDriver(state.cfg.filePath);
                    else driver = createMemoryDriver();
                }
            } catch (e) {
                console.warn("storage: driver selection error; falling back to memory:", e);
                driver = createMemoryDriver();
            }
            state.driver = driver;

            // Best-effort init for FS driver
            if (driver && driver.name === "fs" && typeof driver._init === "function") {
                driver._init().catch((e) => console.warn("storage/fs init error:", e));
            }

            // reset namespace cache
            api._nsAPIs = Object.create(null);
            api._initialized = true;

            return state.driver.name;
        },

        driverName() {
            if (!api._initialized) api.configure(); // auto-configure on first access
            return state.driver?.name || "unknown";
        },

        use(namespace) {
            if (!api._initialized) api.configure();
            const ns = String(namespace || state.cfg.namespace);
            if (api._nsAPIs[ns]) return api._nsAPIs[ns];
            const view = makeAPIForNamespace(ns);
            api._nsAPIs[ns] = view;
            return view;
        },

        // Default namespace convenience (matches cfg.namespace)
        async get(key) { return api.use(state.cfg.namespace).get(key); },
        async set(key, value, options) { return api.use(state.cfg.namespace).set(key, value, options); },
        async getJSON(key) { return api.use(state.cfg.namespace).getJSON(key); },
        async setJSON(key, obj, options) { return api.use(state.cfg.namespace).setJSON(key, obj, options); },
        async has(key) { return api.use(state.cfg.namespace).has(key); },
        async remove(key) { return api.use(state.cfg.namespace).remove(key); },
        async clear() { return api.use(state.cfg.namespace).clear(); },
        async keys() { return api.use(state.cfg.namespace).keys(); },
        async entries() { return api.use(state.cfg.namespace).entries(); },
        async size() { return api.use(state.cfg.namespace).size(); },
        async export() { return api.use(state.cfg.namespace).export(); },
        async import(obj, opts) { return api.use(state.cfg.namespace).import(obj, opts); }
    };

    // Auto-configure with SJS.config.storage if available
    try { api.configure(); } catch { /* lazy */ }

    modules.add("storage", Object.freeze(api));
})(globalThis.SJS.modules || null);

/* NEW MODULE: logger
   Cross-context structured logger with levels, namespace enable/disable patterns,
   pluggable sinks (console, memory, pubSub), and child loggers with context.

   Usage examples near bottom of file.
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }

    //===== HELPERS ============================================================
    const LEVELS = Object.freeze({
        trace: 10,
        debug: 20,
        info: 30,
        warn: 40,
        error: 50,
        fatal: 60,
        silent: 1e9
    });

    const LEVEL_NAMES = Object.keys(LEVELS);

    function clampLevel(lvl) {
        if (typeof lvl === "number") return lvl;
        if (typeof lvl === "string") {
            const key = lvl.toLowerCase();
            if (LEVELS[key] != null) return LEVELS[key];
        }
        return LEVELS.info;
    }

    function isBrowser() {
        return typeof window !== "undefined" && typeof document !== "undefined";
    }

    // Simple wildcard match: * matches any, ? matches one char
    function wildcardMatch(str, pattern) {
        if (pattern === "*") return true;
        // Escape regex specials except * and ?
        const re = new RegExp("^" + pattern
            .replace(/([.+^=!:${}()|[\]\\])/g, "\\$1")
            .replace(/\*/g, ".*")
            .replace(/\?/g, ".") + "$");
        return re.test(str);
    }

    function nowISO() {
        try { return new Date().toISOString(); } catch { return "" + Date.now(); }
    }

    function getConsoleMethod(level) {
        if (typeof console !== "object") return () => { };
        switch (level) {
            case "trace": return console.trace ? console.trace.bind(console) : console.log.bind(console);
            case "debug": return console.debug ? console.debug.bind(console) : console.log.bind(console);
            case "info": return console.info ? console.info.bind(console) : console.log.bind(console);
            case "warn": return console.warn ? console.warn.bind(console) : console.log.bind(console);
            case "error":
            case "fatal": return console.error ? console.error.bind(console) : console.log.bind(console);
            default: return console.log ? console.log.bind(console) : () => { };
        }
    }

    function safeLocalStorageGet(key) {
        try {
            if (!isBrowser()) return null;
            return window.localStorage.getItem(key);
        } catch { return null; }
    }

    //===== STATE =================================================================
    const state = {
        globalLevel: LEVELS.info,
        enablePatterns: [], // inclusions like "app:*"
        disablePatterns: [], // exclusions starting with "-"
        registry: Object.create(null), // name -> logger instance
        buffers: Object.create(null), // name -> ring buffer
        defaultBufferLimit: 200
    };

    function parseEnvConfig() {
        // Enable patterns from env / localStorage / query string
        let patternSrc =
            (typeof process !== "undefined" && process?.env?.SJS_DEBUG) ||
            (typeof process !== "undefined" && process?.env?.SJS_LOG) ||
            safeLocalStorageGet("SJS_DEBUG") ||
            safeLocalStorageGet("SJS_LOG") ||
            null;

        // Query param ?sjsDebug=...
        if (isBrowser() && !patternSrc) {
            try {
                const url = new URL(window.location.href);
                patternSrc = url.searchParams.get("sjsDebug") || url.searchParams.get("sjsLog");
            } catch { }
        }

        if (typeof patternSrc === "string" && patternSrc.trim()) {
            const parts = patternSrc.split(/[,\s]+/).map(s => s.trim()).filter(Boolean);
            state.enablePatterns = [];
            state.disablePatterns = [];
            for (const p of parts) {
                if (p.startsWith("-")) state.disablePatterns.push(p.slice(1));
                else state.enablePatterns.push(p);
            }
        }

        // Level from env / localStorage / query string
        let levelSrc =
            (typeof process !== "undefined" && process?.env?.SJS_LOG_LEVEL) ||
            safeLocalStorageGet("SJS_LOG_LEVEL") ||
            null;

        if (isBrowser() && !levelSrc) {
            try {
                const url = new URL(window.location.href);
                levelSrc = url.searchParams.get("sjsLevel");
            } catch { }
        }
        if (levelSrc) {
            state.globalLevel = clampLevel(levelSrc);
        }
    }

    // Initialize from environment
    parseEnvConfig();

    function createRingBuffer(limit) {
        const max = Math.max(0, Number(limit) || state.defaultBufferLimit);
        const arr = [];
        return {
            push(item) {
                arr.push(item);
                if (arr.length > max) {
                    arr.splice(0, arr.length - max);
                }
            },
            toArray() { return arr.slice(); },
            clear() { arr.length = 0; },
            size() { return arr.length; },
            limit: max
        };
    }

    //===== SINKS ===============================================================
    function consoleSink(rec) {
        const fn = getConsoleMethod(rec.level);
        // Pretty prefix: [ts] level name -
        const prefix = `[${rec.ts}] ${rec.level.toUpperCase()} ${rec.name}:`;
        if (rec.args && rec.args.length > 0) {
            fn(prefix, rec.msg, ...rec.args, rec.context && Object.keys(rec.context).length ? rec.context : "");
        } else {
            fn(prefix, rec.msg, rec.context && Object.keys(rec.context).length ? rec.context : "");
        }
    }

    function makeMemorySink(name) {
        if (!state.buffers[name]) {
            state.buffers[name] = createRingBuffer(state.defaultBufferLimit);
        }
        return function memorySink(rec) {
            try {
                state.buffers[name].push(rec);
            } catch { }
        };
    }

    function makePubSubSink(topic) {
        return function pubSubSink(rec) {
            try {
                const pubSub = modules.get("pubSub");
                if (pubSub) pubSub.publish(topic, rec);
            } catch { }
        };
    }

    //===== CORE LOGGER =========================================================
    function enabledByPattern(name) {
        // If any disable pattern matches, it's disabled
        for (const p of state.disablePatterns) {
            if (wildcardMatch(name, p)) return false;
        }
        // If inclusions exist, at least one must match
        if (state.enablePatterns.length > 0) {
            for (const p of state.enablePatterns) {
                if (wildcardMatch(name, p)) return true;
            }
            return false;
        }
        // No inclusions => default enabled
        return true;
    }

    function Logger(name, options = {}) {
        if (typeof name !== "string" || name.trim().length === 0) {
            throw new Error("logger.create: name must be a non-empty string");
        }
        this.name = name;
        this.level = clampLevel(options.level ?? state.globalLevel);
        this.context = typeof options.context === "object" && options.context ? { ...options.context } : {};
        this.sinks = [];
        // default console sink
        this.sinks.push(consoleSink);
        // optional memory sink
        if (options.memory === true) {
            this.sinks.push(makeMemorySink(name));
        }
        // optional pubSub sink
        if (options.pubSub === true) {
            this.sinks.push(makePubSubSink(options.topic || "log"));
        }
    }

    Logger.prototype.setLevel = function (lvl) {
        this.level = clampLevel(lvl);
        return this;
    };

    Logger.prototype.addSink = function (sinkFn) {
        if (typeof sinkFn === "function") this.sinks.push(sinkFn);
        return this;
    };

    Logger.prototype.removeSink = function (sinkFn) {
        const idx = this.sinks.indexOf(sinkFn);
        if (idx >= 0) this.sinks.splice(idx, 1);
        return this;
    };

    Logger.prototype.child = function (extraContext = {}, nameSuffix) {
        const child = new Logger(nameSuffix ? `${this.name}:${nameSuffix}` : this.name, {
            level: this.level,
            context: { ...this.context, ...extraContext }
        });
        // inherit sinks
        for (const s of this.sinks) child.addSink(s);
        return child;
    };

    Logger.prototype.isLevelEnabled = function (lvlName) {
        const lvl = clampLevel(lvlName);
        return enabledByPattern(this.name) && lvl >= this.level && lvl >= state.globalLevel;
    };

    Logger.prototype._emit = function (level, msg, argsArray) {
        if (!this.isLevelEnabled(level)) return false;
        const rec = {
            ts: nowISO(),
            level,
            lvl: LEVELS[level],
            name: this.name,
            msg,
            args: Array.isArray(argsArray) ? argsArray : [],
            context: this.context
        };
        for (const sink of this.sinks) {
            try { sink(rec); } catch (e) { /* swallow sink errors */ }
        }
        return true;
    };

    for (const levelName of LEVEL_NAMES) {
        if (levelName === "silent") continue;
        Logger.prototype[levelName] = function (msg, ...args) {
            return this._emit(levelName, msg, args);
        };
    }

    Logger.prototype.time = function (label = "default") {
        if (!this._timers) this._timers = Object.create(null);
        this._timers[label] = (typeof performance !== "undefined" && performance.now) ? performance.now() : Date.now();
    };

    Logger.prototype.timeEnd = function (label = "default", level = "info") {
        const start = this._timers?.[label];
        if (start == null) return;
        const end = (typeof performance !== "undefined" && performance.now) ? performance.now() : Date.now();
        const ms = end - start;
        delete this._timers[label];
        this._emit(level, `${label} ${ms.toFixed(2)}ms`, []);
    };

    //===== PUBLIC API ==========================================================
    function create(name, options) {
        if (state.registry[name]) return state.registry[name];
        const logger = new Logger(name, options);
        state.registry[name] = logger;
        return logger;
    }

    function get(name) {
        return state.registry[name] || null;
    }

    function list() {
        return Object.keys(state.registry);
    }

    function setGlobalLevel(level) {
        state.globalLevel = clampLevel(level);
    }

    function getGlobalLevel() {
        return state.globalLevel;
    }

    function enable(patterns) {
        const arr = (Array.isArray(patterns) ? patterns : String(patterns).split(/[,\s]+/))
            .map(s => String(s || "").trim()).filter(Boolean);
        for (const p of arr) {
            if (p.startsWith("-")) state.disablePatterns.push(p.slice(1));
            else state.enablePatterns.push(p);
        }
    }

    function disable(patterns) {
        const arr = (Array.isArray(patterns) ? patterns : String(patterns).split(/[,\s]+/))
            .map(s => String(s || "").trim()).filter(Boolean);
        for (const p of arr) {
            const pat = p.startsWith("-") ? p.slice(1) : p;
            state.disablePatterns.push(pat);
        }
    }

    function clearPatterns() {
        state.enablePatterns = [];
        state.disablePatterns = [];
    }

    function getBuffered(name) {
        return state.buffers[name]?.toArray() || [];
    }

    function clearBuffered(name) {
        if (name) state.buffers[name]?.clear();
        else {
            for (const k of Object.keys(state.buffers)) state.buffers[k].clear();
        }
    }

    function enablePubSub(logger, topic = "log") {
        if (!logger || typeof logger.addSink !== "function") return false;
        logger.addSink(makePubSubSink(topic));
        return true;
    }

    const IFACE = Object.freeze({
        create,
        get,
        list,
        setGlobalLevel,
        getGlobalLevel,
        enable,
        disable,
        clearPatterns,
        getBuffered,
        clearBuffered,
        enablePubSub,
        LEVELS
    });

    modules.add("logger", IFACE);
})(globalThis.SJS.modules || null);

/* NEW MODULE: debug
   Minimal debug toggler with colorized prefixes.
   API:
     - debugOn()
     - debugOff()
     - debugger(level, message, data?)
   Levels: "info" (green, prefix 'INFO--> '),
           "warning" (yellow, prefix 'WARNING--> '),
           "err" (red, prefix 'ERROR--> ')
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }

    function isBrowser() {
        return typeof window !== "undefined" && typeof document !== "undefined";
    }

    const COLORS_NODE = {
        green: "\x1b[32m",
        yellow: "\x1b[33m",
        red: "\x1b[31m",
        reset: "\x1b[0m"
    };

    const LEVEL_META = {
        info: { prefix: "INFO--> ", color: "green", console: () => console.info ? console.info.bind(console) : console.log.bind(console) },
        warning: { prefix: "WARNING--> ", color: "yellow", console: () => console.warn ? console.warn.bind(console) : console.log.bind(console) },
        err: { prefix: "ERROR--> ", color: "red", console: () => console.error ? console.error.bind(console) : console.log.bind(console) }
    };

    let enabled = false;

    function debugOn() { enabled = true; return true; }
    function debugOff() { enabled = false; return true; }
    function isEnabled() { return enabled; }

    function colorizePrefix(levelKey, prefixText) {
        const meta = LEVEL_META[levelKey];
        if (!meta) return { args: [prefixText], css: "" };

        if (isBrowser()) {
            const css = `color: ${meta.color}; font-weight: bold;`;
            // Use %c only for prefix, leave message/data unstyled
            return { args: ["%c" + prefixText, css], css };
        } else {
            const col = COLORS_NODE[meta.color] || "";
            const reset = COLORS_NODE.reset || "";
            return { args: [col + prefixText + reset], css: "" };
        }
    }

    function debuggerFn(level, message, data) {
        if (!enabled) return false;
        const levelKey = typeof level === "string" ? level.toLowerCase() : "";
        const meta = LEVEL_META[levelKey];
        if (!meta) {
            console.warn("debug.debugger: unknown level:", level);
            return false;
        }
        const { args: prefixArgs } = colorizePrefix(levelKey, meta.prefix);
        const logFn = meta.console();

        if (data !== undefined) {
            // Prefix (styled), message, data object
            if (isBrowser()) {
                // prefixArgs = ["%cPREFIX", css]
                logFn(...prefixArgs, message, data);
            } else {
                logFn(prefixArgs[0], message, data);
            }
        } else {
            if (isBrowser()) {
                logFn(...prefixArgs, message);
            } else {
                logFn(prefixArgs[0], message);
            }
        }
        return true;
    }

    const MOD = Object.freeze({
        debugOn,
        debugOff,
        debugger: debuggerFn,
        get enabled() { return enabled; },
        isEnabled
    });

    modules.add("debug", MOD);
})(globalThis.SJS.modules || null);

/* NEW MODULE: input
   Keyboard and mouse input manager with live pressed-keys array, query API, and pubSub broadcasts.

   Features
   - Keeps a Set of pressed keys (exposed as array via keysDown()) and pressed mouse buttons.
   - Optional capturing: use capture phase, preventDefault, stopPropagation.
   - Ignores keystrokes when typing in inputs/textarea/contentEditable (configurable).
   - Publishes events to pubSub if present:
       "input:keydown" | "input:keyup" payload: { key, code, rawKey, repeat, mods, pressed, ts }
       "input:mousedown" | "input:mouseup" payload: { button, buttonName, buttons, x, y, ts }
       "input:mousemove" payload: { x, y, dx, dy, buttons, ts }
       "input:wheel" payload: { dx, dy, dz, x, y, ts }
       Pointer equivalents: "input:pointerdown/up/move" (deduped vs mouse when enabled)
   - Resets pressed keys on blur/visibilitychange to avoid "stuck" keys.
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }
    function isBrowser() {
        return typeof window !== "undefined" && typeof document !== "undefined";
    }
    const pubSub = () => modules.get && modules.get("pubSub");

    const DEFAULTS = {
        enabled: true,
        target: null,               // HTMLElement | "window" | "document" | CSS selector; default window
        capture: false,             // addEventListener capture phase
        preventDefault: false,      // call preventDefault on handled events
        stopPropagation: false,     // call stopPropagation on handled events
        ignoreEditable: true,       // don't track key events in inputs/textarea/select/contentEditable
        publish: true,              // publish to pubSub if available
        useCode: false,             // store KeyboardEvent.code instead of normalized key string
        trackMouse: true,
        trackPointer: true,         // if true, also listen to pointer* events
        trackWheel: true,
        dedupePointerMouse: true,   // ignore mouse events that immediately follow pointer events
        throttleMouseMoveMs: 0      // 0 = no throttle
    };

    const state = {
        cfg: { ...DEFAULTS },
        active: false,
        targetEl: null,
        pressedKeys: new Set(),   // normalized keys or codes (strings)
        pressedButtons: new Set(),// numbers like 0,1,2...
        mouse: {
            x: 0, y: 0,
            lastX: 0, lastY: 0,
            buttonsMask: 0,
            lastMoveTs: 0
        },
        lastPointerTs: 0,
        _handlers: []
    };

    function normalizeKeyValue(key) {
        if (typeof key !== "string") return "";
        let k = key;
        if (k === " " || k === "Spacebar") k = "Space";
        if (k === "Esc") k = "Escape";
        // Normalize single-character letters to lowercase for consistency
        if (k.length === 1) k = k.toLowerCase();
        return k;
    }
    function normalizeKeyFromEvent(e, useCode) {
        if (useCode) return e.code || "";
        return normalizeKeyValue(e.key || "");
    }

    function shouldIgnoreKeyEvent(e) {
        if (!state.cfg.ignoreEditable) return false;
        const t = e.target;
        if (!t || !t.nodeType) return false;
        const el = t.nodeType === 3 ? t.parentElement : t; // text node => parent
        if (!el) return false;
        const tag = (el.tagName || "").toLowerCase();
        if (tag === "input" || tag === "textarea" || tag === "select") return true;
        if (el.isContentEditable) return true;
        return false;
    }

    function getTargetElement(spec) {
        if (!isBrowser()) return null;
        if (!spec || spec === "window") return window;
        if (spec === "document") return document;
        if (typeof spec === "string") {
            try { return document.querySelector(spec); } catch { return window; }
        }
        if (spec && typeof spec.addEventListener === "function") return spec;
        return window;
    }

    function applyEventControl(e) {
        if (state.cfg.preventDefault && e && typeof e.preventDefault === "function") e.preventDefault();
        if (state.cfg.stopPropagation && e && typeof e.stopPropagation === "function") e.stopPropagation();
    }

    function add(el, name, fn, opts) {
        el.addEventListener(name, fn, opts);
        state._handlers.push({ el, name, fn, opts });
    }
    function removeAll() {
        for (const h of state._handlers) {
            try { h.el.removeEventListener(h.name, h.fn, h.opts); } catch { }
        }
        state._handlers.length = 0;
    }

    function publish(topic, payload) {
        if (!state.cfg.publish) return;
        const ps = pubSub();
        if (ps) ps.publish(topic, payload);
    }

    function modsFromEvent(e) {
        return {
            alt: !!e.altKey,
            ctrl: !!e.ctrlKey,
            meta: !!e.metaKey,
            shift: !!e.shiftKey
        };
    }

    function buttonName(b) {
        switch (b) {
            case 0: return "left";
            case 1: return "middle";
            case 2: return "right";
            case 3: return "back";
            case 4: return "forward";
            default: return String(b);
        }
    }

    function onKeyDown(e) {
        if (shouldIgnoreKeyEvent(e)) return;
        const k = normalizeKeyFromEvent(e, state.cfg.useCode);
        if (!k) return;

        // Ignore repeated keydown to avoid duplicate "pressed" membership changes
        if (e.repeat && state.pressedKeys.has(k)) {
            // still publish a keydown if needed? Keep silent to avoid flooding.
        } else {
            state.pressedKeys.add(k);
            applyEventControl(e);
            publish("input:keydown", {
                type: "keydown", key: k, code: e.code || "",
                rawKey: e.key || "",
                repeat: !!e.repeat,
                mods: modsFromEvent(e),
                pressed: Array.from(state.pressedKeys),
                ts: e.timeStamp || Date.now()
            });
        }
    }

    function onKeyUp(e) {
        const k = normalizeKeyFromEvent(e, state.cfg.useCode);
        if (!k) return;
        if (state.pressedKeys.has(k)) state.pressedKeys.delete(k);
        applyEventControl(e);
        publish("input:keyup", {
            type: "keyup", key: k, code: e.code || "",
            rawKey: e.key || "",
            repeat: !!e.repeat,
            mods: modsFromEvent(e),
            pressed: Array.from(state.pressedKeys),
            ts: e.timeStamp || Date.now()
        });
    }

    function onBlurOrHide() {
        if (state.pressedKeys.size > 0 || state.pressedButtons.size > 0) {
            state.pressedKeys.clear();
            state.pressedButtons.clear();
            state.mouse.buttonsMask = 0;
            publish("input:clear", { ts: Date.now() });
        }
    }

    function onMouseDown(e) {
        const now = typeof performance !== "undefined" && performance.now ? performance.now() : Date.now();
        if (state.cfg.dedupePointerMouse && now - state.lastPointerTs < 16) return; // likely duplicate of pointerdown
        state.pressedButtons.add(e.button);
        state.mouse.buttonsMask = e.buttons ?? state.mouse.buttonsMask;
        applyEventControl(e);
        publish("input:mousedown", {
            type: "mousedown",
            button: e.button,
            buttonName: buttonName(e.button),
            buttons: e.buttons,
            x: e.clientX, y: e.clientY,
            mods: modsFromEvent(e),
            ts: e.timeStamp || Date.now()
        });
    }

    function onMouseUp(e) {
        const now = typeof performance !== "undefined" && performance.now ? performance.now() : Date.now();
        if (state.cfg.dedupePointerMouse && now - state.lastPointerTs < 16) return; // likely duplicate of pointerup
        state.pressedButtons.delete(e.button);
        state.mouse.buttonsMask = e.buttons ?? state.mouse.buttonsMask;
        applyEventControl(e);
        publish("input:mouseup", {
            type: "mouseup",
            button: e.button,
            buttonName: buttonName(e.button),
            buttons: e.buttons,
            x: e.clientX, y: e.clientY,
            mods: modsFromEvent(e),
            ts: e.timeStamp || Date.now()
        });
    }

    function onMouseMove(e) {
        const t = e.timeStamp || Date.now();
        if (state.cfg.throttleMouseMoveMs > 0 && (t - state.mouse.lastMoveTs) < state.cfg.throttleMouseMoveMs) return;
        const dx = e.movementX ?? (e.clientX - state.mouse.x);
        const dy = e.movementY ?? (e.clientY - state.mouse.y);
        state.mouse.lastX = state.mouse.x;
        state.mouse.lastY = state.mouse.y;
        state.mouse.x = e.clientX;
        state.mouse.y = e.clientY;
        state.mouse.lastMoveTs = t;
        state.mouse.buttonsMask = e.buttons ?? state.mouse.buttonsMask;
        publish("input:mousemove", {
            type: "mousemove",
            x: state.mouse.x, y: state.mouse.y,
            dx: dx, dy: dy,
            buttons: state.mouse.buttonsMask,
            ts: t
        });
    }

    function onWheel(e) {
        applyEventControl(e);
        publish("input:wheel", {
            type: "wheel",
            dx: e.deltaX || 0, dy: e.deltaY || 0, dz: e.deltaZ || 0,
            x: e.clientX, y: e.clientY,
            ts: e.timeStamp || Date.now()
        });
    }

    function onPointerDown(e) {
        state.lastPointerTs = typeof performance !== "undefined" && performance.now ? performance.now() : Date.now();
        applyEventControl(e);
        publish("input:pointerdown", {
            type: "pointerdown",
            pointerType: e.pointerType,
            button: e.button,
            buttonName: buttonName(e.button),
            buttons: e.buttons,
            x: e.clientX, y: e.clientY,
            ts: e.timeStamp || Date.now()
        });
    }
    function onPointerUp(e) {
        state.lastPointerTs = typeof performance !== "undefined" && performance.now ? performance.now() : Date.now();
        applyEventControl(e);
        publish("input:pointerup", {
            type: "pointerup",
            pointerType: e.pointerType,
            button: e.button,
            buttonName: buttonName(e.button),
            buttons: e.buttons,
            x: e.clientX, y: e.clientY,
            ts: e.timeStamp || Date.now()
        });
    }
    function onPointerMove(e) {
        const t = e.timeStamp || Date.now();
        if (state.cfg.throttleMouseMoveMs > 0 && (t - state.mouse.lastMoveTs) < state.cfg.throttleMouseMoveMs) return;
        const dx = e.movementX ?? (e.clientX - state.mouse.x);
        const dy = e.movementY ?? (e.clientY - state.mouse.y);
        state.mouse.lastX = state.mouse.x;
        state.mouse.lastY = state.mouse.y;
        state.mouse.x = e.clientX;
        state.mouse.y = e.clientY;
        state.mouse.lastMoveTs = t;
        publish("input:pointermove", {
            type: "pointermove",
            pointerType: e.pointerType,
            x: state.mouse.x, y: state.mouse.y, dx, dy, buttons: e.buttons,
            ts: t
        });
    }

    function start() {
        if (state.active || !isBrowser() || !state.cfg.enabled) return false;
        state.targetEl = getTargetElement(state.cfg.target);
        const el = state.targetEl || window;
        const opts = { capture: !!state.cfg.capture, passive: !state.cfg.preventDefault };

        // Keyboard
        add(el, "keydown", onKeyDown, opts);
        add(el, "keyup", onKeyUp, opts);
        // Reset on blur/visibilitychange to avoid stuck keys
        add(window, "blur", onBlurOrHide, { capture: true });
        add(document, "visibilitychange", () => {
            if (document.hidden) onBlurOrHide();
        }, { capture: true });

        // Mouse
        if (state.cfg.trackMouse) {
            add(el, "mousedown", onMouseDown, opts);
            add(el, "mouseup", onMouseUp, opts);
            add(el, "mousemove", onMouseMove, opts);
            if (state.cfg.trackWheel) add(el, "wheel", onWheel, opts);
        }

        // Pointer (dedupe vs mouse when configured)
        if (state.cfg.trackPointer && typeof window.PointerEvent !== "undefined") {
            add(el, "pointerdown", onPointerDown, opts);
            add(el, "pointerup", onPointerUp, opts);
            add(el, "pointermove", onPointerMove, opts);
        }

        state.active = true;
        return true;
    }

    function stop() {
        if (!state.active) return false;
        removeAll();
        state.active = false;
        state.pressedKeys.clear();
        state.pressedButtons.clear();
        return true;
    }

    function configure(cfg = {}) {
        state.cfg = { ...state.cfg, ...(globalThis.SJS?.config?.input || {}), ...cfg };
        // Retarget if running
        if (state.active) {
            stop();
            start();
        }
        return { ...state.cfg };
    }

    function keysDown() {
        return Array.from(state.pressedKeys);
    }

    function isKeyDown(key) {
        const norm = state.cfg.useCode ? String(key || "") : normalizeKeyValue(String(key || ""));
        return state.pressedKeys.has(norm);
    }

    function buttonsDown() {
        return Array.from(state.pressedButtons);
    }

    function isButtonDown(b) {
        if (typeof b === "string") {
            const BN = String(b).toLowerCase();
            if (BN === "left") b = 0;
            else if (BN === "middle") b = 1;
            else if (BN === "right") b = 2;
            else if (BN === "back") b = 3;
            else if (BN === "forward") b = 4;
            else b = Number(b);
        }
        return state.pressedButtons.has(Number(b));
    }

    function getMouse() {
        return {
            x: state.mouse.x,
            y: state.mouse.y,
            buttons: state.mouse.buttonsMask,
            pressedButtons: buttonsDown()
        };
    }

    function clear() {
        state.pressedKeys.clear();
        state.pressedButtons.clear();
        publish("input:clear", { ts: Date.now() });
    }

    // Convenience subscription helpers (wrappers over pubSub)
    function onKeyDownSub(handler) { const ps = pubSub(); return ps ? ps.subscribe("input:keydown", handler) : null; }
    function onKeyUpSub(handler) { const ps = pubSub(); return ps ? ps.subscribe("input:keyup", handler) : null; }
    function onMouseDownSub(handler) { const ps = pubSub(); return ps ? ps.subscribe("input:mousedown", handler) : null; }
    function onMouseUpSub(handler) { const ps = pubSub(); return ps ? ps.subscribe("input:mouseup", handler) : null; }
    function onMouseMoveSub(handler) { const ps = pubSub(); return ps ? ps.subscribe("input:mousemove", handler) : null; }
    function onWheelSub(handler) { const ps = pubSub(); return ps ? ps.subscribe("input:wheel", handler) : null; }

    const IFACE = Object.freeze({
        configure,
        start,
        stop,
        keysDown,
        isKeyDown,
        buttonsDown,
        isButtonDown,
        getMouse,
        clear,
        // subscribe helpers
        onKeyDown: onKeyDownSub,
        onKeyUp: onKeyUpSub,
        onMouseDown: onMouseDownSub,
        onMouseUp: onMouseUpSub,
        onMouseMove: onMouseMoveSub,
        onWheel: onWheelSub
    });

    // Auto-start in browser if enabled by default config
    try {
        configure(); // pull SJS.config.input if present
        if (isBrowser() && state.cfg.enabled) start();
    } catch { /* ignore */ }

    modules.add("input", IFACE);
})(globalThis.SJS.modules || null);

/* NEW MODULE: cache
   Read-through memoization cache with optional persistence (via storage module),
   TTL and stale-while-revalidate (SWR) support, concurrency de-duplication, and pubSub events.

   Why useful here:
   - Wrap async functions (e.g., API calls) and avoid duplicate in-flight requests.
   - Keep values fresh in background (SWR) while serving fast responses.
   - Persist to storage namespace when desired so cache survives reloads.

   API:
     - configure({
         persist?: boolean,           // default true if storage module present
         namespace?: string,          // storage namespace when persist = true (default "cache")
         ttlMs?: number,              // hard TTL before considered stale (default 60_000)
         swrMs?: number,              // serve-stale window after TTL (default 0 = disabled)
         maxEntries?: number,         // in-memory entry cap (simple LRU) (default 1000)
         cacheErrors?: boolean        // cache rejected results as values (default false)
       })
     - memoize(group, fn, options?): returns async (...args) => value
         options: { keyFn?: (args)=>string, ttlMs?, swrMs?, cacheErrors? }
     - get(group, key): Promise<value|null>
     - set(group, key, value, options?): Promise<boolean>
     - del(group, key): Promise<boolean>
     - clear(group?): Promise<boolean> // clear specific group or all
     - stats(): { size, hits, misses, sets, inflight }
   Events (via pubSub if present):
     - "cache:hit" { group, key }
     - "cache:miss" { group, key }
     - "cache:set" { group, key }
     - "cache:invalidate" { group, key? }
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }

    const DEFAULTS = {
        persist: undefined, // auto: true if storage present
        namespace: "cache",
        ttlMs: 60_000,
        swrMs: 0,
        maxEntries: 1000,
        cacheErrors: false
    };

    const state = {
        cfg: { ...DEFAULTS },
        mem: new Map(), // composedKey -> { value, hardTtl, staleUntil }
        inflight: new Map(), // composedKey -> Promise
        hits: 0,
        misses: 0,
        sets: 0
    };

    function now() { return Date.now(); }
    function comp(group, key) {
        const k = typeof key === "string" ? key : JSON.stringify(key);
        return `${group}::${k}`;
    }

    function lruTouch(k, rec) {
        if (!state.mem.has(k)) return;
        state.mem.delete(k);
        state.mem.set(k, rec);
    }

    function lruSet(k, rec, max) {
        if (state.mem.has(k)) state.mem.delete(k);
        state.mem.set(k, rec);
        while (state.mem.size > max) {
            const first = state.mem.keys().next().value;
            state.mem.delete(first);
        }
    }

    function getStorage() {
        const storage = modules.get("storage");
        return storage && typeof storage.use === "function" ? storage.use(state.cfg.namespace) : null;
    }

    function getPubSub() {
        return modules.get("pubSub");
    }

    function publish(topic, payload) {
        const ps = getPubSub();
        if (ps) ps.publish(topic, payload);
    }

    function isExpired(rec, t) {
        return t > rec.staleUntil;
    }
    function isStale(rec, t) {
        return t > rec.hardTtl && t <= rec.staleUntil;
    }

    async function readRecord(group, key) {
        const ck = comp(group, key);
        const t = now();

        const recMem = state.mem.get(ck);
        if (recMem) {
            if (isExpired(recMem, t)) {
                state.mem.delete(ck);
                return null;
            }
            lruTouch(ck, recMem);
            return recMem;
        }

        const storage = (state.cfg.persist ? getStorage() : null);
        if (!storage) return null;

        // Persisted record shape: { value, hardTtl, staleUntil }
        try {
            const v = await storage.get(ck);
            if (!v || typeof v !== "object") return null;
            const rec = { value: v.value, hardTtl: Number(v.hardTtl || 0), staleUntil: Number(v.staleUntil || 0) };
            if (isExpired(rec, t)) {
                await storage.remove(ck).catch(() => { });
                return null;
            }
            lruSet(ck, rec, state.cfg.maxEntries);
            return rec;
        } catch {
            return null;
        }
    }

    async function writeRecord(group, key, rec) {
        const ck = comp(group, key);
        lruSet(ck, rec, state.cfg.maxEntries);
        state.sets++;
        if (state.cfg.persist) {
            const storage = getStorage();
            if (storage) {
                const ttl = Math.max(0, rec.staleUntil - now());
                await storage.set(ck, { value: rec.value, hardTtl: rec.hardTtl, staleUntil: rec.staleUntil }, { ttlMs: ttl }).catch(() => { });
            }
        }
        publish("cache:set", { group, key });
        return true;
    }

    async function getValue(group, key) {
        const rec = await readRecord(group, key);
        if (!rec) return null;
        const t = now();
        if (isExpired(rec, t)) return null;
        return rec.value;
    }

    async function setValue(group, key, value, opts = {}) {
        const ttlMs = Number.isFinite(opts.ttlMs) ? Number(opts.ttlMs) : state.cfg.ttlMs;
        const swrMs = Number.isFinite(opts.swrMs) ? Number(opts.swrMs) : state.cfg.swrMs;
        const t = now();
        const rec = { value, hardTtl: t + ttlMs, staleUntil: t + ttlMs + Math.max(0, swrMs) };
        return writeRecord(group, key, rec);
    }

    async function delValue(group, key) {
        const ck = comp(group, key);
        state.mem.delete(ck);
        if (state.cfg.persist) {
            const storage = getStorage();
            if (storage) await storage.remove(ck).catch(() => { });
        }
        publish("cache:invalidate", { group, key });
        return true;
    }

    async function clearGroup(group) {
        // Remove mem entries by prefix
        const prefix = `${group}::`;
        for (const k of Array.from(state.mem.keys())) {
            if (k.startsWith(prefix)) state.mem.delete(k);
        }
        if (state.cfg.persist) {
            const storage = getStorage();
            if (storage) {
                // storage has only namespace-level clear; we need to remove keys of this group
                const keys = await storage.keys();
                for (const k of keys) {
                    if (k.startsWith(prefix)) {
                        await storage.remove(k).catch(() => { });
                    }
                }
            }
        }
        publish("cache:invalidate", { group });
        return true;
    }

    function defaultKeyFn(args) {
        return JSON.stringify(args.length === 1 ? args[0] : Array.from(args));
    }

    function configure(cfg = {}) {
        state.cfg = { ...DEFAULTS, ...cfg };
        if (state.cfg.persist === undefined) {
            state.cfg.persist = !!modules.get("storage");
        }
        // create storage namespace lazily on first use
        publish("cache:invalidate", { group: "*" });
        return { ...state.cfg };
    }

    function stats() {
        return {
            size: state.mem.size,
            hits: state.hits,
            misses: state.misses,
            sets: state.sets,
            inflight: state.inflight.size
        };
    }

    function memoize(group, fn, options = {}) {
        if (typeof group !== "string" || !group.trim()) {
            throw new Error("cache.memoize: group must be a non-empty string");
        }
        if (typeof fn !== "function") {
            throw new Error("cache.memoize: fn must be a function");
        }
        const keyFn = typeof options.keyFn === "function" ? options.keyFn : defaultKeyFn;
        const ttlMs = Number.isFinite(options.ttlMs) ? Number(options.ttlMs) : state.cfg.ttlMs;
        const swrMs = Number.isFinite(options.swrMs) ? Number(options.swrMs) : state.cfg.swrMs;
        const cacheErrors = options.cacheErrors === true || state.cfg.cacheErrors === true;

        async function computeAndStore(key, args) {
            const ck = comp(group, key);
            if (state.inflight.has(ck)) return state.inflight.get(ck);
            const p = Promise.resolve().then(() => fn(...args))
                .then(async (value) => {
                    await setValue(group, key, value, { ttlMs, swrMs });
                    return value;
                })
                .catch(async (err) => {
                    if (cacheErrors) {
                        await setValue(group, key, { __error: true, error: String(err) }, { ttlMs, swrMs });
                        return { __error: true, error: String(err) };
                    }
                    throw err;
                })
                .finally(() => {
                    state.inflight.delete(ck);
                });
            state.inflight.set(ck, p);
            return p;
        }

        return async function memoized(...args) {
            const key = keyFn(args);
            const ck = comp(group, key);
            const t = now();
            const rec = await readRecord(group, key);
            if (rec) {
                if (t <= rec.hardTtl) {
                    state.hits++;
                    publish("cache:hit", { group, key });
                    return rec.value;
                }
                if (t <= rec.staleUntil && swrMs > 0) {
                    // serve stale and refresh in background
                    state.hits++;
                    publish("cache:hit", { group, key, stale: true });
                    // Fire and forget background refresh
                    computeAndStore(key, args).catch(() => { });
                    return rec.value;
                }
            }
            state.misses++;
            publish("cache:miss", { group, key });
            return computeAndStore(key, args);
        };
    }

    const IFACE = Object.freeze({
        configure,
        memoize,
        async get(group, key) { return getValue(group, key); },
        async set(group, key, value, opts) { return setValue(group, key, value, opts); },
        async del(group, key) { return delValue(group, key); },
        async clear(group) {
            if (group == null) {
                state.mem.clear();
                const storage = getStorage();
                if (storage) await storage.clear();
                publish("cache:invalidate", { group: "*" });
                return true;
            }
            return clearGroup(group);
        },
        stats
    });

    modules.add("cache", IFACE);
})(globalThis.SJS.modules || null);

/* NEW MODULE: http
   A small, robust HTTP client built on fetch with:
   - baseURL, default headers, timeout, retries with exponential backoff and jitter
   - JSON/text/arrayBuffer/blob/raw parsing (auto-detect JSON by content-type)
   - Optional GET response caching via the cache module (ttl/swr)
   - Simple before/after interceptors
   - PubSub events: "http:request", "http:response", "http:error"

   API:
     http.configure({
       baseURL?: string,
       headers?: Record<string,string>,
       timeoutMs?: number,       // default 15000
       retries?: number,         // default 2 (total attempts = retries + 1)
       retryDelayMs?: number,    // base delay, default 300
       backoffFactor?: number,   // default 2.0
       retryStatuses?: number[], // default [408, 429, 500, 502, 503, 504]
       cacheGet?: boolean,       // default false (requires cache module to be effective)
       cacheTtlMs?: number,      // default 10000
       cacheSwrMs?: number       // default 0
     })
     http.request(opts)
       opts = {
         method?: string, url: string, params?: Record<string,any>|URLSearchParams,
         headers?: Record<string,string>, body?: any, data?: any,
         responseType?: 'json'|'text'|'blob'|'arrayBuffer'|'raw', // default auto (json|text)
         signal?: AbortSignal, timeoutMs?, retries?, retryDelayMs?, backoffFactor?, retryStatuses?,
         cache?: boolean, cacheTtlMs?, cacheSwrMs?
       }
     http.get/post/put/patch/del/head(url, options?)
*/
(function (modules) {
    const pubSub = () => modules.get && modules.get("pubSub");
    const cacheMod = () => modules.get && modules.get("cache");

    const DEFAULTS = {
        baseURL: "",
        headers: {},
        timeoutMs: 15000,
        retries: 2,
        retryDelayMs: 300,
        backoffFactor: 2.0,
        retryStatuses: [408, 429, 500, 502, 503, 504],
        cacheGet: false,
        cacheTtlMs: 10000,
        cacheSwrMs: 0
    };

    const state = {
        cfg: { ...DEFAULTS },
        interceptors: {
            before: [], // (opts) => opts | Promise<opts>
            after: []   // (ctx) => void | Promise<void>  where ctx: { request, response?, error? }
        }
    };

    function configure(cfg = {}) {
        state.cfg = { ...state.cfg, ...cfg };
        return { ...state.cfg };
    }

    function toHeaders(h) {
        const out = {};
        for (const k of Object.keys(h || {})) {
            if (h[k] == null) continue;
            out[k] = String(h[k]);
        }
        return out;
    }

    function buildURL(baseURL, url, params) {
        const hasBase = typeof baseURL === "string" && baseURL.length > 0;
        const full = hasBase && !/^https?:\/\//i.test(url) ? baseURL.replace(/\/+$/, "") + "/" + String(url).replace(/^\/+/, "") : url;
        if (!params) return full;
        const u = new URL(full, typeof window !== "undefined" ? window.location.origin : "http://localhost");
        if (params instanceof URLSearchParams) {
            for (const [k, v] of params.entries()) u.searchParams.append(k, v);
        } else if (typeof params === "object") {
            for (const [k, v] of Object.entries(params)) {
                if (v === undefined || v === null) continue;
                if (Array.isArray(v)) {
                    for (const item of v) u.searchParams.append(k, String(item));
                } else {
                    u.searchParams.set(k, String(v));
                }
            }
        }
        return u.toString();
    }

    function sleep(ms) {
        return new Promise(res => setTimeout(res, ms));
    }

    function jitter(ms) {
        const r = Math.random() * 0.3 + 0.85; // 0.85..1.15
        return Math.max(0, Math.floor(ms * r));
    }

    async function applyBeforeInterceptors(opts) {
        let cur = opts;
        for (const fn of state.interceptors.before) {
            try {
                // allow mutation or replacement
                const ret = await Promise.resolve(fn(cur));
                if (ret) cur = ret;
            } catch (e) {
                (pubSub() && pubSub().publish("http:error", { stage: "before", error: String(e), request: cur }));
            }
        }
        return cur;
    }

    async function applyAfterInterceptors(ctx) {
        for (const fn of state.interceptors.after) {
            try {
                await Promise.resolve(fn(ctx));
            } catch (e) {
                (pubSub() && pubSub().publish("http:error", { stage: "after", error: String(e), request: ctx.request }));
            }
        }
    }

    function shouldRetry(attempt, maxRetries, err, res, retryStatuses) {
        if (attempt >= maxRetries) return false;
        if (err) return true; // network/timeout/abort (treat as retryable except abort by caller)
        if (!res) return false;
        return retryStatuses.includes(res.status);
    }

    async function parseResponse(res, responseType) {
        if (responseType === "raw") return res;
        try {
            if (responseType === "json") return await res.json();
            if (responseType === "text") return await res.text();
            if (responseType === "blob" && typeof res.blob === "function") return await res.blob();
            if (responseType === "arrayBuffer") return await res.arrayBuffer();

            // auto-detect
            const ct = res.headers.get("content-type") || "";
            if (ct.includes("application/json")) {
                return await res.json();
            }
            return await res.text();
        } catch (e) {
            return null;
        }
    }

    function makeCacheKey(method, url, headers) {
        const normalizedHeaders = {};
        for (const [k, v] of Object.entries(headers || {})) {
            // Only include simple headers to not explode key cardinality
            if (/^(accept|content-type|authorization)$/i.test(k)) {
                normalizedHeaders[k.toLowerCase()] = v;
            }
        }
        return `${method.toUpperCase()} ${url} ${JSON.stringify(normalizedHeaders)}`;
    }

    // Helper: bridge user signal to attempt controller
    function bridgeAbortSignals(userSignal, attemptController) {
        if (!userSignal || !attemptController) return () => { };
        const onAbort = () => {
            try { attemptController.abort(userSignal.reason || new Error("aborted")); } catch { }
        };
        if (userSignal.aborted) {
            onAbort();
            return () => { };
        }
        userSignal.addEventListener("abort", onAbort, { once: true });
        return () => {
            try { userSignal.removeEventListener("abort", onAbort, { once: true }); } catch { }
        };
    }

    async function request(options) {
        if (typeof fetch !== "function") {
            throw new Error("http: fetch is not available in this environment");
        }
        const cfg = state.cfg;
        const {
            method = "GET",
            url,
            params,
            headers,
            body,
            data,
            responseType,
            signal,
            timeoutMs,
            retries,
            retryDelayMs,
            backoffFactor,
            retryStatuses,
            cache: cacheFlag,
            cacheTtlMs,
            cacheSwrMs
        } = options || {};
        if (!url) throw new Error("http.request: url is required");

        const finalURL = buildURL(cfg.baseURL, url, params);
        const hdrs = { ...toHeaders(cfg.headers), ...toHeaders(headers) };
        const meth = String(method || "GET").toUpperCase();

        let payload = body !== undefined ? body : data;
        if (payload && typeof payload === "object" && !(payload instanceof ArrayBuffer) && !(payload instanceof Blob) && !(payload instanceof FormData)) {
            if (!hdrs["Content-Type"] && !hdrs["content-type"]) {
                hdrs["Content-Type"] = "application/json";
            }
            if ((hdrs["Content-Type"] || hdrs["content-type"]).includes("application/json")) {
                try { payload = JSON.stringify(payload); } catch { /* leave as-is */ }
            }
        }

        const req = {
            method: meth,
            url: finalURL,
            headers: hdrs,
            body: payload,
            responseType,
            timeoutMs: Number.isFinite(timeoutMs) ? timeoutMs : cfg.timeoutMs,
            retries: Number.isFinite(retries) ? retries : cfg.retries,
            retryDelayMs: Number.isFinite(retryDelayMs) ? retryDelayMs : cfg.retryDelayMs,
            backoffFactor: Number.isFinite(backoffFactor) ? backoffFactor : cfg.backoffFactor,
            retryStatuses: Array.isArray(retryStatuses) ? retryStatuses : cfg.retryStatuses
        };

        // Interceptors (before)
        const req2 = await applyBeforeInterceptors(req);

        // Optional GET cache
        const wantCache = (cacheFlag === true) || (cacheFlag === undefined && cfg.cacheGet === true);
        const isCacheable = wantCache && req2.method === "GET";
        const ttl = Number.isFinite(cacheTtlMs) ? cacheTtlMs : cfg.cacheTtlMs;
        const swr = Number.isFinite(cacheSwrMs) ? cacheSwrMs : cfg.cacheSwrMs;
        const cacheModule = cacheMod();
        const cacheKey = isCacheable ? makeCacheKey(req2.method, req2.url, req2.headers) : null;

        if (isCacheable && cacheModule) {
            const v = await cacheModule.get("http", cacheKey);
            if (v != null) {
                pubSub() && pubSub().publish("http:response", { request: req2, response: { cached: true, status: 200, data: v } });
                await applyAfterInterceptors({ request: req2, response: { cached: true, status: 200, data: v } });
                return v;
            }
        }

        pubSub() && pubSub().publish("http:request", { request: req2 });

        let attempt = 0;
        let lastErr = null;
        let lastRes = null;

        while (attempt <= req2.retries) {
            // Per-attempt abort controller and timeout
            const attemptController = typeof AbortController !== "undefined" ? new AbortController() : null;
            const cleanupBridge = bridgeAbortSignals(signal, attemptController);
            let timeoutId = null;
            if (attemptController && req2.timeoutMs > 0) {
                try {
                    timeoutId = setTimeout(() => {
                        try { attemptController.abort(new Error("timeout")); } catch { }
                    }, req2.timeoutMs);
                } catch { /* ignore */ }
            }

            try {
                const res = await fetch(req2.url, {
                    method: req2.method,
                    headers: req2.headers,
                    body: req2.body,
                    signal: attemptController ? attemptController.signal : signal
                });
                lastRes = res;

                if (!res.ok && shouldRetry(attempt, req2.retries, null, res, req2.retryStatuses)) {
                    const delay = jitter(req2.retryDelayMs * Math.pow(req2.backoffFactor, attempt));
                    attempt++;
                    if (timeoutId) clearTimeout(timeoutId);
                    cleanupBridge();
                    await sleep(delay);
                    continue;
                }

                const parsed = await parseResponse(res, req2.responseType);
                if (!res.ok) {
                    const errObj = new Error(`HTTP ${res.status}`);
                    errObj.status = res.status;
                    errObj.data = parsed;
                    pubSub() && pubSub().publish("http:error", { request: req2, error: errObj });
                    await applyAfterInterceptors({ request: req2, error: errObj });
                    throw errObj;
                }

                if (isCacheable && cacheModule) {
                    await cacheModule.set("http", cacheKey, parsed, { ttlMs: ttl, swrMs: swr });
                }
                const ctx = { request: req2, response: { status: res.status, data: parsed } };
                pubSub() && pubSub().publish("http:response", ctx);
                await applyAfterInterceptors(ctx);
                if (timeoutId) clearTimeout(timeoutId);
                cleanupBridge();
                return parsed;
            } catch (e) {
                lastErr = e;
                // If user aborted explicitly, don't retry
                const name = e && (e.name || "");
                const msg = e && (e.message || "");
                const userAborted = name === "AbortError" || msg.toLowerCase().includes("aborted");
                if (userAborted) {
                    if (timeoutId) clearTimeout(timeoutId);
                    cleanupBridge();
                    break;
                }
                if (shouldRetry(attempt, req2.retries, e, lastRes, req2.retryStatuses)) {
                    const delay = jitter(req2.retryDelayMs * Math.pow(req2.backoffFactor, attempt));
                    attempt++;
                    if (timeoutId) clearTimeout(timeoutId);
                    cleanupBridge();
                    await sleep(delay);
                    continue;
                }
                if (timeoutId) clearTimeout(timeoutId);
                cleanupBridge();
                break;
            }
        }

        const errorPayload = { request: req2, error: lastErr && String(lastErr), status: lastRes?.status };
        pubSub() && pubSub().publish("http:error", errorPayload);
        await applyAfterInterceptors({ request: req2, error: lastErr });
        throw lastErr || new Error("http: unknown error");
    }

    function requestShorthand(method) {
        return function (url, options = {}) {
            return request({ ...options, method, url });
        };
    }

    const IFACE = Object.freeze({
        configure,
        addBefore(fn) { if (typeof fn === "function") state.interceptors.before.push(fn); return this; },
        addAfter(fn) { if (typeof fn === "function") state.interceptors.after.push(fn); return this; },
        request,
        get: requestShorthand("GET"),
        post: requestShorthand("POST"),
        put: requestShorthand("PUT"),
        patch: requestShorthand("PATCH"),
        del: requestShorthand("DELETE"),
        head: requestShorthand("HEAD")
    });

    if (!modules || !modules.add) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }
    modules.add("http", IFACE);
})(globalThis.SJS.modules || null);

/* NEW MODULE: metrics
   Lightweight metrics collector for counters, gauges, histograms, and timers.
   - Optional auto-capture of Long Tasks and Resource timing (when supported by browser).
   - Optional persistence via storage module ("metrics" namespace).
   - Optional periodic flush via http module (POST JSON payload).
   - PubSub integration for observability.

   API:
     metrics.configure({
       enabled?: boolean,
       defaultBuckets?: number[],            // histogram buckets in ms (default latencies)
       persist?: boolean,                    // if true and storage present, persist snapshot on flush
       namespace?: string,                   // storage namespace (default "metrics")
       exportIntervalMs?: number|null,       // if set > 0, auto-flush on interval
       httpEndpoint?: string|null,           // if set, POST snapshot JSON to this URL
       httpHeaders?: Record<string,string>,  // headers for POST
       autoLongTask?: boolean,               // auto-capture "longtask" entries (browser)
       autoResource?: boolean                // auto-capture resource timing summary (browser)
     })
     - counterInc(name, value=1, labels?)
     - gaugeSet(name, value, labels?), gaugeInc(name, delta=1, labels?), gaugeDec(name, delta=1, labels?)
     - histObserve(name, value, labels?, buckets?)
     - timeStart(name, labels?) -> token
     - timeEnd(tokenOrName, labels?) -> durationMs
     - timeAsync(name, fn, labels?) -> Promise<value>
     - mark(name), measure(name, startMark, endMark?) -> durationMs
     - snapshot(reset=false)
     - reset()
     - flush() -> { pushed?: boolean, persisted?: boolean }
     - start(), stop()  // starts or stops auto capture / interval
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }

    const DEFAULTS = {
        enabled: true,
        defaultBuckets: [5, 10, 25, 50, 100, 250, 500, 1000, 2500, 5000, 10000],
        persist: true,
        namespace: "metrics",
        exportIntervalMs: null,
        httpEndpoint: null,
        httpHeaders: { "Content-Type": "application/json" },
        autoLongTask: false,
        autoResource: false
    };

    const state = {
        cfg: { ...DEFAULTS },
        counters: new Map(),     // key -> number
        gauges: new Map(),       // key -> number
        histograms: new Map(),   // key -> { buckets: number[], counts: number[], sum: number, count: number }
        timers: new Map(),       // id -> { name, labelsKey, start, hrStart }
        intervalId: null,
        perfObs: null,
        perfObsRes: null,
        seq: 0
    };

    function getPubSub() { return modules.get("pubSub"); }
    function getStorage() { const s = modules.get("storage"); return s ? s.use(state.cfg.namespace) : null; }
    function getHttp() { return modules.get("http"); }
    function now() { return (typeof performance !== "undefined" && performance.now) ? performance.now() : Date.now(); }
    function nowISO() { try { return new Date().toISOString(); } catch { return String(Date.now()); } }

    function labelsKey(labels) {
        if (!labels || typeof labels !== "object") return "";
        const entries = Object.entries(labels).map(([k, v]) => [String(k), String(v)]);
        entries.sort((a, b) => a[0] < b[0] ? -1 : a[0] > b[0] ? 1 : (a[1] < b[1] ? -1 : a[1] > b[1] ? 1 : 0));
        return entries.map(([k, v]) => `${k}=${v}`).join(",");
    }
    function metricKey(name, labels) {
        const lk = labelsKey(labels);
        return lk ? `${name}|${lk}` : name;
    }

    function ensureHistogram(name, labels, buckets) {
        const key = metricKey(name, labels);
        let h = state.histograms.get(key);
        if (!h) {
            const b = Array.isArray(buckets) && buckets.length ? buckets.slice().sort((a, b) => a - b) : state.cfg.defaultBuckets.slice();
            h = { buckets: b, counts: new Array(b.length + 1).fill(0), sum: 0, count: 0, name, labels: labels || {} };
            state.histograms.set(key, h);
        }
        return h;
    }

    function counterInc(name, value = 1, labels) {
        const key = metricKey(name, labels);
        const cur = state.counters.get(key) || 0;
        const nv = cur + Number(value || 0);
        state.counters.set(key, nv);
        const ps = getPubSub(); ps && ps.publish("metrics:counter", { ts: nowISO(), name, labels: labels || {}, value: nv, inc: value });
        return nv;
    }

    function gaugeSet(name, value, labels) {
        const key = metricKey(name, labels);
        const nv = Number(value);
        state.gauges.set(key, nv);
        const ps = getPubSub(); ps && ps.publish("metrics:gauge", { ts: nowISO(), name, labels: labels || {}, value: nv });
        return nv;
    }
    function gaugeInc(name, delta = 1, labels) {
        const key = metricKey(name, labels);
        const nv = (state.gauges.get(key) || 0) + Number(delta);
        state.gauges.set(key, nv);
        const ps = getPubSub(); ps && ps.publish("metrics:gauge", { ts: nowISO(), name, labels: labels || {}, value: nv, delta });
        return nv;
    }
    function gaugeDec(name, delta = 1, labels) {
        return gaugeInc(name, -Number(delta), labels);
    }

    function histObserve(name, value, labels, buckets) {
        const v = Number(value) || 0;
        const h = ensureHistogram(name, labels, buckets);
        h.sum += v; h.count += 1;
        // bucket index
        let idx = h.buckets.findIndex(b => v <= b);
        if (idx === -1) idx = h.counts.length - 1;
        h.counts[idx] += 1;
        const ps = getPubSub(); ps && ps.publish("metrics:histogram", { ts: nowISO(), name, labels: labels || {}, value: v });
        return true;
    }

    function timeStart(name, labels) {
        const id = ++state.seq;
        state.timers.set(id, { name, labelsKey: labelsKey(labels), labels: labels || {}, start: now() });
        return id;
    }

    function timeEnd(tokenOrName, labels) {
        let id = null, rec = null;
        if (typeof tokenOrName === "number") {
            id = tokenOrName;
            rec = state.timers.get(id) || null;
        } else {
            // find latest matching by name + labels
            const k = labelsKey(labels);
            let last = null;
            for (const [tid, t] of state.timers.entries()) {
                if (t.name === tokenOrName && t.labelsKey === k) last = [tid, t];
            }
            if (last) { id = last[0]; rec = last[1]; }
        }
        if (!rec) return null;
        const dur = Math.max(0, now() - rec.start);
        state.timers.delete(id);
        histObserve(`timer:${rec.name}`, dur, rec.labels);
        const ps = getPubSub(); ps && ps.publish("metrics:timer", { ts: nowISO(), name: rec.name, labels: rec.labels, durationMs: dur });
        return dur;
    }

    async function timeAsync(name, fn, labels) {
        const id = timeStart(name, labels);
        try {
            const val = await Promise.resolve().then(() => fn());
            return val;
        } finally {
            timeEnd(id);
        }
    }

    function mark(name) {
        if (typeof performance !== "undefined" && performance.mark) {
            try { performance.mark(name); } catch { }
        }
    }

    function measure(name, startMark, endMark) {
        let dur = null;
        if (typeof performance !== "undefined" && performance.measure && performance.getEntriesByName) {
            try {
                if (endMark) performance.mark(endMark);
                performance.measure(name, startMark, endMark);
                const arr = performance.getEntriesByName(name);
                const last = arr[arr.length - 1];
                dur = last ? last.duration : null;
            } catch { dur = null; }
        }
        if (dur != null) histObserve(`measure:${name}`, dur, {});
        return dur;
    }

    function snapshot(reset = false) {
        const counters = [];
        for (const [k, v] of state.counters.entries()) counters.push({ key: k, value: v });
        const gauges = [];
        for (const [k, v] of state.gauges.entries()) gauges.push({ key: k, value: v });
        const histograms = [];
        for (const [k, h] of state.histograms.entries()) {
            histograms.push({ key: k, buckets: h.buckets.slice(), counts: h.counts.slice(), sum: h.sum, count: h.count });
        }
        const snap = { ts: nowISO(), counters, gauges, histograms };
        if (reset) {
            state.counters.clear();
            state.gauges.clear();
            state.histograms.clear();
        }
        return snap;
    }

    function reset() {
        state.counters.clear();
        state.gauges.clear();
        state.histograms.clear();
        return true;
    }

    async function flush() {
        const snap = snapshot(true);
        let persisted = false, pushed = false;

        try {
            if (state.cfg.persist) {
                const storage = getStorage();
                if (storage) {
                    const k = `metrics:${Date.now()}`;
                    await storage.set(k, snap, { ttlMs: 7 * 24 * 60 * 60 * 1000 });
                    persisted = true;
                }
            }
        } catch { persisted = false; }

        try {
            if (state.cfg.httpEndpoint) {
                const http = getHttp();
                if (http) {
                    await http.post(state.cfg.httpEndpoint, { body: snap, headers: state.cfg.httpHeaders || {} });
                    pushed = true;
                }
            }
        } catch { pushed = false; }

        const ps = getPubSub(); ps && ps.publish("metrics:flush", { ts: nowISO(), persisted, pushed, size: snap.counters.length + snap.gauges.length + snap.histograms.length });
        return { persisted, pushed };
    }

    function stop() {
        if (state.intervalId) {
            try { clearInterval(state.intervalId); } catch { }
            state.intervalId = null;
        }
        if (state.perfObs) {
            try { state.perfObs.disconnect(); } catch { }
            state.perfObs = null;
        }
        if (state.perfObsRes) {
            try { state.perfObsRes.disconnect(); } catch { }
            state.perfObsRes = null;
        }
        return true;
    }

    function start() {
        stop();
        if (!state.cfg.enabled) return false;

        // Auto-flush interval
        if (Number.isFinite(state.cfg.exportIntervalMs) && state.cfg.exportIntervalMs > 0) {
            state.intervalId = setInterval(() => { flush().catch(() => { }); }, state.cfg.exportIntervalMs);
        }

        // Auto-capture long tasks
        if (typeof PerformanceObserver !== "undefined" && state.cfg.autoLongTask) {
            try {
                state.perfObs = new PerformanceObserver((list) => {
                    for (const entry of list.getEntries()) {
                        histObserve("longtask", entry.duration, { name: entry.name || "longtask" });
                    }
                });
                state.perfObs.observe({ entryTypes: ["longtask"] });
            } catch { state.perfObs = null; }
        }

        // Auto-capture resource timing summary (size/duration)
        if (typeof PerformanceObserver !== "undefined" && state.cfg.autoResource) {
            try {
                state.perfObsRes = new PerformanceObserver((list) => {
                    for (const entry of list.getEntries()) {
                        // Summarize only main-resource and fetch/xhr types
                        if (entry.initiatorType && /^(fetch|xmlhttprequest|script|img|css|link|navigation)$/.test(entry.initiatorType)) {
                            histObserve(`resource:${entry.initiatorType}:duration`, entry.duration, { name: entry.name });
                            if (entry.transferSize != null) {
                                histObserve(`resource:${entry.initiatorType}:size`, entry.transferSize, { name: entry.name },
                                    [1024, 10 * 1024, 100 * 1024, 1024 * 1024, 5 * 1024 * 1024]);
                            }
                        }
                    }
                });
                state.perfObsRes.observe({ entryTypes: ["resource"] });
            } catch { state.perfObsRes = null; }
        }

        return true;
    }

    function configure(cfg = {}) {
        state.cfg = { ...state.cfg, ...(globalThis.SJS?.config?.metrics || {}), ...cfg };
        if (state.cfg.enabled && (state.intervalId || state.perfObs || state.perfObsRes)) {
            // reconfigure running collectors
            start();
        }
        return { ...state.cfg };
    }

    const IFACE = Object.freeze({
        configure,
        start,
        stop,
        counterInc,
        gaugeSet,
        gaugeInc,
        gaugeDec,
        histObserve,
        timeStart,
        timeEnd,
        timeAsync,
        mark,
        measure,
        snapshot,
        reset,
        flush
    });

    try {
        configure();
        if (state.cfg.enabled) start();
    } catch { }

    modules.add("metrics", IFACE);
})(globalThis.SJS.modules || null);

/* NEW MODULE: router (with micro template engine)
   Client-side router for SPAs with a small, fast template compiler.
   - History/hash/memory modes
   - Lightweight path patterns: /users/:id, optional "/:slug?"
   - Wildcards "*" catch-all
   - Link interception, popstate/hashchange handling
   - Pluggable route hooks and simple data loaders
   - Tiny template engine (<%, <%=, <%-) compatible with Underscore/Lodash-style templates
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! cannot add module! moduleManager does not exist.");
        return;
    }
    const g = globalThis;

    function isBrowser() {
        return typeof window !== "undefined" && typeof document !== "undefined";
    }

    const pubSub = () => modules.get && modules.get("pubSub");
    const http = () => modules.get && modules.get("http");
    const logger = () => modules.get && modules.get("logger")?.create("router");

    const DEFAULTS = {
        enabled: true,
        mode: "history",          // "history" | "hash" | "memory"
        basePath: "",
        root: "#app",
        autoStart: true,
        linksSelector: "a[href]",
        linkActiveClass: "active",
        scrollBehavior: "auto",   // "auto" | "top" | "restore"
        notFoundTemplate: "<h1>404 - Not Found</h1>",
        trailingSlash: "ignore",  // "ignore" | "always" | "never"
        emitPubSub: true,
        decodeParams: true
    };

    const state = {
        cfg: { ...DEFAULTS },
        started: false,
        routes: [], // [{ name, path, keys, re, ... }]
        current: null, // { path, fullPath, params, query, hash, route, state }
        history: [], // for memory mode
        historyIndex: -1,
        listeners: [],
        rootEl: null
    };

    function publish(topic, payload) {
        if (!state.cfg.emitPubSub) return;
        const ps = pubSub();
        if (ps) ps.publish(topic, payload);
    }

    function normalizeTrailingSlash(path) {
        const p = String(path || "");
        if (state.cfg.trailingSlash === "ignore") return p;
        if (state.cfg.trailingSlash === "always") {
            if (p === "" || p === "/") return "/";
            return p.endsWith("/") ? p : p + "/";
        }
        // "never"
        if (p === "/") return "/";
        return p.endsWith("/") ? p.slice(0, -1) : p;
    }

    function withBase(path) {
        const base = String(state.cfg.basePath || "");
        if (!base) return path;
        const bp = base.replace(/\/+$/, "");
        const p = String(path || "").replace(/^\/+/, "");
        return bp + "/" + p;
    }

    function stripBase(path) {
        const base = String(state.cfg.basePath || "");
        if (!base) return path;
        const bp = base.replace(/\/+$/, "");
        if (path === bp) return "/";
        if (path.startsWith(bp + "/")) return path.slice(bp.length);
        return path;
    }

    function parseQuery(qs) {
        const out = {};
        if (!qs) return out;
        const s = qs.startsWith("?") ? qs.slice(1) : qs;
        if (!s) return out;
        for (const part of s.split("&")) {
            if (!part) continue;
            const [k, v] = part.split("=");
            const key = decodeURIComponent(k || "");
            const val = v != null ? decodeURIComponent(v) : "";
            if (out[key] === undefined) out[key] = val;
            else if (Array.isArray(out[key])) out[key].push(val);
            else out[key] = [out[key], val];
        }
        return out;
    }

    function buildQuery(obj) {
        if (!obj || typeof obj !== "object") return "";
        const usp = new URLSearchParams();
        for (const [k, v] of Object.entries(obj)) {
            if (v == null) continue;
            if (Array.isArray(v)) {
                for (const item of v) usp.append(k, String(item));
            } else {
                usp.set(k, String(v));
            }
        }
        const s = usp.toString();
        return s ? "?" + s : "";
    }

    function decodeParam(val) {
        if (!state.cfg.decodeParams) return val;
        try { return decodeURIComponent(val); } catch { return val; }
    }

    function compilePath(path) {
        // Create a regex and keys array from a path like "/users/:id/:tab?"
        const keys = [];
        let reStr = String(path || "/");

        // Escape regex special chars first, except for ":, *, /, ?"
        reStr = reStr.replace(/([.+^${}()|[\]\\])/g, "\\$1");

        // Optional params "/:id?" -> "(?:/([^/]+))?"
        reStr = reStr.replace(/\/:([A-Za-z0-9_]+)\?/g, function (_, key) {
            keys.push(key);
            return "(?:\\/([^\\/]+))?";
        });

        // Normal params "/:id" -> "/([^/]+)"
        reStr = reStr.replace(/\/:([A-Za-z0-9_]+)/g, function (_, key) {
            keys.push(key);
            return "\\/([^\\/]+)";
        });

        // Wildcards "*" -> "(.*)"
        reStr = reStr.replace(/\*/g, "(.*)");

        // Allow optional trailing slash
        reStr = "^" + reStr + "\\/?$";

        return { re: new RegExp(reStr), keys };
    }

    function matchRoute(pathname) {
        const path = normalizeTrailingSlash(pathname);
        for (const r of state.routes) {
            const m = r.re.exec(path);
            if (m) {
                const params = {};
                for (let i = 0; i < r.keys.length; i++) {
                    const raw = m[i + 1] || "";
                    params[r.keys[i]] = decodeParam(raw);
                }
                return { route: r, params };
            }
        }
        return null;
    }

    function buildPathFromPattern(pattern, params) {
        let p = String(pattern || "/");
        if (!params) params = {};
        // Remove optional params if not provided
        p = p.replace(/\/:([A-Za-z0-9_]+)\?/g, function (_, key) {
            if (params[key] == null) return "";
            return "/" + encodeURIComponent(String(params[key]));
        });
        // Replace required params
        p = p.replace(/\/:([A-Za-z0-9_]+)/g, function (_, key) {
            if (params[key] == null) throw new Error(`router.build: missing param "${key}"`);
            return "/" + encodeURIComponent(String(params[key]));
        });
        // Make sure normalization matches config
        p = normalizeTrailingSlash(p);
        return p;
    }

    function getLocationParts() {
        if (!isBrowser()) {
            return { fullPath: "/", path: "/", query: {}, hash: "" };
        }
        if (state.cfg.mode === "hash") {
            // Expect paths in location.hash like "#/path?query#hash"
            const raw = window.location.hash || "#/";
            const hash = raw.startsWith("#") ? raw.slice(1) : raw;
            const url = new URL(hash, window.location.origin);
            const path = url.pathname || "/";
            return {
                fullPath: hash,
                path: normalizeTrailingSlash(path),
                query: parseQuery(url.search),
                hash: url.hash || ""
            };
        }

        // history or default
        const loc = window.location;
        const path = stripBase(loc.pathname);
        return {
            fullPath: stripBase(loc.pathname) + (loc.search || "") + (loc.hash || ""),
            path: normalizeTrailingSlash(path),
            query: parseQuery(loc.search),
            hash: loc.hash || ""
        };
    }

    function setHistory(url, { replace = false, stateObj = null } = {}) {
        if (!isBrowser()) return;
        if (state.cfg.mode === "hash") {
            const target = "#" + url.replace(/^#/, "");
            if (replace) window.location.replace(target);
            else window.location.hash = target;
            return;
        }
        const full = withBase(url);
        try {
            if (replace) window.history.replaceState(stateObj, "", full);
            else window.history.pushState(stateObj, "", full);
        } catch {
            // Some browsers may throw for certain URLs; fallback
            window.location.assign(full);
        }
    }

    function onPopState() {
        if (state.cfg.mode === "memory") return; // ignore
        handleLocationChange({ cause: "popstate" }).catch((e) => logger()?.warn("router popstate error", e));
    }

    function onHashChange() {
        if (state.cfg.mode !== "hash") return;
        handleLocationChange({ cause: "hashchange" }).catch((e) => logger()?.warn("router hashchange error", e));
    }

    function isLeftClickNoMod(e) {
        return e && e.button === 0 && !e.metaKey && !e.ctrlKey && !e.shiftKey && !e.altKey;
    }

    function isSameOrigin(href) {
        try {
            const u = new URL(href, window.location.href);
            return u.origin === window.location.origin;
        } catch { return false; }
    }

    function onLinkClick(e) {
        if (!isLeftClickNoMod(e)) return;
        const a = e.target.closest(state.cfg.linksSelector);
        if (!a || a.target && a.target !== "" && a.target !== "_self") return;
        const href = a.getAttribute("href") || "";
        if (!href || href.startsWith("mailto:") || href.startsWith("tel:") || href.startsWith("javascript:")) return;

        if (state.cfg.mode === "hash") {
            if (!href.startsWith("#/") && !href.startsWith("#")) return;
        } else {
            if (!isSameOrigin(href)) return;
        }

        // Prevent browser navigation and use router
        e.preventDefault();
        if (state.cfg.mode === "hash") {
            const target = href.startsWith("#") ? href.slice(1) : href;
            navigate(target).catch((err) => logger()?.warn("navigate error (hash link)", err));
        } else {
            const u = new URL(href, window.location.href);
            const path = stripBase(u.pathname) + (u.search || "") + (u.hash || "");
            navigate(path).catch((err) => logger()?.warn("navigate error (link)", err));
        }
    }

    async function ensureRoot() {
        if (!isBrowser()) return null;
        if (state.rootEl && state.rootEl.nodeType) return state.rootEl;
        let el = state.cfg.root;
        if (el && typeof el === "string") {
            el = document.querySelector(el);
        }
        if (!el || !el.nodeType) {
            // no root present; create one
            const created = document.createElement("div");
            created.id = "app";
            document.body.appendChild(created);
            el = created;
        }
        state.rootEl = el;
        return el;
    }

    // Template engine (Underscore/Lodash-style)
    const template = (function () {
        const settings = {
            escape: /<%-([\s\S]+?)%>/g,
            interpolate: /<%=([\s\S]+?)%>/g,
            evaluate: /<%([\s\S]+?)%>/g
        };
        const noMatch = /(.)^/;
        const escapes = {
            "&": "&amp;",
            "<": "&lt;",
            ">": "&gt;",
            '"': "&quot;",
            "'": "&#x27;",
            "`": "&#x60;"
        };
        const escaper = /[&<>"'`]/g;
        function escapeHTML(str) {
            const s = String(str == null ? "" : str);
            return s.replace(escaper, (m) => escapes[m] || m);
        }

        const registry = Object.create(null);
        const cache = new Map();

        function compile(text, options = {}) {
            const opt = { ...settings, ...options };
            const matcher = new RegExp([
                opt.escape || noMatch,
                opt.interpolate || noMatch,
                opt.evaluate || noMatch
            ].map(r => r.source).join("|") + "|$", "g");

            let index = 0;
            let source = "__p+='";
            const input = String(text || "");
            input.replace(matcher, function (match, escape, interpolate, evaluate, offset) {
                source += input.slice(index, offset).replace(/\\|'|\r|\n|\u2028|\u2029/g, function (m) {
                    return {
                        "'": "\\'",
                        "\\": "\\\\",
                        "\r": "\\r",
                        "\n": "\\n",
                        "\u2028": "\\u2028",
                        "\u2029": "\\u2029"
                    }[m];
                });
                if (escape) {
                    source += "'+\n((__t=(" + escape + "))==null?'':__e(__t))+\n'";
                } else if (interpolate) {
                    source += "'+\n((__t=(" + interpolate + "))==null?'':__t)+\n'";
                } else if (evaluate) {
                    source += "';\n" + evaluate + "\n__p+='";
                }
                index = offset + match.length;
                return match;
            });
            source += "';\n";

            source = "let __t,__p='';\n" +
                "const __e=__esc;\n" +
                "with(obj||{}){\n" + source + "}\n" +
                "return __p;\n";

            const render = new Function("obj", "__esc", source);
            const fn = function (data) {
                return render.call(this, data, escapeHTML);
            };
            return fn;
        }

        function register(name, text) {
            if (typeof name !== "string" || !name.trim()) throw new Error("template.register: invalid name");
            registry[name] = String(text || "");
            cache.delete(name);
            return true;
        }

        function unregister(name) {
            delete registry[name];
            cache.delete(name);
            return true;
        }

        function has(name) {
            return Object.prototype.hasOwnProperty.call(registry, name);
        }

        function render(nameOrText, data) {
            if (!nameOrText) return "";
            const key = String(nameOrText);
            let fn = cache.get(key);
            if (!fn) {
                const text = has(key) ? registry[key] : key;
                fn = compile(text);
                cache.set(key, fn);
            }
            const out = fn(data || {});
            publish("template:render", { name: has(key) ? key : null, size: out.length });
            return out;
        }

        function fromDOM(selectorOrId) {
            if (!isBrowser()) return "";
            let el = null;
            if (selectorOrId.startsWith("#")) el = document.getElementById(selectorOrId.slice(1));
            else el = document.querySelector(selectorOrId);
            if (!el) return "";
            const txt = (el.tagName.toLowerCase() === "template") ? (el.innerHTML || el.content?.firstChild?.textContent || "") : (el.textContent || el.innerHTML || "");
            return txt || "";
        }

        async function fromURL(url) {
            try {
                const h = http();
                if (!h) throw new Error("http module not available");
                const txt = await h.get(url, { responseType: "text", cache: true });
                return String(txt || "");
            } catch (e) {
                logger()?.warn("template.fromURL error", e);
                return "";
            }
        }

        return Object.freeze({
            compile, render, register, unregister, has, fromDOM, fromURL
        });
    })();

    async function renderToRoot(html) {
        const root = await ensureRoot();
        if (!root) return false;
        if (typeof html === "string") root.innerHTML = html;
        else if (html && html.nodeType) {
            root.innerHTML = "";
            root.appendChild(html);
        } else {
            root.innerHTML = "";
        }
        publish("router:render", { ok: true });
        return true;
    }

    function setActiveLinks(path) {
        if (!isBrowser()) return;
        try {
            const els = document.querySelectorAll(state.cfg.linksSelector);
            for (const el of els) {
                const href = el.getAttribute("href") || "";
                let match = false;
                if (state.cfg.mode === "hash") {
                    const tgt = href.startsWith("#") ? href.slice(1) : href;
                    match = normalizeTrailingSlash(tgt.split("?")[0] || "/") === path;
                } else {
                    const u = new URL(href, window.location.href);
                    match = normalizeTrailingSlash(stripBase(u.pathname)) === path;
                }
                el.classList.toggle(state.cfg.linkActiveClass, match);
            }
        } catch { /* ignore */ }
    }

    async function handleLocationChange({ cause = "navigate", replace = false, pushState = false, navState = null } = {}) {
        if (!isBrowser() && state.cfg.mode !== "memory") return false;

        const parts = state.cfg.mode === "memory" ? state.current : getLocationParts();
        const full = state.cfg.mode === "memory" ? (state.current?.fullPath || "/") : parts.fullPath;
        const path = normalizeTrailingSlash(parts.path);

        const match = matchRoute(path);
        let ctx = {
            fullPath: full,
            path,
            query: parts.query || {},
            hash: parts.hash || "",
            params: {},
            route: null,
            state: navState || null,
            from: state.current
        };

        if (!match) {
            logger()?.warn("router: no match", path);
            publish("router:error", { type: "notfound", path });
            if (state.cfg.notFoundTemplate) {
                await renderToRoot(template.render(state.cfg.notFoundTemplate, { path }));
            }
            setActiveLinks(path);
            state.current = ctx;
            return false;
        }

        ctx.params = match.params;
        ctx.route = match.route;

        // Route/Global hooks
        try {
            publish("router:before", { from: state.current, to: ctx });
            if (typeof match.route.beforeEnter === "function") {
                const ok = await Promise.resolve(match.route.beforeEnter(ctx));
                if (ok === false) {
                    publish("router:error", { type: "blocked", path, route: match.route.name || match.route.path });
                    return false;
                }
            }
            if (Array.isArray(api._beforeEach)) {
                for (const fn of api._beforeEach) {
                    const ok = await Promise.resolve(fn(ctx));
                    if (ok === false) return false;
                }
            }
        } catch (e) {
            logger()?.warn("router before hook error", e);
        }

        // Run optional data loader
        let data = { params: ctx.params, query: ctx.query, hash: ctx.hash, path: ctx.path, fullPath: ctx.fullPath, state: ctx.state };
        if (typeof match.route.load === "function") {
            try {
                const loaded = await Promise.resolve(match.route.load(ctx));
                if (loaded && typeof loaded === "object") data = { ...data, ...loaded };
            } catch (e) {
                publish("router:error", { type: "load", error: String(e), route: match.route.name || match.route.path });
            }
        }

        // Render: preference order render() -> templateName/template -> templateId -> templateSelector -> templateURL
        let html = "";
        try {
            if (typeof match.route.render === "function") {
                html = await Promise.resolve(match.route.render(ctx, data, template));
            } else if (match.route.templateName && template.has(match.route.templateName)) {
                html = template.render(match.route.templateName, data);
            } else if (typeof match.route.template === "string") {
                html = template.render(match.route.template, data);
            } else if (typeof match.route.templateId === "string") {
                const txt = template.fromDOM("#" + match.route.templateId);
                html = template.render(txt, data);
            } else if (typeof match.route.templateSelector === "string") {
                const txt = template.fromDOM(match.route.templateSelector);
                html = template.render(txt, data);
            } else if (typeof match.route.templateURL === "string") {
                const txt = await template.fromURL(match.route.templateURL);
                html = template.render(txt, data);
            } else {
                html = "";
            }
        } catch (e) {
            publish("router:error", { type: "render", error: String(e), route: match.route.name || match.route.path });
            html = template.render(state.cfg.notFoundTemplate || "<h1>Error</h1>", { error: e, path });
        }

        // DOM update
        await renderToRoot(html);

        // Title
        try {
            if (isBrowser() && match.route.title) {
                document.title = typeof match.route.title === "function" ? String(match.route.title(ctx, data) || "") : String(match.route.title);
            }
        } catch { /* ignore */ }

        // Scroll behavior
        try {
            if (isBrowser()) {
                if (state.cfg.scrollBehavior === "top") window.scrollTo({ top: 0 });
                // "auto" leaves default scroll restoration
            }
        } catch { }

        // Active links
        setActiveLinks(path);

        // after hooks
        try {
            if (typeof match.route.afterEnter === "function") {
                await Promise.resolve(match.route.afterEnter(ctx, data));
            }
            if (Array.isArray(api._afterEach)) {
                for (const fn of api._afterEach) {
                    await Promise.resolve(fn(ctx, data));
                }
            }
        } catch (e) {
            logger()?.warn("router after hook error", e);
        }

        publish("router:after", { to: ctx });

        state.current = ctx;
        return true;
    }

    async function navigate(to, options = {}) {
        if (!state.started && state.cfg.mode !== "memory") await api.start();
        const { replace = false, state: navState = null, query = null } = options || {};
        try {
            // Support named route navigation object: { name, params?, query? }
            if (typeof to === "object" && to && to.name) {
                const r = state.routes.find(x => x.name === to.name);
                if (!r) throw new Error(`navigate: unknown route name "${to.name}"`);
                const builtPath = buildPathFromPattern(r.path, to.params || {});
                const url = builtPath + buildQuery(to.query || query || {});
                if (state.cfg.mode === "memory") {
                    const full = url;
                    // memory history
                    state.history = state.history.slice(0, state.historyIndex + 1);
                    state.history.push(full);
                    state.historyIndex++;
                    state.current = { fullPath: full, path: normalizeTrailingSlash(builtPath), query: to.query || {}, hash: "", state: navState };
                    publish("router:navigate", { url: full, replace, memory: true });
                    await handleLocationChange({ cause: "navigate", replace, navState });
                    return true;
                }
                publish("router:navigate", { url, replace });
                setHistory(url, { replace, stateObj: navState });
                return handleLocationChange({ cause: "navigate", replace, navState });
            }

            // to is a string
            const urlObj = new URL(String(to), (isBrowser() ? window.location.origin : "http://localhost"));
            const p = normalizeTrailingSlash(urlObj.pathname);
            const q = urlObj.search ? parseQuery(urlObj.search) : (query || {});
            const url = p + buildQuery(q) + (urlObj.hash || "");

            if (state.cfg.mode === "memory") {
                state.history = state.history.slice(0, state.historyIndex + 1);
                state.history.push(url);
                state.historyIndex++;
                state.current = { fullPath: url, path: p, query: q, hash: urlObj.hash || "", state: navState };
                publish("router:navigate", { url, replace, memory: true });
                await handleLocationChange({ cause: "navigate", replace, navState });
                return true;
            }

            publish("router:navigate", { url, replace });
            setHistory(url, { replace, stateObj: navState });
            return handleLocationChange({ cause: "navigate", replace, navState });
        } catch (e) {
            publish("router:error", { type: "navigate", error: String(e), to });
            return false;
        }
    }

    async function start() {
        if (state.started) return true;
        state.cfg = { ...DEFAULTS, ...(g.SJS?.config?.router || {}), ...state.cfg };
        if (!state.cfg.enabled) return false;
        state.started = true;

        if (!isBrowser() && state.cfg.mode !== "memory") {
            logger()?.info("router start: non-browser environment; no-ops");
            return true;
        }

        // Compile routes
        for (const r of state.routes) {
            const { re, keys } = compilePath(normalizeTrailingSlash(r.path));
            r.re = re;
            r.keys = keys;
        }

        // Listeners
        if (isBrowser()) {
            state.listeners.push(["popstate", onPopState]);
            window.addEventListener("popstate", onPopState);
            state.listeners.push(["hashchange", onHashChange]);
            window.addEventListener("hashchange", onHashChange);
            state.listeners.push(["click", onLinkClick, true]);
            document.addEventListener("click", onLinkClick, true);
        }

        // Initial render
        if (state.cfg.mode === "memory") {
            const first = "/";
            state.history = [first];
            state.historyIndex = 0;
            state.current = { fullPath: first, path: first, query: {}, hash: "", state: null };
            await handleLocationChange({ cause: "start" });
        } else {
            await handleLocationChange({ cause: "start" });
        }

        return true;
    }

    function stop() {
        if (!state.started) return false;
        if (isBrowser()) {
            for (const [ev, fn, cap] of state.listeners) {
                try { (ev === "click" ? document : window).removeEventListener(ev, fn, cap); } catch { }
            }
        }
        state.listeners = [];
        state.started = false;
        return true;
    }

    function back() {
        if (!isBrowser() && state.cfg.mode !== "memory") return false;
        if (state.cfg.mode === "memory") {
            if (state.historyIndex > 0) {
                state.historyIndex--;
                state.current = { ...state.current, fullPath: state.history[state.historyIndex] };
                return handleLocationChange({ cause: "back" });
            }
            return false;
        }
        window.history.back();
        return true;
    }

    function forward() {
        if (!isBrowser() && state.cfg.mode !== "memory") return false;
        if (state.cfg.mode === "memory") {
            if (state.historyIndex < state.history.length - 1) {
                state.historyIndex++;
                state.current = { ...state.current, fullPath: state.history[state.historyIndex] };
                return handleLocationChange({ cause: "forward" });
            }
            return false;
        }
        window.history.forward();
        return true;
    }

    function configure(cfg = {}) {
        state.cfg = { ...state.cfg, ...(g.SJS?.config?.router || {}), ...cfg };
        // Retain started state; recompile patterns if routes already present
        if (state.routes.length) {
            for (const r of state.routes) {
                const { re, keys } = compilePath(normalizeTrailingSlash(r.path));
                r.re = re; r.keys = keys;
            }
        }
        return { ...state.cfg };
    }

    function addRoute(route) {
        if (!route || typeof route.path !== "string") throw new Error("router.addRoute: route must include a 'path' string");
        const r = {
            name: route.name || null,
            path: normalizeTrailingSlash(route.path),
            beforeEnter: route.beforeEnter,
            afterEnter: route.afterEnter,
            onLeave: route.onLeave,
            render: route.render,
            template: route.template,
            templateId: route.templateId,
            templateSelector: route.templateSelector,
            templateURL: route.templateURL,
            templateName: route.templateName,
            load: route.load,
            title: route.title,
            re: null,
            keys: []
        };
        const { re, keys } = compilePath(r.path);
        r.re = re; r.keys = keys;

        // If named route present with same name, replace; else push
        if (r.name) {
            const idx = state.routes.findIndex(x => x.name === r.name);
            if (idx >= 0) state.routes.splice(idx, 1, r);
            else state.routes.push(r);
        } else {
            state.routes.push(r);
        }
        return true;
    }

    function setRoutes(routes) {
        state.routes = [];
        for (const r of routes || []) addRoute(r);
        return true;
    }

    function build(name, params, query) {
        const r = state.routes.find(x => x.name === name);
        if (!r) throw new Error(`router.build: unknown route "${name}"`);
        const p = buildPathFromPattern(r.path, params || {});
        return p + buildQuery(query || {});
    }

    function current() {
        return state.current;
    }

    const api = Object.freeze({
        configure,
        start,
        stop,
        back,
        forward,
        navigate,
        addRoute,
        setRoutes,
        build,
        current,
        template,
        // Global hooks
        _beforeEach: [],
        _afterEach: [],
        beforeEach(fn) { if (typeof fn === "function") this._beforeEach.push(fn); return this; },
        afterEach(fn) { if (typeof fn === "function") this._afterEach.push(fn); return this; }
    });

    // Initialize with config if available (no start; boot() may autoStart)
    try { api.configure(); } catch { /* ignore */ }

    modules.add("router", api);
})(globalThis.SJS.modules || null);


// Usage (existing examples remain; independent from the bootstrap)

// Usage
const SJS = globalThis.SJS;
const runner = SJS.modules.get("runner");
const taskManager = SJS.modules.get("taskManager");

// Example: taskManager
const tasks = [
    new taskManager.Task("test1", (message) => { console.log(message); return true; }, ["hello from test1"]),
    new taskManager.Task("test2", (message) => { console.log(message); return true; }, ["hello from test2"]),
    new taskManager.Task("test3", (message) => { console.log(message); return true; }, ["hello from test3"]),
];

taskManager.asyncManager(tasks)
    .then((results) => {
        console.log("All done:", results);
        // Example results: [ { id: "test1", value: true }, { id: "test2", value: true }, { id: "test3", value: true } ]
    })
    .catch((err) => console.error(err));

// Example: runner with dependency resolution
const m = new runner.Manifest("example");

// Entries are functions here (can be sync or async). You can also use string src to load a script.
const eA = new runner.ManifestEntry("A", () => { console.log("A running"); return "A done"; }, []);
const eB = new runner.ManifestEntry("B", () => new Promise(res => {
    setTimeout(() => { console.log("B running after A"); res("B done"); }, 150);
}), ["A"]);
const eC = new runner.ManifestEntry("C", () => { console.log("C running after A"); return "C done"; }, ["A"]);
const eD = new runner.ManifestEntry("D", () => { console.log("D running after B and C"); return "D done"; }, ["B", "C"]);

m.addEntry(eA);
m.addEntry(eB);
m.addEntry(eC);
m.addEntry(eD);

runner.run(m, (summary) => {
    console.log("Runner callback summary:", summary);
}).then((summary) => {
    console.log("Runner finished:", summary);
}).catch((err) => {
    console.error("Runner error:", err);
});

// Example: pubSub
const pubSub = SJS.modules.get("pubSub");

// Subscribe normally
const token = pubSub.subscribe("greet", (payload, meta) => {
    console.log("greet received:", payload, "meta:", meta);
});

// Subscribe once
pubSub.once("greet", (payload) => {
    console.log("greet (once) received:", payload);
});

// Publish sync
pubSub.publish("greet", { msg: "hello there" });
// -> both handlers fire; the once handler removes itself

// Publish again
pubSub.publish("greet", { msg: "hello again" });
// -> only the normal handler fires

// Unsubscribe the remaining handler using its token
pubSub.unsubscribe(token);

// Nothing left
console.log("greet subscriber count:", pubSub.count("greet")); // 0
console.log("topics:", pubSub.list()); // [] or whatever topics remain elsewhere

// You can also publish async (awaits any returned Promises from handlers)
(async () => {
    pubSub.subscribe("slow", async (payload) => {
        await new Promise(r => setTimeout(r, 100));
        console.log("slow handler done", payload);
    });
    await pubSub.publishAsync("slow", { msg: "work" });
})();

// ===== NEW USAGE: logger module ===============================================

const logger = SJS.modules.get("logger");

// Create a logger for your app namespace
const appLog = logger.create("app", { level: "info", memory: true });
// Log with structured context and levels
appLog.info("Application starting...", { version: "1.0.0" });
appLog.time("boot");
// Messages below the current level are suppressed unless enabled
appLog.debug("This is a debug message (may be hidden)");

// Enable all app:* logs dynamically (supports wildcards, comma-separated)
logger.enable("app:*");
appLog.debug("Now debug is visible after enabling pattern");

// Child logger with extra context and optional name suffix
const sessionLog = appLog.child({ sessionId: Math.random().toString(36).slice(2, 8) }, "session");
sessionLog.warn("Session warmup");

// Publish logs to pubSub as well (topic: "logs")
logger.enablePubSub(appLog, "logs");
pubSub.subscribe("logs", (rec) => {
    // rec is the structured log record
    console.log("pubSub/logs:", rec.level, rec.name, "-", rec.msg);
});
appLog.error("An example error sent to pubSub sink", { code: 500 });

// Runner integration: log summary via logger
runner.run(m, (summary) => {
    appLog.info("Runner summary (via logger)", summary);
});

// Inspect buffered logs (memory sink)
console.log("Buffered app logs:", logger.getBuffered("app"));

// ===== NEW USAGE: debug module ================================================
const debug = SJS.modules.get("debug");
debug.debugOn();
debug.debugger("info", "Debug info example", { foo: 1 });
debug.debugger("warning", "Debug warning example", { bar: 2 });
debug.debugger("err", "Debug error example", { baz: 3 });
debug.debugOff();

// ===== NEW USAGE: storage module =============================================
const storage = SJS.modules.get("storage");
// Demonstrate default namespace (from config.storage.namespace or "default")
(async () => {
    const cur = await storage.get("counter");
    await storage.set("counter", (typeof cur === "number" ? cur : 0) + 1);
    console.log("storage driver:", storage.driverName(), "counter:", await storage.get("counter"));

    // Namespaced view
    const userStore = storage.use("user");
    await userStore.set("profile", { id: "u1", name: "Ada" }, { ttlMs: 3600000 });
    console.log("user profile:", await userStore.get("profile"));

    // Listen to storage change events via pubSub
    const token2 = pubSub.subscribe("storage:set", (p) => console.log("storage set:", p));
    await storage.set("hello", "world");
    pubSub.unsubscribe(token2);
})();

// ===== NEW USAGE: cache module ===============================================

const cache = SJS.modules.get("cache");

// Configure cache: persist to storage if available, 5s TTL and 10s SWR
cache.configure({ ttlMs: 5000, swrMs: 10000, maxEntries: 500 });

// Observe cache activity
pubSub.subscribe("cache:hit", (p) => console.log("cache hit:", p.group, p.key, p.stale ? "(stale)" : ""));
pubSub.subscribe("cache:miss", (p) => console.log("cache miss:", p.group, p.key));
pubSub.subscribe("cache:set", (p) => console.log("cache set:", p.group, p.key));

// Wrap an async function with memoize (e.g., fetch user by id)
const getUserById = cache.memoize("userById", async (id) => {
    // Simulate network/API
    await new Promise(r => setTimeout(r, 50));
    return { id, name: "User " + id, fetchedAt: new Date().toISOString() };
}, {
    ttlMs: 3000,  // fresh for 3s
    swrMs: 7000   // serve stale up to 7s more while refreshing
});

// Demonstrate cache behavior
(async () => {
    const u1 = await getUserById(123); // miss + set
    console.log("u1 first:", u1);
    const u2 = await getUserById(123); // hit (fresh)
    console.log("u1 second (cached):", u2);

    // Direct set/get API
    await cache.set("settings", "theme", { mode: "dark" }, { ttlMs: 10_000 });
    console.log("settings:theme:", await cache.get("settings", "theme"));

    console.log("cache stats:", cache.stats());
})();

// ===== NEW USAGE: http module ================================================

const http = SJS.modules.get("http");

// Configure http (example: set base URL and default headers, enable GET cache)
http.configure({
    baseURL: "https://jsonplaceholder.typicode.com",
    headers: { "Accept": "application/json" },
    cacheGet: true,
    cacheTtlMs: 8000,
    cacheSwrMs: 4000,
    retries: 2,
    timeoutMs: 8000
});

// Observe http events
pubSub.subscribe("http:request", (p) => console.log("HTTP -> request", p.request.method, p.request.url));
pubSub.subscribe("http:response", (p) => console.log("HTTP <- response", p.response.status ?? (p.response.cached ? "cached" : "ok")));
pubSub.subscribe("http:error", (p) => console.warn("HTTP !! error", p.status || "", p.error));

// Try a GET (will be cached for 8s, SWR 4s)
(async () => {
    try {
        const posts = await http.get("/posts", { params: { _limit: 2 } });
        console.log("http GET posts:", posts);
        const posts2 = await http.get("/posts", { params: { _limit: 2 } });
        console.log("http GET posts (cached):", posts2);
    } catch (e) {
        console.error("http error:", e);
    }
})();

// ===== NEW USAGE: input module ===============================================

const input = SJS.modules.get("input");

// Configure input (example: do not capture/stop defaults, track mouse + pointer)
input.configure({
    capture: false,
    preventDefault: false,
    stopPropagation: false,
    ignoreEditable: true,
    useCode: false,
    trackMouse: true,
    trackPointer: true,
    dedupePointerMouse: true
});

// Observe input events via pubSub
pubSub.subscribe("input:keydown", (p) => console.log("KEY DOWN:", p.key, "pressed:", p.pressed));
pubSub.subscribe("input:keyup", (p) => console.log("KEY UP:", p.key, "pressed:", p.pressed));
pubSub.subscribe("input:mousedown", (p) => console.log("MOUSE DOWN:", p.buttonName, `(${p.button}) at`, p.x, p.y));
pubSub.subscribe("input:mouseup", (p) => console.log("MOUSE UP:", p.buttonName, `(${p.button}) at`, p.x, p.y));
pubSub.subscribe("input:mousemove", (p) => { /* e.g., throttle in config to reduce spam */ });
// Query API example
setTimeout(() => {
    console.log("Currently pressed keys:", input.keysDown());
    console.log("Is 'Control' down?", input.isKeyDown("Control"));
    console.log("Mouse state:", input.getMouse());
}, 1000);

// ===== NEW USAGE: metrics module =============================================

const metrics = SJS.modules.get("metrics");
metrics.configure({
    enabled: true,
    persist: true,
    exportIntervalMs: 30000, // flush every 30s
    autoLongTask: true
});
metrics.start();

// Collect some metrics
metrics.counterInc("app_starts", 1, { env: "demo" });
metrics.gaugeSet("active_users", 1);
const t = metrics.timeStart("boot");
setTimeout(() => {
    const dur = metrics.timeEnd(t);
    console.log("boot time measured (ms):", dur);
    metrics.histObserve("custom_latency", Math.random() * 200, { route: "/home" });
    metrics.flush(); // manual flush
}, 120);

// ===== NEW USAGE: router module ==============================================

const router = SJS.modules.get("router");

// Register two simple inline templates and routes
router.template.register("homeTpl",
    "<div class='page home'>" +
    "  <h2>Home</h2>" +
    "  <p>Welcome, <%- (name || 'Guest') %>!</p>" +
    "  <p>Now: <%= new Date().toLocaleString() %></p>" +
    "</div>"
);
router.template.register("userTpl",
    "<div class='page user'>" +
    "  <h2>User</h2>" +
    "  <p>ID: <%- params.id %></p>" +
    "  <% if (user) { %><p>Name: <%- user.name %></p><% } %>" +
    "</div>"
);

// Configure and set routes
router.configure({ root: "#app", autoStart: true, mode: "history" });
router.setRoutes([
    { name: "home", path: "/", templateName: "homeTpl", title: "Home", load: (ctx) => ({ name: "Stranger" }) },
    {
        name: "user",
        path: "/user/:id",
        templateName: "userTpl",
        title: (ctx) => `User ${ctx.params.id}`,
        async load(ctx) {
            // Demo fake load
            await new Promise(r => setTimeout(r, 25));
            return { user: { id: ctx.params.id, name: "User " + ctx.params.id }, params: ctx.params };
        }
    },
    { path: "*", template: "<h2>Not Found</h2>" }
]);

// Observe router events
pubSub.subscribe("router:navigate", (p) => console.log("ROUTER NAV:", p.url));
pubSub.subscribe("router:render", () => console.log("ROUTER RENDER"));
pubSub.subscribe("router:error", (p) => console.warn("ROUTER ERROR:", p));

// Auto-start if configured in config.json, else start manually here for demo
router.start().catch((e) => console.warn("router start error:", e));

/* NEW MODULE: errorReporter
   Centralized error tracking and reporting system with context aggregation.
   
   Features:
   - Automatic error capture (unhandled rejections, window errors)
   - Context enrichment (user agent, URL, timestamp, stack traces)
   - Error aggregation (dedupe similar errors)
   - Sampling and rate limiting
   - Multiple sinks (console, storage, HTTP endpoint, pubSub)
   - Integration with stacktrace module for enhanced traces
   - Breadcrumb trail (user actions leading to error)
   
   API:
     errorReporter.configure({
       enabled?: boolean,
       autoCapture?: boolean,           // Capture window.onerror and unhandledrejection
       sampleRate?: number,              // 0-1, percentage of errors to report (default 1.0)
       maxBreadcrumbs?: number,          // Max breadcrumbs to keep (default 50)
       maxErrors?: number,               // Max errors to store (default 100)
       persistErrors?: boolean,          // Save errors to storage (default true)
       namespace?: string,               // Storage namespace (default "errors")
       httpEndpoint?: string|null,       // POST errors to this URL
       httpHeaders?: Record<string,string>,
       dedupe?: boolean,                 // Deduplicate similar errors (default true)
       dedupeWindowMs?: number,          // Time window for deduping (default 60000)
       beforeSend?: (error) => error|null, // Transform/filter before sending
       onError?: (error) => void         // Callback when error is captured
     })
     
     - captureError(error, context?)   // Manually capture an error
     - captureMessage(message, level?, context?)  // Capture non-error message
     - addBreadcrumb(category, message, data?)    // Add breadcrumb
     - clearBreadcrumbs()
     - getBreadcrumbs()
     - getErrors(limit?)
     - clearErrors()
     - setUser(user)                    // Set current user context
     - setContext(key, value)           // Add custom context
     - start(), stop()
     
   Events (via pubSub if present):
     - "error:captured" { error, errorId, timestamp }
     - "error:sent" { errorId, endpoint }
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! errorReporter module requires module system");
        return;
    }

    const isBrowser = () => typeof window !== "undefined" && typeof document !== "undefined";

    // Configuration
    let config = {
        enabled: true,
        autoCapture: true,
        sampleRate: 1.0,
        maxBreadcrumbs: 50,
        maxErrors: 100,
        persistErrors: true,
        namespace: "errors",
        httpEndpoint: null,
        httpHeaders: { "Content-Type": "application/json" },
        dedupe: true,
        dedupeWindowMs: 60000,
        beforeSend: null,
        onError: null
    };

    // State
    let breadcrumbs = [];
    let errors = [];
    let userContext = null;
    let customContext = {};
    let errorFingerprints = new Map(); // For deduplication
    let isStarted = false;
    let originalErrorHandler = null;
    let originalRejectionHandler = null;

    // Error fingerprinting for deduplication
    function getFingerprint(error) {
        const msg = error.message || String(error);
        const stack = error.stack || "";
        // Use first 2 stack frames + message as fingerprint
        const stackLines = stack.split("\n").slice(0, 3).join("|");
        return `${msg}::${stackLines}`.replace(/:\d+:\d+/g, ""); // Remove line:col numbers
    }

    function shouldCapture(error) {
        if (!config.enabled) return false;

        // Sample rate check
        if (Math.random() > config.sampleRate) return false;

        // Deduplication check
        if (config.dedupe) {
            const fp = getFingerprint(error);
            const lastSeen = errorFingerprints.get(fp);
            const now = Date.now();

            if (lastSeen && (now - lastSeen) < config.dedupeWindowMs) {
                return false; // Skip duplicate within time window
            }

            errorFingerprints.set(fp, now);
        }

        return true;
    }

    function enrichError(error, context = {}) {
        const enriched = {
            errorId: `err_${Date.now()}_${Math.random().toString(36).slice(2, 9)}`,
            timestamp: new Date().toISOString(),
            message: error.message || String(error),
            name: error.name || "Error",
            stack: error.stack || null,

            // Context
            url: isBrowser() ? window.location.href : null,
            userAgent: isBrowser() ? navigator.userAgent : null,
            user: userContext,
            custom: { ...customContext, ...context },

            // Breadcrumbs (last N actions)
            breadcrumbs: breadcrumbs.slice(-10),

            // Environment
            platform: isBrowser() ? "browser" : "node",
            viewport: isBrowser() ? {
                width: window.innerWidth,
                height: window.innerHeight
            } : null
        };

        // Enhanced stack trace if stacktrace module available
        const stacktrace = modules.get("stacktrace");
        if (stacktrace && typeof stacktrace.aria_capture_stacktrace === "function") {
            try {
                const trace = stacktrace.aria_capture_stacktrace();
                enriched.enhancedStack = trace;
            } catch (e) {
                // Ignore if stack capture fails
            }
        }

        return enriched;
    }

    async function sendError(enrichedError) {
        const pubSub = modules.get("pubSub");

        // beforeSend hook
        if (typeof config.beforeSend === "function") {
            enrichedError = config.beforeSend(enrichedError);
            if (!enrichedError) return; // Filtered out
        }

        // Store in memory
        errors.push(enrichedError);
        if (errors.length > config.maxErrors) {
            errors.shift(); // Remove oldest
        }

        // Persist to storage
        if (config.persistErrors) {
            const storage = modules.get("storage");
            if (storage) {
                try {
                    const errorStore = storage.use(config.namespace);
                    await errorStore.set(enrichedError.errorId, enrichedError);
                } catch (e) {
                    console.warn("Failed to persist error:", e);
                }
            }
        }

        // Send to HTTP endpoint
        if (config.httpEndpoint) {
            try {
                const response = await fetch(config.httpEndpoint, {
                    method: "POST",
                    headers: config.httpHeaders,
                    body: JSON.stringify(enrichedError)
                });

                if (pubSub) {
                    pubSub.publish("error:sent", {
                        errorId: enrichedError.errorId,
                        endpoint: config.httpEndpoint,
                        status: response.status
                    });
                }
            } catch (e) {
                console.warn("Failed to send error to endpoint:", e);
            }
        }

        // Publish to pubSub
        if (pubSub) {
            pubSub.publish("error:captured", {
                error: enrichedError,
                errorId: enrichedError.errorId,
                timestamp: enrichedError.timestamp
            });
        }

        // Custom callback
        if (typeof config.onError === "function") {
            try {
                config.onError(enrichedError);
            } catch (e) {
                console.error("Error in onError callback:", e);
            }
        }
    }

    function captureError(error, context = {}) {
        if (!shouldCapture(error)) return null;

        const enriched = enrichError(error, context);
        sendError(enriched);

        return enriched.errorId;
    }

    function captureMessage(message, level = "info", context = {}) {
        const syntheticError = new Error(message);
        syntheticError.name = level.toUpperCase();
        syntheticError.level = level;

        return captureError(syntheticError, context);
    }

    function addBreadcrumb(category, message, data = {}) {
        breadcrumbs.push({
            category,
            message,
            data,
            timestamp: Date.now()
        });

        if (breadcrumbs.length > config.maxBreadcrumbs) {
            breadcrumbs.shift();
        }
    }

    function clearBreadcrumbs() {
        breadcrumbs = [];
    }

    function getBreadcrumbs() {
        return [...breadcrumbs];
    }

    function getErrors(limit = config.maxErrors) {
        return errors.slice(-limit);
    }

    function clearErrors() {
        errors = [];
        errorFingerprints.clear();
    }

    function setUser(user) {
        userContext = user;
    }

    function setContext(key, value) {
        customContext[key] = value;
    }

    function handleWindowError(event) {
        const error = event.error || new Error(event.message || "Unknown error");
        error.filename = event.filename;
        error.lineno = event.lineno;
        error.colno = event.colno;

        captureError(error, { source: "window.onerror" });
    }

    function handleUnhandledRejection(event) {
        const error = event.reason instanceof Error
            ? event.reason
            : new Error(String(event.reason));

        captureError(error, { source: "unhandledrejection" });
    }

    function start() {
        if (isStarted) return;

        if (!isBrowser()) {
            console.warn("errorReporter: autoCapture only works in browser");
            isStarted = true;
            return;
        }

        if (config.autoCapture) {
            // Capture window errors
            originalErrorHandler = window.onerror;
            window.addEventListener("error", handleWindowError);

            // Capture unhandled promise rejections
            originalRejectionHandler = window.onunhandledrejection;
            window.addEventListener("unhandledrejection", handleUnhandledRejection);
        }

        isStarted = true;
    }

    function stop() {
        if (!isStarted) return;

        if (isBrowser() && config.autoCapture) {
            window.removeEventListener("error", handleWindowError);
            window.removeEventListener("unhandledrejection", handleUnhandledRejection);

            if (originalErrorHandler) window.onerror = originalErrorHandler;
            if (originalRejectionHandler) window.onunhandledrejection = originalRejectionHandler;
        }

        isStarted = false;
    }

    function configure(opts) {
        config = { ...config, ...opts };

        if (config.enabled && config.autoCapture && !isStarted) {
            start();
        }

        return config;
    }

    const IFACE = Object.freeze({
        configure,
        captureError,
        captureMessage,
        addBreadcrumb,
        clearBreadcrumbs,
        getBreadcrumbs,
        getErrors,
        clearErrors,
        setUser,
        setContext,
        start,
        stop
    });

    modules.add("errorReporter", IFACE);
})(globalThis.SJS?.modules || null);

/* NEW MODULE: state (Reactive State Management)
   A lightweight reactive state manager that integrates with pubSub for change notifications.
   Perfect for building reactive UIs without a heavy framework.
   
   Features:
   - Reactive state with automatic change detection
   - Computed properties (derived state)
   - Watchers (side effects on state changes)
   - Action dispatching with middleware support
   - Time-travel debugging (undo/redo)
   - State persistence via storage module
   - Integration with pubSub for cross-component reactivity
   - Immutable updates (no direct mutation)
   
   API:
     state.configure({
       persist?: boolean,               // Auto-persist to storage (default false)
       namespace?: string,              // Storage namespace (default "state")
       historySize?: number,            // Max history entries for time-travel (default 50)
       strict?: boolean,                // Throw on mutations outside actions (default false)
       emitPubSub?: boolean            // Publish state changes via pubSub (default true)
     })
     
     - createStore(name, initialState, options?)
     - store.get(path?)                  // Get state or nested property
     - store.set(path, value)            // Set nested property
     - store.update(path, updater)       // Update with function
     - store.commit(mutation)            // Commit state mutation
     - store.dispatch(action, payload)   // Dispatch action
     - store.subscribe(path, handler)    // Watch specific path
     - store.computed(name, fn)          // Create computed property
     - store.watch(fn)                   // Watch for any changes
     - store.undo(), store.redo()        // Time-travel
     - store.reset()                     // Reset to initial state
     - store.destroy()                   // Cleanup
     
   Events (via pubSub):
     - "state:changed" { store, path, oldValue, newValue }
     - "state:action" { store, action, payload }
     - "state:mutation" { store, mutation }
*/
(function (modules) {
    if (!modules) {
        console.error("ERROR! state module requires module system");
        return;
    }

    let globalConfig = {
        persist: false,
        namespace: "state",
        historySize: 50,
        strict: false,
        emitPubSub: true
    };

    const stores = new Map();

    function deepClone(obj) {
        if (obj === null || typeof obj !== "object") return obj;
        if (obj instanceof Date) return new Date(obj);
        if (obj instanceof Array) return obj.map(deepClone);
        if (obj instanceof Map) return new Map(Array.from(obj, ([k, v]) => [k, deepClone(v)]));
        if (obj instanceof Set) return new Set(Array.from(obj, deepClone));

        const cloned = {};
        for (const key in obj) {
            if (Object.prototype.hasOwnProperty.call(obj, key)) {
                cloned[key] = deepClone(obj[key]);
            }
        }
        return cloned;
    }

    function getNestedValue(obj, path) {
        if (!path) return obj;
        const keys = path.split(".");
        let value = obj;
        for (const key of keys) {
            if (value == null) return undefined;
            value = value[key];
        }
        return value;
    }

    function setNestedValue(obj, path, value) {
        const keys = path.split(".");
        const lastKey = keys.pop();
        let target = obj;

        for (const key of keys) {
            if (!(key in target) || typeof target[key] !== "object") {
                target[key] = {};
            }
            target = target[key];
        }

        target[lastKey] = value;
    }

    function createStore(name, initialState = {}, options = {}) {
        const config = { ...globalConfig, ...options };

        let state = deepClone(initialState);
        let history = [deepClone(state)];
        let historyIndex = 0;
        let computedCache = new Map();
        let computedDeps = new Map();
        let watchers = [];
        let subscribers = new Map();
        let isCommitting = false;

        const pubSub = modules.get("pubSub");
        const storage = modules.get("storage");

        // Load persisted state
        if (config.persist && storage) {
            (async () => {
                const stateStore = storage.use(config.namespace);
                const persisted = await stateStore.get(name);
                if (persisted) {
                    state = persisted;
                    history = [deepClone(state)];
                }
            })();
        }

        function notifyChange(path, oldValue, newValue) {
            // Path-specific subscribers
            if (subscribers.has(path)) {
                for (const handler of subscribers.get(path)) {
                    try {
                        handler(newValue, oldValue, path);
                    } catch (e) {
                        console.error("Error in state subscriber:", e);
                    }
                }
            }

            // Global watchers
            for (const watcher of watchers) {
                try {
                    watcher(state, path, oldValue, newValue);
                } catch (e) {
                    console.error("Error in state watcher:", e);
                }
            }

            // Invalidate computed properties that depend on this path
            for (const [compName, deps] of computedDeps.entries()) {
                if (deps.includes(path) || path.startsWith(deps[0]?.split(".")[0])) {
                    computedCache.delete(compName);
                }
            }

            // PubSub notification
            if (config.emitPubSub && pubSub) {
                pubSub.publish("state:changed", {
                    store: name,
                    path,
                    oldValue,
                    newValue
                });
            }
        }

        function saveToHistory() {
            // Remove any future history if we're in the middle
            if (historyIndex < history.length - 1) {
                history = history.slice(0, historyIndex + 1);
            }

            history.push(deepClone(state));

            if (history.length > config.historySize) {
                history.shift();
            } else {
                historyIndex++;
            }
        }

        async function persist() {
            if (config.persist && storage) {
                try {
                    const stateStore = storage.use(config.namespace);
                    await stateStore.set(name, state);
                } catch (e) {
                    console.error("Failed to persist state:", e);
                }
            }
        }

        function get(path) {
            return path ? getNestedValue(state, path) : deepClone(state);
        }

        function set(path, value) {
            if (config.strict && !isCommitting) {
                throw new Error("State mutations must happen inside actions in strict mode");
            }

            const oldValue = getNestedValue(state, path);
            if (oldValue === value) return; // No change

            const newState = deepClone(state);
            setNestedValue(newState, path, value);

            state = newState;
            notifyChange(path, oldValue, value);
            saveToHistory();
            persist();
        }

        function update(path, updater) {
            const current = getNestedValue(state, path);
            const newValue = updater(current);
            set(path, newValue);
        }

        function commit(mutation) {
            isCommitting = true;
            try {
                const oldState = deepClone(state);
                mutation(state);

                if (config.emitPubSub && pubSub) {
                    pubSub.publish("state:mutation", { store: name, mutation: mutation.name });
                }

                saveToHistory();
                persist();
            } finally {
                isCommitting = false;
            }
        }

        function dispatch(action, payload) {
            if (config.emitPubSub && pubSub) {
                pubSub.publish("state:action", { store: name, action, payload });
            }

            // Actions are just named mutations
            const actionFn = store.actions?.[action];
            if (typeof actionFn === "function") {
                return actionFn(store, payload);
            } else {
                console.warn(`Action "${action}" not found in store "${name}"`);
            }
        }

        function subscribe(path, handler) {
            if (!subscribers.has(path)) {
                subscribers.set(path, []);
            }
            subscribers.get(path).push(handler);

            // Return unsubscribe function
            return () => {
                const handlers = subscribers.get(path);
                const index = handlers.indexOf(handler);
                if (index > -1) handlers.splice(index, 1);
            };
        }

        function computed(name, fn, deps = []) {
            computedDeps.set(name, deps);

            Object.defineProperty(store, name, {
                get() {
                    if (!computedCache.has(name)) {
                        computedCache.set(name, fn(state));
                    }
                    return computedCache.get(name);
                },
                enumerable: true
            });
        }

        function watch(fn) {
            watchers.push(fn);

            // Return unwatch function
            return () => {
                const index = watchers.indexOf(fn);
                if (index > -1) watchers.splice(index, 1);
            };
        }

        function undo() {
            if (historyIndex > 0) {
                historyIndex--;
                state = deepClone(history[historyIndex]);
                notifyChange("", state, state);
                persist();
            }
        }

        function redo() {
            if (historyIndex < history.length - 1) {
                historyIndex++;
                state = deepClone(history[historyIndex]);
                notifyChange("", state, state);
                persist();
            }
        }

        function reset() {
            state = deepClone(initialState);
            history = [deepClone(state)];
            historyIndex = 0;
            computedCache.clear();
            notifyChange("", null, state);
            persist();
        }

        function destroy() {
            subscribers.clear();
            watchers = [];
            computedCache.clear();
            computedDeps.clear();
            stores.delete(name);
        }

        const store = {
            name,
            get,
            set,
            update,
            commit,
            dispatch,
            subscribe,
            computed,
            watch,
            undo,
            redo,
            reset,
            destroy,
            actions: {}, // User-defined actions go here
            getState: () => deepClone(state),
            canUndo: () => historyIndex > 0,
            canRedo: () => historyIndex < history.length - 1
        };

        stores.set(name, store);
        return store;
    }

    function getStore(name) {
        return stores.get(name) || null;
    }

    function configure(opts) {
        globalConfig = { ...globalConfig, ...opts };
        return globalConfig;
    }

    const IFACE = Object.freeze({
        configure,
        createStore,
        getStore
    });

    modules.add("state", IFACE);
})(globalThis.SJS?.modules || null);


// ===== USAGE EXAMPLES =====

// ===== NEW USAGE: errorReporter module =======================================

const errorReporter = SJS.modules.get("errorReporter");
const pubSub = SJS.modules.get("pubSub");

// Configure error reporting
errorReporter.configure({
    enabled: true,
    autoCapture: true,
    sampleRate: 1.0,
    persistErrors: true,
    dedupe: true,
    httpEndpoint: null  // Set to your error reporting service URL
});

// Set user context
errorReporter.setUser({ id: 123, email: "user@example.com" });

// Set custom context
errorReporter.setContext("appVersion", "1.0.0");
errorReporter.setContext("environment", "production");

// Subscribe to error events
pubSub.subscribe("error:captured", (data) => {
    console.log("Error captured:", data.errorId, data.error.message);
});

// Add breadcrumbs (user actions leading to error)
errorReporter.addBreadcrumb("ui", "Button clicked", { buttonId: "submit" });
errorReporter.addBreadcrumb("api", "API call started", { endpoint: "/users" });
errorReporter.addBreadcrumb("ui", "Form submitted", { formId: "login" });

// Manually capture errors
try {
    throw new Error("Something went wrong!");
} catch (error) {
    errorReporter.captureError(error, { customData: "extra context" });
}

// Capture messages at different levels
errorReporter.captureMessage("User logged in", "info", { userId: 123 });
errorReporter.captureMessage("Unusual activity detected", "warning");
errorReporter.captureMessage("Critical failure", "error");

// Automatic error capture (already started with autoCapture: true)
// Will automatically catch:
// - Unhandled exceptions
// - Unhandled promise rejections
// - window.onerror events

// View captured errors
console.log("Recent errors:", errorReporter.getErrors(10));

// View breadcrumbs
console.log("Breadcrumb trail:", errorReporter.getBreadcrumbs());

// Integration with beforeSend hook for filtering/transforming
errorReporter.configure({
    beforeSend: (error) => {
        // Filter out specific errors
        if (error.message.includes("Script error")) {
            return null;  // Don't send
        }
        
        // Add extra context
        error.custom.buildNumber = "12345";
        
        return error;  // Send modified error
    }
});

// ===== NEW USAGE: state module ===============================================

const state = SJS.modules.get("state");

// Create a store for user state
const userStore = state.createStore("user", {
    profile: { name: "", email: "" },
    isLoggedIn: false,
    preferences: {
        theme: "light",
        notifications: true
    }
}, {
    persist: true,
    emitPubSub: true
});

// Define actions
userStore.actions.login = async (store, { email, password }) => {
    errorReporter.addBreadcrumb("auth", "Login attempt", { email });
    
    try {
        // Simulate API call
        await new Promise(r => setTimeout(r, 100));
        
        store.commit((state) => {
            state.profile = { name: "Ada Lovelace", email };
            state.isLoggedIn = true;
        });
        
        errorReporter.setUser({ email });
        return { success: true };
    } catch (error) {
        errorReporter.captureError(error, { action: "login" });
        return { success: false, error };
    }
};

userStore.actions.logout = (store) => {
    errorReporter.addBreadcrumb("auth", "Logout");
    
    store.commit((state) => {
        state.profile = { name: "", email: "" };
        state.isLoggedIn = false;
    });
    
    errorReporter.setUser(null);
};

userStore.actions.updateTheme = (store, theme) => {
    store.set("preferences.theme", theme);
    errorReporter.addBreadcrumb("ui", "Theme changed", { theme });
};

// Create computed properties
userStore.computed("fullProfile", (state) => {
    return {
        ...state.profile,
        theme: state.preferences.theme,
        loggedIn: state.isLoggedIn
    };
}, ["profile", "preferences", "isLoggedIn"]);

// Subscribe to specific state changes
const unsubscribe = userStore.subscribe("preferences.theme", (newTheme, oldTheme) => {
    console.log(`Theme changed from ${oldTheme} to ${newTheme}`);
    document.body.className = newTheme;
});

// Watch all state changes
const unwatch = userStore.watch((state, path, oldValue, newValue) => {
    console.log(`State changed at ${path}:`, oldValue, "→", newValue);
});

// Listen to state changes via pubSub
pubSub.subscribe("state:changed", (data) => {
    console.log("State changed:", data.store, data.path);
});

pubSub.subscribe("state:action", (data) => {
    console.log("Action dispatched:", data.action, data.payload);
});

// Use the store
(async () => {
    // Dispatch actions
    await userStore.dispatch("login", { 
        email: "ada@example.com", 
        password: "secret" 
    });
    
    // Get state
    console.log("User profile:", userStore.get("profile"));
    console.log("Is logged in:", userStore.get("isLoggedIn"));
    console.log("Full state:", userStore.get());
    
    // Access computed property
    console.log("Full profile:", userStore.fullProfile);
    
    // Update state
    userStore.dispatch("updateTheme", "dark");
    
    // Direct state updates (outside actions)
    userStore.set("preferences.notifications", false);
    
    // Update with function
    userStore.update("profile.name", (name) => name.toUpperCase());
    
    // Time-travel debugging
    userStore.undo();  // Revert last change
    console.log("After undo:", userStore.get());
    
    userStore.redo();  // Re-apply change
    console.log("After redo:", userStore.get());
    
    console.log("Can undo?", userStore.canUndo());
    console.log("Can redo?", userStore.canRedo());
})();

// Create an app state store
const appStore = state.createStore("app", {
    loading: false,
    errors: [],
    notifications: []
}, { persist: false });

appStore.actions.showNotification = (store, message) => {
    store.commit((state) => {
        state.notifications.push({
            id: Date.now(),
            message,
            timestamp: new Date()
        });
    });
    
    // Auto-dismiss after 3 seconds
    setTimeout(() => {
        store.commit((state) => {
            state.notifications.shift();
        });
    }, 3000);
};

appStore.actions.setLoading = (store, isLoading) => {
    store.set("loading", isLoading);
};

// Integration: Link error reporter to app state
pubSub.subscribe("error:captured", (data) => {
    appStore.commit((state) => {
        state.errors.push({
            id: data.errorId,
            message: data.error.message,
            timestamp: data.timestamp
        });
    });
    
    appStore.dispatch("showNotification", `Error: ${data.error.message}`);
});

// ===== INTEGRATION EXAMPLE: All modules working together =====================

async function buildFullApp() {
    const storage = SJS.modules.get("storage");
    const http = SJS.modules.get("http");
    const cache = SJS.modules.get("cache");
    const logger = SJS.modules.get("logger");
    const metrics = SJS.modules.get("metrics");
    
    // Configure all modules
    errorReporter.configure({ enabled: true, autoCapture: true });
    state.configure({ persist: true, emitPubSub: true });
    metrics.configure({ enabled: true, persist: true });
    
    const log = logger.create("app", { level: "info" });
    
    // Create application state
    const appState = state.createStore("fullApp", {
        user: null,
        data: [],
        ui: {
            currentPage: "home",
            sidebarOpen: true
        }
    }, { persist: true });
    
    // Define actions with full error handling and metrics
    appState.actions.fetchData = async (store) => {
        const timer = metrics.timeStart("fetch_data");
        errorReporter.addBreadcrumb("api", "Fetching data");
        
        try {
            store.set("ui.loading", true);
            log.info("Fetching data...");
            
            // Use HTTP with caching
            http.configure({ cacheGet: true });
            const data = await http.get("https://jsonplaceholder.typicode.com/users");
            
            store.commit((state) => {
                state.data = data;
                state.ui.loading = false;
            });
            
            metrics.counterInc("api_success");
            log.info("Data fetched successfully", { count: data.length });
            
        } catch (error) {
            errorReporter.captureError(error, { action: "fetchData" });
            metrics.counterInc("api_error");
            log.error("Failed to fetch data", { error: error.message });
            
            store.set("ui.loading", false);
        } finally {
            metrics.timeEnd(timer);
        }
    };
    
    // Subscribe to state changes
    appState.subscribe("user", (newUser) => {
        if (newUser) {
            errorReporter.setUser({ id: newUser.id, email: newUser.email });
            log.info("User logged in", { userId: newUser.id });
            metrics.counterInc("user_login");
        } else {
            errorReporter.setUser(null);
            log.info("User logged out");
            metrics.counterInc("user_logout");
        }
    });
    
    // Watch for errors in state
    appState.watch((state, path) => {
        if (path.startsWith("errors")) {
            // React to errors being added to state
            console.warn("Error added to app state");
        }
    });
    
    return appState;
}

// Run the full integrated app
buildFullApp().then((appState) => {
    console.log("✅ Full integrated app ready!");
    console.log("State management ✓");
    console.log("Error reporting ✓");
    console.log("Metrics collection ✓");
    console.log("Logging ✓");
    console.log("HTTP with caching ✓");
    console.log("Storage persistence ✓");
    console.log("PubSub messaging ✓");
    
    // Dispatch an action to see everything working
    appState.dispatch("fetchData");
});


// Usage (existing examples remain; independent from the bootstrap)