const connections = new Set();
const subscriptions = new Map(); // Map<Channel, Set<Port>>

self.onconnect = (e) => {
  const port = e.ports[0];
  connections.add(port);
  port.start();

  port.onmessage = (event) => {
    const { action, channel, payload } = event.data;
    switch (action) {
      case 'subscribe': handleSubscribe(port, channel); break;
      case 'unsubscribe': handleUnsubscribe(port, channel); break;
      case 'publish': handlePublish(port, channel, payload); break;
      case 'broadcast': handleBroadcast(port, payload); break;
    }
  };
};

function handleSubscribe(port, channel) {
  if (!subscriptions.has(channel)) subscriptions.set(channel, new Set());
  subscriptions.get(channel).add(port);
}

function handleUnsubscribe(port, channel) {
  if (subscriptions.has(channel)) {
    subscriptions.get(channel).delete(port);
    if (subscriptions.get(channel).size === 0) subscriptions.delete(channel);
  }
}

function handlePublish(senderPort, channel, payload) {
  if (!subscriptions.has(channel)) return;
  subscriptions.get(channel).forEach((clientPort) => {
    clientPort.postMessage({ channel, payload });
  });
}

function handleBroadcast(senderPort, payload) {
  connections.forEach((clientPort) => {
    if (clientPort !== senderPort) {
      clientPort.postMessage({ channel: 'BROADCAST', payload });
    }
  });
}