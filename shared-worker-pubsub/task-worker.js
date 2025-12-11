import { PubSubClient } from './pubsub-client.js';

let hub = null;

self.onmessage = (e) => {
  const { command } = e.data;
  if (command === 'init-pubsub') {
    const sharedPort = e.ports[0];
    hub = new PubSubClient(sharedPort);
    
    // Announce existence
    hub.broadcast({ source: 'Dedicated Worker', text: 'I am online!' });

    // Listen for work
    hub.subscribe('heavy-math-request', (payload) => {
      const result = payload.number * 2; 
      hub.publish('heavy-math-result', { 
        input: payload.number, 
        result: result, 
        workerId: 'Worker-01' 
      });
    });
  }
};