import express from "express";
import morgan from "morgan";
import path from "path";
import http from "http";
import https from "https";

import { routes } from "./routes";
import { getCerts } from "./certs";
import { initWebSockets } from "./websockets";

const PORT = process.env.PORT || 80;
const SSL_PORT = process.env.SSL_PORT || 443;
const STATIC_DIR = path.join(__dirname, "../../static");

export default async function httpServer() {
  const app = express();

  // Logging
  app.use(morgan("tiny"));

  // Static file serving
  app.use("/static", express.static(path.join(__dirname, STATIC_DIR)));

  // Routes
  app.use(routes);

  // Create server
  const certs = getCerts();
  const httpServer = http.createServer(app);
  const httpsServer = https.createServer(certs, app);

  // WebSocket
  initWebSockets(httpServer);
  initWebSockets(httpsServer);
  console.log("🔌 WebSockets started");

  // Start server
  httpServer.listen(Number(PORT));
  httpsServer.listen(Number(SSL_PORT));
  console.log(`🚀 Server at localhost:${PORT}`);
}
