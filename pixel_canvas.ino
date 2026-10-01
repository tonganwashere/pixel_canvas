<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <title>Pixel Art</title>
  <style>
    * { box-sizing: border-box; touch-action: none; -webkit-user-select: none; }
    body { background: #111; color: #fff; font-family: sans-serif; display: flex; flex-direction: column; align-items: center; margin: 0; padding: 12px; }
    #grid { display: grid; grid-template-columns: repeat(16, 1fr); width: 88vw; max-width: 320px; height: 88vw; max-height: 320px; border: 2px solid #444; background: #000; border-radius: 4px; }
    .cell { border: 1px solid #1a1a1a; background: #000; }
    .bar { width: 88vw; max-width: 320px; display: flex; gap: 8px; margin-top: 10px; align-items: center; }
    button { flex: 1; padding: 10px 4px; font-weight: bold; font-size: 13px; border: none; border-radius: 6px; background: #262626; color: #eee; cursor: pointer; }
    input[type="color"] { width: 44px; height: 44px; border: none; border-radius: 50%; background: none; cursor: pointer; }
    input[type="range"] { flex: 1; }
  </style>
</head>
<body>
  <div id="grid"></div>

  <!-- Drawing Bar -->
  <div class="bar">
    <button id="conn" onclick="connect()" style="background:#00b4d8">Connect</button>
    <input type="color" id="picker" value="#00ffcc">
    <button onclick="picker.value='#000000'">Eraser</button>
    <button onclick="send('CLEAR')" style="background:#e63946">Clear</button>
  </div>

  <!-- Presets Row 1 -->
  <div class="bar">
    <button onclick="send('M,1')">🔥 Flame</button>
    <button onclick="send('M,2')">🌈 Rainbow</button>
    <button onclick="send('M,3')">⚡ Matrix</button>
  </div>

  <!-- Presets Row 2 -->
  <div class="bar">
    <button onclick="send('M,4')">🌌 Stars</button>
    <button onclick="send('M,5')">🪩 Pulsar</button>
  </div>

  <!-- Brightness Slider -->
  <div class="bar">
    <span style="font-size:12px; color:#aaa;">Brightness</span>
    <input type="range" min="2" max="60" value="20" oninput="send('B,'+this.value)">
  </div>

  <script>
    let rx, drawing = false;
    const enc = new TextEncoder();
    const grid = document.getElementById('grid');
    const picker = document.getElementById('picker');

    // Build 16x16 grid
    for (let y = 0; y < 16; y++) {
      for (let x = 0; x < 16; x++) {
        let d = document.createElement('div');
        d.className = 'cell';
        d.dataset.xy = `${x},${y}`;
        grid.appendChild(d);
      }
    }

    async function connect() {
      try {
        const dev = await navigator.bluetooth.requestDevice({
          filters: [{ name: "PixelArt-BLE" }],
          optionalServices: ["6e400001-b5a3-f393-e0a9-e50e24dcca9e"]
        });
        const s = await (await dev.gatt.connect()).getPrimaryService("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
        rx = await s.getCharacteristic("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
        document.getElementById('conn').innerText = "Connected";
        document.getElementById('conn').style.background = "#2a9d8f";
      } catch (e) {
        alert("Bluetooth error: " + e);
      }
    }

    function send(str) {
      if (rx) rx.writeValueWithoutResponse(enc.encode(str)).catch(() => {});
      if (str === "CLEAR") document.querySelectorAll('.cell').forEach(c => c.style.backgroundColor = '#000');
    }

    function draw(el) {
      if (!el || !el.dataset.xy || !rx) return;
      el.style.backgroundColor = picker.value;
      let c = picker.value;
      send(`${el.dataset.xy},${parseInt(c.slice(1,3),16)},${parseInt(c.slice(3,5),16)},${parseInt(c.slice(5,7),16)}`);
    }

    grid.onpointerdown = (e) => { drawing = true; draw(document.elementFromPoint(e.clientX, e.clientY)); };
    window.onpointerup = () => drawing = false;
    grid.onpointermove = (e) => { if (drawing) draw(document.elementFromPoint(e.clientX, e.clientY)); };
  </script>
</body>
</html>