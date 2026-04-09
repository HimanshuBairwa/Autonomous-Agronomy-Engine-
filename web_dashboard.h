#pragma once
#include <WebServer.h>
#include <ArduinoJson.h>

// ── STANDARD WEB SERVER (Port 80) ──
extern WebServer server;

// ── HTML PAGE (MINIFIED CSS+JS) ──
// V3.0: Incorporates Chart.js, SVG Circular Progress Bars, and Red-Alert Safe Mode Banner!
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <title>Autonomous Agronomy AI</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;700;900&display=swap" rel="stylesheet">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    :root {
      --bg: #0b1120;
      --card-bg: rgba(30, 41, 59, 0.7);
      --card-border: rgba(255, 255, 255, 0.08);
      --text: #f8fafc;
      --text-muted: #94a3b8;
      --primary: #3b82f6;
      --success: #10b981;
      --warning: #f59e0b;
      --danger: #ef4444;
    }
    body {
      font-family: 'Inter', sans-serif;
      background: radial-gradient(circle at 50% 0%, rgb(15, 23, 42) 0%, rgb(9, 9, 11) 100%);
      color: var(--text);
      margin: 0; padding: 0; min-height: 100vh;
    }
    .alert-banner {
      background: var(--danger); color: white; text-align: center; font-weight: 900;
      padding: 12px; letter-spacing: 2px; text-transform: uppercase;
      box-shadow: 0 0 20px rgba(239, 68, 68, 0.6);
      display: none; animation: flash 1s infinite alternate;
    }
    @keyframes flash { from { opacity: 0.8; } to { opacity: 1; } }
    
    .container { padding: 30px 15px; max-width: 1400px; margin: 0 auto; }
    .header { text-align: center; margin-bottom: 40px; }
    h1 { font-weight: 900; font-size: 2.4rem; margin: 0; background: linear-gradient(90deg, #3b82f6, #10b981); -webkit-background-clip: text; -webkit-text-fill-color: transparent;}
    .subtitle { color: var(--text-muted); font-size: 1rem; font-weight: 500; margin-top: 5px; }

    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(340px, 1fr)); gap: 24px; }
    
    .card {
      background: var(--card-bg); backdrop-filter: blur(12px); -webkit-backdrop-filter: blur(12px);
      border: 1px solid var(--card-border); border-radius: 24px; padding: 25px;
      box-shadow: 0 10px 40px -10px rgba(0,0,0,0.5); transition: 0.3s;
    }
    .card:hover { transform: translateY(-5px); border-color: rgba(255,255,255,0.15); box-shadow: 0 15px 50px -10px rgba(59, 130, 246, 0.15); }
    h2 { font-size: 1.15rem; font-weight: 800; color: #fff; text-transform: uppercase; letter-spacing: 1.5px; margin: 0 0 20px 0; display: flex; align-items: center; }

    .metric { display: flex; justify-content: space-between; align-items: center; padding: 14px 0; border-bottom: 1px solid rgba(255,255,255,0.05); }
    .metric:last-child { border-bottom: none; }
    .label { color: var(--text-muted); font-weight: 600; font-size: 0.95rem; }
    .value { font-weight: 800; font-size: 1.2rem; }

    .status-badge { padding: 10px 20px; border-radius: 30px; font-weight: 900; font-size: 1.1rem; letter-spacing: 1px; text-align: center; display: block; box-shadow: 0 4px 15px rgba(0,0,0,0.2); margin-bottom: 20px;}
    
    .control-group { display: flex; gap: 8px; background: rgba(0,0,0,0.3); padding: 6px; border-radius: 12px;}
    .btn { flex: 1; padding: 12px 0; border: none; border-radius: 8px; font-weight: 800; font-size: 0.8rem; cursor: pointer; transition: 0.3s; background: transparent; color: var(--text-muted); }
    .btn.active { background: #3b82f6; color: white; box-shadow: 0 0 15px rgba(59,130,246,0.5); }
    
    .pump-btn { flex:1; font-size: 1.2rem; padding: 16px; border-radius: 12px; color: white; border: none; cursor: pointer; font-weight: 900; letter-spacing: 2px;}
    .pump-on { background: linear-gradient(135deg, #10b981, #059669); }
    .pump-off { background: linear-gradient(135deg, #ef4444, #dc2626); }

    .radial-wrap { display: flex; justify-content: center; align-items: center; gap: 20px; padding: 10px 0;}
    .circle-chart { width: 120px; height: 120px; }
    .circle-chart__circle { stroke: #3b82f6; stroke-width: 3; stroke-linecap: round; fill: none; transition: stroke-dasharray 1s ease-out; }
    .circle-chart__bg { stroke: rgba(255,255,255,0.1); stroke-width: 3; fill: none; }
    .circle-chart__info { fill: white; font-weight: 900; font-size: 8px; text-anchor: middle; font-family: Inter; alignment-baseline: middle;}
    .circle-chart__sub { fill: var(--text-muted); font-size: 3px; text-anchor: middle; font-weight: 500;}

    .chart-container { width: 100%; height: 250px; margin-top: 15px; }

    .log-box { background: rgba(0,0,0,0.4); padding: 25px; border-radius: 16px; border-left: 4px solid var(--success); font-family: 'Courier New', monospace; }
    .btn-download { background: linear-gradient(135deg, #8b5cf6, #6d28d9); width: 100%; display: block; padding: 18px; margin-top: 25px; border-radius: 12px; color: white; text-decoration: none; font-weight: 800; font-size: 1.1rem; text-align: center; border: none; cursor: pointer; transition: 0.3s; }
    .btn-download:hover { transform: translateY(-3px); box-shadow: 0 10px 25px rgba(139,92,246,0.4); }
  </style>
</head>
<body>

  <div id="alert-banner" class="alert-banner">
    ⚠️ CRITICAL HARDWARE FAULT ⚠️ SYSTEM RUNNING IN SAFE MODE FALLBACK
  </div>

  <div class="container">
    <div class="header">
      <h1>Autonomous Agronomy Engine</h1>
      <div class="subtitle"><span style="color:#10b981;">● LIVE TELEMETRY</span> &nbsp;|&nbsp; Edge Intelligence V3.0</div>
    </div>
    
    <div class="grid">
      
      <!-- MODE & HEALTH -->
      <div class="card">
        <h2>🛡️ Core State</h2>
        <div id="sys-mode-badge" class="status-badge">SYNCING...</div>
        
        <div class="radial-wrap">
          <svg class="circle-chart" viewBox="0 0 36 36">
            <path class="circle-chart__bg" d="M18 2 a 16 16 0 0 1 0 32 a 16 16 0 0 1 0 -32" />
            <path id="health-circle" class="circle-chart__circle" d="M18 2 a 16 16 0 0 1 0 32 a 16 16 0 0 1 0 -32" stroke-dasharray="0, 100" />
            <text id="health-txt" x="18" y="16" class="circle-chart__info">--%</text>
            <text x="18" y="22" class="circle-chart__sub">SENSOR TRUST</text>
          </svg>
          <svg class="circle-chart" viewBox="0 0 36 36">
            <path class="circle-chart__bg" d="M18 2 a 16 16 0 0 1 0 32 a 16 16 0 0 1 0 -32" />
            <path id="score-circle" class="circle-chart__circle" stroke="#8b5cf6" d="M18 2 a 16 16 0 0 1 0 32 a 16 16 0 0 1 0 -32" stroke-dasharray="0, 100" />
            <text id="score-txt" x="18" y="16" class="circle-chart__info">--</text>
            <text x="18" y="22" class="circle-chart__sub">DROP NEED</text>
          </svg>
        </div>
        
        <div style="margin-top:20px;">
          <div class="control-group">
            <button id="btn-m" class="btn" onclick="setMode(0)">MANUAL</button>
            <button id="btn-ba" class="btn" onclick="setMode(1)">AUTO</button>
            <button id="btn-sa" class="btn active" onclick="setMode(2)">SMART</button>
            <button id="btn-ai" class="btn" onclick="setMode(3)">EDGE AI</button>
          </div>
          <div id="manual-controls" style="display:none; margin-top:15px; display:flex; gap:10px;">
            <button class="pump-btn pump-on" onclick="setPump('on')">PUMP ON</button>
            <button class="pump-btn pump-off" onclick="setPump('off')">PUMP OFF</button>
          </div>
        </div>
      </div>
      
      <!-- ENVIRONMENT & LIVE CHART -->
      <div class="card" style="grid-column: span 2;">
        <h2>📊 Live Thermodynamic Flow</h2>
        <div style="display:flex; justify-content: space-between; border-bottom:1px solid rgba(255,255,255,0.05); padding-bottom: 10px;">
          <div><span class="label">Soil Moist: </span><span id="soil" class="value" style="color:#3b82f6;">--%</span></div>
          <div><span class="label">Temp: </span><span id="temp" class="value" style="color:#f59e0b;">--°C</span></div>
          <div><span class="label">Hum: </span><span id="hum" class="value" style="color:#10b981;">--%</span></div>
          <div><span class="label">Rain Risk: </span><span id="rain" class="value">--%</span></div>
        </div>
        <div class="chart-container">
          <canvas id="liveChart"></canvas>
        </div>
      </div>
      
      <!-- INTELLIGENCE & ECONOMICS -->
      <div class="card" style="grid-column: span 1;">
        <h2>🧠 AI Agronomy Data</h2>
        <div class="metric"><span class="label">VPD (Evapotranspiration)</span> <span class="value" id="et">--</span></div>
        <div class="metric"><span class="label">S-Curve Drought Index</span> <span class="value" id="dri">--</span></div>
        <div class="metric"><span class="label">AI Dispensed Volume</span> <span class="value" id="used">-- L</span></div>
        <div class="metric"><span class="label">Legacy Timer Baseline</span> <span class="value" id="baseline">-- L</span></div>
        <div class="metric"><span class="label" style="color:var(--success);">🏆 Litres Saved</span> <span class="value" id="saved" style="color:var(--success);">-- L</span></div>
      </div>
      
      <!-- LOG & EXPORT -->
      <div class="card" style="grid-column: span 2;">
        <h2>📡 Decision Engine Log</h2>
        <div class="log-box">
          <div style="color:var(--text-muted); font-size:0.9rem;">> PIPELINE_EXECUTION_STATE</div>
          <div style="color:white; font-size: 1.4rem; font-weight:900; margin: 10px 0;" id="log-action">AWAITING DATALINK...</div>
          <div style="color:var(--success); font-size: 1.0rem;" id="log-reason">> ROOT_CAUSE: INITIALIZING</div>
        </div>
        <a href="/export" download="Agriculture_Research_Data.csv">
          <button class="btn-download">📥 EXPORT 7-DAY CSV FOR POWER BI / MLOPS</button>
        </a>
      </div>
      
    </div>
  </div>

<script>
// Live Chart.js Setup
const ctx = document.getElementById('liveChart').getContext('2d');
const liveChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: Array(20).fill(''),
        datasets: [{
            label: 'Soil Moisture (%)',
            borderColor: '#3b82f6',
            backgroundColor: 'rgba(59, 130, 246, 0.1)',
            borderWidth: 2,
            pointRadius: 0,
            fill: true,
            tension: 0.4,
            data: Array(20).fill(null)
        }, {
            label: 'Irrigation Need (Score x100)',
            borderColor: '#8b5cf6',
            borderWidth: 2,
            borderDash: [5, 5],
            pointRadius: 0,
            tension: 0.4,
            data: Array(20).fill(null)
        }]
    },
    options: {
        responsive: true, maintainAspectRatio: false,
        animation: { duration: 0 },
        scales: {
            y: { min: 0, max: 100, grid: { color: 'rgba(255,255,255,0.05)' }, ticks: { color: '#94a3b8' } },
            x: { grid: { display: false } }
        },
        plugins: { legend: { labels: { color: '#94a3b8' } } }
    }
});

function updateChart(soil, score) {
    let dataSoil = liveChart.data.datasets[0].data;
    let dataScore = liveChart.data.datasets[1].data;
    dataSoil.push(soil);
    dataSoil.shift();
    dataScore.push(score * 100);
    dataScore.shift();
    liveChart.update();
}

// Controls
function setMode(m) { fetch('/setMode?val=' + m, {method: 'POST'}); }
function setPump(state) { fetch('/setPump?val=' + state, {method: 'POST'}); }

// Main Polling Loop
setInterval(() => {
  fetch('/data').then(res => res.json()).then(data => {
    
    document.getElementById('alert-banner').style.display = (data.sysMode == "SAFE MODE") ? "block" : "none";
    
    let badge = document.getElementById('sys-mode-badge');
    badge.innerText = "SYS: " + data.sysMode;
    if(data.sysMode == "NORMAL") { badge.style.background = "var(--success)"; badge.style.color = "white"; }
    else if(data.sysMode == "SAFE MODE") { badge.style.background = "var(--danger)"; badge.style.color = "white"; }
    else { badge.style.background = "var(--warning)"; badge.style.color = "#0f172a"; }
    
    document.getElementById('btn-m').className = (data.userMode == "MANUAL") ? "btn active" : "btn";
    document.getElementById('btn-ba').className = (data.userMode == "BASIC AUTO") ? "btn active" : "btn";
    document.getElementById('btn-sa').className = (data.userMode == "SMART AUTO") ? "btn active" : "btn";
    document.getElementById('btn-ai').className = (data.userMode == "EDGE AI PREDICTIVE") ? "btn active" : "btn";
    
    document.getElementById('manual-controls').style.display = (data.userMode == "MANUAL") ? "flex" : "none";
    
    let hScore = (data.health * 100).toFixed(0);
    document.getElementById('health-txt').innerText = hScore + '%';
    document.getElementById('health-circle').setAttribute('stroke-dasharray', `${hScore}, 100`);
    document.getElementById('health-circle').style.stroke = (hScore > 60) ? "var(--success)" : "var(--danger)";
    
    let iscore = (data.score * 100).toFixed(0);
    document.getElementById('score-txt').innerText = data.score.toFixed(2);
    document.getElementById('score-circle').setAttribute('stroke-dasharray', `${iscore}, 100`);

    document.getElementById('soil').innerText = data.soil.toFixed(1) + '%';
    document.getElementById('temp').innerText = data.temp.toFixed(1) + '°C';
    document.getElementById('hum').innerText = data.hum.toFixed(1) + '%';
    document.getElementById('rain').innerText = data.rain.toFixed(0) + '%';
    
    updateChart(data.soil, data.score);

    document.getElementById('et').innerText = data.et.toFixed(3);
    document.getElementById('dri').innerText = data.dri.toFixed(3);
    document.getElementById('used').innerText = data.waterUsed.toFixed(2) + ' L';
    document.getElementById('baseline').innerText = data.waterBase.toFixed(2) + ' L';
    document.getElementById('saved').innerText = data.waterSaved.toFixed(2) + ' L';
    
    document.getElementById('log-action').innerText = "> " + data.action;
    document.getElementById('log-reason').innerText = "> REASON: " + data.reason;
    if(data.action == "FULL_IRRIGATION" || data.action == "PARTIAL_BURST") {
        document.getElementById('log-action').style.color = "#3b82f6";
    } else {
        document.getElementById('log-action').style.color = "white";
    }
  });
}, 2000);
</script>
</body>
</html>
)rawliteral";
