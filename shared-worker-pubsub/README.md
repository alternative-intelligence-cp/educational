# SharedWorker Pub/Sub

## How to Run
1. Since this uses Workers, **you must use a local server**.
2. Run `npx http-server . -c-1`
3. Open `http://127.0.0.1:8080/index.html` in two different tabs.

## Challenge Mode
1. **Typing Indicator:** Add a throttled "User is typing..." broadcast.
2. **History Buffer:** Modify `shared-worker.js` to replay the last 5 chat messages to new users.
3. **Kill Switch:** Add an admin command to remotely close a specific user's port.