export class PubSubClient {
  constructor(options) {
    if (typeof options === 'string') {
      // MODE 1: Standard (Main Thread)
      this.worker = new SharedWorker(options);
      this.port = this.worker.port;
    } else if (options instanceof MessagePort) {
      // MODE 2: Port Transfer (Dedicated Worker)
      this.port = options;
    } else {
      throw new Error('Invalid constructor arguments');
    }

    this.listeners = new Map();
    this.port.start();

    this.port.onmessage = (event) => {
      const { channel, payload } = event.data;
      this._distributeLocal(channel, payload);
    };
  }

  subscribe(channel, callback) {
    if (!this.listeners.has(channel)) {
      this.listeners.set(channel, new Set());
      this._send('subscribe', { channel });
    }
    this.listeners.get(channel).add(callback);
    return () => this.unsubscribe(channel, callback);
  }

  unsubscribe(channel, callback) {
    const channelListeners = this.listeners.get(channel);
    if (channelListeners) {
      channelListeners.delete(callback);
      if (channelListeners.size === 0) {
        this.listeners.delete(channel);
        this._send('unsubscribe', { channel });
      }
    }
  }

  publish(channel, payload) {
    this._send('publish', { channel, payload });
  }

  broadcast(payload) {
    this._send('broadcast', { payload });
  }

  _send(action, data) {
    this.port.postMessage({ action, ...data });
  }

  _distributeLocal(channel, payload) {
    if (this.listeners.has(channel)) {
      this.listeners.get(channel).forEach(cb => cb(payload));
    }
    if (channel === 'BROADCAST' && this.listeners.has('BROADCAST')) {
      this.listeners.get('BROADCAST').forEach(cb => cb(payload));
    }
  }
}