import http.server
import socketserver

# python3 serve_coop_coep.py

PORT = 8000

class Handler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        # Add COOP/COEP headers
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        # Optionally add your CSP header here or serve from your HTML file
        # self.send_header('Content-Security-Policy', "default-src 'self'; script-src 'self'; connect-src 'self'; worker-src blob:; style-src 'self';")
        super().end_headers()

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    httpd.allow_reuse_address = True
    print(f"Serving at http://localhost:{PORT}")
    httpd.serve_forever()

