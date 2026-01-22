const express = require('express');
const app = express();
const PORT = process.env.PORT || 3000;

// Store the latest frame in memory
let latestFrame = null;
let lastUpdate = 0;

// Parse raw binary data for image uploads
app.use('/upload', express.raw({ type: 'image/jpeg', limit: '5mb' }));

// ESP32 posts frames here
app.post('/upload', (req, res) => {
  if (req.body && req.body.length > 0) {
    latestFrame = req.body;
    lastUpdate = Date.now();
    console.log(`Frame received: ${req.body.length} bytes`);
    res.status(200).send('OK');
  } else {
    res.status(400).send('No image data');
  }
});

// Get the latest frame as JPEG
app.get('/frame', (req, res) => {
  if (latestFrame) {
    res.set('Content-Type', 'image/jpeg');
    res.set('Cache-Control', 'no-cache');
    res.send(latestFrame);
  } else {
    res.status(404).send('No frame available');
  }
});

// Simple HTML viewer page
app.get('/', (req, res) => {
  res.send(`
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Camera Stream</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }
    h1 { margin-bottom: 20px; }
    #stream { max-width: 100%; border: 2px solid #444; border-radius: 8px; }
    #status { margin-top: 10px; color: #888; }
  </style>
</head>
<body>
  <h1>ESP32 Camera Stream</h1>
  <img id="stream" src="/frame" alt="Camera Stream">
  <div id="status">Connecting...</div>
  <script>
    const img = document.getElementById('stream');
    const status = document.getElementById('status');
    let frameCount = 0;

    function refreshFrame() {
      const newImg = new Image();
      newImg.onload = function() {
        img.src = this.src;
        frameCount++;
        status.textContent = 'Live - Frames: ' + frameCount;
      };
      newImg.onerror = function() {
        status.textContent = 'Waiting for camera...';
      };
      newImg.src = '/frame?' + Date.now();
    }

    // Refresh every 200ms (5 fps)
    setInterval(refreshFrame, 200);
  </script>
</body>
</html>
  `);
});

// Health check for Render
app.get('/health', (req, res) => {
  res.json({ status: 'ok', lastFrame: lastUpdate ? new Date(lastUpdate).toISOString() : null });
});

app.listen(PORT, () => {
  console.log(`Camera relay server running on port ${PORT}`);
});
