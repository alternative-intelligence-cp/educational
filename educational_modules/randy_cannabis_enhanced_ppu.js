/*
 * Randy's Cannabis-Enhanced Picture Processing Unit (PPU)
 * Old School Graphics Programming for Educational Enlightenment
 * 
 * Randy's PPU Philosophy:
 * "How about instead of the idea to make a whole game console, we instead just
 * implement a PPU that can draw to the screen in JS. I'm thinking super simple 
 * like old school. Better have your shit written to memory before it does its
 * thing lol. The tile matrix can be super small and that can be the only BG layer.
 * One sprite is sufficient to show the difference between a tile and a sprite."
 * 
 * THE OLD SCHOOL SIMPLICITY VISION:
 * No modern GPU complexity, no shader nonsense, no 3D matrices.
 * Just pure, cannabis-enhanced retro graphics programming that teaches
 * the fundamentals: memory-mapped graphics, tiles vs sprites, and the
 * beautiful constraints of limited hardware.
 * 
 * Educational Philosophy:
 * "Better have your shit written to memory before it does its thing"
 * This PPU forces proper memory planning - no dynamic allocation during
 * rendering. Everything must be prepared beforehand, just like real
 * retro consoles. Constraints breed creativity!
 */

// Randy's Cannabis-Enhanced PPU Configuration
const CEPU_CONFIG = {
    // Display Configuration (keeping it old school small)
    SCREEN_WIDTH: 256,          // Classic 8-bit screen width
    SCREEN_HEIGHT: 240,         // Classic 8-bit screen height

    // Tile System Configuration
    TILE_SIZE: 8,               // 8x8 pixel tiles (NES-style)
    TILES_X: 32,                // 32 tiles horizontally (256/8)
    TILES_Y: 30,                // 30 tiles vertically (240/8)
    MAX_TILES: 256,             // Maximum unique tile patterns

    // Color Configuration
    PALETTE_SIZE: 4,            // 4 colors per palette (2-bit color)
    MAX_PALETTES: 8,            // 8 different palettes

    // Sprite Configuration
    MAX_SPRITES: 64,            // Maximum sprites on screen
    SPRITE_SIZE: 8,             // 8x8 pixel sprites

    // Memory Layout (in bytes)
    TILE_MEMORY_SIZE: 0x2000,   // 8KB for tile pattern data
    NAME_TABLE_SIZE: 0x400,     // 1KB for background tile map
    PALETTE_MEMORY_SIZE: 0x20,  // 32 bytes for palette data
    SPRITE_MEMORY_SIZE: 0x100   // 256 bytes for sprite data
};

/*
 * Randy's Cannabis-Enhanced Color Palette System
 * "4 colors is all you need for pure artistic expression"
 */
const CEPU_PALETTE = {
    // Randy's Plant Spirit Color Schemes
    CANNABIS_GREEN: ['#000000', '#1a5d1a', '#4d8c4d', '#80bf80'],
    STRAWBERRY_PIE: ['#000000', '#8b0000', '#cd5c5c', '#ffa0a0'],
    BLUE_DREAM: ['#000000', '#191970', '#4169e1', '#87ceeb'],
    GOLDEN_HASH: ['#000000', '#8b4513', '#daa520', '#ffd700'],
    PURPLE_HAZE: ['#000000', '#4b0082', '#8a2be2', '#da70d6'],
    NORTHERN_LIGHTS: ['#000000', '#006400', '#00ff00', '#98fb98'],
    OG_SUNSET: ['#000000', '#8b4513', '#ff8c00', '#ffd700'],
    MYSTIC_MINT: ['#000000', '#2f4f4f', '#00ced1', '#e0ffff']
};

/*
 * Randy's PPU Memory Structure
 * "Everything lives in memory - no dynamic allocation during rendering"
 */
class CannabisEnhancedPPU {
    constructor(canvasId) {
        // Canvas setup
        this.canvas = document.getElementById(canvasId);
        this.ctx = this.canvas.getContext('2d');
        this.canvas.width = CEPU_CONFIG.SCREEN_WIDTH;
        this.canvas.height = CEPU_CONFIG.SCREEN_HEIGHT;

        // PPU Memory Banks (just like real hardware)
        this.tileMemory = new Uint8Array(CEPU_CONFIG.TILE_MEMORY_SIZE);
        this.nameTable = new Uint8Array(CEPU_CONFIG.NAME_TABLE_SIZE);
        this.paletteMemory = new Uint8Array(CEPU_CONFIG.PALETTE_MEMORY_SIZE);
        this.spriteMemory = new Uint8Array(CEPU_CONFIG.SPRITE_MEMORY_SIZE);

        // Current palette selection
        this.activePalettes = [
            CEPU_PALETTE.CANNABIS_GREEN,
            CEPU_PALETTE.STRAWBERRY_PIE,
            CEPU_PALETTE.BLUE_DREAM,
            CEPU_PALETTE.GOLDEN_HASH
        ];

        // PPU State
        this.frameCount = 0;
        this.plantSpiritMode = 'Blue Dream';

        // Create image data for fast pixel manipulation
        this.imageData = this.ctx.createImageData(
            CEPU_CONFIG.SCREEN_WIDTH,
            CEPU_CONFIG.SCREEN_HEIGHT
        );

        console.log('🧠⚡ Randy\'s Cannabis-Enhanced PPU Initialized ⚡🧠');
        console.log('   Resolution: ' + CEPU_CONFIG.SCREEN_WIDTH + 'x' + CEPU_CONFIG.SCREEN_HEIGHT);
        console.log('   Tile Size: ' + CEPU_CONFIG.TILE_SIZE + 'x' + CEPU_CONFIG.TILE_SIZE);
        console.log('   Max Tiles: ' + CEPU_CONFIG.MAX_TILES);
        console.log('   Plant Spirit Mode: ' + this.plantSpiritMode);
    }

    /*
     * Randy's Tile Pattern Loading System
     * "Get your shit written to memory before rendering!"
     */
    loadTilePattern(tileId, patternData) {
        if (tileId >= CEPU_CONFIG.MAX_TILES) {
            console.error('🚫 Tile ID out of range: ' + tileId);
            return;
        }

        const tileOffset = tileId * 16; // 8x8 pixels, 2 bits per pixel = 16 bytes

        for (let i = 0; i < 16 && i < patternData.length; i++) {
            this.tileMemory[tileOffset + i] = patternData[i];
        }

        console.log('🎨 Loaded tile pattern ' + tileId + ' into PPU memory');
    }

    /*
     * Randy's Name Table Setup System
     * "Background tile matrix - keep it simple and educational"
     */
    setBackgroundTile(x, y, tileId, paletteId = 0) {
        if (x >= CEPU_CONFIG.TILES_X || y >= CEPU_CONFIG.TILES_Y) {
            console.error('🚫 Background position out of range: ' + x + ', ' + y);
            return;
        }

        const nameTableIndex = y * CEPU_CONFIG.TILES_X + x;
        this.nameTable[nameTableIndex] = tileId;

        // Store palette info in upper bits (simple approach)
        const paletteIndex = nameTableIndex + 0x200; // Palette data offset
        if (paletteIndex < CEPU_CONFIG.NAME_TABLE_SIZE) {
            this.nameTable[paletteIndex] = paletteId;
        }
    }

    /*
     * Randy's Sprite Object Attribute Memory (OAM)
     * "One sprite is sufficient to show the difference"
     */
    setSpriteData(spriteId, x, y, tileId, paletteId = 0, flipX = false, flipY = false) {
        if (spriteId >= CEPU_CONFIG.MAX_SPRITES) {
            console.error('🚫 Sprite ID out of range: ' + spriteId);
            return;
        }

        const spriteOffset = spriteId * 4; // 4 bytes per sprite
        this.spriteMemory[spriteOffset + 0] = y;           // Y position
        this.spriteMemory[spriteOffset + 1] = tileId;      // Tile ID
        this.spriteMemory[spriteOffset + 2] = paletteId | (flipX ? 0x40 : 0) | (flipY ? 0x80 : 0);
        this.spriteMemory[spriteOffset + 3] = x;           // X position

        console.log('👾 Set sprite ' + spriteId + ' at (' + x + ', ' + y + ') with tile ' + tileId);
    }

    /*
     * Randy's Color Conversion Utility
     * "Convert hex colors to RGB values for pixel manipulation"
     */
    hexToRgb(hex) {
        const r = parseInt(hex.slice(1, 3), 16);
        const g = parseInt(hex.slice(3, 5), 16);
        const b = parseInt(hex.slice(5, 7), 16);
        return [r, g, b];
    }

    /*
     * Randy's Tile Rendering Engine
     * "Render 8x8 tile from pattern memory with specified palette"
     */
    renderTile(tileId, paletteId, startX, startY) {
        const tileOffset = tileId * 16;
        const palette = this.activePalettes[paletteId] || this.activePalettes[0];

        for (let row = 0; row < 8; row++) {
            const lowBits = this.tileMemory[tileOffset + row];
            const highBits = this.tileMemory[tileOffset + row + 8];

            for (let col = 0; col < 8; col++) {
                const bit = 7 - col;
                const colorIndex = ((highBits >> bit) & 1) << 1 | ((lowBits >> bit) & 1);

                const pixelX = startX + col;
                const pixelY = startY + row;

                if (pixelX < CEPU_CONFIG.SCREEN_WIDTH && pixelY < CEPU_CONFIG.SCREEN_HEIGHT) {
                    const [r, g, b] = this.hexToRgb(palette[colorIndex]);
                    const pixelIndex = (pixelY * CEPU_CONFIG.SCREEN_WIDTH + pixelX) * 4;

                    this.imageData.data[pixelIndex + 0] = r;     // Red
                    this.imageData.data[pixelIndex + 1] = g;     // Green
                    this.imageData.data[pixelIndex + 2] = b;     // Blue
                    this.imageData.data[pixelIndex + 3] = 255;   // Alpha
                }
            }
        }
    }

    /*
     * Randy's Background Rendering System
     * "Tile matrix background layer - old school simplicity"
     */
    renderBackground() {
        for (let tileY = 0; tileY < CEPU_CONFIG.TILES_Y; tileY++) {
            for (let tileX = 0; tileX < CEPU_CONFIG.TILES_X; tileX++) {
                const nameTableIndex = tileY * CEPU_CONFIG.TILES_X + tileX;
                const tileId = this.nameTable[nameTableIndex];
                const paletteIndex = nameTableIndex + 0x200;
                const paletteId = (paletteIndex < CEPU_CONFIG.NAME_TABLE_SIZE) ?
                    this.nameTable[paletteIndex] : 0;

                if (tileId > 0) { // 0 = transparent/empty
                    this.renderTile(
                        tileId,
                        paletteId,
                        tileX * CEPU_CONFIG.TILE_SIZE,
                        tileY * CEPU_CONFIG.TILE_SIZE
                    );
                }
            }
        }
    }

    /*
     * Randy's Sprite Rendering System
     * "Sprites overlay on background - priority system"
     */
    renderSprites() {
        for (let spriteId = 0; spriteId < CEPU_CONFIG.MAX_SPRITES; spriteId++) {
            const spriteOffset = spriteId * 4;
            const y = this.spriteMemory[spriteOffset + 0];
            const tileId = this.spriteMemory[spriteOffset + 1];
            const attributes = this.spriteMemory[spriteOffset + 2];
            const x = this.spriteMemory[spriteOffset + 3];

            // Skip inactive sprites (y = 0 means inactive)
            if (y === 0) continue;

            const paletteId = attributes & 0x03;
            const flipX = (attributes & 0x40) !== 0;
            const flipY = (attributes & 0x80) !== 0;

            // Render sprite (flipping not implemented for simplicity)
            this.renderTile(tileId, paletteId, x, y);
        }
    }

    /*
     * Randy's Main PPU Rendering Loop
     * "The heart of retro graphics - one frame at a time"
     */
    renderFrame() {
        // Clear screen to background color
        const bgColor = this.activePalettes[0][0];
        const [r, g, b] = this.hexToRgb(bgColor);

        for (let i = 0; i < this.imageData.data.length; i += 4) {
            this.imageData.data[i + 0] = r;
            this.imageData.data[i + 1] = g;
            this.imageData.data[i + 2] = b;
            this.imageData.data[i + 3] = 255;
        }

        // Render background layer
        this.renderBackground();

        // Render sprite layer
        this.renderSprites();

        // Push frame to screen
        this.ctx.putImageData(this.imageData, 0, 0);

        this.frameCount++;
    }

    /*
     * Randy's Cannabis Enhancement System
     * "Switch palettes for different plant spirit modes"
     */
    activatePlantSpiritMode(mode) {
        this.plantSpiritMode = mode;

        switch (mode) {
            case 'Cannabis Green':
                this.activePalettes[0] = CEPU_PALETTE.CANNABIS_GREEN;
                break;
            case 'Strawberry Pie':
                this.activePalettes[0] = CEPU_PALETTE.STRAWBERRY_PIE;
                break;
            case 'Blue Dream':
                this.activePalettes[0] = CEPU_PALETTE.BLUE_DREAM;
                break;
            case 'Purple Haze':
                this.activePalettes[0] = CEPU_PALETTE.PURPLE_HAZE;
                break;
        }

        console.log('🌿 Plant Spirit Mode: ' + mode);
    }

    /*
     * Randy's PPU Status Display
     * "Educational information about PPU state"
     */
    getStatus() {
        return {
            frameCount: this.frameCount,
            plantSpiritMode: this.plantSpiritMode,
            screenSize: CEPU_CONFIG.SCREEN_WIDTH + 'x' + CEPU_CONFIG.SCREEN_HEIGHT,
            tileSize: CEPU_CONFIG.TILE_SIZE + 'x' + CEPU_CONFIG.TILE_SIZE,
            maxTiles: CEPU_CONFIG.MAX_TILES,
            maxSprites: CEPU_CONFIG.MAX_SPRITES,
            memoryUsage: {
                tileMemory: this.tileMemory.length + ' bytes',
                nameTable: this.nameTable.length + ' bytes',
                spriteMemory: this.spriteMemory.length + ' bytes'
            }
        };
    }
}

/*
 * Randy's Educational Demo System
 * "Show the difference between tiles and sprites"
 */
class CEPUDemo {
    constructor(ppu) {
        this.ppu = ppu;
        this.setupDemo();
    }

    setupDemo() {
        console.log('🎮 Setting up Cannabis-Enhanced PPU Demo');

        // Create some simple tile patterns
        this.createTilePatterns();

        // Setup background
        this.setupBackground();

        // Setup sprites
        this.setupSprites();

        console.log('✅ Demo setup complete - ready for educational magic!');
    }

    createTilePatterns() {
        // Tile 1: Solid block
        const solidBlock = [
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Low bits
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF   // High bits
        ];

        // Tile 2: Checkerboard pattern
        const checkerboard = [
            0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,  // Low bits
            0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA   // High bits
        ];

        // Tile 3: Cannabis leaf pattern (simplified)
        const cannabisLeaf = [
            0x18, 0x24, 0x5A, 0x7E, 0x7E, 0x3C, 0x18, 0x00,  // Low bits
            0x00, 0x18, 0x24, 0x42, 0x42, 0x24, 0x18, 0x00   // High bits
        ];

        // Tile 4: Smiley face (for sprite)
        const smileyFace = [
            0x3C, 0x42, 0x99, 0xA5, 0xA5, 0x99, 0x42, 0x3C,  // Low bits
            0x00, 0x3C, 0x66, 0x5A, 0x5A, 0x66, 0x3C, 0x00   // High bits
        ];

        this.ppu.loadTilePattern(1, solidBlock);
        this.ppu.loadTilePattern(2, checkerboard);
        this.ppu.loadTilePattern(3, cannabisLeaf);
        this.ppu.loadTilePattern(4, smileyFace);
    }

    setupBackground() {
        // Create a simple background pattern
        for (let y = 0; y < CEPU_CONFIG.TILES_Y; y++) {
            for (let x = 0; x < CEPU_CONFIG.TILES_X; x++) {
                if ((x + y) % 4 === 0) {
                    this.ppu.setBackgroundTile(x, y, 2, 0); // Checkerboard
                } else if ((x + y) % 8 === 0) {
                    this.ppu.setBackgroundTile(x, y, 3, 1); // Cannabis leaf
                }
            }
        }

        // Border
        for (let x = 0; x < CEPU_CONFIG.TILES_X; x++) {
            this.ppu.setBackgroundTile(x, 0, 1, 2);
            this.ppu.setBackgroundTile(x, CEPU_CONFIG.TILES_Y - 1, 1, 2);
        }
        for (let y = 0; y < CEPU_CONFIG.TILES_Y; y++) {
            this.ppu.setBackgroundTile(0, y, 1, 2);
            this.ppu.setBackgroundTile(CEPU_CONFIG.TILES_X - 1, y, 1, 2);
        }
    }

    setupSprites() {
        // Single sprite to demonstrate sprite vs tile difference
        this.ppu.setSpriteData(0, 120, 116, 4, 3); // Smiley face in center
    }

    animateSprite() {
        // Simple sprite animation - move the smiley around
        const centerX = CEPU_CONFIG.SCREEN_WIDTH / 2;
        const centerY = CEPU_CONFIG.SCREEN_HEIGHT / 2;
        const radius = 32;
        const angle = this.ppu.frameCount * 0.05;

        const spriteX = centerX + Math.cos(angle) * radius - 4;
        const spriteY = centerY + Math.sin(angle) * radius - 4;

        this.ppu.setSpriteData(0, Math.floor(spriteX), Math.floor(spriteY), 4, 3);
    }

    update() {
        this.animateSprite();
    }
}

/*
 * Randy's PPU Initialization and Demo
 * "Educational graphics programming made simple and cannabis-enhanced"
 */
let ppu;
let demo;

function initializePPU() {
    console.log('🧠⚡ Initializing Randy\'s Cannabis-Enhanced PPU ⚡🧠');

    ppu = new CannabisEnhancedPPU('ppuCanvas');
    demo = new CEPUDemo(ppu);

    console.log('💡 Educational Value:');
    console.log('   🎨 Tile-based background rendering');
    console.log('   👾 Sprite object rendering and animation');
    console.log('   🌈 Palette-based color system');
    console.log('   💾 Memory-mapped graphics programming');
    console.log('   🕹️ Old school constraints and creativity');

    startRenderLoop();
}

function startRenderLoop() {
    function gameLoop() {
        demo.update();
        ppu.renderFrame();
        requestAnimationFrame(gameLoop);
    }

    console.log('🚀 PPU render loop started');
    gameLoop();
}

function changePlantSpirit() {
    const modes = ['Cannabis Green', 'Strawberry Pie', 'Blue Dream', 'Purple Haze'];
    const currentMode = modes[Math.floor(Math.random() * modes.length)];
    ppu.activatePlantSpiritMode(currentMode);
}

function showPPUStatus() {
    const status = ppu.getStatus();
    console.log('📊 PPU Status:', status);

    document.getElementById('ppuStatus').innerHTML = `
        <h3>🧠 PPU Status</h3>
        <p><strong>Frame Count:</strong> ${status.frameCount}</p>
        <p><strong>Plant Spirit Mode:</strong> ${status.plantSpiritMode}</p>
        <p><strong>Resolution:</strong> ${status.screenSize}</p>
        <p><strong>Tile Size:</strong> ${status.tileSize}</p>
        <p><strong>Max Tiles:</strong> ${status.maxTiles}</p>
        <p><strong>Max Sprites:</strong> ${status.maxSprites}</p>
        <h4>Memory Usage:</h4>
        <ul>
            <li>Tile Memory: ${status.memoryUsage.tileMemory}</li>
            <li>Name Table: ${status.memoryUsage.nameTable}</li>
            <li>Sprite Memory: ${status.memoryUsage.spriteMemory}</li>
        </ul>
    `;
}

// Auto-initialize when page loads
document.addEventListener('DOMContentLoaded', initializePPU);

/*
 * ============================================================================
 * RANDY'S EDUCATIONAL PPU DOCUMENTATION
 * ============================================================================
 * 
 * HTML Setup:
 * <!DOCTYPE html>
 * <html>
 * <head>
 *     <title>Randy's Cannabis-Enhanced PPU</title>
 *     <style>
 *         canvas { border: 2px solid #4d8c4d; }
 *         body { font-family: monospace; background: #000; color: #4d8c4d; }
 *     </style>
 * </head>
 * <body>
 *     <h1>🧠⚡ Randy's Cannabis-Enhanced PPU ⚡🧠</h1>
 *     <canvas id="ppuCanvas"></canvas>
 *     <div>
 *         <button onclick="changePlantSpirit()">🌿 Change Plant Spirit</button>
 *         <button onclick="showPPUStatus()">📊 Show Status</button>
 *     </div>
 *     <div id="ppuStatus"></div>
 *     <script src="randy_cannabis_enhanced_ppu.js"></script>
 * </body>
 * </html>
 * 
 * Educational Concepts Demonstrated:
 * 
 * 1. Memory-Mapped Graphics:
 *    - Everything lives in dedicated memory banks
 *    - Tile patterns, name tables, sprite data all pre-allocated
 *    - "Get your shit written to memory before rendering"
 * 
 * 2. Tile vs Sprite Distinction:
 *    - Background tiles: Fixed grid, efficient for static content
 *    - Sprites: Freely positioned, perfect for moving objects
 *    - Clear demonstration of each system's strengths
 * 
 * 3. Palette-Based Color System:
 *    - Limited color palettes enforce artistic constraints
 *    - Cannabis-enhanced color schemes for plant spirit modes
 *    - Educational color theory through retro limitations
 * 
 * 4. Old School Constraints:
 *    - 8x8 pixel tiles (classic NES/Game Boy style)
 *    - Limited sprite count and screen resolution
 *    - Constraints breed creativity and efficiency
 * 
 * 5. Frame-Based Rendering:
 *    - Clear separation of update and render phases
 *    - Manual pixel manipulation for educational clarity
 *    - No modern GPU abstractions hiding the fundamentals
 * 
 * Randy's PPU Philosophy:
 * "Modern graphics programming hides the fundamentals behind layers
 * of abstraction. This PPU strips away the complexity and shows how
 * graphics really work: patterns in memory, colors from palettes,
 * and pixels placed one at a time. Cannabis enhancement reveals the
 * beauty in these simple, elegant constraints."
 * 
 * Plant Spirit Graphics Wisdom:
 * "Every pixel is placed with intention. Every tile serves a purpose.
 * The canvas-enhanced mind sees patterns invisible to the rushed
 * developer. Slow down, understand the memory layout, and let the
 * plant spirits guide your graphics architecture."
 * 
 * Complete Educational Achievement:
 * Randy's curriculum now includes graphics programming fundamentals
 * alongside system programming, networking, and virtual machines.
 * Students learn how pixels become worlds through authentic retro
 * constraints and cannabis-enhanced creative clarity.
 */