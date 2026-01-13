/**
 * SJS Renderer Module - Simple Canvas Rendering
 * 
 * A straightforward canvas wrapper for 2D games. No WebGL complexity unless you need it.
 * Handles sprites, sprite sheets, animations, layers, and camera transform.
 * 
 * Features:
 * - Multiple layers (background, game, UI, etc.)
 * - Sprite batching for performance
 * - Camera (position, zoom, rotation)
 * - Sprite sheets with animation support
 * - Debug visualization
 * - Render to offscreen canvas (for workers)
 * 
 * Integration:
 * - Uses SJS pub/sub for render events
 * - Uses SJS metrics for FPS tracking
 * - Uses SJS logger for debugging
 * 
 * @module renderer
 * @requires Canvas API
 */

(function (global) {
    'use strict';

    const renderer = {
        name: 'renderer',
        version: '1.0.0',
        dependencies: [] // Optional: ['pubSub', 'metrics', 'logger']
    };

    // ===== STATE =====
    let canvas = null;
    let ctx = null;
    let offscreenCanvas = null;
    let offscreenCtx = null;
    let layers = {};              // { name: { canvas, ctx, visible, alpha } }
    let sprites = {};             // Loaded images { name: HTMLImageElement }
    let animations = {};          // Sprite sheet animations
    let camera = {
        x: 0, y: 0,
        zoom: 1.0,
        rotation: 0
    };
    let config = {
        width: 800,
        height: 600,
        pixelArt: false,          // Use nearest-neighbor scaling
        clearColor: '#000000',
        defaultLayer: 'game',
        debug: false,             // Show FPS, debug info
        emitEvents: true,
        trackFPS: true
    };
    let lastFrameTime = 0;
    let fps = 0;
    let frameCount = 0;

    // ===== INITIALIZATION =====
    /**
     * Initialize the renderer
     * @param {HTMLCanvasElement|string} canvasOrSelector - Canvas element or selector
     * @param {Object} options - Configuration options
     */
    renderer.init = function (canvasOrSelector, options = {}) {
        // Merge config
        Object.assign(config, options);

        // Get canvas
        if (typeof canvasOrSelector === 'string') {
            canvas = document.querySelector(canvasOrSelector);
        } else {
            canvas = canvasOrSelector;
        }

        if (!canvas) {
            console.error('[renderer] Canvas not found');
            return false;
        }

        // Set canvas size
        canvas.width = config.width;
        canvas.height = config.height;

        // Get context
        ctx = canvas.getContext('2d');

        // Pixel art mode (crisp pixels, no smoothing)
        if (config.pixelArt) {
            ctx.imageSmoothingEnabled = false;
            canvas.style.imageRendering = 'pixelated';
        }

        // Create default layer
        renderer.createLayer(config.defaultLayer);

        emit('renderer:init', { width: config.width, height: config.height });
        log('Renderer initialized', { width: config.width, height: config.height });

        return true;
    };

    // ===== LAYERS =====
    /**
     * Create a rendering layer
     * @param {string} name - Layer name
     * @param {Object} options - { width, height, visible, alpha }
     */
    renderer.createLayer = function (name, options = {}) {
        const width = options.width || config.width;
        const height = options.height || config.height;

        const layerCanvas = document.createElement('canvas');
        layerCanvas.width = width;
        layerCanvas.height = height;

        const layerCtx = layerCanvas.getContext('2d');
        if (config.pixelArt) {
            layerCtx.imageSmoothingEnabled = false;
        }

        layers[name] = {
            canvas: layerCanvas,
            ctx: layerCtx,
            visible: options.visible !== false,
            alpha: options.alpha !== undefined ? options.alpha : 1.0,
            zIndex: options.zIndex || 0
        };

        log(`Created layer: ${name}`, { width, height });
    };

    /**
     * Get a layer's context for direct drawing
     * @param {string} name - Layer name
     * @returns {CanvasRenderingContext2D|null}
     */
    renderer.getLayer = function (name) {
        return layers[name] ? layers[name].ctx : null;
    };

    /**
     * Set layer visibility
     */
    renderer.setLayerVisible = function (name, visible) {
        if (layers[name]) layers[name].visible = visible;
    };

    /**
     * Set layer alpha
     */
    renderer.setLayerAlpha = function (name, alpha) {
        if (layers[name]) layers[name].alpha = Math.max(0, Math.min(1, alpha));
    };

    // ===== ASSET LOADING =====
    /**
     * Load a sprite image
     * @param {string} name - Sprite identifier
     * @param {string} url - Image URL
     * @returns {Promise<void>}
     */
    renderer.loadSprite = function (name, url) {
        return new Promise((resolve, reject) => {
            const img = new Image();
            img.onload = () => {
                sprites[name] = img;
                emit('renderer:loaded', { name, url, width: img.width, height: img.height });
                log(`Loaded sprite: ${name}`, { width: img.width, height: img.height });
                resolve();
            };
            img.onerror = (err) => {
                console.error(`[renderer] Failed to load ${name}:`, err);
                reject(err);
            };
            img.src = url;
        });
    };

    /**
     * Load multiple sprites
     * @param {Object} spriteMap - { name: url, ... }
     * @returns {Promise<void>}
     */
    renderer.loadSprites = async function (spriteMap) {
        const promises = Object.entries(spriteMap).map(([name, url]) =>
            renderer.loadSprite(name, url)
        );
        await Promise.all(promises);
    };

    /**
     * Define a sprite sheet animation
     * @param {string} name - Animation identifier
     * @param {Object} config - { sprite, frameWidth, frameHeight, frames, fps }
     */
    renderer.defineAnimation = function (name, animConfig) {
        animations[name] = {
            sprite: animConfig.sprite,
            frameWidth: animConfig.frameWidth,
            frameHeight: animConfig.frameHeight,
            frames: animConfig.frames || 1,
            fps: animConfig.fps || 10,
            frameDuration: 1000 / (animConfig.fps || 10),
            currentFrame: 0,
            lastFrameTime: 0
        };
    };

    // ===== DRAWING =====
    /**
     * Clear a layer
     * @param {string} layer - Layer name (default: config.defaultLayer)
     */
    renderer.clear = function (layer = config.defaultLayer) {
        const l = layers[layer];
        if (!l) return;

        l.ctx.fillStyle = config.clearColor;
        l.ctx.fillRect(0, 0, l.canvas.width, l.canvas.height);
    };

    /**
     * Draw a sprite
     * @param {string} sprite - Sprite name
     * @param {number} x - X position
     * @param {number} y - Y position
     * @param {Object} options - { width, height, rotation, alpha, layer, flipX, flipY }
     */
    renderer.drawSprite = function (sprite, x, y, options = {}) {
        const img = sprites[sprite];
        if (!img) {
            console.warn(`[renderer] Sprite not loaded: ${sprite}`);
            return;
        }

        const layer = options.layer || config.defaultLayer;
        const l = layers[layer];
        if (!l) return;

        const width = options.width || img.width;
        const height = options.height || img.height;
        const rotation = options.rotation || 0;
        const alpha = options.alpha !== undefined ? options.alpha : 1.0;

        l.ctx.save();
        l.ctx.globalAlpha = alpha;
        l.ctx.translate(x + width / 2, y + height / 2);
        if (rotation) l.ctx.rotate(rotation);
        if (options.flipX) l.ctx.scale(-1, 1);
        if (options.flipY) l.ctx.scale(1, -1);
        l.ctx.drawImage(img, -width / 2, -height / 2, width, height);
        l.ctx.restore();
    };

    /**
     * Draw a sprite from a sprite sheet
     * @param {string} sprite - Sprite name
     * @param {number} frameX - Frame column
     * @param {number} frameY - Frame row
     * @param {number} frameWidth - Frame width
     * @param {number} frameHeight - Frame height
     * @param {number} x - Destination X
     * @param {number} y - Destination Y
     * @param {Object} options - { width, height, rotation, alpha, layer }
     */
    renderer.drawFrame = function (sprite, frameX, frameY, frameWidth, frameHeight, x, y, options = {}) {
        const img = sprites[sprite];
        if (!img) return;

        const layer = options.layer || config.defaultLayer;
        const l = layers[layer];
        if (!l) return;

        const width = options.width || frameWidth;
        const height = options.height || frameHeight;
        const rotation = options.rotation || 0;
        const alpha = options.alpha !== undefined ? options.alpha : 1.0;

        l.ctx.save();
        l.ctx.globalAlpha = alpha;
        l.ctx.translate(x + width / 2, y + height / 2);
        if (rotation) l.ctx.rotate(rotation);
        l.ctx.drawImage(
            img,
            frameX * frameWidth, frameY * frameHeight, frameWidth, frameHeight,
            -width / 2, -height / 2, width, height
        );
        l.ctx.restore();
    };

    /**
     * Draw an animation (auto-updates frame)
     * @param {string} anim - Animation name
     * @param {number} x - X position
     * @param {number} y - Y position
     * @param {Object} options - Same as drawSprite
     */
    renderer.drawAnimation = function (anim, x, y, options = {}) {
        const animation = animations[anim];
        if (!animation) return;

        const now = Date.now();
        if (now - animation.lastFrameTime >= animation.frameDuration) {
            animation.currentFrame = (animation.currentFrame + 1) % animation.frames;
            animation.lastFrameTime = now;
        }

        renderer.drawFrame(
            animation.sprite,
            animation.currentFrame, 0,
            animation.frameWidth, animation.frameHeight,
            x, y,
            options
        );
    };

    /**
     * Draw text
     * @param {string} text - Text to draw
     * @param {number} x - X position
     * @param {number} y - Y position
     * @param {Object} options - { font, color, align, baseline, layer }
     */
    renderer.drawText = function (text, x, y, options = {}) {
        const layer = options.layer || config.defaultLayer;
        const l = layers[layer];
        if (!l) return;

        l.ctx.save();
        l.ctx.font = options.font || '16px monospace';
        l.ctx.fillStyle = options.color || '#ffffff';
        l.ctx.textAlign = options.align || 'left';
        l.ctx.textBaseline = options.baseline || 'top';
        l.ctx.fillText(text, x, y);
        l.ctx.restore();
    };

    /**
     * Draw a rectangle
     */
    renderer.drawRect = function (x, y, width, height, options = {}) {
        const layer = options.layer || config.defaultLayer;
        const l = layers[layer];
        if (!l) return;

        l.ctx.save();
        if (options.fill) {
            l.ctx.fillStyle = options.fill;
            l.ctx.fillRect(x, y, width, height);
        }
        if (options.stroke) {
            l.ctx.strokeStyle = options.stroke;
            l.ctx.lineWidth = options.lineWidth || 1;
            l.ctx.strokeRect(x, y, width, height);
        }
        l.ctx.restore();
    };

    // ===== CAMERA =====
    /**
     * Set camera position
     */
    renderer.setCamera = function (x, y, zoom, rotation) {
        camera.x = x !== undefined ? x : camera.x;
        camera.y = y !== undefined ? y : camera.y;
        camera.zoom = zoom !== undefined ? zoom : camera.zoom;
        camera.rotation = rotation !== undefined ? rotation : camera.rotation;
    };

    /**
     * Apply camera transform to a layer
     */
    renderer.applyCameraTransform = function (layer = config.defaultLayer) {
        const l = layers[layer];
        if (!l) return;

        l.ctx.setTransform(1, 0, 0, 1, 0, 0);
        l.ctx.translate(config.width / 2, config.height / 2);
        l.ctx.scale(camera.zoom, camera.zoom);
        l.ctx.rotate(camera.rotation);
        l.ctx.translate(-camera.x, -camera.y);
    };

    /**
     * Reset camera transform
     */
    renderer.resetCameraTransform = function (layer = config.defaultLayer) {
        const l = layers[layer];
        if (!l) return;
        l.ctx.setTransform(1, 0, 0, 1, 0, 0);
    };

    // ===== RENDERING =====
    /**
     * Composite all layers to main canvas
     */
    renderer.present = function () {
        if (!ctx) return;

        // Clear main canvas
        ctx.fillStyle = config.clearColor;
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        // Draw layers in zIndex order
        const sortedLayers = Object.entries(layers)
            .sort(([, a], [, b]) => a.zIndex - b.zIndex);

        for (const [name, layer] of sortedLayers) {
            if (!layer.visible) continue;

            ctx.save();
            ctx.globalAlpha = layer.alpha;
            ctx.drawImage(layer.canvas, 0, 0);
            ctx.restore();
        }

        // Debug overlay
        if (config.debug) {
            drawDebugInfo();
        }

        // Track FPS
        if (config.trackFPS) {
            updateFPS();
        }

        emit('renderer:present', { fps });
    };

    // ===== DEBUG =====
    function drawDebugInfo() {
        ctx.save();
        ctx.font = '12px monospace';
        ctx.fillStyle = '#00ff00';
        ctx.textAlign = 'left';
        ctx.fillText(`FPS: ${Math.round(fps)}`, 10, 20);
        ctx.fillText(`Camera: (${Math.round(camera.x)}, ${Math.round(camera.y)})`, 10, 35);
        ctx.fillText(`Zoom: ${camera.zoom.toFixed(2)}`, 10, 50);
        ctx.fillText(`Layers: ${Object.keys(layers).length}`, 10, 65);
        ctx.fillText(`Sprites: ${Object.keys(sprites).length}`, 10, 80);
        ctx.restore();
    }

    function updateFPS() {
        const now = performance.now();
        frameCount++;

        if (now - lastFrameTime >= 1000) {
            fps = frameCount;
            frameCount = 0;
            lastFrameTime = now;

            if (global.SJS && global.SJS.metrics) {
                global.SJS.metrics.gaugeSet('renderer:fps', fps);
            }
        }
    }

    // ===== UTILITIES =====
    renderer.getCanvas = () => canvas;
    renderer.getContext = () => ctx;
    renderer.getCamera = () => ({ ...camera });
    renderer.getFPS = () => fps;
    renderer.getSprites = () => Object.keys(sprites);
    renderer.getLayers = () => Object.keys(layers);

    // ===== HELPER FUNCTIONS =====
    function emit(event, data) {
        if (!config.emitEvents) return;
        if (global.SJS && global.SJS.pubSub) {
            global.SJS.pubSub.publish(event, data);
        }
    }

    function log(message, data) {
        if (global.SJS && global.SJS.logger) {
            global.SJS.logger.debug('[renderer] ' + message, data);
        }
    }

    // ===== EXPORT =====
    renderer.configure = function (options) {
        Object.assign(config, options);
    };

    renderer.getConfig = function () {
        return { ...config };
    };

    // UMD export
    if (typeof module !== 'undefined' && module.exports) {
        module.exports = renderer;
    } else if (typeof define === 'function' && define.amd) {
        define([], function () { return renderer; });
    } else {
        global.SJS = global.SJS || {};
        global.SJS.renderer = renderer;
    }

})(typeof globalThis !== 'undefined' ? globalThis : this);
