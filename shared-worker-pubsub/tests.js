import { PubSubClient } from './pubsub-client.js';

const results = { passed: 0, failed: 0 };
async function test(name, fn) {
  console.group(`🧪 ${name}`);
  try {
    await fn();
    console.log('%c✔ PASSED', 'color:green');
    results.passed++;
  } catch (e) {
    console.error('%c✘ FAILED', 'color:red', e);
    results.failed++;
  }
  console.groupEnd();
}

function waitFor(client, channel) {
  return new Promise(resolve => {
    const unsub = client.subscribe(channel, data => {
      unsub();
      resolve(data);
    });
  });
}

export async function runSuite() {
  const c1 = new PubSubClient('./shared-worker.js');
  const c2 = new PubSubClient('./shared-worker.js');

  await test('Direct Messaging', async () => {
    const p = waitFor(c2, 'test-chan');
    c1.publish('test-chan', { val: 123 });
    const res = await p;
    if (res.val !== 123) throw new Error('Value mismatch');
  });

  await test('Broadcast', async () => {
    const p = waitFor(c2, 'BROADCAST');
    c1.broadcast({ val: 'alert' });
    const res = await p;
    if (res.val !== 'alert') throw new Error('Broadcast mismatch');
  });

  console.log(`🏁 Done: ${results.passed} Passed, ${results.failed} Failed`);
}