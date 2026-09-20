const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>Robot Control</title>
<script src="https://cdnjs.cloudflare.com/ajax/libs/three.js/r128/three.min.js" onerror="console.log('Brak internetu - tryb 2D')"></script>
<script src="https://cdn.jsdelivr.net/npm/three@0.128.0/examples/js/controls/OrbitControls.js" onerror="console.log('Brak internetu - tryb 2D')"></script>

<style>
    :root { --bg: #121212; --panel: #1e1e1e; --text: #e0e0e0; --accent: #00d2ff; --card: #2a2a2a; --danger: #ff4444; --success: #00ff88; --warning: #ffcc00; --delay: #ff9900; }
    body { margin: 0; font-family: sans-serif; background: var(--bg); color: var(--text); height: 100vh; display: flex; flex-direction: column; overflow: hidden; }
    #layout { display: flex; flex-direction: column; height: 100%; width: 100%; }
    
    /* Zmiana: Jeśli brak 3D, panel sterowania zajmuje więcej miejsca */
    #viz-container { flex: 0.4; min-height: 20vh; background: radial-gradient(circle, #2b2b2b 0%, #121212 100%); position: relative; border-bottom: 1px solid #333; }
    #panel { flex: 1; background: var(--panel); display: flex; flex-direction: column; box-shadow: 0 -5px 20px #000; z-index: 10; border-radius: 20px 20px 0 0; overflow: hidden; }

    .tabs { display: flex; background: #252525; border-bottom: 2px solid #333; }
    .tab-btn { flex: 1; padding: 15px; background: none; border: none; color: #777; font-weight: bold; cursor: pointer; transition: 0.2s; }
    .tab-btn.active { color: var(--accent); border-bottom: 3px solid var(--accent); background: rgba(0, 210, 255, 0.05); }
    .content { display: none; flex: 1; overflow-y: auto; padding: 20px; padding-bottom: 80px; }
    .content.active { display: block; }
    
    .card { background: var(--card); padding: 10px; border-radius: 10px; margin-bottom: 10px; border-left: 3px solid var(--accent); }
    .card-head { display: flex; justify-content: space-between; font-size: 14px; color: #aaa; margin-bottom: 5px; }
    .card-val { color: var(--accent); font-weight: bold; }
    .ctrl-row { display: flex; align-items: center; gap: 10px; }
    .btn-srv { width: 40px; height: 40px; background: #333; border: none; color: #fff; border-radius: 5px; font-size: 20px; cursor: pointer; }
    .btn-srv:active { background: var(--accent); color: #000; }
    
    input[type=range] { flex: 1; -webkit-appearance: none; background: transparent; padding: 10px 0; }
    input[type=range]::-webkit-slider-runnable-track { height: 8px; background: #444; border-radius: 4px; }
    input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; height: 24px; width: 24px; background: var(--accent); border-radius: 50%; margin-top: -8px; border: 2px solid #fff; }

    .pt-item { background: var(--card); padding: 10px; margin: 5px 0; border-radius: 8px; display: flex; flex-direction: column; gap: 5px; border-left: 3px solid #555; }
    .pt-head { display: flex; justify-content: space-between; align-items: center; font-weight: bold; cursor: pointer; }
    .pt-name { color: #fff; font-size: 15px; }
    .pt-data { font-family: monospace; font-size: 11px; color: #888; background: #111; padding: 3px; border-radius: 3px; }
    .pt-acts { display: flex; gap: 5px; margin-top: 5px; }
    .btn { border: none; padding: 8px; border-radius: 5px; cursor: pointer; font-weight: bold; font-size: 12px; }
    .btn-add { background: var(--accent); color: #000; flex: 1; }
    .btn-del { background: transparent; border: 1px solid var(--danger); color: var(--danger); }
    .btn-edit { background: transparent; border: none; color: #aaa; font-size: 16px; cursor: pointer; padding: 0 5px; }
    
    #prog-list { min-height: 80px; border: 2px dashed #444; border-radius: 10px; padding: 10px; margin-bottom: 15px; }
    .prog-step { background: #333; margin: 8px 0; padding: 10px; border-radius: 8px; display: flex; align-items: center; gap: 10px; border-left: 4px solid var(--success); font-size: 14px; position: relative; }
    .prog-step.delay-step { border-left-color: var(--delay); background: #2d2d22; }
    .prog-step.active { background: #004400; border-color: #00ff00; transform: scale(1.02); transition: 0.2s; box-shadow: 0 0 10px rgba(0,255,0,0.5); }
    .step-content { flex: 1; display: flex; flex-direction: column; justify-content: center; }
    .step-title { font-weight: bold; color: #fff; }
    .step-desc { font-size: 11px; color: #aaa; }
    .delay-input { background: #111; border: 1px solid var(--delay); color: var(--delay); width: 70px; padding: 5px; border-radius: 5px; font-weight: bold; text-align: center; }
    .sort-btns { display: flex; flex-direction: column; gap: 2px; margin-right: 5px; }
    .btn-sort { background: #222; border: 1px solid #444; color: #888; width: 24px; height: 24px; font-size: 10px; cursor: pointer; border-radius: 4px; display: flex; align-items: center; justify-content: center; }
    
    .btn-run { width: 100%; padding: 15px; font-size: 16px; font-weight: bold; background: var(--success); border: none; border-radius: 8px; margin-top: 10px; cursor: pointer; }
    .btn-run.stop { background: var(--danger); color: white; animation: pulse 2s infinite; }
    .btn-estop { width: 100%; padding: 12px; background: var(--danger); color: white; border: 2px solid #fff; font-weight: 900; letter-spacing: 1px; border-radius: 8px; margin-bottom: 20px; cursor: pointer; }
    .btn-estop.active { background: yellow; color: black; border-color: red; }
    .btn-reset { width: 100%; padding: 10px; background: #444; color: #ddd; border: 1px solid #666; border-radius: 8px; cursor: pointer; font-weight: bold; margin-top: 5px; transition: 0.2s; }
    .btn-add-delay { width: 100%; padding: 12px; background: #333; color: var(--delay); border: 1px dashed var(--delay); border-radius: 8px; font-weight: bold; cursor: pointer; margin-bottom: 10px; }
    
    .speed-card { background: var(--card); padding: 15px; border-radius: 10px; margin-bottom: 15px; border-left: 3px solid var(--warning); }
    .speed-labels { display: flex; justify-content: space-between; font-size: 12px; color: #888; margin-top: 5px; }
    input.speed-range { -webkit-appearance: none; width: 100%; background: transparent; margin: 10px 0; }
    input.speed-range::-webkit-slider-runnable-track { height: 8px; background: #333; border-radius: 5px; border: 1px solid #555; }
    input.speed-range::-webkit-slider-thumb { -webkit-appearance: none; height: 24px; width: 24px; background: var(--warning); border-radius: 50%; margin-top: -9px; border: 2px solid #fff; box-shadow: 0 2px 5px black; }
    
    .fab { position: fixed; bottom: 20px; right: 20px; width: 55px; height: 55px; background: var(--accent); border-radius: 50%; border: none; font-size: 30px; display: flex; justify-content: center; align-items: center; box-shadow: 0 4px 15px rgba(0,210,255,0.4); z-index: 100; cursor: pointer; }
    @keyframes pulse { 0% { box-shadow: 0 0 0 0 rgba(255, 68, 68, 0.7); } 70% { box-shadow: 0 0 0 10px rgba(255, 68, 68, 0); } 100% { box-shadow: 0 0 0 0 rgba(255, 68, 68, 0); } }
    .setting-row { display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px; font-size: 14px; }
    .inp-num { background: #111; border: 1px solid #444; color: #fff; width: 60px; padding: 5px; border-radius: 5px; text-align: center; }
    .switch { position: relative; display: inline-block; width: 40px; height: 20px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #555; transition: .4s; border-radius: 20px; }
    .slider:before { position: absolute; content: ""; height: 16px; width: 16px; left: 2px; bottom: 2px; background-color: white; transition: .4s; border-radius: 50%; }
    input:checked + .slider { background-color: var(--success); }
    input:checked + .slider:before { transform: translateX(20px); }
    #toast { visibility: hidden; min-width: 250px; background-color: var(--success); color: #000; text-align: center; border-radius: 50px; padding: 16px; position: fixed; z-index: 200; left: 50%; bottom: 30px; transform: translateX(-50%); font-size: 14px; font-weight: bold; box-shadow: 0 4px 10px rgba(0,0,0,0.5); opacity: 0; transition: opacity 0.3s, bottom 0.3s; }
    #toast.show { visibility: visible; opacity: 1; bottom: 90px; }
</style>
</head>
<body>
<div id="toast">Powiadomienie</div>

<div id="layout">
    <div id="viz-container">
        <div id="no-3d-msg" style="position:absolute; top:50%; left:50%; transform:translate(-50%,-50%); color:#555; text-align:center;">
            <h3>Tryb Offline</h3>
            <p style="font-size:12px">Wizualizacja 3D niedostępna bez internetu.<br>Sterowanie suwakami działa normalnie.</p>
        </div>
    </div>

    <div id="panel">
        <div class="tabs">
            <button class="tab-btn active" onclick="setTab('manual')">STEROWANIE</button>
            <button class="tab-btn" onclick="setTab('program')">PROGRAM</button>
        </div>

        <div id="tab-manual" class="content active">
            <div id="servos-list"></div>
            
            <div class="speed-card">
                <div class="card-head"><span>Prędkość Ruchu</span><span id="speed-label" style="color:var(--warning)">Średnio</span></div>
                <input type="range" id="speed-slider" class="speed-range" min="1" max="3" step="1" value="2" oninput="changeSpeed(this.value)">
                <div class="speed-labels"><span>Wolno</span><span>Średnio</span><span>Szybko</span></div>
            </div>

            <button class="btn-reset" onclick="resetToDefault()">↺ RESETUJ DO DOMYŚLNYCH</button>
            
            <div style="margin-top: 20px;">
                <h3>Zapisane pozycje</h3>
                <div id="points-list"></div>
            </div>
        </div>

        <div id="tab-program" class="content">
            <button class="btn-estop" id="estop-btn" onclick="toggleEStop()">STOP AWARYJNY</button>
            <button class="btn-add-delay" onclick="addDelayToProg()">+ DODAJ OCZEKIWANIE (DELAY)</button>
            <div id="prog-list"><div style="text-align:center; color:#555; padding:10px">Pusta sekwencja</div></div>
            <div style="background:#222; padding:10px; border-radius:8px;">
                <div class="setting-row"><span>Odstęp pętli (ms):</span><input type="number" id="loop-time" value="500" class="inp-num"></div>
                <div class="setting-row"><span>Zapętlanie:</span><label class="switch"><input type="checkbox" id="loop-check"><span class="slider"></span></label></div>
            </div>
            <button class="btn-run" id="run-btn" onclick="toggleRun()">▶ URUCHOM</button>
            <button class="btn" style="width:100%; margin-top:5px; background:#333; color:#aaa" onclick="clearProg()">Wyczyść</button>
        </div>
    </div>
</div>

<button class="fab" id="fab" onclick="savePoint()">+</button>

<script>
const SERVO_CFG = [
    { id: 1, name: "Baza", min: 0, max: 180, val: 90 },
    { id: 2, name: "Ramię Dolne", min: 90, max: 180, val: 90 }, 
    { id: 3, name: "Ramię Górne", min: 0, max: 180, val: 90 },
    { id: 4, name: "Nadgarstek", min: 0, max: 180, val: 90 },
    { id: 5, name: "Chwytak", min: 0, max: 100, val: 100 } 
];

let state = { servos: [90, 90, 90, 90, 100], points: {}, program: [], pointId: 0, isRunning: false, isEStop: false, speedMs: 1000 };
let net = { queue: [null,null,null,null,null], busy: false };

function processQueue() {
    if (state.isEStop || net.busy) return;
    let targetIdx = -1;
    for(let i=0; i<5; i++) { if(net.queue[i] !== null) { targetIdx = i; break; } }
    if (targetIdx !== -1) {
        net.busy = true; const angle = net.queue[targetIdx]; net.queue[targetIdx] = null;
        fetch(`/servo?nr=${targetIdx+1}&angle=${angle}`).finally(() => { net.busy = false; processQueue(); });
    }
}
function sendCmd(nr, angle) { if (state.isEStop) return; net.queue[nr-1] = angle; processQueue(); }
function showToast(msg) { var x = document.getElementById("toast"); x.innerText = msg; x.className = "show"; setTimeout(()=>{x.className=""}, 2500); }
function changeSpeed(val) {
    const labels = {1:"Wolno", 2:"Średnio", 3:"Szybko"}; const times = {1:2000, 2:1000, 3:500};
    document.getElementById("speed-label").innerText = labels[val]; state.speedMs = times[val];
    fetch(`/speed?val=${val}`).catch(e=>{});
}

// TO JEST NAJWAŻNIEJSZA FUNKCJA - TWORZY SUWAKI
function initUI() {
    console.log("Generowanie suwakow...");
    const cont = document.getElementById("servos-list");
    cont.innerHTML = ""; // Wyczyść stare
    SERVO_CFG.forEach((cfg, i) => {
        const div = document.createElement("div"); div.className = "card";
        div.innerHTML = `<div class="card-head"><span>S${cfg.id}: ${cfg.name}</span><span id="disp-${i}" class="card-val">${cfg.val}°</span></div><div class="ctrl-row"><button class="btn-srv" onmousedown="modServo(${i}, -1)" ontouchstart="modServo(${i}, -1)">-</button><input type="range" id="rng-${i}" min="${cfg.min}" max="${cfg.max}" value="${cfg.val}"><button class="btn-srv" onmousedown="modServo(${i}, 1)" ontouchstart="modServo(${i}, 1)">+</button></div>`;
        cont.appendChild(div);
        const rng = document.getElementById(`rng-${i}`);
        rng.addEventListener('input', (e) => { const val = parseInt(e.target.value); state.servos[i] = val; updateVis(i, val); });
        rng.addEventListener('change', (e) => { sendCmd(i+1, parseInt(e.target.value)); });
    });
}

function modServo(idx, delta) {
    if(event.type === 'touchstart') event.preventDefault();
    const rng = document.getElementById(`rng-${idx}`);
    let val = parseInt(rng.value) + delta;
    if (val < SERVO_CFG[idx].min) val = SERVO_CFG[idx].min; 
    if (val > SERVO_CFG[idx].max) val = SERVO_CFG[idx].max;
    rng.value = val; state.servos[idx] = val; updateVis(idx, val); sendCmd(idx+1, val);
}

function updateVis(idx, val) { 
    document.getElementById(`disp-${idx}`).innerText = val + (idx===4?"%":"°"); 
    // Sprawdzamy czy 3D istnieje zanim wywołamy
    if (typeof robotParts !== 'undefined' && robotParts.s1) update3DModel(idx+1, val); 
}

async function resetToDefault() {
    if(state.isEStop || state.isRunning) return;
    const defaults = [90, 90, 90, 90, 100];
    document.getElementById('speed-slider').value = 2; changeSpeed(2);
    defaults.forEach((val, i) => sendCmd(i+1, val));
    await animateGUI(defaults, 1000, true);
    showToast("Zresetowano!");
}
function savePoint() {
    fetch(`/save?s1=${state.servos[0]}&s2=${state.servos[1]}&s3=${state.servos[2]}&s4=${state.servos[3]}&s5=${state.servos[4]}`).finally(() => {
        state.pointId++; const pid = state.pointId;
        state.points[pid] = { name: "Poz #" + pid, data: [...state.servos] };
        const div = document.createElement("div"); div.className = "pt-item"; div.id = `pt-row-${pid}`;
        div.innerHTML = `<div class="pt-head"><span id="pt-name-${pid}" class="pt-name" onclick="loadPoint(${pid})">${state.points[pid].name}</span><button class="btn-edit" onclick="renamePoint(${pid})">✎</button></div><div class="pt-data" onclick="loadPoint(${pid})">S:[${state.points[pid].data.join(',')}]</div><div class="pt-acts"><button class="btn btn-add" onclick="addToProg(${pid})">DODAJ</button><button class="btn btn-del" onclick="delPoint(${pid})">USUŃ</button></div>`;
        document.getElementById("points-list").appendChild(div);
        showToast("Zapisano: " + state.points[pid].name);
    });
}
function renamePoint(pid) { const n = prompt("Nazwa:", state.points[pid].name); if(n){ state.points[pid].name = n; document.getElementById(`pt-name-${pid}`).innerText = n; renderProg(); } }
function delPoint(pid) { document.getElementById(`pt-row-${pid}`).remove(); delete state.points[pid]; }
async function loadPoint(pid) {
    if(state.isEStop || state.isRunning) return;
    const vals = state.points[pid].data;
    
    // Wysyłamy jedną komendę zbiorczą, żeby robot ruszył płynnie wszystkimi stawami
    fetch(`/moveAll?s1=${vals[0]}&s2=${vals[1]}&s3=${vals[2]}&s4=${vals[3]}&s5=${vals[4]}`);
    
    // Animujemy suwaki na ekranie
    await animateGUI(vals, state.speedMs, true);
}
function addToProg(pid) { state.program.push({ type: 'point', val: pid }); renderProg(); showToast("Dodano punkt"); }
function addDelayToProg() { state.program.push({ type: 'delay', val: 1000 }); renderProg(); showToast("Dodano pauzę"); }
function moveStep(idx, dir) { if(state.isRunning) return; const newIdx = idx + dir; if(newIdx < 0 || newIdx >= state.program.length) return; const temp = state.program[idx]; state.program[idx] = state.program[newIdx]; state.program[newIdx] = temp; renderProg(); }
function updateDelayVal(idx, val) { state.program[idx].val = parseInt(val); }
function renderProg() {
    const list = document.getElementById("prog-list"); list.innerHTML = "";
    if(state.program.length === 0) { list.innerHTML = '<div style="text-align:center; color:#555; padding:20px; font-style:italic;">Brak kroków</div>'; return; }
    state.program.forEach((step, idx) => {
        const div = document.createElement("div");
        let content = "";
        if(step.type === 'point') {
            div.className = "prog-step"; const ptName = state.points[step.val] ? state.points[step.val].name : "???";
            content = `<div class="step-content"><div class="step-title">${ptName}</div><div class="step-desc">Ruch do pozycji</div></div>`;
        } else {
            div.className = "prog-step delay-step";
            content = `<div class="step-content"><div class="step-title" style="color:var(--delay)">PAUZA</div><div class="step-desc">Oczekiwanie</div></div> <input type="number" class="delay-input" value="${step.val}" onchange="updateDelayVal(${idx}, this.value)"> <span style="font-size:12px; color:#888">ms</span>`;
        }
        div.innerHTML = content + `<div class="sort-btns"><button class="btn-sort" onclick="moveStep(${idx}, -1)">▲</button><button class="btn-sort" onclick="moveStep(${idx}, 1)">▼</button></div><span style="color:#d44; cursor:pointer; font-weight:bold; padding:0 10px;" onclick="remStep(${idx})">✖</span>`;
        div.id = `step-${idx}`; list.appendChild(div);
    });
}
function remStep(idx) { if(!state.isRunning) { state.program.splice(idx, 1); renderProg(); } }
function clearProg() { if(!state.isRunning) { state.program = []; renderProg(); } }
function toggleEStop() {
    state.isEStop = !state.isEStop; state.isRunning = false; const btn = document.getElementById("estop-btn");
    if(state.isEStop) { btn.innerText = "ODBLOKUJ ROBOTA"; btn.classList.add("active"); document.body.style.filter = "grayscale(1)"; }
    else { btn.innerText = "STOP AWARYJNY"; btn.classList.remove("active"); document.body.style.filter = "none"; }
}
async function toggleRun() {
    if(state.isEStop) return; if(state.isRunning) { state.isRunning = false; return; } if(state.program.length === 0) { showToast("Pusta sekwencja!"); return; }
    state.isRunning = true; const btn = document.getElementById("run-btn"); btn.innerText = "⛔ ZATRZYMAJ"; btn.classList.add("stop");
    const loopMs = parseInt(document.getElementById("loop-time").value) || 500; const isLoop = document.getElementById("loop-check").checked;
    do {
        for(let i=0; i<state.program.length; i++) {
            if(!state.isRunning || state.isEStop) break;
            document.querySelectorAll(".prog-step").forEach(e => e.classList.remove("active"));
            const stepEl = document.getElementById(`step-${i}`); if(stepEl) { stepEl.classList.add("active"); stepEl.scrollIntoView({block:"center", behavior:"smooth"}); }
            const step = state.program[i];
            if(step.type === 'point') { if(state.points[step.val]) { const target = state.points[step.val].data; target.forEach((val, idx) => sendCmd(idx+1, val)); await animateGUI(target, state.speedMs); } }
            else if (step.type === 'delay') { await delay(step.val); }
        }
        if(state.isRunning && isLoop && !state.isEStop) await delay(loopMs);
    } while(state.isRunning && isLoop && !state.isEStop);
    state.isRunning = false; btn.innerText = "▶ URUCHOM"; btn.classList.remove("stop"); document.querySelectorAll(".prog-step").forEach(e => e.classList.remove("active"));
}
function animateGUI(targetVals, duration, force = false) {
    return new Promise(resolve => {
        if(state.isEStop || (!state.isRunning && !force)) { resolve(); return; }
        const startVals = [...state.servos]; const startTime = performance.now();
        function frame(now) {
            if(state.isEStop || (!state.isRunning && !force)) { resolve(); return; }
            const progress = Math.min((now - startTime) / duration, 1);
            targetVals.forEach((t, i) => {
                const s = startVals[i]; const curr = Math.floor(s + (t - s) * progress);
                const rng = document.getElementById(`rng-${i}`);
                if(rng) { rng.value = curr; updateVis(i, curr); state.servos[i] = curr; }
            });
            if(progress < 1) requestAnimationFrame(frame); else resolve();
        }
        requestAnimationFrame(frame);
    });
}
const delay = ms => new Promise(res => setTimeout(res, ms));

// --- THREE.JS (Tylko jeśli biblioteka się załaduje) ---
let scene, camera, renderer, controls, robotParts={};
function init3D() {
    if (typeof THREE === 'undefined') { console.log("Brak THREE.js - pomijam 3D"); return; }
    
    document.getElementById("no-3d-msg").style.display = "none";
    const c = document.getElementById('viz-container'); scene = new THREE.Scene(); 
    const amb = new THREE.AmbientLight(0xffffff, 0.5); scene.add(amb); const dir = new THREE.DirectionalLight(0xffffff, 0.8); dir.position.set(10,20,10); scene.add(dir);
    camera = new THREE.PerspectiveCamera(45, c.clientWidth/c.clientHeight, 0.1, 1000); camera.position.set(45, 40, 45); 
    renderer = new THREE.WebGLRenderer({antialias:true, alpha:true}); renderer.setSize(c.clientWidth, c.clientHeight); c.appendChild(renderer.domElement);
    controls = new THREE.OrbitControls(camera, renderer.domElement); controls.target.set(0,5,0); controls.enableDamping=true;
    scene.add(new THREE.GridHelper(50, 50, 0x444444, 0x222222));
    buildRobot(); animate();
    window.addEventListener('resize', () => { camera.aspect = c.clientWidth/c.clientHeight; camera.updateProjectionMatrix(); renderer.setSize(c.clientWidth, c.clientHeight); });
}
function mkJoint(r, len) { const g = new THREE.Group(); const cyl = new THREE.Mesh(new THREE.CylinderGeometry(r, r, len, 32), new THREE.MeshPhongMaterial({color:0x888888})); cyl.rotation.x = Math.PI/2; g.add(cyl); return g; }
function buildRobot() {
    const matD = new THREE.MeshPhongMaterial({color:0x333}); const matL = new THREE.MeshPhongMaterial({color:0x00d2ff});
    const base = new THREE.Mesh(new THREE.CylinderGeometry(4, 5, 2, 32), matD); base.position.y = 1; scene.add(base);
    robotParts.s1 = new THREE.Group(); robotParts.s1.position.y = 2; base.add(robotParts.s1); robotParts.s1.add(new THREE.Mesh(new THREE.BoxGeometry(4, 2, 4), matD)).position.y = 1;
    robotParts.s2 = new THREE.Group(); robotParts.s2.position.y = 2; robotParts.s1.add(robotParts.s2); robotParts.s2.add(mkJoint(1.5, 5)); const arm1 = new THREE.Mesh(new THREE.BoxGeometry(1.5, 9, 2), matL); arm1.position.y = 4.5; robotParts.s2.add(arm1);
    robotParts.s3 = new THREE.Group(); robotParts.s3.position.y = 9; robotParts.s2.add(robotParts.s3); robotParts.s3.add(mkJoint(1.2, 3)); const arm2 = new THREE.Mesh(new THREE.BoxGeometry(1.2, 9, 1.5), matL); arm2.position.y = 4.5; robotParts.s3.add(arm2);
    robotParts.s4 = new THREE.Group(); robotParts.s4.position.y = 9; robotParts.s3.add(robotParts.s4); robotParts.s4.add(mkJoint(1, 2.5)); const wrist = new THREE.Mesh(new THREE.BoxGeometry(2, 0.5, 1.5), matD); wrist.position.y = 0.5; robotParts.s4.add(wrist);
    const fMat = new THREE.MeshPhongMaterial({color:0xff0055}); const fGeo = new THREE.BoxGeometry(0.3, 2, 0.5); robotParts.gL = new THREE.Mesh(fGeo, fMat); robotParts.gL.position.set(-0.5, 1.5, 0); robotParts.gR = new THREE.Mesh(fGeo, fMat); robotParts.gR.position.set(0.5, 1.5, 0); robotParts.s4.add(robotParts.gL); robotParts.s4.add(robotParts.gR);
}
function update3DModel(nr, val) {
    if(!robotParts.s1) return; const rad = val * (Math.PI/180);
    switch(nr) { case 1: robotParts.s1.rotation.y = -rad + Math.PI/2; break; case 2: robotParts.s2.rotation.z = rad - Math.PI/2; break; case 3: robotParts.s3.rotation.z = -rad + Math.PI/2; break; case 4: robotParts.s4.rotation.z = rad - Math.PI/2; break; case 5: const w = 0.2 + (val/100)*0.8; robotParts.gL.position.x = -w; robotParts.gR.position.x = w; break; }
}
function animate() { requestAnimationFrame(animate); controls.update(); renderer.render(scene, camera); }
function setTab(name) { document.querySelectorAll('.content').forEach(e => e.classList.remove('active')); document.querySelectorAll('.tab-btn').forEach(e => e.classList.remove('active')); document.getElementById(`tab-${name}`).classList.add('active'); event.target.classList.add('active'); document.getElementById('fab').style.display = name==='manual' ? 'flex':'none'; }

// START (Zmieniona kolejność - najpierw UI, potem próba 3D)
window.onload = function() { 
    initUI(); // Najpierw suwaki!
    try { init3D(); } catch(e) { console.log("Brak 3D"); } // Potem 3D (może się nie udać)
    
    // Pobranie pozycji z ESP
    fetch('/read').then(r=>r.text()).then(d=>{ 
        const p = d.split(';'); 
        if(p.length===5) p.forEach((v,i) => { 
            state.servos[i]=parseInt(v); 
            const el = document.getElementById(`rng-${i}`);
            if(el) el.value=v; 
            updateVis(i, v); 
        }); 
    }).catch(e=>{ console.log("Błąd odczytu"); }); 
};
</script>
</body>
</html>
)rawliteral";