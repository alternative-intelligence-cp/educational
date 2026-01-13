/**
 * SJS Audio Module - Simple Game Audio Mixer
 * 
 * A no-nonsense audio mixer for games. Forget the Web Audio API node graph complexity.
 * This gives you what you actually need: load sounds, play them, control volume, done.
 * 
 * Features:
 * - Load multiple audio files (mp3, wav, ogg)
 * - Play with volume control
 * - Loop support
 * - Master volume + per-sound volume
 * - Soft limiting to prevent clipping (ported from ShaggySound Java mixer)
 * - Simple API: load(), play(), stop(), setVolume()
 * 
 * Integration:
 * - Uses SJS pub/sub for events
 * - Uses SJS storage for volume persistence (optional)
 * - Uses SJS logger for debugging
 * 
 * @module audio
 * @requires Web Audio API (all modern browsers)
 */

(function (global) {
    'use strict';

    const audio = {
        name: 'audio',
        version: '1.0.0',
        dependencies: [] // Optional: ['pubSub', 'storage', 'logger']
    };

    // ===== STATE =====
    let context = null;           // AudioContext
    let masterGain = null;        // Master volume node
    let compressor = null;        // Dynamics compressor (limiter)
    let sounds = {};              // Loaded audio buffers { name: buffer }
    let playing = {};             // Currently playing sources { id: {source, gain, ...} }
    let nextId = 0;               // Unique ID for each play() call
    let config = {
        enabled: true,
        masterVolume: 0.8,        // 0.0 to 1.0
        limitThreshold: 0.9,      // Soft limit kicks in at 90%
        limitRatio: 5,            // Compression ratio above threshold
        persistVolume: false,     // Save volume to storage
        storageKey: 'audio:volume',
        emitEvents: true          // Emit pub/sub events
    };

    // ===== SOFT LIMITER (from ShaggySound) =====
    /**
     * Soft limiter algorithm ported from Java ShaggySound mixer
     * Prevents harsh clipping by compressing signals above 90% of max
     * 
     * @param {number} input - Input sample value
     * @param {number} limit - Target limit (0.0 to 1.0, typically 0.8)
     * @param {number} max - Maximum value (typically 1.0)
     * @returns {number} Limited sample value
     */
    function softLimit(input, limit, max) {
        const negative = input < 0;
        let ratio = negative ? -input / max : input / max;
        let limited = ratio * limit;
        ratio = limited / limit;

        // Soft compression above 90%
        if (ratio > 0.9) {
            const compressionFactor = ratio / 20;
            const drop = (ratio - 0.9) * 10;
            const compressed = limit * (ratio - (compressionFactor * drop));
            return negative ? -compressed : compressed;
        } else {
            return negative ? -limited : limited;
        }
    }

    /**
     * Hard clip (safety net after soft limiting)
     */
    function clip(input, max) {
        return Math.max(-max, Math.min(max, input));
    }

    // ===== INITIALIZATION =====
    /**
     * Initialize the audio system
     * Call this on user interaction (browsers require it)
     */
    audio.init = function (options = {}) {
        if (!config.enabled) return false;

        // Merge config
        Object.assign(config, options);

        try {
            // Create AudioContext
            const AudioContext = window.AudioContext || window.webkitAudioContext;
            if (!AudioContext) {
                console.warn('[audio] Web Audio API not supported');
                config.enabled = false;
                return false;
            }

            context = new AudioContext();

            // Master gain node
            masterGain = context.createGain();
            masterGain.gain.value = config.masterVolume;

            // Compressor/Limiter (uses built-in DynamicsCompressor)
            // This is simpler than manual limiting in the mixing stage
            compressor = context.createDynamicsCompressor();
            compressor.threshold.value = -10;      // dB
            compressor.knee.value = 0;             // Hard knee
            compressor.ratio.value = 20;           // Heavy compression
            compressor.attack.value = 0.003;       // 3ms attack
            compressor.release.value = 0.25;       // 250ms release

            // Chain: masterGain -> compressor -> destination
            masterGain.connect(compressor);
            compressor.connect(context.destination);

            // Restore saved volume
            if (config.persistVolume && global.SJS && global.SJS.storage) {
                global.SJS.storage.get(config.storageKey).then(vol => {
                    if (vol !== null) audio.setMasterVolume(vol);
                });
            }

            emit('audio:init', { context, sampleRate: context.sampleRate });
            log('Audio initialized', { sampleRate: context.sampleRate });

            return true;
        } catch (err) {
            console.error('[audio] Initialization failed:', err);
            config.enabled = false;
            return false;
        }
    };

    // ===== LOADING =====
    /**
     * Load an audio file
     * @param {string} name - Identifier for this sound
     * @param {string} url - Path to audio file
     * @returns {Promise<void>}
     */
    audio.load = async function (name, url) {
        if (!config.enabled) return;
        if (!context) audio.init();

        try {
            const response = await fetch(url);
            const arrayBuffer = await response.arrayBuffer();
            const audioBuffer = await context.decodeAudioData(arrayBuffer);

            sounds[name] = audioBuffer;
            emit('audio:loaded', { name, url, duration: audioBuffer.duration });
            log(`Loaded sound: ${name}`, { duration: audioBuffer.duration });
        } catch (err) {
            console.error(`[audio] Failed to load ${name}:`, err);
            emit('audio:error', { name, url, error: err.message });
        }
    };

    /**
     * Load multiple sounds
     * @param {Object} soundMap - { name: url, ... }
     * @returns {Promise<void>}
     */
    audio.loadAll = async function (soundMap) {
        const promises = Object.entries(soundMap).map(([name, url]) =>
            audio.load(name, url)
        );
        await Promise.all(promises);
    };

    // ===== PLAYBACK =====
    /**
     * Play a loaded sound
     * @param {string} name - Sound name (from load())
     * @param {Object} options - { volume, loop, onEnd, startTime, offset }
     * @returns {number} Playback ID (use to stop/modify this instance)
     */
    audio.play = function (name, options = {}) {
        if (!config.enabled || !context) return -1;

        const buffer = sounds[name];
        if (!buffer) {
            console.warn(`[audio] Sound not loaded: ${name}`);
            return -1;
        }

        // Resume context if suspended (mobile browsers)
        if (context.state === 'suspended') {
            context.resume();
        }

        const id = nextId++;
        const source = context.createBufferSource();
        const gain = context.createGain();

        source.buffer = buffer;
        source.loop = options.loop || false;
        gain.gain.value = options.volume !== undefined ? options.volume : 1.0;

        // Chain: source -> gain -> masterGain
        source.connect(gain);
        gain.connect(masterGain);

        // Start playback
        const startTime = options.startTime || context.currentTime;
        const offset = options.offset || 0;
        source.start(startTime, offset);

        // Track playing sound
        playing[id] = {
            source,
            gain,
            name,
            startTime,
            loop: source.loop
        };

        // Cleanup on end
        source.onended = () => {
            delete playing[id];
            if (options.onEnd) options.onEnd();
            emit('audio:ended', { id, name });
        };

        emit('audio:play', { id, name, volume: gain.gain.value, loop: source.loop });
        log(`Playing: ${name}`, { id, loop: source.loop });

        return id;
    };

    /**
     * Stop a playing sound
     * @param {number} id - Playback ID from play()
     */
    audio.stop = function (id) {
        const sound = playing[id];
        if (!sound) return;

        try {
            sound.source.stop();
            delete playing[id];
            emit('audio:stop', { id, name: sound.name });
        } catch (err) {
            // Already stopped, ignore
        }
    };

    /**
     * Stop all sounds with a given name
     * @param {string} name - Sound name
     */
    audio.stopAll = function (name) {
        Object.entries(playing).forEach(([id, sound]) => {
            if (!name || sound.name === name) {
                audio.stop(parseInt(id));
            }
        });
    };

    // ===== VOLUME CONTROL =====
    /**
     * Set master volume
     * @param {number} volume - 0.0 to 1.0
     */
    audio.setMasterVolume = function (volume) {
        config.masterVolume = Math.max(0, Math.min(1, volume));
        if (masterGain) {
            masterGain.gain.value = config.masterVolume;
        }

        // Persist if enabled
        if (config.persistVolume && global.SJS && global.SJS.storage) {
            global.SJS.storage.set(config.storageKey, config.masterVolume);
        }

        emit('audio:volume', { master: config.masterVolume });
    };

    /**
     * Get master volume
     * @returns {number} 0.0 to 1.0
     */
    audio.getMasterVolume = function () {
        return config.masterVolume;
    };

    /**
     * Set volume for a playing sound
     * @param {number} id - Playback ID
     * @param {number} volume - 0.0 to 1.0
     */
    audio.setVolume = function (id, volume) {
        const sound = playing[id];
        if (!sound) return;
        sound.gain.gain.value = Math.max(0, Math.min(1, volume));
    };

    /**
     * Fade volume
     * @param {number} id - Playback ID
     * @param {number} targetVolume - Target volume (0.0 to 1.0)
     * @param {number} duration - Fade duration in seconds
     */
    audio.fade = function (id, targetVolume, duration) {
        const sound = playing[id];
        if (!sound || !context) return;

        const now = context.currentTime;
        sound.gain.gain.linearRampToValueAtTime(targetVolume, now + duration);
    };

    // ===== UTILITIES =====
    /**
     * Get list of loaded sounds
     * @returns {string[]} Array of sound names
     */
    audio.getSounds = function () {
        return Object.keys(sounds);
    };

    /**
     * Get currently playing sounds
     * @returns {Object[]} Array of { id, name, loop }
     */
    audio.getPlaying = function () {
        return Object.entries(playing).map(([id, sound]) => ({
            id: parseInt(id),
            name: sound.name,
            loop: sound.loop
        }));
    };

    /**
     * Unload a sound
     * @param {string} name - Sound name
     */
    audio.unload = function (name) {
        audio.stopAll(name);
        delete sounds[name];
        emit('audio:unload', { name });
    };

    /**
     * Unload all sounds
     */
    audio.unloadAll = function () {
        audio.stopAll();
        sounds = {};
        emit('audio:unloadAll');
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
            global.SJS.logger.debug('[audio] ' + message, data);
        }
    }

    // ===== EXPORT =====
    audio.configure = function (options) {
        Object.assign(config, options);
    };

    audio.getConfig = function () {
        return { ...config };
    };

    // Auto-init on user interaction if in browser
    if (typeof window !== 'undefined') {
        const autoInit = () => {
            if (!context) audio.init();
            document.removeEventListener('click', autoInit);
            document.removeEventListener('touchstart', autoInit);
            document.removeEventListener('keydown', autoInit);
        };
        document.addEventListener('click', autoInit, { once: true });
        document.addEventListener('touchstart', autoInit, { once: true });
        document.addEventListener('keydown', autoInit, { once: true });
    }

    // UMD export
    if (typeof module !== 'undefined' && module.exports) {
        module.exports = audio;
    } else if (typeof define === 'function' && define.amd) {
        define([], function () { return audio; });
    } else {
        global.SJS = global.SJS || {};
        global.SJS.audio = audio;
    }

})(typeof globalThis !== 'undefined' ? globalThis : this);
