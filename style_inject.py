import re

with open("index.html", "r", encoding="utf-8") as f:
    html = f.read()

custom_style = """
<link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Rajdhani:wght@600;700&display=swap" rel="stylesheet"/>
<style>
:root {
  --green: #00ff88; --green-dim: #00cc66;
  --green-glow: rgba(0,255,136,0.15);
  --bg: #050e0a; --panel: #080f0b;
  --border: #0d2b1a; --text: #b0f0cc; --text-dim: #4a8a62;
}
body {
  background: var(--bg) !important;
  color: var(--text) !important;
  font-family: 'Share Tech Mono', monospace !important;
  display: flex !important;
  flex-direction: column !important;
  align-items: center !important;
  padding: 0 16px 24px !important;
  margin: 0 !important;
}
body::before {
  content: '';
  position: fixed; inset: 0;
  background: repeating-linear-gradient(0deg,transparent,transparent 2px,rgba(0,0,0,0.06) 2px,rgba(0,0,0,0.06) 4px);
  pointer-events: none; z-index: 999;
}
/* Header banner */
body::after {
  content: '🏦  MINI BANKING SYSTEM';
  display: block;
  font-family: 'Rajdhani', sans-serif;
  font-weight: 700; font-size: 1.3rem;
  color: var(--green);
  letter-spacing: 3px;
  text-transform: uppercase;
  padding: 18px 0 14px;
  border-bottom: 1px solid var(--border);
  width: 100%; max-width: 860px;
  text-align: left;
  order: -1;
}
/* Hide default emscripten logo/header junk */
.emscripten_border { display: none !important; }
figure { display: none !important; }
/* The output textarea */
#output {
  width: 100% !important; max-width: 860px !important;
  min-height: 460px !important;
  background: var(--panel) !important;
  color: var(--green) !important;
  font-family: 'Share Tech Mono', monospace !important;
  font-size: 0.87rem !important;
  line-height: 1.75 !important;
  padding: 20px !important;
  border: 1px solid var(--border) !important;
  border-radius: 10px !important;
  resize: none !important;
  outline: none !important;
  box-shadow: 0 0 40px rgba(0,255,136,0.04), 0 8px 32px rgba(0,0,0,0.6) !important;
  margin-top: 16px !important;
}
/* Canvas hidden */
#canvas { display: none !important; }
/* Spinner/status */
#status, #progress { color: var(--text-dim) !important; font-size: 0.7rem !important; }
</style>
"""

# Inject before </head>
html = html.replace("</head>", custom_style + "\n</head>", 1)

with open("index.html", "w", encoding="utf-8") as f:
    f.write(html)

print("Done! index.html has been styled.")
