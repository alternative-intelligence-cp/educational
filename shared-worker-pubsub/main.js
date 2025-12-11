import { PubSubClient } from './pubsub-client.js';

// A. Connect Main Thread
const hub = new PubSubClient('./shared-worker.js');

// B. Connect Dedicated Worker (The Handshake)
const dedicatedWorker = new Worker('./task-worker.js', { type: 'module' });
const workerLink = new SharedWorker('./shared-worker.js');
dedicatedWorker.postMessage({ command: 'init-pubsub' }, [workerLink.port]);

// C. UI Logic
const output = document.getElementById('logs');
const log = (msg) => {
  const div = document.createElement('div');
  div.textContent = `[${new Date().toLocaleTimeString()}] ${msg}`;
  output.prepend(div);
};

// Subscribe to general chat
hub.subscribe('chat', (data) => log(`Chat: ${data.text}`));

// Subscribe to broadcasts
hub.subscribe('BROADCAST', (data) => {
  log(`📢 BROADCAST: ${JSON.stringify(data)}`);
  document.body.style.backgroundColor = '#fff3e0';
  setTimeout(() => document.body.style.backgroundColor = '#fff', 500);
});

// Subscribe to math results
hub.subscribe('heavy-math-result', (data) => {
  log(`🧮 Math Result: ${data.input} x 2 = ${data.result}`);
});

// UI Event Handlers
document.getElementById('send-chat').onclick = () => {
  const text = document.getElementById('chat-input').value;
  hub.publish('chat', { text });
};

document.getElementById('send-broadcast').onclick = () => {
  hub.broadcast({ msg: 'System Alert!' });
};

document.getElementById('send-math').onclick = () => {
  const num = Math.floor(Math.random() * 100);
  log(`Requesting math for ${num}...`);
  hub.publish('heavy-math-request', { number: num });
};