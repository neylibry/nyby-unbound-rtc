from pathlib import Path
import json
root = Path(__file__).resolve().parent
payload = json.loads((root / 'payload.json').read_text(encoding='utf-8'))
html = (root / 'installer.template.html').read_text(encoding='utf-8')
html = html.replace('__PAYLOAD__', json.dumps(payload, ensure_ascii=False).replace('<', '\\u003c'))
html = html.replace('__CORE__', (root / 'installer-core.js').read_text(encoding='utf-8'))
(root.parent.parent / 'INSTALLER-FR-EN.html').write_text(html, encoding='utf-8')
(root.parent.parent / 'INSTALLER-FR.html').write_text(html, encoding='utf-8')
english = html.replace('<option value="en">English</option>', '<option value="en" selected>English</option>')
(root.parent.parent / 'INSTALLER-EN.html').write_text(english, encoding='utf-8')
