/**
 * SJS Game Demo - Space Invaders Clone
 * 
 * Demonstrates:
 * - ECS (entities, components, systems)
 * - Renderer (sprites, layers, camera)
 * - Audio (sound effects, music)
 * - Input (keyboard handling from SJS)
 * - Pub/Sub (game events)
 * 
 * This is intentionally simple - focus on architecture, not fancy graphics.
 */

(function () {
    'use strict';

    // Grab SJS modules
    const { ecs, renderer, audio, input, pubSub } = SJS;

    // ===== GAME STATE =====
    const GAME = {
        width: 800,
        height: 600,
        score: 0,
        lives: 3,
        state: 'menu', // 'menu' | 'playing' | 'gameover'
        level: 1,
        lastTime: 0
    };

    // ===== COMPONENT DEFINITIONS =====
    ecs.registerComponent('Position', { x: 0, y: 0 });
    ecs.registerComponent('Velocity', { x: 0, y: 0 });
    ecs.registerComponent('Sprite', { name: '', width: 32, height: 32, color: '#fff' });
    ecs.registerComponent('Player', { speed: 200, shootCooldown: 0.25, lastShot: 0 });
    ecs.registerComponent('Enemy', { speed: 50, points: 10 });
    ecs.registerComponent('Bullet', { speed: 400, damage: 1 });
    ecs.registerComponent('Bounds', { width: 32, height: 32 });
    ecs.registerComponent('Lifetime', { duration: 2.0, elapsed: 0 });

    // ===== PREFABS (entity templates) =====
    ecs.definePrefab('player', {
        Position: { x: 400, y: 550 },
        Velocity: { x: 0, y: 0 },
        Sprite: { name: 'player', width: 32, height: 32, color: '#0ff' },
        Player: { speed: 250, shootCooldown: 0.25, lastShot: 0 },
        Bounds: { width: 32, height: 32 }
    });

    ecs.definePrefab('enemy', {
        Position: { x: 0, y: 0 },
        Velocity: { x: 50, y: 0 },
        Sprite: { name: 'enemy', width: 32, height: 32, color: '#f0f' },
        Enemy: { speed: 50, points: 10 },
        Bounds: { width: 32, height: 32 }
    });

    ecs.definePrefab('bullet', {
        Position: { x: 0, y: 0 },
        Velocity: { x: 0, y: -400 },
        Sprite: { name: 'bullet', width: 4, height: 12, color: '#ff0' },
        Bullet: { speed: 400, damage: 1 },
        Bounds: { width: 4, height: 12 },
        Lifetime: { duration: 2.0, elapsed: 0 }
    });

    // ===== SYSTEMS =====

    // Movement system
    ecs.registerSystem('movement', ['Position', 'Velocity'], (dt, entities) => {
        entities.forEach(id => {
            const pos = ecs.get(id, 'Position');
            const vel = ecs.get(id, 'Velocity');
            pos.x += vel.x * dt;
            pos.y += vel.y * dt;
        });
    });

    // Player input system
    ecs.registerSystem('playerInput', ['Player', 'Position', 'Velocity'], (dt, entities) => {
        if (GAME.state !== 'playing') return;

        entities.forEach(id => {
            const player = ecs.get(id, 'Player');
            const pos = ecs.get(id, 'Position');
            const vel = ecs.get(id, 'Velocity');

            // Horizontal movement
            vel.x = 0;
            if (input.isKeyDown('ArrowLeft') || input.isKeyDown('a')) {
                vel.x = -player.speed;
            }
            if (input.isKeyDown('ArrowRight') || input.isKeyDown('d')) {
                vel.x = player.speed;
            }

            // Shooting
            player.lastShot += dt;
            if (input.isKeyDown(' ') && player.lastShot >= player.shootCooldown) {
                spawnBullet(pos.x + 14, pos.y);
                player.lastShot = 0;
                audio.play('shoot', { volume: 0.3 });
            }

            // Keep in bounds
            pos.x = Math.max(0, Math.min(GAME.width - 32, pos.x));
        });
    });

    // Enemy movement system (back and forth, step down)
    ecs.registerSystem('enemyMovement', ['Enemy', 'Position', 'Velocity'], (dt, entities) => {
        if (GAME.state !== 'playing') return;

        let hitEdge = false;

        entities.forEach(id => {
            const pos = ecs.get(id, 'Position');
            const vel = ecs.get(id, 'Velocity');

            if (pos.x <= 0 || pos.x >= GAME.width - 32) {
                hitEdge = true;
            }
        });

        if (hitEdge) {
            entities.forEach(id => {
                const pos = ecs.get(id, 'Position');
                const vel = ecs.get(id, 'Velocity');
                vel.x = -vel.x;
                pos.y += 20; // Step down

                // Game over if enemies reach bottom
                if (pos.y > 500) {
                    gameOver();
                }
            });
        }
    });

    // Collision system (bullets vs enemies)
    ecs.registerSystem('collision', ['Bullet'], (dt, bullets) => {
        if (GAME.state !== 'playing') return;

        const enemies = ecs.query('Enemy', 'Position', 'Bounds');

        bullets.forEach(bulletId => {
            const bulletPos = ecs.get(bulletId, 'Position');
            const bulletBounds = ecs.get(bulletId, 'Bounds');

            enemies.forEach(enemyId => {
                const enemyPos = ecs.get(enemyId, 'Position');
                const enemyBounds = ecs.get(enemyId, 'Bounds');
                const enemy = ecs.get(enemyId, 'Enemy');

                if (aabbCollision(
                    bulletPos.x, bulletPos.y, bulletBounds.width, bulletBounds.height,
                    enemyPos.x, enemyPos.y, enemyBounds.width, enemyBounds.height
                )) {
                    // Hit!
                    ecs.destroy(bulletId);
                    ecs.destroy(enemyId);
                    GAME.score += enemy.points;
                    updateUI();
                    audio.play('explosion', { volume: 0.4 });

                    // Check win condition
                    if (ecs.query('Enemy').length === 0) {
                        nextLevel();
                    }
                }
            });
        });
    });

    // Lifetime system (despawn after duration)
    ecs.registerSystem('lifetime', ['Lifetime'], (dt, entities) => {
        entities.forEach(id => {
            const lifetime = ecs.get(id, 'Lifetime');
            lifetime.elapsed += dt;
            if (lifetime.elapsed >= lifetime.duration) {
                ecs.destroy(id);
            }
        });
    });

    // Render system
    ecs.registerSystem('render', ['Position', 'Sprite'], (dt, entities) => {
        renderer.clear('game');

        entities.forEach(id => {
            const pos = ecs.get(id, 'Position');
            const sprite = ecs.get(id, 'Sprite');

            // Draw simple colored rectangle (no sprites loaded)
            renderer.drawRect(
                pos.x, pos.y, sprite.width, sprite.height,
                { fill: sprite.color, layer: 'game' }
            );
        });

        // UI Layer
        renderer.clear('ui');
        if (GAME.state === 'menu') {
            renderer.drawText('SPACE INVADERS', GAME.width / 2 - 100, GAME.height / 2 - 50, {
                font: '32px monospace',
                color: '#0ff',
                layer: 'ui'
            });
            renderer.drawText('Press any key to start', GAME.width / 2 - 120, GAME.height / 2 + 20, {
                font: '16px monospace',
                layer: 'ui'
            });
        } else if (GAME.state === 'gameover') {
            renderer.drawText('GAME OVER', GAME.width / 2 - 80, GAME.height / 2 - 50, {
                font: '32px monospace',
                color: '#f00',
                layer: 'ui'
            });
            renderer.drawText(`Final Score: ${GAME.score}`, GAME.width / 2 - 80, GAME.height / 2 + 20, {
                font: '20px monospace',
                layer: 'ui'
            });
            renderer.drawText('Press R to restart', GAME.width / 2 - 100, GAME.height / 2 + 60, {
                font: '16px monospace',
                layer: 'ui'
            });
        }

        renderer.present();
    });

    // ===== HELPER FUNCTIONS =====

    function aabbCollision(x1, y1, w1, h1, x2, y2, w2, h2) {
        return x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2;
    }

    function spawnBullet(x, y) {
        ecs.spawn('bullet', {
            Position: { x, y }
        });
    }

    function spawnEnemies() {
        const rows = 3 + Math.floor(GAME.level / 2);
        const cols = 8;
        const spacing = 60;
        const startX = 100;
        const startY = 50;

        for (let row = 0; row < rows; row++) {
            for (let col = 0; col < cols; col++) {
                ecs.spawn('enemy', {
                    Position: { x: startX + col * spacing, y: startY + row * 50 },
                    Velocity: { x: 50 + GAME.level * 10, y: 0 }
                });
            }
        }
    }

    function nextLevel() {
        GAME.level++;
        GAME.lives++;
        updateUI();
        spawnEnemies();
        pubSub.publish('game:nextLevel', { level: GAME.level });
    }

    function gameOver() {
        GAME.state = 'gameover';
        audio.stopAll('music');
        audio.play('gameover', { volume: 0.5 });
        pubSub.publish('game:over', { score: GAME.score });
    }

    function restart() {
        GAME.score = 0;
        GAME.lives = 3;
        GAME.level = 1;
        GAME.state = 'playing';

        ecs.clear();
        ecs.spawn('player');
        spawnEnemies();

        updateUI();
        pubSub.publish('game:restart');
    }

    function updateUI() {
        document.getElementById('score').textContent = GAME.score;
        document.getElementById('lives').textContent = GAME.lives;
    }

    // ===== GAME LOOP =====

    function gameLoop(timestamp) {
        const dt = GAME.lastTime ? (timestamp - GAME.lastTime) / 1000 : 0;
        GAME.lastTime = timestamp;

        // Cap delta time to prevent spiral of death
        const cappedDt = Math.min(dt, 0.1);

        // Update systems
        ecs.update(cappedDt);

        // Update FPS display
        const fps = renderer.getFPS();
        document.getElementById('fps').textContent = Math.round(fps);

        requestAnimationFrame(gameLoop);
    }

    // ===== INITIALIZATION =====

    function init() {
        console.log('🎮 Initializing SJS Game Demo...');

        // Initialize renderer
        renderer.init('#gameCanvas', {
            width: GAME.width,
            height: GAME.height,
            pixelArt: true,
            clearColor: '#000',
            debug: false
        });

        // Create layers
        renderer.createLayer('game', { zIndex: 0 });
        renderer.createLayer('ui', { zIndex: 100 });

        // Initialize audio (mock sounds with beep generator)
        audio.init();

        // Create simple beep sounds using oscillator
        // (In real game, you'd load actual audio files)
        createBeepSound('shoot', 200, 0.1);
        createBeepSound('explosion', 100, 0.2);
        createBeepSound('gameover', 50, 0.5);

        // Initialize input
        input.configure({ enabled: true });

        // Subscribe to game events
        pubSub.subscribe('game:start', () => {
            console.log('Game started!');
        });

        pubSub.subscribe('game:over', (data) => {
            console.log('Game over! Score:', data.score);
            document.getElementById('status').textContent = 'Game Over! Press R to restart';
        });

        // Handle keyboard input for game state
        document.addEventListener('keydown', (e) => {
            if (GAME.state === 'menu' && e.key !== 'r') {
                restart();
                document.getElementById('status').textContent = 'Playing...';
            } else if (GAME.state === 'gameover' && e.key === 'r') {
                restart();
                document.getElementById('status').textContent = 'Playing...';
            }
        });

        // Start game loop
        requestAnimationFrame(gameLoop);

        console.log('✅ Game initialized! Systems:', ecs.getSystems().length);
    }

    // Simple beep sound generator (placeholder until real audio files)
    function createBeepSound(name, frequency, duration) {
        // This is a hack - we'd normally load actual audio files
        // For demo purposes, we'll just register them as placeholders
        // The audio module would need Web Audio API oscillator support for real beeps
        console.log(`[Audio] Registered placeholder sound: ${name}`);
    }

    // ===== START =====

    // Wait for DOM to load
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', init);
    } else {
        init();
    }

})();
